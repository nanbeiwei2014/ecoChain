#include "common.h"
#include "interface.h"

typedef struct
{
	char	PriKey[60];
	char 	PubKey[60];
	char 	Data[512];
	char 	result[1024];
	char 	expect_result[1024];
}TestSignCase;

TestSignCase test_sign_case[] = {
	{
		"KxqPamrCFAzRNC9CqtWDF5jb9BRy2MW1dNGyKq2VqyM4m1dv1uiR", 
		"yGv9fkig2WvW4JUFVhUVQnpBDFa1Aid8Z7jcaefYKLbV",
		"123456",
		"",
		"{\"result\":\"AN1rKvt1rYWu9Q7DB6MKe3JGktAma6wXypC5m4zowNWFs1iUXsxehQYHpasscwvKLT8qtamV2xdgWep2M8Qz4dosdomfRXqJ1\",\"error\":null,\"id\":1}"
	},
	{
		"abcPamrCFAzRNC9CqtWDF5jb9BRy2MW1dNGyKq2VqyM4m1dv1uiR", 
		"yGv9fkig2WvW4JUFVhUVQnpBDFa1Aid8Z7jcaefYKLbV",
		"123456",
		"",
		"{\"result\":null,\"error\":{\"code\":-5,\"message\":\"Invalid private key\"},\"id\":1}"
	},
	{
		"PamrCFAzRNC9CqtWDF5jb9BRy2MW1dNGyKq2VqyM4m1dv1uiR", 
		"yGv9fkig2WvW4JUFVhUVQnpBDFa1Aid8Z7jcaefYKLbV",
		"123456",
		"",
		"{\"result\":null,\"error\":{\"code\":-5,\"message\":\"Invalid private key\"},\"id\":1}"
	},
	{
		"KxqPamrCFAzRNC9CqtWDF5jb9BRy2MW1dNGyKq2VqyM4m1dv1uiR", 
		"yGv9fkig2WvW4JUFVhUVQnpBDFa1Aid8Z7jcaefYKLbV",
		"123ÖÐ¹ú456",
		"",
		"{\"result\":null,\"error\":{\"code\":-32700,\"message\":\"Parse error\"},\"id\":null}"
	}	
};	


int test_batch_sign(char *url)
{
	int i = 0;
	int ret = 0;
	char sendurl[1024];
	json_object *post_data = NULL;
	char *res = NULL;
	
	sprintf(sendurl, "%s/send_data_for_sign", url);
	
	for(i = 0; i < sizeof(test_sign_case)/sizeof(test_sign_case[0]); i++)
	{
		post_data = json_object_new_object();
		if(post_data == NULL) 
	    {
	        return -1;
		}
		json_object_object_add(post_data, "privateKey", json_object_new_string(test_sign_case[i].PriKey));
		json_object_object_add(post_data, "publicKey", json_object_new_string(test_sign_case[i].PubKey));
		json_object_object_add(post_data, "data", json_object_new_string(test_sign_case[i].Data));
		res = http_post(sendurl, json_object_to_json_string(post_data));
		if(res != NULL)
		{
			//printf("test_batch_sign recv---> %s\n\n\n", res);
			sprintf(test_sign_case[i].result, "%s", res);
			free(res);
		}
		else
		{
			ret = -1;
		}
		json_object_put(post_data); 
	}

	for(i = 0; i < sizeof(test_sign_case)/sizeof(test_sign_case[0]); i++)
	{
		if(strncmp(test_sign_case[i].result, test_sign_case[i].expect_result, strlen(test_sign_case[i].result) - 1) != 0)
		{
			//printf("%s\n",test_sign_case[i].result);
			//printf("%s\n",test_sign_case[i].expect_result);
			ret = -1;
			break;
		}
	}
	return ret;
}

int test_base_sign(char *url)
{
	int ret = -1;
	char sendurl[1024];
	char *result = NULL,*res = NULL;
	char *ptr = NULL;
	json_object *json_obj = NULL;
	json_object *post_data = NULL;
	char sendbuf[1024];
	
	post_data = json_object_new_object();
	if(post_data == NULL) 
    {
        return ret;
	}
	
	sprintf(sendurl, "%s/get_new_key", url);
	result = http_post(sendurl, "\"01234567890123456789012345678abc\"");
	if(result != NULL)
	{
		if((ptr = strstr(result, ":")) != NULL && (ptr = strstr(result, "privateKey")) != NULL && (ptr = strstr(result, "publicKey")) != NULL)
		{
			//printf("%s\n",result);
			json_obj = json_tokener_parse(result);
			if(json_obj != NULL)
			{
				
				json_object_object_foreach(json_obj, key0, val0)
				{
					//printf("%s  %s\n",key0, json_object_to_json_string(val0));
					if(json_type_object == json_object_get_type(val0))
					{
						json_object_object_foreach(val0, key1, val1)
						{
							//printf("%s %s\n",key1,json_object_to_json_string(val1));
							if(strcasecmp(key1,"privateKey") == 0)
							{
								json_object_object_add(post_data,"privateKey",(val1));
							}
							else if(strcasecmp(key1,"publicKey") == 0)
							{
								json_object_object_add(post_data,"publicKey",(val1));
								break;
							}	
						}
						break;
					}
				}
				json_object_object_add(post_data,"data",json_object_new_string("123456"));
				sprintf(sendbuf, "%s", json_object_to_json_string(post_data));
				sprintf(sendurl, "%s/send_data_for_sign", url);
				res = http_post(sendurl, sendbuf);
				if(res != NULL)
				{
					//printf("send---> %s\n",sendbuf);
					//printf("recv---> %s\n\n\n", res);
					free(res);
					json_object_put(post_data); 
					ret = 0;
				}
				else
				{
					ret = -1;
				}
			}
			else
			{
				ret = -1;
			}
		}
		else
		{
			ret = -1;
		}
		
		if(result != NULL)
		{
			free(result);
			result = NULL;
		}
	}
	else
	{
		ret = -1;
	}
	return ret;
}


int create_data_sign(char *url)
{
	int i = 0,ret = 0;
	char sendurl[1024];
	json_object *post_data = NULL;
	char *res = NULL;

	sprintf(sendurl, "%s/send_data_for_sign", url);
	for(i = 0; i < global_data_set.result_num; i++)
	{
		post_data = json_object_new_object();
		if(post_data == NULL) 
	    {
	        return -1;
		}
		json_object_object_add(post_data, "privateKey", json_object_new_string_len(global_data_set.dataResSet[i].priKey + 1, strlen(global_data_set.dataResSet[i].priKey) - 2));
		json_object_object_add(post_data, "publicKey", json_object_new_string_len(global_data_set.dataResSet[i].pubkey + 1, strlen(global_data_set.dataResSet[i].pubkey) - 2));
		json_object_object_add(post_data, "data", json_object_new_string(global_data_set.dataResSet[i].data));
		res = http_post(sendurl, json_object_to_json_string(post_data));
		
		if(res != NULL)
		{
			//printf("%d send_data_for_sign recv---> %s\n\n\n",i, res);
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
							//printf("sign %s\n",json_object_to_json_string(val0));
							sprintf(global_data_set.dataResSet[i].sign,"%s",json_object_to_json_string(val0));
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
		json_object_put(post_data); 
	}
	return ret;
}

int SendDataForSignTest(char *url)
{
	int ret = 0;
	ret = test_base_sign(url);
	if(ret == -1)
		return ret;

	ret = test_batch_sign(url);
	if(ret == -1)
		return ret;

	ret = create_data_sign(url);
	if(ret == -1)
		return ret;

	return ret;
}
