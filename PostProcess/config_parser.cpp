#include "config_parser.h"
#include "string_helper.h"

#include <string>

#define _CRT_SECURE_NO_WARNINGS
namespace tool_ns
{
	ConfigParser::ConfigParser(const char* config_file)
	{
		for (int i = 0; i < MAX_CONFIG_NUM; i++)
		{
			config_list[i] = NULL;
		}
		config_num = 0;
		strcpy(config_file_name, "");
		init_config(config_file);
	}

	ConfigParser::~ConfigParser()
	{
		this->free_list();
	}

	void ConfigParser::dump_config_list(std::vector<std::string> & config_vec)
	{
		for (int i = 0; i < config_num; i++)
		{
			std::string key = config_list[i]->key;
			config_vec.push_back(key);
			std::string value = config_list[i]->value;
			std::vector<std::string> tmp;
			String_Helper::Split(value, " ", tmp);
			config_vec.insert(config_vec.end(), tmp.begin(), tmp.end());
		}
		return;
	}

	void ConfigParser::dump_config_list()
	{
		for (int i = 0; i < config_num; i++)
		{
			fprintf(stderr, " - %-30s = %+30s\n", config_list[i]->key, config_list[i]->value);
		}
		fprintf(stderr, "\n");
		return;
	}

	//  ok return 0 , failed return -1
	int ConfigParser::init_config(const char *config_file)
	{

		strcpy(config_file_name, config_file);
		FILE *ifile;
		if ((ifile = fopen(config_file, "r")) == NULL)
		{
			fprintf(stderr, "[ERROR]: Config File %s Open Failed!\n", config_file);
			//fclose(ifile);
			exit(1);
		}

		fprintf(stderr, "[INFO] Load Config File From %s!\n", config_file);

		char line[MAX_CONFIG_LENGTH], key[MAX_CONFIG_LENGTH], value[MAX_CONFIG_LENGTH];
		while (fgets(line, 1024, ifile) != NULL)
		{
			if (strlen(line) <= 5 || line[0] == '#')
				continue;
			// note: a potential risk is that when the line is wrong spelled,
			// e.g. param="A" vaue="B", then the real value can not be "B"
			// instead of the value of previous line
			sscanf(line, "param=\"%[^\"]\" value=\"%[^\"]\"", key, value);

			//printf("key : %s , value : %s \n",key,value);
			PUSH_LIST_RET_CODE ret = push_to_list(key, value);
			switch (ret)
			{
			case PUSH_LISH_OK:
				break;
			case PUSH_LIST_NULL:
				fprintf(stderr, "[WARNING] ignore key:%s value:%s due to null pointer \n", key, value);
				break;
			case PUSH_LIST_BEYOND_MAX_CONFIG_NUM:
				fprintf(stderr, "[WARNING] ignore key:%s value:%s due to beyond MAX_CONFIG_NUM:%d \n", key, value, MAX_CONFIG_NUM);
				break;
			case PUSH_LIST_BEYOND_MAX_CONFIG_LENGTH:
				fprintf(stderr, "[WARNING] ignore key:%s value:%s due to beyond MAX_CONFIG_LENGTH:%d \n", key, value, MAX_CONFIG_LENGTH);
				break;
			}
		}

		fclose(ifile);

		fprintf(stderr, "[INFO] Load Config File Done ! \n");
		return 0;
	}

	char* ConfigParser::get_value_by_key(const char * key, bool is_required)
	{
		for (int i = 0; i < MAX_CONFIG_NUM; i++)
		{
			if (config_list[i] == NULL)
				return NULL;

			if (strcmp(config_list[i]->key, key) == 0)
			{
				return config_list[i]->value;
			}
		}
		if (is_required)
		{
			fprintf(stderr, "Lack of key=%s \n", key);
			exit(-1);
		}
		return NULL;
	}

	char* ConfigParser::get_config_file_name()
	{
		return config_file_name;
	}

	bool ConfigParser::reload_config()
	{
		free_list();
		init_config(config_file_name);
		return true;
	}

	bool ConfigParser::free_list()
	{
		for (int i = 0; i < MAX_CONFIG_NUM; i++)
		{
			if (config_list[i] != NULL)
			{
				delete config_list[i];
				config_list[i] = NULL;
			}
		}
		return true;
	}

	PUSH_LIST_RET_CODE ConfigParser::push_to_list(const char *key, char *value)
	{
		if (strlen(key) > MAX_CONFIG_LENGTH - 1 || strlen(value) > MAX_CONFIG_LENGTH - 1)
			return PUSH_LIST_BEYOND_MAX_CONFIG_LENGTH;
		if (config_num >= MAX_CONFIG_NUM - 1)
			return PUSH_LIST_BEYOND_MAX_CONFIG_NUM;

		config_list[config_num] = new S_CONFIG();
		if (config_list[config_num] == NULL)
			return PUSH_LIST_NULL;
		strcpy(config_list[config_num]->key, key);
		strcpy(config_list[config_num]->value, value);
		config_num++;

		return PUSH_LISH_OK;
	}

	bool ConfigParser::get_key(char * key)
	{
		char	temp_key[512];
		char*	p_str = strstr(key, "param=\"");

		if (p_str == NULL || strlen(p_str) < 1)
			return false;

		strcpy(temp_key, p_str + strlen("param=\""));

		if (temp_key[strlen(temp_key) - 1] == '"')
			temp_key[strlen(temp_key) - 1] = '\0';

		strcpy(key, temp_key);
		return true;
	}

	bool ConfigParser::get_value(char * value)
	{
		char	temp_value[512];
		char*	p_str = strstr(value, "value=\"");

		if (p_str == NULL || strlen(p_str) < 1)
			return false;

		strcpy(temp_value, p_str + strlen("value=\""));

		if (temp_value[strlen(temp_value) - 1] == '"')
			temp_value[strlen(temp_value) - 1] = '\0';

		strcpy(value, temp_value);
		return true;
	}

	
}