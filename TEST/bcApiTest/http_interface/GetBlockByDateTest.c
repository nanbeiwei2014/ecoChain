#include "common.h"
#include "interface.h"

typedef struct
{
	char	date[60];
	char 	*result;
	char 	expect_result[1024];
}TestBlockByDateCase;

TestBlockByDateCase test_block_by_date_case[] = {
	{
		"\"2017-01-10 09:00:00\"",
		NULL,
		""
	},
	{
		"2017-01-09 00:00:00", 
		NULL,
		"{\"result\":null,\"error\":{\"code\":-32700,\"message\":\"Parse error\"},\"id\":null}"
	},
	{
		"\"2017-1-09\"", 
		NULL,
		"{\"result\":null,\"error\":{\"code\":-8,\"message\":\"time format error!\"},\"id\":1}"
	}
};	

int GetBlockByDateTest(char *url)
{
	int i = 0;
	int ret = 0;
	char sendurl[1024];
	char *res = NULL;
	
	sprintf(sendurl, "%s/GetBlockByDate", url);
	
	for(i = 0; i < sizeof(test_block_by_date_case)/sizeof(test_block_by_date_case[0]); i++)
	{
		if(i == 0)
		{
			char sys_time[60];
			getNowTime(sys_time);
			sprintf(test_block_by_date_case[i].date, "\"%s\"", sys_time);
		}
		
		res = http_post(sendurl, test_block_by_date_case[i].date);
		if(res != NULL)
		{
			test_block_by_date_case[i].result = res;
		}
		else
		{
			ret = -1;
		}
	}

	for(i = 0; i < sizeof(test_block_by_date_case)/sizeof(test_block_by_date_case[0]); i++)
	{
		if(i == 0 && test_block_by_date_case[i].result != NULL)
		{
			json_object *json_obj = NULL;
			json_obj = json_tokener_parse(test_block_by_date_case[i].result);
			
			if(json_obj != NULL)
			{
				json_object_object_foreach(json_obj, key0, val0)
				{
					if(strncasecmp(key0, "result", strlen("result")) == 0)
					{
						if(json_type_array == json_object_get_type(val0))
						{
							int n = 0,m = 0;
							m = json_object_array_length(val0);
							global_block_info.blockSum = m;
							
							for(n = 0; n < m; n++)
							{
								//printf("%d --->", n);
								json_object *element = json_object_array_get_idx(val0, n);
								//printf("%s\n", json_object_to_json_string(element));
								json_object_object_foreach(element, key1, val1)
								{
									//printf("%s %s\n", key1, json_object_to_json_string(val1));
									if(strncasecmp(key1, "height", strlen("height")) == 0)
									{
										//printf("%s %s\t", key1, json_object_to_json_string(val1));
										sprintf(global_block_info.BlockChainInfo[n].blockHeight,"%s",json_object_to_json_string(val1));
									}
									else if(strncasecmp(key1, "generateTime", strlen("generateTime")) == 0)
									{
										//printf("%s %s\t", key1, json_object_to_json_string(val1));
										sprintf(global_block_info.BlockChainInfo[n].generateTime,"%s",json_object_to_json_string(val1));
									}
									else if(strncasecmp(key1, "records", strlen("records")) == 0)
									{
										//printf("%s %s\t", key1, json_object_to_json_string(val1));
										sprintf(global_block_info.BlockChainInfo[n].records,"%s",json_object_to_json_string(val1));
									}
									else if(strncasecmp(key1, "generateNode", strlen("generateNode")) == 0)
									{
										//printf("%s %s\t", key1, json_object_to_json_string(val1));
										sprintf(global_block_info.BlockChainInfo[n].generateNode,"%s",json_object_to_json_string(val1));
									}
									else if(strncasecmp(key1, "blockSize", strlen("blockSize")) == 0)
									{
										//printf("%s %s\t", key1, json_object_to_json_string(val1));
										sprintf(global_block_info.BlockChainInfo[n].blockSize,"%s",json_object_to_json_string(val1));
									}
									else if(strncasecmp(key1, "hash", strlen("hash")) == 0)
									{
										//printf("%s %s\n", key1, json_object_to_json_string(val1));
										sprintf(global_block_info.BlockChainInfo[n].blockHash,"%s",json_object_to_json_string(val1));
									}
								}
							}
						}
						else
						{
							ret = -1;
							break;
						}	
					}
				}
				json_object_put(json_obj);
			}
			else
			{
				printf("%s %s\n","json_tokener_parse",test_block_by_date_case[i].result);
				ret = -1;
				break;
			}
		}
		else if(test_block_by_date_case[i].result != NULL)
		{
			if(strncmp(test_block_by_date_case[i].result, test_block_by_date_case[i].expect_result, strlen(test_block_by_date_case[i].result) - 1) != 0)
			{
   				printf("%s\n",test_block_by_date_case[i].result);
   				printf("%s\n",test_block_by_date_case[i].expect_result);
   				ret = -1;
   				break;
	       	}
	   }
	}
	       
   	for(i = 0; i < sizeof(test_block_by_date_case)/sizeof(test_block_by_date_case[0]); i++)
   	{
   		if(test_block_by_date_case[i].result != NULL)
   		{	
   			free(test_block_by_date_case[i].result);
   			test_block_by_date_case[i].result = NULL;
   		}
   	}
	return ret;
}
