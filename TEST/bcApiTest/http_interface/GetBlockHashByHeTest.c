#include "common.h"
#include "interface.h"



int test_batch_blockhashbyheight(char *url)
{
	int i = 0,ret = 0;
	char sendurl[1024];
	json_object *post_data = NULL;
	char *res = NULL;

	sprintf(sendurl, "%s/GetBlockHashByHe", url);
	for(i = 0; i < global_block_info.blockSum; i++)
	{
		res = http_post(sendurl, global_block_info.BlockChainInfo[i].blockHeight);
		if(res != NULL)
		{	
			json_object *json_obj = NULL;
			json_obj = json_tokener_parse(res);
			if(json_obj != NULL)
			{
				json_object_object_foreach(json_obj, key0, val0)
				{
					if(strncasecmp(key0, "result", strlen("result")) == 0)
					{
						if(json_type_string == json_object_get_type(val0))
						{
							if(strncmp(global_block_info.BlockChainInfo[i].blockHash, (char*)json_object_to_json_string(val0), strlen(global_block_info.BlockChainInfo[i].blockHash)) != 0)
							{
								printf("blockhash 1 %s\n",global_block_info.BlockChainInfo[i].blockHash);
								printf("blockhash 2 %s\n",json_object_to_json_string(val0));
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


typedef struct
{
	char 	blockheight[60];
	char 	result[1024];
	char 	expect_result[1024];
}TestBlockHeightCase;


TestBlockHeightCase test_block_height_case[] = {
	{
		"\"0\"", 
		"",
		"{\"result\":null,\"error\":{\"code\":-1,\"message\":\"JSON value is not an integer as expected\"},\"id\":1}"
	},
	{
		"-1", 
		"",
		"{\"result\":null,\"error\":{\"code\":-5,\"message\":\"Block not found\"},\"id\":1}"
	}	

};



int test_base_blockhashbyheight(char *url)
{
	
	int i = 0;
	int ret = 0;
	char sendurl[1024];
	char *res = NULL;
	
	sprintf(sendurl, "%s/GetBlockHashByHe", url);
	
	for(i = 0; i < sizeof(test_block_height_case)/sizeof(test_block_height_case[0]); i++)
	{
		res = http_post(sendurl, test_block_height_case[i].blockheight);
		if(res != NULL)
		{
			//printf("GetBlockHashByHe recv---> %s\n\n\n", res);
			sprintf(test_block_height_case[i].result, "%s", res);
			free(res);
		}
		else
		{
			ret = -1;
		}
	}

	for(i = 0; i < sizeof(test_block_height_case)/sizeof(test_block_height_case[0]); i++)
	{
		if(strncmp(test_block_height_case[i].result, test_block_height_case[i].expect_result, strlen(test_block_height_case[i].result) - 1) != 0)
		{
			printf("%s\n",test_block_height_case[i].result);
			printf("%s\n",test_block_height_case[i].expect_result);
			ret = -1;
			break;
		}
	}
	return ret;
}

int GetBlockHashByHeTest(char *url)
{
	int ret = 0;
	ret = test_batch_blockhashbyheight(url);
	if(ret == -1)
		return ret;	

	ret = test_base_blockhashbyheight(url);
	if(ret == -1)
		return ret;	
	return ret;
}
