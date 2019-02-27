#include "post_processor.h"
#include "config_parser.h"
#include <iostream>
#include <algorithm>

using namespace std;
using namespace tool_ns;

namespace post_processor
{
bool cmp1(vector<string> a, vector<string> b)
{
	double a1 = atof(a[1].c_str());
	double b1 = atof(b[1].c_str());
	return a1 < b1;
}

bool PostProcessor::Init(const char *config)
{
	ConfigParser parser(config);
	parser.dump_config_list();

	parser.get_param<double>("min-lcs", "700", true, this->MIN_LCS);
	parser.get_param<double>("num-space", "100", true, this->NUM_SPACE);
	parser.get_param<double>("max-levenshtein", "300", true, this->MAX_LEVENSHTEIN);
	parser.get_param<double>("min-word-length", "3000", true, this->MIN_WORD_LENGTH);
	parser.get_param<bool>("is-write-log", "1", true, this->WRITE_LOG_FILE);

	if (this->WRITE_LOG_FILE)
	{
		m_logger.open("post.log", ios::app);
	}
	return true;
}

std::string PostProcessor::Process(const std::string &src_line, const std::string &tgt_line)
{
	const bool is_zh2en_task = _lang == "zh2en" ? true : false;

	std::string line_zh(is_zh2en_task ? src_line : tgt_line);
	std::string line_en(is_zh2en_task ? tgt_line : src_line);
	// replace $userdict
	if ((line_zh.find(" |||| ") != string::npos) && (line_zh.find("$userdict") != string::npos))
		ReplaceUserdict(line_zh);
	if ((line_en.find(" |||| ") != string::npos) && (line_en.find("$userdict") != string::npos))
		ReplaceUserdict(line_en);

	std::string en_processed = LowerCaseRemoveSpaces(line_en);
	if (WRITE_LOG_FILE)
	{
		m_logger << "After LowerCaseRemoveSpaces:" << endl;
		m_logger << "ZH: " << line_zh << endl;
		m_logger << "EN: " << line_en << endl;
	}
	std::string line_en_unchanged = line_en;
	//line_zh_old = line_zh;

	// Extract all English parts
	vector<string> list_notzh;
	ExtractAllEnglish(line_zh, list_notzh);

	// traversal the English part in Chinese
	for (int count_list_notzh = 0; count_list_notzh < list_notzh.size(); count_list_notzh++)
	{
		std::string non_chinese = list_notzh[count_list_notzh];
		bool flag_have_en = false;
		for (int count_non_chinese = 0; count_non_chinese < non_chinese.length(); count_non_chinese++)
		{
			if (isalpha(non_chinese[count_non_chinese]))
				flag_have_en = true;
		}
		if (flag_have_en == false)
			continue;
		//remove $userdict
		string_replace(non_chinese, "$userdict", "");
		std::string non_chinese_without_punc = "";
		non_chinese = strip(non_chinese);
		// Remove the punctuation
		for (string::size_type i = 0; i < non_chinese.size(); i++)
			if ((!ispunct(non_chinese[i])) || (non_chinese[i] == '-') || (non_chinese[i] == '_'))
				non_chinese_without_punc += non_chinese[i];
		non_chinese_without_punc = strip(non_chinese_without_punc);
		// Is there any meaningful depunctuation
		int flag_str_punc = 0;
		if ((non_chinese_without_punc != non_chinese) && (non_chinese_without_punc.length() != 0))
			flag_str_punc = 1;

		std::string zh_processed = LowerCaseRemoveSpaces(non_chinese);
		std::string zh_processed_without_punc = LowerCaseRemoveSpaces(non_chinese_without_punc);
		// The sentence already has an or that is too short to be replaced: continue
		if ((line_en.find(non_chinese) != string::npos) || (line_en.find(non_chinese_without_punc) != string::npos) || (non_chinese_without_punc.length() <= MIN_WORD_LENGTH))
			continue;
		// complete match
		if (en_processed.find(zh_processed) != string::npos)
			CompleteMatch(en_processed, zh_processed, non_chinese, line_en, line_zh);
		// complete match after punctuation is removed
		else if ((en_processed.find(zh_processed_without_punc) != string::npos) && (flag_str_punc == 1))
			NoPuncMatch(en_processed, zh_processed_without_punc, non_chinese_without_punc, line_en, line_zh);
		// Incomplete match
		else
			IncompleteMatch(en_processed, zh_processed, non_chinese, line_en, line_zh, line_en_unchanged);
	}

	return strip(is_zh2en_task ? line_en : line_zh);
}

//Levenshtein
int PostProcessor::LevenshteinDistance(const std::string &source, const std::string &target)
{
	//step 1

	int n = source.length();
	int m = target.length();
	if (m == 0)
		return n;
	if (n == 0)
		return m;
	//Construct a matrix
	typedef vector<vector<int>> Tmatrix;
	Tmatrix matrix(n + 1);
	for (int i = 0; i <= n; i++)
		matrix[i].resize(m + 1);

	//step 2 Initialize

	for (int i = 1; i <= n; i++)
		matrix[i][0] = i;
	for (int i = 1; i <= m; i++)
		matrix[0][i] = i;

	//step 3
	for (int i = 1; i <= n; i++)
	{
		const char si = source[i - 1];
		//step 4
		for (int j = 1; j <= m; j++)
		{

			const char dj = target[j - 1];
			//step 5
			int cost;
			if (si == dj)
			{
				cost = 0;
			}
			else
			{
				cost = 1;
			}
			//step 6
			const int above = matrix[i - 1][j] + 1;
			const int left = matrix[i][j - 1] + 1;
			const int diag = matrix[i - 1][j - 1] + cost;
			matrix[i][j] = min(above, min(left, diag));
		}
	} //step7
	return matrix[n][m];
}

std::string PostProcessor::do_strip(const std::string &str, int striptype, const std::string &chars)
{
	// #define LEFTSTRIP 0
	// #define RIGHTSTRIP 1
	// #define BOTHSTRIP 2
	// std::string::size_type strlen = str.size();
	// std::string::size_type charslen = chars.size();
	// std::string::size_type i, j;
	int strlen = str.size();
	int charslen = chars.size();
	int i, j;

	//By default, whitespace is removed
	if (0 == charslen)
	{
		i = 0;
		//Remove the left blank character
		if (striptype != 1)
		{
			while (i < strlen && ::isspace(str[i]))
			{
				i++;
			}
		}
		j = strlen;
		//Remove the white space character on the right
		if (striptype != 0)
		{
			j--;
			while (j >= i && ::isspace(str[j]))
			{
				j--;
			}
			j++;
		}
	}
	else
	{
		//Change the delete sequence to a c string
		const char *sep = chars.c_str();
		i = 0;
		if (striptype != 1)
		{
			//memchr��������sepָ����ڴ������ǰcharslen���ֽڲ���str[i]
			while (i < strlen && memchr(sep, str[i], charslen))
			{
				i++;
			}
		}
		j = strlen;
		if (striptype != 0)
		{
			j--;
			while (j >= i && memchr(sep, str[j], charslen))
			{
				j--;
			}
			j++;
		}
		//If no characters need to be deleted
		if (0 == i && j == strlen)
		{
			return str;
		}
		else
		{
			return str.substr(i, j - i);
		}
	}
}

std::string PostProcessor::strip(const std::string &str, const std::string &chars)
{
	return do_strip(str, 2, chars);
}

std::string PostProcessor::lstrip(const std::string &str, const std::string &chars)
{
	return do_strip(str, 0, chars);
}

std::string PostProcessor::rstrip(const std::string &str, const std::string &chars)
{
	return do_strip(str, 1, chars);
}

void PostProcessor::string_replace(std::string &s1, const std::string &s2, const std::string &s3)
{
	string::size_type pos = 0;
	string::size_type a = s2.size();
	string::size_type b = s3.size();
	while ((pos = s1.find(s2, pos)) != string::npos)
	{
		s1.replace(pos, a, s3);
		pos += b;
	}
}

//Remove the Space in line and all lowercase
std::string PostProcessor::LowerCaseRemoveSpaces(const std::string &str)
{
	string newstr = str;
	string_replace(newstr, " ", "");
	transform(newstr.begin(), newstr.end(), newstr.begin(), ::tolower);
	return newstr;
}

int PostProcessor::LCSLength(const char *la, const char *lb)
{
	int dInit = 0, dLeft = 1, dUp = 2, dUpLeft = 3;
	if (!la || !lb)
		return 0;

	int lengthA = strlen(la);
	int lengthB = strlen(lb);

	if (!lengthA || !lengthB)
		return 0;

	int i, j;

	int **LCS_length;
	LCS_length = (int **)(new int[lengthA]);
	for (i = 0; i < lengthA; ++i)
		LCS_length[i] = (int *)new int[lengthB];

	for (i = 0; i < lengthA; ++i)
		for (j = 0; j < lengthB; ++j)
			LCS_length[i][j] = 0;

	int **LCS__lang;
	LCS__lang = (int **)(new int[lengthA]);
	for (i = 0; i < lengthA; ++i)
		LCS__lang[i] = (int *)new int[lengthB];

	for (i = 0; i < lengthA; ++i)
		for (j = 0; j < lengthB; ++j)
			LCS__lang[i][j] = dInit;

	for (i = 0; i < lengthA; ++i)
	{
		for (j = 0; j < lengthB; ++j)
		{
			if (i == 0 || j == 0)
			{
				if (la[i] == lb[j])
				{
					LCS_length[i][j] = 1;
					LCS__lang[i][j] = dUpLeft;
				}
				else
				{
					if (i > 0)
					{
						LCS_length[i][j] = LCS_length[i - 1][j];
						LCS__lang[i][j] = dUp;
					}
					if (j > 0)
					{
						LCS_length[i][j] = LCS_length[i][j - 1];
						LCS__lang[i][j] = dLeft;
					}
				}
			}

			else if (la[i] == lb[j])
			{
				LCS_length[i][j] = LCS_length[i - 1][j - 1] + 1;
				LCS__lang[i][j] = dUpLeft;
			}

			else if (LCS_length[i - 1][j] > LCS_length[i][j - 1])
			{
				LCS_length[i][j] = LCS_length[i - 1][j];
				LCS__lang[i][j] = dUp;
			}

			else
			{
				LCS_length[i][j] = LCS_length[i][j - 1];
				LCS__lang[i][j] = dLeft;
			}
		}
	}
	//LCSPrint(LCS__lang, la, lb, lengthA - 1, lengthB - 1);

	return LCS_length[lengthA - 1][lengthB - 1];
}

void PostProcessor::split(const std::string &s, const std::string &delim, std::vector<std::string> *ret)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != string::npos)
	{
		ret->push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		ret->push_back(s.substr(last, index - last));
	}
}

