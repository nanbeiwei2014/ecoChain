#include "common.h"
#include "interface.h"


typedef struct
{
	char	blockhash[160];
	char 	result[1024];
	char 	expect_result[1024];
}TestBlockDetailInfoCase;


TestBlockDetailInfoCase test_block_detailinfo_case[] = {
	{
		"abc3ae7de0b9a1c18069f1cf4a9f19673ac1e764e578d6a854beea78cfe117f4", 
		"",
		"{\"result\":null,\"error\":{\"code\":-32700,\"message\":\"Parse error\"},\"id\":null}"
	},
	{
		"\"abc3ae7de0b9a1c18069f1cf4a9f19673ac1e764e578d6a854beea78cfe117f4\"", 
		"",
		"{\"result\":null,\"error\":{\"code\":-5,\"message\":\"Block not found\"},\"id\":1}"
	},
	{
		"1", 
		"",
		"{\"result\":null,\"error\":{\"code\":-1,\"message\":\"JSON value is not a string as expected\"},\"id\":1}"
	},
	{
		"\"1\"", 
		"",
		"{\"result\":null,\"error\":{\"code\":-5,\"message\":\"Block not found\"},\"id\":1}"
	}
};	


int test_batch_get_block_detail(char *url)
{
	int i = 0;
	int ret = 0;
	char sendurl[1024];
	json_object *post_data = NULL;
	char *res = NULL;
	
	sprintf(sendurl, "%s/GetBlockDetail", url);
	
	for(i = 0; i < sizeof(test_block_detailinfo_case)/sizeof(test_block_detailinfo_case[0]); i++)
	{
		res = http_post(sendurl, test_block_detailinfo_case[i].blockhash);
		if(res != NULL)
		{
			sprintf(test_block_detailinfo_case[i].result, "%s", res);
			free(res);
		}
		else
		{
			ret = -1;
		}
	}

	for(i = 0; i < sizeof(test_block_detailinfo_case)/sizeof(test_block_detailinfo_case[0]); i++)
	{
		if(strncmp(test_block_detailinfo_case[i].result, test_block_detailinfo_case[i].expect_result, strlen(test_block_detailinfo_case[i].result) - 1) != 0)
		{
			printf("0 %s\n",test_block_detailinfo_case[i].blockhash);
			printf("1 %s\n",test_block_detailinfo_case[i].result);
			printf("2 %s\n",test_block_detailinfo_case[i].expect_result);
			ret = -1;
			break;
		}
	}
	return ret;
}


int test_base_get_block_detail(char *url)
{
	int i = 0;
	int ret = 0;
	char sendurl[1024];
	char *res = NULL;
	char post_send_data[1024];
	
	sprintf(sendurl, "%s/GetBlockDetail", url);

	for(i = 0; i < global_block_info.blockSum; i++)
	{	
		sprintf(post_send_data, "%s", global_block_info.BlockChainInfo[i].blockHash);
		res = http_post(sendurl, post_send_data);
		if(res != NULL)
		{
			json_object *json_obj = NULL;
			json_obj = json_tokener_parse(res);
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
								if(strncasecmp(key1, "blockHeight", strlen("blockHeight")) == 0)
								{
									if(strcmp(global_block_info.BlockChainInfo[i].blockHeight, json_object_to_json_string(val1)) != 0)
									{
										printf("%s %s\n", key1, json_object_to_json_string(val1));
										ret = -1;
										break;
									}
								}
								else if(strncasecmp(key1, "blockSize", strlen("blockSize")) == 0)
								{
									if(strcmp(global_block_info.BlockChainInfo[i].blockSize, json_object_to_json_string(val1)) != 0)
									{
										printf("%s %s\n", key1, json_object_to_json_string(val1));
										ret = -1;
										break;
									}
								}
								else if(strncasecmp(key1, "records", strlen("records")) == 0)
								{
									if(strcmp(global_block_info.BlockChainInfo[i].records, json_object_to_json_string(val1)) != 0)
									{
										printf("%s %s\n", key1, json_object_to_json_string(val1));
										ret = -1;
										break;
									}

								}
								else if(strncasecmp(key1, "generateTime", strlen("generateTime")) == 0)
								{
									if(strcmp(global_block_info.BlockChainInfo[i].generateTime, json_object_to_json_string(val1)) != 0)
									{
										printf("%s %s\n", key1, json_object_to_json_string(val1));
										ret = -1;
										break;
									}

								}
								else if(strncasecmp(key1, "generateNode", strlen("generateNode")) == 0)
								{
									if(strcmp(global_block_info.BlockChainInfo[i].generateNode, json_object_to_json_string(val1)) != 0)
									{
										printf("%s %s\n", key1, json_object_to_json_string(val1));
										ret = -1;
										break;
									}

								}
								else if(strncasecmp(key1, "blockHash", strlen("blockHash")) == 0)
								{
									if(strcmp(global_block_info.BlockChainInfo[i].blockHash, json_object_to_json_string(val1)) != 0)
									{
										printf("%s %s\n", key1, json_object_to_json_string(val1));
										ret = -1;
										break;
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
			
			
			free(res);
		}
		else
		{
			ret = -1;
		}
	}
	return ret;

}


int GetBlockDetailTest(char *url)
{
	int ret = 0;
	ret = test_base_get_block_detail(url);
	if(ret == -1)
		return ret;

	ret = test_batch_get_block_detail(url);
	if(ret == -1)
		return ret;

	return ret;

}
