#ifndef _CONFIG_PARSER_H_
#define _CONFIG_PARSER_H_
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
#include <sstream>
#include "string_helper.h"

using namespace tool_ns;
#define MAX_CONFIG_LENGTH 1024
#define MAX_CONFIG_NUM 1024

namespace tool_ns
{
	enum PUSH_LIST_RET_CODE { PUSH_LISH_OK = 0, PUSH_LIST_NULL, PUSH_LIST_BEYOND_MAX_CONFIG_LENGTH, PUSH_LIST_BEYOND_MAX_CONFIG_NUM };

	typedef struct s_config
	{
		char key[MAX_CONFIG_LENGTH];
		char value[MAX_CONFIG_LENGTH];
	}S_CONFIG;

	class ConfigParser
	{
	public:
		ConfigParser(const char* config_file);
		~ConfigParser();
		int init_config(const char* config_file);
		char* get_value_by_key(const char * key, bool is_required);
		bool reload_config();
		char* get_config_file_name();
		void dump_config_list(std::vector<std::string> & config_vec);
		void dump_config_list();

		template <typename T>
		bool get_param(const char* key, const char* defaut, bool is_required, T & value);

		template <typename T>
		bool get_param_vector(const char* key, const char* defaut, bool is_required, std::vector<T> & value);

		template <typename T>
		bool get_param_set(const char* key, const char* defaut, bool is_required, std::set<T> & value);

	private:
		S_CONFIG *config_list[MAX_CONFIG_NUM];
		int config_num;
		char config_file_name[MAX_CONFIG_LENGTH];
	private:
		bool get_key(char * key);
		bool get_value(char * value);
		PUSH_LIST_RET_CODE push_to_list(const char* key, char* value);
		bool free_list();
	};

	template <typename T>
	bool ConfigParser::get_param(const char* key, const char* defaut, bool is_required, T & value)
	{
		std::string paramKey(key);
		std::string paramDefValue(defaut);
		char* value_str = get_value_by_key(key, is_required);
		if (value_str == NULL)
		{
			if (is_required)
			{
				fprintf(stderr, "Lack of param:%s \n", key);
				exit(-1);
			}
			else
				push_to_list(key, (char*)defaut);
		}
		std::istringstream ss;
		ss.str(get_value_by_key(key, is_required));
		ss >> value;
		return true;
	}

	template <typename T>
	bool ConfigParser::get_param_vector(const char* key, const char* defaut, bool is_required, std::vector<T> & value)
	{
		std::string paramKey(key);
		std::string paramDefValue(defaut);
		char* value_str = get_value_by_key(key, is_required);
		if (value_str == NULL)
		{
			if (is_required)
			{
				fprintf(stderr, "Lack of param:%s \n", key);
				exit(-1);
			}
			else
				push_to_list(key, (char*)defaut);
		}
		
		std::vector<std::string> tmp;
		String_Helper::Split(get_value_by_key(key, is_required), " ", tmp);
		value.assign(tmp.size(), -1);
		for (int i = 0; i < tmp.size(); i++)
		{
			std::istringstream ss;
			ss.str(tmp[i]);
			ss >> value[i];
		}
		return true;
	}

	template <typename T>
	bool ConfigParser::get_param_set(const char* key, const char* defaut, bool is_required, std::set<T> & value)
	{
		std::string paramKey(key);
		std::string paramDefValue(defaut);
		char* value_str = get_value_by_key(key, is_required);
		if (value_str == NULL)
		{
			if (is_required)
			{
				fprintf(stderr, "Lack of param:%s \n", key);
				exit(-1);
			}
			else
				push_to_list(key, (char*)defaut);
		}

		std::vector<std::string> tmp;
		String_Helper::Split(get_value_by_key(key, is_required), " ", tmp);
		for (int i = 0; i < tmp.size(); i++)
		{
			std::istringstream ss;
			ss.str(tmp[i]);
			T var;
			ss >> var;
			value.insert(var);
		}
		return true;
	}

}

#endif