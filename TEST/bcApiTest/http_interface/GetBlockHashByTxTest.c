#include "common.h"
#include "interface.h"

typedef struct
{
	char 	blockhash[160];
	char 	result[150];
	char 	expect_result[150];
}TestBlockHashByTXCase;


TestBlockHashByTXCase test_blockhash_by_tx[] = {
	{
		"\"1234ecfe07665b74bbb3d0133d42dbbbc4e0dd28ec7d83628e8b68113d908b3c\"", 
		"",
		"{\"result\":null,\"error\":{\"code\":-5,\"message\":\"No information avaliable about the data you want get it\"},\"id\":1}"
	},
	{
		"\"-1\"", 
		"",
		"{\"result\":null,\"error\":{\"code\":-8,\"message\":\"parameter 1 must be hexadecimal string (not '-1')\"},\"id\":1}"
	},
	{
		"-1", 
		"",
		"{\"result\":null,\"error\":{\"code\":-1,\"message\":\"JSON value is not a string as expected\"},\"id\":1}"
	},
	{
		"\"0000ecfe07665b74bbb3d0133d42dbbbc4e0dd28ec7d83628e8b68113d908b3cAA\"", 
		"",
		"{\"result\":null,\"error\":{\"code\":-5,\"message\":\"No information avaliable about the data you want get it\"},\"id\":1}"
	}		

};


int test_base_blockhash_by_tx(char *url)
{

	int i = 0;
	int ret = 0;
	char sendurl[1024];
	char *res = NULL;

	sprintf(sendurl, "%s/GetBlockHashByTx", url);

	for(i = 0; i < sizeof(test_blockhash_by_tx)/sizeof(test_blockhash_by_tx[0]); i++)
	{
		res = http_post(sendurl, test_blockhash_by_tx[i].blockhash);
		if(res != NULL)
		{
			//printf("GetBlockHashByHe recv---> %s\n\n\n", res);
			sprintf(test_blockhash_by_tx[i].result, "%s", res);
			free(res);
		}
		else
		{
			ret = -1;
		}
	}

	for(i = 0; i < sizeof(test_blockhash_by_tx)/sizeof(test_blockhash_by_tx[0]); i++)
	{
		if(strncmp(test_blockhash_by_tx[i].result, test_blockhash_by_tx[i].expect_result, strlen(test_blockhash_by_tx[i].result) - 1) != 0)
		{
			printf("0 %s\n",test_blockhash_by_tx[i].blockhash);
			printf("1 %s\n",test_blockhash_by_tx[i].result);
			printf("2 %s\n",test_blockhash_by_tx[i].expect_result);
			ret = -1;
			break;
		}
	}
	return ret;


}


int test_batch_blockhash_by_tx(char *url)
{
	int i = 0,ret = 0;
	char sendurl[1024];
	json_object *post_data = NULL;
	char *res = NULL;
	char post_str[1024]; 

	sprintf(sendurl, "%s/GetBlockHashByTx", url);
	for(i = 0; i < global_data_set.result_num; i++)
	{
		sprintf(post_str, "%s", global_data_set.dataResSet[i].dataHash);
		res = http_post(sendurl,post_str);
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
						
						if(json_type_string == json_object_get_type(val0))
						{
							//printf("dataHash %s\n", global_data_set.dataResSet[i].dataHash);
							//printf("blockHash %s\n",json_object_to_json_string(val0));
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

int GetBlockHashByTxTest(char *url)
{
	int ret = 0;
	ret = test_batch_blockhash_by_tx(url);
	if(ret == -1)
		return ret;

	ret = test_base_blockhash_by_tx(url);
	if(ret == -1)
		return ret;
	return ret;
}
