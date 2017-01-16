#include "common.h"
#include "interface.h"

typedef struct
{
	char	isum[10];
	char 	result[1024];
	int 	ret;
	int 	expect_ret;
}TestNewBlockInfoCase;


TestNewBlockInfoCase test_newblock_info_case[] = {
	{
		"", 
		"",
		-1,
		1
	},
	{
		"1", 
		"",
		-1,
		1
	},
	{
		"\"1\"", 
		"",
		-1,
		1
	}
};	



int GetDataLastNewTest(char *url)
{
	int i = 0;
	int ret = 0;
	char sendurl[1024];
	json_object *post_data = NULL;
	char *res = NULL;
	
	sprintf(sendurl, "%s/GetDataLastNew", url);
	
	for(i = 0; i < sizeof(test_newblock_info_case)/sizeof(test_newblock_info_case[0]); i++)
	{
		res = http_post(sendurl, test_newblock_info_case[i].isum);
		if(res != NULL)
		{
			//printf("%s\n",res);
			sprintf(test_newblock_info_case[i].result, "%s", res);
			free(res);
		}
		else
		{
			ret = -1;
		}
	}
	
	for(i = 0; i < sizeof(test_newblock_info_case)/sizeof(test_newblock_info_case[0]); i++)
	{
		json_object *json_obj = NULL;
		json_obj = json_tokener_parse(test_newblock_info_case[i].result);
		if(json_obj != NULL)
		{
			json_object_object_foreach(json_obj, key0, val0)
			{
				if(strncasecmp(key0,"result",strlen("result")) == 0)
				{
					if(json_type_object == json_object_get_type(val0))
					{
						json_object_object_foreach(val0, key1, val1)
						{
							//printf("%s %s\n", key1, json_object_to_json_string(val1));
							if(strncasecmp(key1, "blockNo", strlen("blockNo")) == 0)
							{
								//printf("%s %s\n", key1, json_object_to_json_string(val1));
							}
							else if(strncasecmp(key1, "generateTime", strlen("generateTime")) == 0)
							{
								//printf("%s %s\n", key1, json_object_to_json_string(val1));
							}
						}
						test_newblock_info_case[i].ret = 0;
					}
					else if(json_type_null == json_object_get_type(val0))
					{
						test_newblock_info_case[i].ret = 0;
					}
				}
				else if(strncasecmp(key0,"error",strlen("error")) == 0)
				{
					if(i == 0)
					{
						if(json_type_null == json_object_get_type(val0))
						{
							test_newblock_info_case[i].ret += 1;
						}
					}
					else 
					{
						if(json_type_object == json_object_get_type(val0))
						{
							test_newblock_info_case[i].ret += 1;
						}
					}
				}
			}
			json_object_put(json_obj);
		}
		else
		{
			return -1;
		}
	}

	for(i = 0; i < sizeof(test_newblock_info_case)/sizeof(test_newblock_info_case[0]); i++)
	{
		if(test_newblock_info_case[i].ret != test_newblock_info_case[i].expect_ret)
		{
			ret = -1;
		}
	}
	return ret;
}
