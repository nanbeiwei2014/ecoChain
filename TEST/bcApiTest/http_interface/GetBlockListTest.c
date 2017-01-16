#include "common.h"
#include "interface.h"

typedef struct
{
	char	isum[10];
	char 	result[1024];
	char 	expect_result[1024];
}TestBlockInfoCase;


TestBlockInfoCase test_block_info_case[] = {
	{
		"1", 
		"",
		""
	},
	{
		"0", 
		"",
		"{\"result\":null,\"error\":{\"code\":-8,\"message\":\"The Num is out of range\"},\"id\":1}"
	},
	{
		"101", 
		"",
		"{\"result\":null,\"error\":{\"code\":-8,\"message\":\"The Num is out of range\"},\"id\":1}"
	},
	{
		"\"1\"", 
		"",
		"{\"result\":null,\"error\":{\"code\":-1,\"message\":\"JSON value is not an integer as expected\"},\"id\":1}"
	}	
};	


int GetBlockListTest(char *url)
{
	int i = 0;
	int ret = 0;
	char sendurl[1024];
	json_object *post_data = NULL;
	char *res = NULL;
	
	sprintf(sendurl, "%s/GetBlockList", url);
	
	for(i = 0; i < sizeof(test_block_info_case)/sizeof(test_block_info_case[0]); i++)
	{
		res = http_post(sendurl, test_block_info_case[i].isum);
		if(res != NULL)
		{
			sprintf(test_block_info_case[i].result, "%s", res);
			free(res);
		}
		else
		{
			ret = -1;
		}
	}

	for(i = 0; i < sizeof(test_block_info_case)/sizeof(test_block_info_case[0]); i++)
	{
		if(i == 0)
		{
			json_object *json_obj = NULL;
			json_obj = json_tokener_parse(test_block_info_case[i].result);
			if(json_obj != NULL)
			{
				json_object_object_foreach(json_obj, key0, val0)
				{
					if(strncasecmp(key0,"result",strlen("result")) == 0)
					{
						if(json_type_array == json_object_get_type(val0))
						{
							int n = 0,m = 0;
							m = json_object_array_length(val0);
							for(n = 0; n < m; n++)
							{
								json_object *element = json_object_array_get_idx(val0, n);
								json_object_object_foreach(element, key1, val1)
								{
									if(strncasecmp(key1, "height", strlen("height")) == 0)
									{
										//printf("%s %s\n", key1, json_object_to_json_string(val1));
									}
									else if(strncasecmp(key1, "generateTime", strlen("generateTime")) == 0)
									{
										//printf("%s %s\n", key1, json_object_to_json_string(val1));
									}
									else if(strncasecmp(key1, "records", strlen("records")) == 0)
									{
										//printf("%s %s\n", key1, json_object_to_json_string(val1));
									}
									else if(strncasecmp(key1, "blockHash", strlen("blockHash")) == 0)
									{
										//printf("%s %s\n", key1, json_object_to_json_string(val1));
									}
									else if(strncasecmp(key1, "generateNode", strlen("generateNode")) == 0)
									{
										//printf("%s %s\n", key1, json_object_to_json_string(val1));
									}
									else if(strncasecmp(key1, "blockSize", strlen("blockSize")) == 0)
									{
										//printf("%s %s\n", key1, json_object_to_json_string(val1));
									}	
								}
							}
						}
						else
						{
							ret = -1;
						}
						break;
					}
				}
				json_object_put(json_obj);
			}
		}
		else
		{
			if(strncmp(test_block_info_case[i].result, test_block_info_case[i].expect_result, strlen(test_block_info_case[i].result) - 1) != 0)
			{
				printf("%s\n",test_block_info_case[i].result);
				printf("%s\n",test_block_info_case[i].expect_result);
				ret = -1;
				break;
			}
		}
	}
	return ret;

}
