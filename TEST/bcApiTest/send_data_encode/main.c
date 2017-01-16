#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include "comm.h"
#include "json.h" 
#include "http.h" 


void init_env()
{   
	memset(&global_data_set, 0, sizeof(global_data_set));
	
	sprintf(global_data_set.seed, "%s", "0123456789qwertyuiopasdfghjkl2cd");

	sprintf(global_data_set.dataResSet[0].charset, "%s","ISO-8859-1");
	sprintf(global_data_set.dataResSet[1].charset, "%s","UTF-8");
	sprintf(global_data_set.dataResSet[2].charset, "%s","GBK");
	sprintf(global_data_set.dataResSet[3].charset, "%s","GB2312");

	sprintf(global_data_set.dataResSet[0].data, "%s",CHARSET_ISO);
	sprintf(global_data_set.dataResSet[1].data, "%s",CHARSET_UTF8);
	sprintf(global_data_set.dataResSet[2].data, "%s",CHARSET_GBK);
	sprintf(global_data_set.dataResSet[3].data, "%s",CHARSET_GB2312);

	global_data_set.dataResSet[0].date_length = strlen(global_data_set.dataResSet[0].data);
	global_data_set.dataResSet[1].date_length = strlen(global_data_set.dataResSet[1].data);
	global_data_set.dataResSet[2].date_length = strlen(global_data_set.dataResSet[2].data);
	global_data_set.dataResSet[3].date_length = strlen(global_data_set.dataResSet[3].data);

	//ckey = new CKeyAndSign();
	
}

void destory_env()
{
	/*
	if(ckey != NULL)
	{
		delete ckey;
		ckey = NULL;
	}*/
}

int get_pripub_key_from_server(char *url)
{
	int ret = 0;
	char sendurl[1024];
	char *result = NULL;
	char post_seed[60];
	
	sprintf(post_seed, "\"%s\"", global_data_set.seed);
	sprintf(sendurl, "%s/get_new_key", url);
	result = http_post(sendurl, "UTF-8", strlen(post_seed), post_seed);
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
								sprintf(global_data_set.priKey, "%s", json_object_to_json_string(val1));
								//printf("%s\n",global_data_set.priKey);
							}
							else if(strncasecmp(key1, "publicKey", strlen("publicKey")) == 0)
							{
								sprintf(global_data_set.pubkey, "%s", json_object_to_json_string(val1));
								//printf("%s\n",global_data_set.pubkey);
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
	return ret;
}

int get_pripub_key_from_local()
{
	//string pri_key;
	//string pub_key;
	char pri_key[120];
	char pub_key[120];
	char err_msg[1024];
	string seed(global_data_set.seed);
	//if(ckey->GetPriPubKey(seed, pri_key, pub_key) == false)
	if(GetPriPubKey(global_data_set.seed, pri_key, pub_key,err_msg) != 0)
	{
		return -1;
	}
	else
	{
		//sprintf(global_data_set.priKey, "%s", pri_key.c_str());
		//sprintf(global_data_set.pubkey, "%s", pub_key.c_str());	
		sprintf(global_data_set.priKey, "%s", pri_key);
		sprintf(global_data_set.pubkey, "%s", pub_key);	
	}
	return 0;
}

