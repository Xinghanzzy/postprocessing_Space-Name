/*
 * File: 
 *		string_helper.h 
 * Author:
 *		user (email: wangqiangneu@gmail.com)
 * Created Time:
 *		2018-5-28
 * Description:
 *		
 */
#ifndef STRING_HELPER_H_
#define STRING_HELPER_H_

#include <string>
#include <vector>
#include <sstream>
#include <set>
namespace tool_ns
{
	class String_Helper
	{
	public:
		static bool Split(const std::string & input, const char & sep, std::vector< std::string > & vec);
		static bool Split(const std::string & input, const std::string & sep, std::vector< std::string > & vec);


	public:
		static std::string Get_String_From_Vec(const std::vector<std::string> & vec, int beg, int end);

		static bool Start_With_Possible_Set(std::string & str, const std::set<std::string> & possible_set);

	public:
		static bool Replace_String(std::string & original , const std::string & source_str , const std::string & target_str);
		static int	Get_Word_Count(std::string & input , char sep);
		static bool Clear_Line(std::string & line);



	public:
		template <typename T>
		static std::string To_String(T & num)
		{
			std::stringstream ss;
			ss << num;
			return ss.str();
		}


	public:
		static bool To_Upper(std::string & str);
		static bool To_Lower(std::string & str);

	public:
		// remove spaces at ending
		static bool Remove_Ending_Space(std::string & str);

		// remove spaces at beginning
		static bool Remove_Beginning_Space(std::string & str);

		// remove spaces between words (only keep one space)
		static bool Remove_Extra_Space(std::string &input_string, std::string &output_string);


	public:
		// return the item number given a string and separator
		static int Get_Item_Count(std::string & str, const char sep);
	};
}



#endif