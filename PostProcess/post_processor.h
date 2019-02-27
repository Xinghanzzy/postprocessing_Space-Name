#ifndef _DECODER_POST_PROCESSOR_H_
#define _DECODER_POST_PROCESSOR_H_
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

namespace post_processor
{


	class PostProcessor
	{
	public:
		PostProcessor(const std::string & lang): _lang(lang){};
		~PostProcessor(){ if (m_logger.is_open()) m_logger.close(); };
		bool Init(const char* config);

		std::string Process(const std::string & src, const std::string & tgt);

	private:
		int LevenshteinDistance(const std::string &source, const std::string &target);
		std::string do_strip(const std::string &str, int striptype, const std::string &chars);
		std::string strip(const std::string &str, const std::string &chars = " ");
		std::string lstrip(const std::string &str, const std::string &chars = " ");
		std::string rstrip(const std::string &str, const std::string &chars = " ");
		void string_replace(std::string &s1, const std::string &s2, const std::string &s3);
		std::string LowerCaseRemoveSpaces(const std::string &str);
		int LCSLength(const char *la, const char *lb);
		void split(const std::string &s, const std::string &delim, std::vector<std::string> *ret);
		bool Split(const std::string &input, const std::string &sep, std::vector<std::string> &vec);
		void ReplaceUserdict(std::string &line);
		int NumOfSpace(const std::string &s);
		void ExtractAllEnglish(const std::string &line_zh, std::vector<std::string> & list_notzh);
		void CompleteMatch(const std::string &en_processed, const std::string &zh_processed, const std::string &non_chinese, std::string &line_en, std::string &line_zh);
		void NoPuncMatch(const std::string &en_processed, const std::string &zh_processed_without_punc, const std::string &non_chinese_without_punc, std::string &line_en, std::string &line_zh);
		void IncompleteMatch(const std::string &en_processed, const std::string &zh_processed, const std::string &non_chinese, std::string &line_en, std::string &line_zh, const std::string &line_en_unchanged);

		template <typename T>
		std::string toString(const T &t)
		{
			std::ostringstream oss; 
			oss << t;          
			return oss.str();
		}

		template <class Type>
		Type stringToNum(const std::string &str)
		{
			std::istringstream iss(str);
			Type num;
			iss >> num;
			return num;
		}

	private:
		double MIN_LCS;
		double NUM_SPACE;
		double MAX_LEVENSHTEIN;
		double MIN_WORD_LENGTH;
		bool WRITE_LOG_FILE;
		const std::string _lang;
		std::ofstream m_logger;

	};
}

#endif