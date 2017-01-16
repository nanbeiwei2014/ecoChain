#include "common.h"
#include "interface.h"

typedef struct
{
	char	*iparam;
	int 	ret;
	int 	expect_ret;
}TestCase;

TestCase testcase[] = {
	{NULL, -100, 0},
	{"\"01234567890123456789012345678901\"", -100, 0},
	{"\"123456\"", -100, -1},
	{"\"0123456789012345678901234567890123456\"", -100, -1},
	{"01234567890123456789012345678901", -100, -1}, //test input  01234567890123456789012345678901
	{"\"012345678901234567890123456789жа\"", -100, -1}
};	


int Test_Base_GetNewKey(char *url)
{
	int i = 0,ret = 0;
	char *ptr = NULL;
	char sendurl[1024];
	char *result = NULL;
	
	sprintf(sendurl, "%s/get_new_key", url);

	for(i = 0; i < sizeof(testcase)/sizeof(testcase[0]); i++)
	{
		result = http_post(sendurl,testcase[i].iparam);
		if(result != NULL)
		{
			//printf("%s\n",result);
			switch(i)
			{
				case 0:
					fprintf(fp, "input: NULL  output:%s\n", result);
					if((ptr = strstr(result,"\"error\":")) != NULL && strncmp(ptr + strlen("\"error\":"), "null", 4) == 0)
					{
						testcase[i].ret = 0;

					}
					else
					{
						testcase[i].ret = -1;
					}
					break;
				case 1:
					fprintf(fp, "input: \"%s\"  output:%s\n", testcase[1].iparam, result);
					if((ptr = strstr(result,"\"error\":")) != NULL && strncmp(ptr+ strlen("\"error\":"),"null",4) == 0)
					{
						testcase[i].ret = 0;

					}
					else
					{
						testcase[i].ret = -1;
					}
					break;
				case 2:
					fprintf(fp, "input: \"%s\"  output:%s\n", testcase[2].iparam, result);
					if((ptr = strstr(result,"\"error\":")) != NULL && strncmp(ptr + strlen("\"error\":"),"null",4) == 0)
					{
						testcase[i].ret = 0;

					}
					else
					{
						testcase[i].ret = -1;
					}
					break;
				case 3:
					fprintf(fp, "input: \"%s\"  output:%s\n", testcase[3].iparam, result);
					if((ptr = strstr(result,"\"error\":")) != NULL && strncmp(ptr + strlen("\"error\":"),"null",4) == 0)
					{
						testcase[i].ret = 0;

					}
					else
					{
						testcase[i].ret = -1;
					}
					break;
				case 4:
					fprintf(fp, "input: 01234567890123456789012345678901  output:%s\n", result);
					if((ptr = strstr(result,"\"error\":")) != NULL && strncmp(ptr + strlen("\"error\":"),"null",4) == 0)
					{
						testcase[i].ret = 0;

					}
					else
					{
						testcase[i].ret = -1;
					}
					break;
				case 5:
					fprintf(fp, "input: \"%s\"  output:%s\n", testcase[5].iparam, result);
					if((ptr = strstr(result,"\"error\":")) != NULL && strncmp(ptr + strlen("\"error\":"),"null",4) == 0)
					{
						testcase[i].ret = 0;

					}
					else
					{
						testcase[i].ret = -1;
					}
					break;
			}
			
			free(result);
			result = NULL;
			
		}
		else
		{
			testcase[i].ret = -1;
			fprintf(fp, "call get_new_key %d return -1\n", i);
		}
		
	}

	for(i = 0; i < sizeof(testcase)/sizeof(testcase[0]); i++)
	{
		if(testcase[i].ret != testcase[i].expect_ret)
		{
			ret = -1;
			break;
		}
	}
	return ret;

}

int test_batch_create_key(char *url)
{
	int i = 0,ret = 0;
	char *ptr = NULL;
	char sendurl[1024];
	char *result = NULL;
	char post_send_data[128];
	
	sprintf(sendurl, "%s/get_new_key", url);

	for(i = 0; i < global_data_set.result_num; i++)
	{
		sprintf(post_send_data,"\"%s\"",global_data_set.dataResSet[i].param_for_create_key);
		result = http_post(sendurl, post_send_data);
		if(result != NULL)
		{
			//printf("%s\n",result);
			json_object *json_obj = NULL;
			json_obj = json_tokener_parse(result);
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
								if(strncasecmp(key1, "privateKey", strlen("privateKey")) == 0)
								{
									//printf("%s %s\n", key1, json_object_to_json_string(val1));
									sprintf(global_data_set.dataResSet[i].priKey, "%s", json_object_to_json_string(val1));
								}
								else if(strncasecmp(key1, "publicKey", strlen("publicKey")) == 0)
								{
									//printf("%s %s\n", key1, json_object_to_json_string(val1));
									sprintf(global_data_set.dataResSet[i].pubkey, "%s", json_object_to_json_string(val1));
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
			
			free(result);
		}
		else
		{
			ret = -1;
		}
	}

	return ret;
}

int GetNewKeyTest(char *url)
{
	int ret = 0;
	ret = Test_Base_GetNewKey(url);
	if(ret == -1)
		return ret;

	ret = test_batch_create_key(url);
	if(ret == -1)
		return ret;

	return ret;
}