bool PostProcessor::Split(const std::string &input, const std::string &sep, std::vector<std::string> &vec)
{
	std::string str = input;
	std::string substring;
	std::string::size_type start = 0, index = 0;
	std::string::size_type separator_len = sep.size();
	while (index != std::string::npos && start < input.size())
	{
		index = input.find(sep, start);
		if (index == 0)
		{
			if (start == 0)
				vec.push_back("");
			start = start + separator_len;
			continue;
		}
		if (index == std::string::npos)
		{
			vec.push_back(input.substr(start));
			break;
		}
		vec.push_back(input.substr(start, index - start));
		start = index + separator_len;
	}
	return true;
}

void PostProcessor::ReplaceUserdict(std::string &line)
{
	//  line == src line_en == tst
	string line_src_old = line;
	vector<string> temp_line(0);
	string s_space_line(" |||| ");
	Split(line_src_old, s_space_line, temp_line);

	line = temp_line[0];

	if (line_src_old.find(" ||| ") != string::npos)
	{
		string line_src_back = temp_line[1];

		vector<string> temp_kuohao(0);
		string s_space_kuoha("}{");
		Split(line_src_back, s_space_kuoha, temp_kuohao);

		int pos_userdict;
		string temp_num_kuohao;
		pos_userdict = 0;
		for (unsigned int i_temp = 0; i_temp < temp_kuohao.size(); i_temp++)
		{

			string str_kuohao = temp_kuohao[i_temp];
			string_replace(str_kuohao, "}", "");
			string_replace(str_kuohao, "{", "");

			vector<string> temp_threeline(0);
			string s_space_threeline(" ||| ");
			Split(str_kuohao, s_space_threeline, temp_threeline);

			if ((i_temp != 0) && (temp_num_kuohao == temp_threeline[0]))
				continue;
			temp_num_kuohao = temp_threeline[0];

			string userdict_word = temp_threeline[4];
			pos_userdict = line.find("$userdict", pos_userdict);
			line = line.replace(pos_userdict, 9, userdict_word);
		}
	}
}

