#include "testbatchcase.h"
#include "http.h"
#include "json.h" 
#include <string.h>
int TestBatchCase::init_test_case()
{
	unsigned long i = 0;
	int n = 0;
	if(dataSetNum <= 0)
	{
		cout<<"dataSetNum less or equal 0!"<<endl;
		return -1;
	}
	
	dataResSet = new SingleDataRecord[dataSetNum];
	if(dataResSet == NULL)
	{
		return -1;
	}
	std::default_random_engine generator(time(NULL));  
	std::uniform_int_distribution<int> dis(0,25);

	for(i = 0; i < auto_create_key; i++)
	{
		dataResSet[i].use_seed = true;
		for(n = 0; n < 60; n++)
		{
			dataResSet[i].data[n] = 'a' + dis(generator);
		}
		dataResSet[i].data[n] = '\0';
	}
	for(i = auto_create_key; i < dataSetNum; i++)
	{	
		dataResSet[i].use_seed = false;
		for(n = 0; n < 32; n++)
		{
			dataResSet[i].seed[n] = 'A' + dis(generator);
		}
		dataResSet[i].seed[n] = '\0';

	}
	for(i = auto_create_key; i < dataSetNum; i++)
	{	
		for(n = 0; n < 60; n++)
		{
			dataResSet[i].data[n] = 'a' + dis(generator);
		}
		dataResSet[i].data[n] = '\0';
	}
	
	return 0;
}

int TestBatchCase::destory_test_case()
{
	if(dataResSet != NULL)
	{
		delete[] dataResSet;
		dataResSet = NULL;
	}
	return 0;	
}

void TestBatchCase::displayData()
{
	unsigned long i = 0;
	if(dataSetNum <= 0)
	{
		cout<<"dataSetNum less or equal 0!"<<endl;
		return;
	}
	if(dataResSet != NULL)
	{
		for(i = 0; i < dataSetNum; i++)
		{
			if((dataResSet + i) != NULL)
			{
				cout<<"index:"<<i<<endl;
				if(dataResSet[i].use_seed)
				{
					cout<<"auto create key"<<endl;
				}
				else
				{
					cout<<"seed create key:"<<dataResSet[i].seed<<endl;
				}
				cout<<"privateKey:"<<dataResSet[i].priKey<<endl;
				cout<<"publicKey:"<<dataResSet[i].pubkey<<endl;
				cout<<"data:"<<dataResSet[i].data<<endl;
				cout<<"sign:"<<dataResSet[i].sign<<endl;
				cout<<"________________________________________________________________________"<<endl;
				
			}
		}
	}
}

TestBatchCase::TestBatchCase(unsigned long test_num,char *url)
{
	dataSetNum = test_num;
	if((dataSetNum % 2) == 0)
	{
		auto_create_key = seed_create_key = dataSetNum/2;
	}
	else
	{
		auto_create_key =  dataSetNum/2;
		seed_create_key =  auto_create_key + 1;
	}
	sprintf(m_url, "%s", url);
	init_test_case();
}

TestBatchCase::~TestBatchCase()
{
	destory_test_case();
	//delete ckey;
}

int TestBatchCase::BatchGetPriPubKey()
{
	unsigned long i = 0;
	int ret = 0;
	char pri_key[120];
	char pub_key[120];
	char err_msg[1024];
	
	for(i = 0; i < auto_create_key; i++)
	{
		if(GetPriPubKey(NULL, pri_key, pub_key, err_msg) != 0)
		{
			return -1;
		}
		else
		{	
			sprintf(dataResSet[i].priKey, "%s", pri_key);
			sprintf(dataResSet[i].pubkey, "%s", pub_key);	
		}
	}
	
	for(i = auto_create_key; i < dataSetNum; i++)
	{	
		if(GetPriPubKey(dataResSet[i].seed, pri_key, pub_key, err_msg) != 0)
		{
			return -1;
		}
		else
		{
			sprintf(dataResSet[i].priKey, "%s", pri_key);
			sprintf(dataResSet[i].pubkey, "%s", pub_key);	
		}
	}
	return ret;
}

int TestBatchCase::BatchGetDataSign()
{
	int ret = 0;
	unsigned long i = 0;	
	//string sign;
	char sign[150];
	char err_msg[1024];
	
	for(i = 0; i < auto_create_key; i++)
	{
		if(GetDataSign(dataResSet[i].priKey, dataResSet[i].data, sign, err_msg) != 0)
		{
			return -1;
		}
		else
		{
			sprintf(dataResSet[i].sign, "%s", sign);
		}
	}
	
	for(i = auto_create_key; i < dataSetNum; i++)
	{	
		string pri_key(dataResSet[i].priKey);
		string data(dataResSet[i].data);
		if(GetDataSign(dataResSet[i].priKey, dataResSet[i].data, sign, err_msg) != 0)
		{
			return -1;
		}
		else
		{
			sprintf(dataResSet[i].sign, "%s", sign);
		}
	}
	return ret;
}