int get_sgin_from_server(char *url)
{
	int ret = 0,i = 0;
	char sendurl[1024];
	char *result = NULL;
	
	sprintf(sendurl, "%s/send_data_for_sign", url);

	for(i = 0; i < TEST_CASE_DATA_SIZE; i++)
	{
		json_object *post_data = NULL;
		post_data = json_object_new_object();
		if(post_data == NULL) 
		{
			return -1;
		}
		json_object_object_add(post_data, "privateKey", json_object_new_string_len(global_data_set.priKey + 1, strlen(global_data_set.priKey) - 2));
		json_object_object_add(post_data, "publicKey", json_object_new_string_len(global_data_set.pubkey + 1, strlen(global_data_set.priKey) - 2));
		json_object_object_add(post_data, "data", json_object_new_string(global_data_set.dataResSet[i].data));

		char *post_str = (char*)json_object_to_json_string(post_data);

		printf("%s\n",post_str);
		result = http_post(sendurl, "UTF-8",strlen(post_str),post_str);
		if(result != NULL)
		{
			printf("send_data_for_sign recv---> %s\n", result);
			json_object *json_obj = NULL;
			json_obj = json_tokener_parse(result);
			if(json_obj != NULL)
			{
				json_object_object_foreach(json_obj, key0, val0)
				{
					if(strncasecmp(key0, "result", strlen("result")) == 0)
					{
						
						if(json_type_string == json_object_get_type(val0))
						{
							printf("sign %s\n",json_object_to_json_string(val0));
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
			free(result);
		}
		else
		{
			json_object_put(post_data); 
			return -1;
		}
		json_object_put(post_data); 
	}
	return ret;

}


int get_sgin_from_local()
{
	int i = 0;
	//string sign;
	string pri_key(global_data_set.priKey);
	char sign[512];
	char err_msg[1024];
	
	for(i = 0; i< TEST_CASE_DATA_SIZE; i++)
	{	
		string data(global_data_set.dataResSet[i].data);
		//sign = ckey->GetDataSign(pri_key, data);
		//if(sign.empty())
		if(GetDataSign(global_data_set.priKey, global_data_set.dataResSet[i].data,sign, err_msg) != 0)
		{
			return -1;
		}
		else
		{
			sprintf(global_data_set.dataResSet[i].sign, "%s", sign);
		}
	}
	return 0;
}

int send_data_to_server(char *url)
{
	int i = 0,ret = 0;
	char sendurl[1024];
	json_object *post_data = NULL;
	char *res = NULL;
	char *post_str = NULL;
	
	sprintf(sendurl, "%s/send_data_to_sys", url);
	for(i = 0; i< TEST_CASE_DATA_SIZE; i++)
	//for(i = 0; i< 1; i++)
	{
		post_data = json_object_new_object();
		if(post_data == NULL) 
		{
			return -1;
		}

		json_object_object_add(post_data, "publicKey", json_object_new_string(global_data_set.pubkey));
		json_object_object_add(post_data, "data", json_object_new_string(global_data_set.dataResSet[i].data));
		json_object_object_add(post_data, "sign", json_object_new_string(global_data_set.dataResSet[i].sign));
		post_str = (char*)json_object_to_json_string(post_data);
		printf("%s\n",post_str);
		res = http_post(sendurl,global_data_set.dataResSet[i].charset,strlen(post_str),post_str);
		if(res != NULL)
		{
			//printf("%s \n",res);
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
							printf("%s \n",res);
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


int recv_data_to_server(char *url)
{
	int i = 0;
	int ret = 0;
	char *res = NULL;
	char sendurl[1024];
	
	sprintf(sendurl, "%s/get_data_from_sys", url);

	for(i = 0; i< TEST_CASE_DATA_SIZE; i++)
	//for(i = 0; i< 1; i++)
	{
		res = http_post(sendurl,"UTF-8",strlen(global_data_set.dataResSet[i].dataHash),global_data_set.dataResSet[i].dataHash);
		if(res != NULL)
		{
			//printf( "%s", res);
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
								if(strncasecmp(key1, "publicKey", strlen("publicKey")) == 0)
								{	
									if(strncmp(global_data_set.pubkey, json_object_to_json_string(val1), strlen(global_data_set.pubkey)) == 0)
									{
										printf("ERROR %s %s\n", key1, json_object_to_json_string(val1));
										ret = -1;
									}
								}
								else if(strncasecmp(key1, "data", strlen("data")) == 0)
								{
									if(strncmp(global_data_set.dataResSet[i].data, json_object_to_json_string(val1), strlen(global_data_set.dataResSet[i].data)) == 0)
									{
										printf("ERROR %s %s\n", key1, json_object_to_json_string(val1));
										ret = -1;
									}
								}
								else if(strncasecmp(key1, "sign", strlen("sign")) == 0)
								{
									if(strncmp(global_data_set.dataResSet[i].sign, json_object_to_json_string(val1), strlen(global_data_set.dataResSet[i].sign)) == 0)
									{
										printf("ERROR %s %s\n", key1, json_object_to_json_string(val1));
										ret = -1;
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


int main(int argc, char *argv[])
{
	printf("INTERFACE API TEST CASE USING BYTES CHARSET\n");
 	int err = -1;
 	char *url = NULL;
 	int on = 0;
 	if(argc != 3)
 	{
 		printf("input parameter such as '0 http://192.168.1.140:8332'\n");
		printf("or\n");
		printf("input parameter such as '1 http://192.168.1.140:8332'\n");
 		exit(0);
 	}
 	else
 	{
		on = atoi(argv[1]);
 		url = argv[2];
 	}
 
 	init_env();

	if(on == 0)
	{
	 	err = get_pripub_key_from_server(url);
	 	if(err == -1)
	 	{
	 		printf("ERROR get_pripub_key_from_server\n");
			return -1;
	 	}
		err = get_sgin_from_server(url);
		if(err == -1)
	 	{
	 		printf("ERROR get_sgin_from_server\n");
			return -1;
	 	}
	}
	else
	{
		err = get_pripub_key_from_local();
	 	if(err == -1)
	 	{
	 		printf("ERROR get_pripub_key_from_local\n");
			return -1;
	 	}

		err = get_sgin_from_local();
		if(err == -1)
	 	{
	 		printf("ERROR get_sgin_from_local\n");
			return -1;
	 	}
	}

	err = send_data_to_server(url);
 	if(err == -1)
 	{
 		printf("ERROR send_data_to_server\n");
		return -1;
 	}

	err = recv_data_to_server(url);
 	if(err == -1)
 	{
 		printf("ERROR recv_data_to_server\n");
		return -1;
 	}
	
 	destory_env();
	return 0;
}

