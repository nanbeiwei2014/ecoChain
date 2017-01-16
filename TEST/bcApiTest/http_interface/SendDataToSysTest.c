#include "common.h"
#include "interface.h"



int test_batch_datasign(char *url)
{
	int i = 0,ret = 0;
	char sendurl[1024];
	json_object *post_data = NULL;
	char *res = NULL;

	sprintf(sendurl, "%s/send_data_to_sys", url);
	for(i = 0; i < global_data_set.result_num; i++)
	{
		post_data = json_object_new_object();
		if(post_data == NULL) 
		{
			return -1;
		}
		json_object_object_add(post_data, "publicKey", json_object_new_string_len(global_data_set.dataResSet[i].pubkey + 1, strlen(global_data_set.dataResSet[i].pubkey) - 2));
		json_object_object_add(post_data, "data", json_object_new_string(global_data_set.dataResSet[i].data));
		json_object_object_add(post_data, "sign", json_object_new_string_len(global_data_set.dataResSet[i].sign + 1, strlen(global_data_set.dataResSet[i].sign) - 2));
		res = http_post(sendurl, json_object_to_json_string(post_data));
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
							//printf("dataHash %s\n",json_object_to_json_string(val0));
							sprintf(global_data_set.dataResSet[i].dataHash,"%s",json_object_to_json_string(val0));
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
		json_object_put(post_data); 
	}
	return ret;
}




typedef struct
{
	char 	PubKey[60];
	char 	Data[512];
	char 	sign[1024];
	char 	hash[1024];
	char 	expect_hash1[1024];
	char 	expect_hash2[1024];
}TestDataSignCase;

TestDataSignCase test_datasign_case[] = {
	{
		"yGv9fkig2WvW4JUFVhUVQnpBDFa1Aid8Z7jcaefYKLbV",
		"123456",
		"AN1rKvt1rYWu9Q7DB6MKe3JGktAma6wXypC5m4zowNWFs1iUXsxehQYHpasscwvKLT8qtamV2xdgWep2M8Qz4dosdomfRXqJ1",
		"",
		"{\"result\":\"d37e24518877133ce88db3bcc77cac629f2d34f00f6ca8cdd6b06aad734c8639\",\"error\":null,\"id\":1}",
		"{\"result\":false,\"error\":null,\"id\":1}"
	},
	{
		"abcdfkig2WvW4JUFVhUVQnpBDFa1Aid8Z7jcaefYKLbV",
		"123456",
		"AN1rKvt1rYWu9Q7DB6MKe3JGktAma6wXypC5m4zowNWFs1iUXsxehQYHpasscwvKLT8qtamV2xdgWep2M8Qz4dosdomfRXqJ1",
		"",
		"{\"result\":{\"result\":\"data verify failure!\"},\"error\":null,\"id\":1}",
		""
	},
	{
		"yGv9fkig2WvW4JUFVhUVQnpBDFa1Aid8Z7jcaefYKLbV",
		"abc456",
		"AN1rKvt1rYWu9Q7DB6MKe3JGktAma6wXypC5m4zowNWFs1iUXsxehQYHpasscwvKLT8qtamV2xdgWep2M8Qz4dosdomfRXqJ1",
		"",
		"{\"result\":{\"result\":\"data verify failure!\"},\"error\":null,\"id\":1}",
		""
	},
	{
		"yGv9fkig2WvW4JUFVhUVQnpBDFa1Aid8Z7jcaefYKLbV",
		"123456",
		"abcrKvt1rYWu9Q7DB6MKe3JGktAma6wXypC5m4zowNWFs1iUXsxehQYHpasscwvKLT8qtamV2xdgWep2M8Qz4dosdomfRXqJ1",
		"",
		"{\"result\":{\"result\":\"data verify failure!\"},\"error\":null,\"id\":1}",
		""
	}
};

int test_base_datasign(char *url)
{
	int ret = 0,i = 0;
	json_object *post_data = NULL;
	char *res = NULL;

	char sendurl[1024];
	sprintf(sendurl, "%s/send_data_to_sys", url);

	for(i = 0; i < sizeof(test_datasign_case)/sizeof(test_datasign_case[0]); i++)
	{
		post_data = json_object_new_object();
		if(post_data == NULL) 
		{
			return -1;
		}
		json_object_object_add(post_data, "publicKey", json_object_new_string(test_datasign_case[i].PubKey));
		json_object_object_add(post_data, "data", json_object_new_string(test_datasign_case[i].Data));
		json_object_object_add(post_data, "sign", json_object_new_string(test_datasign_case[i].sign));
		//printf("send_data_to_sys %s\n",json_object_to_json_string(post_data));
		res = http_post(sendurl, json_object_to_json_string(post_data));
		if(res != NULL)
		{
			//printf("test_batch_datasign send---> %s\n\n\n", json_object_to_json_string(post_data));
			//printf("test_batch_datasign recv---> %s\n\n\n", res);
			sprintf(test_datasign_case[i].hash, "%s", res);
			free(res);
		}
		else
		{
			ret = -1;
		}
		json_object_put(post_data); 
	}

	for(i = 0; i < sizeof(test_datasign_case)/sizeof(test_datasign_case[0]); i++)
	{
		if(i == 0)
		{
			if(strstr(test_datasign_case[i].hash,"false") != NULL)
			{
				if(strncmp(test_datasign_case[i].hash, test_datasign_case[i].expect_hash2, strlen(test_datasign_case[i].hash) - 1) != 0)
				{
					//printf("%s\n",test_datasign_case[i].hash);
					//printf("%s\n",test_datasign_case[i].expect_hash2);
					ret = -1;
					break;
				}
			}
			else
			{
				if(strncmp(test_datasign_case[i].hash, test_datasign_case[i].expect_hash1, strlen(test_datasign_case[i].hash) - 1) != 0)
				{
					//printf("%s\n",test_datasign_case[i].hash);
					//printf("%s\n",test_datasign_case[i].expect_hash1);
					ret = -1;
					break;
				}
			}
		}
		else
		{
			if(strncmp(test_datasign_case[i].hash, test_datasign_case[i].expect_hash1, strlen(test_datasign_case[i].hash) - 1) != 0)
			{
				//printf("%s\n",test_datasign_case[i].hash);
				//printf("%s\n",test_datasign_case[i].expect_hash1);
				ret = -1;
				break;
			}
		}
	}
	
	return ret;
}


int SendDataToSysTest(char *url)
{
	int ret = 0;
	ret = test_base_datasign(url);
	if(ret == -1)
		return ret;

	ret = test_batch_datasign(url);
	if(ret == -1)
		return ret;
	


	return 0;
}