int PostProcessor::NumOfSpace(const std::string &s)
{
	int num = 0;

	for (int index = 0; index < s.length(); ++index)
	{
		if (isspace(s[index]))
		{
			num++;
		}
	}

	return num;
}

void PostProcessor::ExtractAllEnglish(const std::string &line_zh, std::vector<std::string> &list_notzh)
{
	// Extract all English parts
	bool flag_find = false;
	string temp_en = "";
	for (int i = 0; i < line_zh.length(); i++)
	{
		unsigned char a = (unsigned char)line_zh[i];
		if (a < 0x80)
		{
			temp_en += line_zh[i];
			flag_find = true;
		}
		else if (flag_find)
		{
			if (WRITE_LOG_FILE)
				m_logger << "Extracted word : " << temp_en << endl;
			if (temp_en.length() >= MIN_WORD_LENGTH)
			{
				list_notzh.push_back(temp_en);
			}
			temp_en = "";
			flag_find = false;
		}
	}
	if (flag_find)
	{
		if (WRITE_LOG_FILE)
			m_logger << "Extracted word : " << temp_en << endl;
		if (temp_en.length() >= MIN_WORD_LENGTH)
		{
			list_notzh.push_back(temp_en);
		}
		temp_en = "";
		flag_find = false;
	}
}