int TestBatchCase::BatchVerifyDataSign()
{
	int ret = 0;
	unsigned long i = 0;	
	char err_msg[1024];
	
	for(i = 0; i < auto_create_key; i++)
	{
		string pub_key(dataResSet[i].pubkey);
		string data(dataResSet[i].data);
		string sign(dataResSet[i].sign);

		if(VerifySign(dataResSet[i].pubkey, dataResSet[i].data, dataResSet[i].sign, err_msg) != 0)
		{
			return -1;
		}
	}
	
	for(i = auto_create_key; i < dataSetNum; i++)
	{	
		string pub_key(dataResSet[i].pubkey);
		string data(dataResSet[i].data);
		string sign(dataResSet[i].sign);
		
		if(VerifySign(dataResSet[i].pubkey, dataResSet[i].data, dataResSet[i].sign, err_msg) != 0)
		{
			cout<<"privateKey:"<<dataResSet[i].priKey<<endl;
			cout<<"publicKey:"<<dataResSet[i].pubkey<<endl;
			cout<<"data:"<<dataResSet[i].data<<endl;
			cout<<"sign:"<<dataResSet[i].sign<<endl;
			return -1;
		}
	}	
	return ret;
}

int TestBatchCase::SendDataToNet()
{

	unsigned long i = 0;
	int ret = 0;
	char sendurl[1024];
	json_object *post_data = NULL;
	char *res = NULL;

	sprintf(sendurl, "%s/send_data_to_sys", m_url);
	for(i = 0; i < dataSetNum; i++)
	{
		post_data = json_object_new_object();
		if(post_data == NULL) 
		{
			return -1;
		}
		json_object_object_add(post_data, "publicKey", json_object_new_string(dataResSet[i].pubkey));
		json_object_object_add(post_data, "data", json_object_new_string(dataResSet[i].data));
		json_object_object_add(post_data, "sign", json_object_new_string(dataResSet[i].sign));
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
							sprintf(dataResSet[i].dataHash, "%s", json_object_to_json_string(val0));
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


int TestBatchCase::RecvDataFromNet()
{
	unsigned long i = 0;
	int ret = 0;
	char *res = NULL;
	char sendurl[1024];
	
	sprintf(sendurl, "%s/get_data_from_sys", m_url);

	for(i = 0; i < dataSetNum; i++)
	{
		res = http_post(sendurl, dataResSet[i].dataHash);
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
									if(strncmp(dataResSet[i].pubkey, json_object_to_json_string(val1), strlen(dataResSet[i].pubkey)) == 0)
									{
										printf("ERROR %s %s\n", key1, json_object_to_json_string(val1));
										ret = -1;
									}
								}
								else if(strncasecmp(key1, "data", strlen("data")) == 0)
								{
									if(strncmp(dataResSet[i].data, json_object_to_json_string(val1), strlen(dataResSet[i].data)) == 0)
									{
										printf("ERROR %s %s\n", key1, json_object_to_json_string(val1));
										ret = -1;
									}
								}
								else if(strncasecmp(key1, "sign", strlen("sign")) == 0)
								{
									if(strncmp(dataResSet[i].sign, json_object_to_json_string(val1), strlen(dataResSet[i].sign)) == 0)
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

int TestBatchCase::AbortBatchTestCase()
{
	char seed[120] = "012345678901234567890123456789ab";
	char pri_key[120];
	char pub_key[120];
	char data[150] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	char err_msg[1024];
	
	if(GetPriPubKey(seed, pri_key, pub_key, err_msg) != 0)
	{
		cout<<"GetPriPubKey run error"<<endl;
		return -1;
	}
	
	sprintf(seed, "%s", "012345678901234567890123456789ab");
	if(GetPriPubKey(seed, pri_key, pub_key, err_msg) != 0)
	{
		cout<<"GetPriPubKey run error"<<endl;
		return -1;
	}
	
	char sign[250];
	if(GetDataSign(pri_key, data, sign, err_msg) != 0)
	{
		return -1;
	}
	
	if(VerifySign(pub_key, data, sign, err_msg) != 0)
	{
		cout<<"verfy sign error:"<<endl;
		cout<<"privateKey:"<<pri_key<<endl;
		cout<<"publicKey:"<<pub_key<<endl;
		cout<<"data:"<<data<<endl;
		cout<<"sign:"<<sign<<endl;
		return -1;
	}
		

	sprintf(data, "%s", "aaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	sprintf(pub_key, "%s", "282eKbo7UsipgVVnXTRv5ABpeyTtRq4929ytNyTb4rE4J");
	sprintf(sign, "%s", "381yXZAQMte3k3nZP5ocDJgXGFmrMXQPHo9fGo4DqHWft4BmjUnQVkj78ea1HQJYkSqSpNUwXF7iuxWkNqmZMR5PJf5cKZBV");
	if(VerifySign(pub_key, data, sign, err_msg) != 0)
	{
		cout<<"verfy sign error:"<<endl;
		cout<<"privateKey:"<<pri_key<<endl;
		cout<<"publicKey:"<<pub_key<<endl;
		cout<<"data:"<<data<<endl;
		cout<<"sign:"<<sign<<endl;
		return -1;
	}


	sprintf(data,"%s","aaaaaaaaaaaaaaaaaaaaaaaaaaaA");
	sprintf(pub_key,"%s","282eKbo7UsipgVVnXTRv5ABpeyTtRq4929ytNyTb4rE4J");
	sprintf(sign,"%s","381yXZAQMte3k3nZP5ocDJgXGFmrMXQPHo9fGo4DqHWft4BmjUnQVkj78ea1HQJYkSqSpNUwXF7iuxWkNqmZMR5PJf5cKZBV");
	if(VerifySign(pub_key, data, sign, err_msg) == 0)
	{
		cout<<"verfy sign error:"<<endl;
		cout<<"privateKey:"<<pri_key<<endl;
		cout<<"publicKey:"<<pub_key<<endl;
		cout<<"data:"<<data<<endl;
		cout<<"sign:"<<sign<<endl;
		return -1;
	}	


	sprintf(data,"%s","aaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	sprintf(pub_key,"%s","282eKbo7UsipgVVnXTRv5ABpeyTtRq4929ytNyTb4rE4A");
	sprintf(sign,"%s","381yXZAQMte3k3nZP5ocDJgXGFmrMXQPHo9fGo4DqHWft4BmjUnQVkj78ea1HQJYkSqSpNUwXF7iuxWkNqmZMR5PJf5cKZBV");
	if(VerifySign(pub_key, data, sign, err_msg) == 0)
	{
		cout<<"verfy sign error:"<<endl;
		cout<<"privateKey:"<<pri_key<<endl;
		cout<<"publicKey:"<<pub_key<<endl;
		cout<<"data:"<<data<<endl;
		cout<<"sign:"<<sign<<endl;
		return -1;
	}	

	sprintf(data,"%s","aaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	sprintf(pub_key,"%s","282eKbo7UsipgVVnXTRv5ABpeyTtRq4929ytNyTb4rE4J");
	sprintf(sign,"%s","381yXZAQMte3k3nZP5ocDJgXGFmrMXQPHo9fGo4DqHWft4BmjUnQVkj78ea1HQJYkSqSpNUwXF7iuxWkNqmZMR5PJf5cKZBA");
	if(VerifySign(pub_key, data, sign, err_msg) == 0)
	{
		cout<<"verfy sign error:"<<endl;
		cout<<"privateKey:"<<pri_key<<endl;
		cout<<"publicKey:"<<pub_key<<endl;
		cout<<"data:"<<data<<endl;
		cout<<"sign:"<<sign<<endl;
		return -1;
	}



	sprintf(data,"%s","aaaaaaaaaaaaaaaaaaaaaaaaaaaA");
	sprintf(pub_key,"%s","282eKbo7UsipgVVnXTRv5ABpeyTtRq4929ytNyTb4rE4A");
	sprintf(sign,"%s","381yXZAQMte3k3nZP5ocDJgXGFmrMXQPHo9fGo4DqHWft4BmjUnQVkj78ea1HQJYkSqSpNUwXF7iuxWkNqmZMR5PJf5cKZBA");	
	if(VerifySign(pub_key, data, sign, err_msg) == 0)
	{
		cout<<"verfy sign error:"<<endl;
		cout<<"privateKey:"<<pri_key<<endl;
		cout<<"publicKey:"<<pub_key<<endl;
		cout<<"data:"<<data<<endl;
		cout<<"sign:"<<sign<<endl;
		return -1;
	}
	return 0;
}












