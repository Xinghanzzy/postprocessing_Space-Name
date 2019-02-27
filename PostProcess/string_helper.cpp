/*
 * File: 
 *		string_helper.cpp 
 * Author:
 *		user (email: wangqiangneu@gmail.com)
 * Created Time:
 *		2018-5-28
 * Description:
 *		
 */
#include "string_helper.h"
#include <string.h>
using namespace std;

namespace tool_ns
{
	bool String_Helper::Split(const std::string & input, const std::string & sep, std::vector< std::string > & vec)
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


	bool String_Helper::Split(const std::string & input, const char & sep, std::vector< std::string > & vec)
	{
		std::string::size_type splitPos = input.find(sep);
		std::string::size_type lastSplitPos = 0;
		std::string tempString;
		while (splitPos != std::string::npos)
		{
			tempString = input.substr(lastSplitPos, splitPos - lastSplitPos);
			if (!tempString.empty())
				vec.push_back(tempString);

			lastSplitPos = splitPos + 1;
			splitPos = input.find(sep, lastSplitPos);
		}
		if (lastSplitPos < input.size())
		{
			tempString = input.substr(lastSplitPos);
			vec.push_back(tempString);
		}

		return vec.empty() ? false : true;
	}

	std::string String_Helper::Get_String_From_Vec(const std::vector<std::string> & vec, int beg, int end)
	{
		string output = "";
		for ( size_t i = beg; i < end; ++i ) 
		{
			output += vec.at( i ) + " ";
		}
		Remove_Ending_Space(output);
		return output;
	}

	bool String_Helper::Start_With_Possible_Set(string & str, const set<string> & possible_set)
	{
		if(possible_set.empty() || str.length() == 0)
			return false;
		set<string>::const_iterator ite = possible_set.begin();
		while(ite != possible_set.end())
		{
			string cmp_str = (*ite + " ");
			if(strncmp(str.c_str(), cmp_str.c_str(), cmp_str.length()) == 0)
				return true;
			ite++;
		}
		return false;
	}

	bool String_Helper::Replace_String(std::string & original , const std::string & source_str , const std::string & target_str)
	{
		string::size_type pos = 0;
		string::size_type src_len = source_str.size();
		string::size_type tgt_len = target_str.size();
		while( (pos = original.find(source_str, pos)) != string::npos)
		{
			original.replace(pos, src_len, target_str);
			pos += tgt_len;
		}
		return true;
	}

	int String_Helper::Get_Word_Count(std::string & input , char sep)
	{
		Remove_Ending_Space(input);
		int word_count = 0;
		string::size_type split_pos = input.find(sep);
		string::size_type last_split_pos = 0;

		while (split_pos != string::npos)
		{
			word_count++;
			last_split_pos = split_pos + 1;
			split_pos = input.find(sep, last_split_pos);
		}

		return ++word_count;
	}

	bool String_Helper::Clear_Line(std::string & line)
	{
		string::size_type pos = 0;
		while( ( pos = line.find( "\r", pos ) ) != string::npos )
			line.replace( pos, 1, "" );

		pos = 0;
		while( ( pos = line.find( "\n", pos ) ) != string::npos )
			line.replace( pos, 1, "" );
		return true;
	}


	bool String_Helper::To_Upper(std::string & str)
	{
		for( string::size_type i = 0; i < str.size(); ++i )
		{
			if( islower((unsigned char)str[i]) )
				str.at( i ) = toupper( ( unsigned char )str.at( i ) );
		}

		return true;
	}

	bool String_Helper::To_Lower(std::string & str)
	{
		for (string::size_type i = 0; i < str.size(); ++i)
		{
			if (isupper((unsigned char)str.at(i)))
				str.at(i) = tolower((unsigned char)str.at(i));
		}
		return true;
	}

	bool String_Helper::Remove_Beginning_Space(std::string & str)
	{
		string tmpStr;
		size_t pos = 0;
		for( string::iterator iter = str.begin(); iter != str.end(); ++iter )
		{
			if( *iter != ' ' )
			{
				tmpStr = str.substr( pos, str.length() - pos );
				break;
			}
			else
			{
				++pos;
			}
		}
		str = tmpStr;
		return true;
	}

	bool String_Helper::Remove_Ending_Space(std::string & str)
	{
		if (str != "")
		{
			string tmpStr;
			int pos = (int)str.length() - 1;
			while (pos >= 0 && str[ pos ] == ' ')
				--pos;
			tmpStr = str.substr(0, pos + 1);
			str = tmpStr;
		}
		return true;
	}

	bool String_Helper::Remove_Extra_Space(std::string &input_string, std::string &output_string)
	{
		std::string tmp("");
		char preceded_char = ' ';
		for( string::iterator iter = input_string.begin(); iter != input_string.end(); ++ iter )
		{
			if( *iter == ' ' && preceded_char == ' ' )
			{
				continue;
			} 
			else
			{
				tmp.push_back( *iter );
				preceded_char = *iter;
			}
		}
		output_string = tmp;
		return true;
	}

	int String_Helper::Get_Item_Count(std::string & str, const char sep)
	{
		size_t len = str.length();
		if(len == 0)
			return 0;
		size_t beg = 0;
		size_t end = 0;
		// skip 'sep' at beginning
		size_t index = 0;
		while(index < len && (str[index] == sep))
			index++;
		beg = index;
		
		// skip 'sep' at ending
		index = len -1;
		while(index >= 0 && (str[index] == sep))
			index--;
		end = index;

		if(beg >= len || end < 0)
			return 0;
		// at least 1
		int count = 1;
		for(size_t i=beg+1; i<end; i++)
			if(str[i] == sep)
				count++;
		return count;
	}
}
