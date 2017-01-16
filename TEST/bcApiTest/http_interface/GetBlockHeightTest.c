#include "common.h"
#include "interface.h"

typedef struct
{
	char 	blockhash[60];
	char 	result[150];
	char 	expect_result[150];
}TestBlockHashHeightCase;


TestBlockHashHeightCase test_block_hash_height_case[] = {
	{
		"\"d0568a2d97d6caa64a89c514e74ed3dc0cf78b540a14af2ec929dcbb1239\"", 
		"",
		"{\"result\":null,\"error\":{\"code\":-32700,\"message\":\"Parse error\"},\"id\":null}"
	},
	{
		"\"2312312312312111110\"", 
		"",
		"{\"result\":null,\"error\":{\"code\":-8,\"message\":\"parameter 1 must be hexadecimal string (not '2312312312312111110')\"},\"id\":1}",
	},
	{
		"-1", 
		"",
		"{\"result\":null,\"error\":{\"code\":-1,\"message\":\"JSON value is not a string as expected\"},\"id\":1}"
	}
};


int test_base_by_blockheight(char *url)
{
	int i = 0;
	int ret = 0;
	char sendurl[1024];
	char *res = NULL;
	
	sprintf(sendurl, "%s/GetBlockHeight", url);
	
	for(i = 0; i < sizeof(test_block_hash_height_case)/sizeof(test_block_hash_height_case[0]); i++)
	{
		res = http_post(sendurl, test_block_hash_height_case[i].blockhash);
		if(res != NULL)
		{
			//printf("GetBlockHashByHe recv---> %s\n\n\n", res);
			sprintf(test_block_hash_height_case[i].result, "%s", res);
			free(res);
		}
		else
		{
			ret = -1;
		}
	}

	for(i = 0; i < sizeof(test_block_hash_height_case)/sizeof(test_block_hash_height_case[0]); i++)
	{
		if(strncmp(test_block_hash_height_case[i].result, test_block_hash_height_case[i].expect_result, strlen(test_block_hash_height_case[i].result) - 1) != 0)
		{
			printf("0 %s\n",test_block_hash_height_case[i].blockhash);
			printf("1 %s\n",test_block_hash_height_case[i].result);
			printf("2 %s\n",test_block_hash_height_case[i].expect_result);
			ret = -1;
			break;
		}
	}
	return ret;


}

int test_batch_by_blockheight(char *url)
{
	int i = 0,ret = 0;
	char sendurl[1024];
	json_object *post_data = NULL;
	char *res = NULL;

	sprintf(sendurl, "%s/GetBlockHeight", url);
	for(i = 0; i < global_block_info.blockSum; i++)
	{
		res = http_post(sendurl, global_block_info.BlockChainInfo[i].blockHash);
		if(res != NULL)
		{	
			//printf("%s\n",res);
			json_object *json_obj = NULL;
			json_obj = json_tokener_parse(res);
			if(json_obj != NULL)
			{
				json_object_object_foreach(json_obj, key0, val0)
				{
					if(strncasecmp(key0, "result", strlen("result")) == 0)
					{
						if(json_type_object == json_object_get_type(val0))
						{
							json_object_object_foreach(val0, key1, val1)
							{
								if(strncasecmp(key1, "blockHeight", strlen("blockHeight")) == 0)
								{
									if(atol(global_block_info.BlockChainInfo[i].blockHeight) != atol(json_object_to_json_string(val1)))
									{
										printf("%s blockHeight %s\n",global_block_info.BlockChainInfo[i].blockHeight,json_object_to_json_string(val1));
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
			break;
		}
	}
	return ret;
}



int GetBlockHeightTest(char *url)
{
	int ret = 0;
	ret = test_batch_by_blockheight(url);
	if(ret == -1)
		return ret;	

	ret = test_base_by_blockheight(url);
	if(ret == -1)
		return ret;	
	return ret;
}