void PostProcessor::CompleteMatch(const std::string &en_processed, const std::string &zh_processed, const std::string &non_chinese, std::string &line_en, std::string &line_zh)
{
	if (WRITE_LOG_FILE)
		m_logger << "CompleteMatch" << endl;
	int pos = 0;
	int start = 0;
	int subpos = en_processed.find(zh_processed);
	while (subpos != string::npos)
	{
		string temp_en = "";
		int start = subpos;
		int tempsubpos = subpos;
		for (string::size_type i = 0; i < line_en.size(); i++)
		{
			if (tempsubpos == 0)
			{
				temp_en += line_en[i];
				if ((LowerCaseRemoveSpaces(temp_en) == zh_processed) && (line_en[i + 1] == ' '))
				{
					if (strip(temp_en) != strip(non_chinese))
					{
						if (_lang == "en2zh")
							string_replace(line_zh, non_chinese, strip(temp_en));
						if (_lang == "zh2en")
							string_replace(line_en, strip(temp_en), non_chinese);
						break;
					}
				}
				continue;
			}
			if (line_en[i] != ' ')
				tempsubpos = tempsubpos - 1;
		}
		subpos = en_processed.find(zh_processed, subpos + 1);
	}
}

void PostProcessor::NoPuncMatch(const std::string &en_processed, const std::string &zh_processed_without_punc, const std::string &non_chinese_without_punc, std::string &line_en, std::string &line_zh)
{
	if (WRITE_LOG_FILE)
		m_logger << "NoPuncMatch" << endl;
	int pos = 0;
	int start = 0;
	int subpos = en_processed.find(zh_processed_without_punc);
	while (subpos != string::npos)
	{
		string temp_en = "";
		int start = subpos;
		int tempsubpos = subpos;
		for (string::size_type i = 0; i < line_en.size(); i++)
		{
			if (tempsubpos == 0)
			{
				temp_en += line_en[i];
				if (LowerCaseRemoveSpaces(temp_en) == zh_processed_without_punc)
				{
					if ((strip(temp_en) != strip(non_chinese_without_punc)) && (line_en[i + 1] == ' '))
					{
						if (_lang == "en2zh")
							string_replace(line_zh, non_chinese_without_punc, strip(temp_en));
						if (_lang == "zh2en")
							string_replace(line_en, strip(temp_en), non_chinese_without_punc);
						break;
					}
				}
				continue;
			}
			if (line_en[i] != ' ')
				tempsubpos = tempsubpos - 1;
		}
		subpos = en_processed.find(zh_processed_without_punc, subpos + 1);
	}
}

void PostProcessor::IncompleteMatch(const std::string &en_processed, const std::string &zh_processed, const std::string &non_chinese, std::string &line_en, std::string &line_zh, const std::string &line_en_unchanged)
{
	if (WRITE_LOG_FILE)
		m_logger << "IncompleteMatch" << endl;
	if ((LCSLength(en_processed.c_str(), zh_processed.c_str()) / (double)zh_processed.length() > MIN_LCS) && (line_zh.find(non_chinese) != string::npos))
	{
		vector<string> temp(0);
		string s_space(" ");
		split(line_en, s_space, &temp);
		vector<vector<string>> word_group_score(0);
		int count_word_group_score = 0;
		string word = "";
		string word_2gram = "";
		string word_3gram = "";
		string list_word[3];
		for (unsigned int i_temp = 0; i_temp < temp.size(); i_temp++)
		{
			// 3-gram
			word = temp[i_temp];
			if (i_temp < temp.size() - 1)
			{
				word_2gram = temp[i_temp] + " " + temp[i_temp + 1];
			}
			if (i_temp < temp.size() - 2)
			{
				word_3gram = temp[i_temp] + " " + temp[i_temp + 1] + " " + temp[i_temp + 2];
			}

			if (temp[i_temp].length() == 0)
				continue;
			list_word[0] = word;
			list_word[1] = word_2gram;
			list_word[2] = word_3gram;

			for (int i_list = 0; i_list < 3; i_list++)
			{
				string i_list_word = list_word[i_list];

				if (LowerCaseRemoveSpaces(non_chinese) != LowerCaseRemoveSpaces(i_list_word))
				{
					double score_LEV = LevenshteinDistance(LowerCaseRemoveSpaces(i_list_word), LowerCaseRemoveSpaces(non_chinese)) / (double)non_chinese.length();
					if (score_LEV <= MAX_LEVENSHTEIN)
					{

						if (line_en_unchanged.find(non_chinese) != string::npos)
							continue;

						int a = LowerCaseRemoveSpaces(non_chinese).length();
						int b = LowerCaseRemoveSpaces(i_list_word).length();
						int long_LEG = max(a, b);
						int short_LEG = min(a, b);
						double score_LEG = (double)(long_LEG - short_LEG) / non_chinese.length();
						a = NumOfSpace(non_chinese);
						b = NumOfSpace(i_list_word);
						long_LEG = max(a, b);
						short_LEG = min(a, b);

						if (WRITE_LOG_FILE)
							m_logger << "score old:" << score_LEG << endl;
						score_LEG = score_LEG * (1.0 + (long_LEG - short_LEG) * NUM_SPACE);
						if (WRITE_LOG_FILE)
							m_logger << "score new:" << score_LEG << endl;

						vector<string> temp(2);
						temp[0] = i_list_word;
						temp[1] = toString(score_LEG + score_LEV);
						word_group_score.push_back(temp);

						if (WRITE_LOG_FILE)
						{

							m_logger << "non_chinese:" << non_chinese << endl;
							m_logger << "i_list_word:" << i_list_word << endl;
							m_logger << "score:" << word_group_score[count_word_group_score][1] << endl
									 << endl;
						}
						count_word_group_score++;
					}
				}
			}
		}
		sort(word_group_score.begin(), word_group_score.end(), cmp1);

		if (word_group_score.size() >= 1)
		{
			// string_replace(word_group_score[0][0], "\xEF", "");
			if (WRITE_LOG_FILE)
			{
				m_logger << "last non_chinese:" << non_chinese << endl;
				m_logger << "last word_group_score[0][0]:" << word_group_score[0][0] << endl;
			}
			if (_lang == "zh2en")
			{
				string_replace(line_en, word_group_score[0][0], non_chinese);
			}
			if (_lang == "en2zh")
			{
				string_replace(line_zh, non_chinese, word_group_score[0][0]);
			}
		}
	}
}

} // namespace post_processor