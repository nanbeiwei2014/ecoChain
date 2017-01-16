#include "common.h"
#include "interface.h"

typedef struct
{
	char 	hash[1024];
	char 	result[1024];
	char 	expect_result[1024];
}TestHashDataSignCase;

TestHashDataSignCase test_hash_case[] = {
	{
		"\"d37e24518877133ce88db3bcc77cac629f2d34f00f6ca8cdd6b06aad734c8639\"",
		"",
		"{\"result\":{\"publicKey\":\"yGv9fkig2WvW4JUFVhUVQnpBDFa1Aid8Z7jcaefYKLbV\",\"data\":\"123456\",\"sign\":\"AN1rKvt1rYWu9Q7DB6MKe3JGktAma6wXypC5m4zowNWFs1iUXsxehQYHpasscwvKLT8qtamV2xdgWep2M8Qz4dosdomfRXqJ1\",\"blockHash\":\"0000000000000000000000000000000000000000000000000000000000000000\"},\"error\":null,\"id\":1}"
	},
	{
		"\"abcd24518877133ce88db3bcc77cac629f2d34f00f6ca8cdd6b06aad734c8639\"",
		"",
		"{\"result\":null,\"error\":{\"code\":-5,\"message\":\"No information avaliable about the data you want get it\"},\"id\":1}"
	}
};

int test_batch_hash(char *url)
{
	int ret = 0,i = 0;
	json_object *post_data = NULL;
	char *res = NULL;
	char sendurl[1024];
	
	sprintf(sendurl, "%s/get_data_from_sys", url);

	for(i = 0; i < sizeof(test_hash_case)/sizeof(test_hash_case[0]); i++)
	{
		post_data = json_object_new_object();
		if(post_data == NULL) 
		{
			return -1;
		}
		res = http_post(sendurl, test_hash_case[i].hash);
		if(res != NULL)
		{
			sprintf(test_hash_case[i].result, "%s", res);
			free(res);
		}
		else
		{
			ret = -1;
		}
		json_object_put(post_data); 
	}

	for(i = 0; i < sizeof(test_hash_case)/sizeof(test_hash_case[0]); i++)
	{
		if(strncmp(test_hash_case[i].result, test_hash_case[i].expect_result, strlen(test_hash_case[i].result) - 1) != 0)
		{
			printf("%s\n",test_hash_case[i].result);
			printf("%s\n",test_hash_case[i].expect_result);
			ret = -1;
			break;
		}
	}
	return ret;
}

int GetDataFromSysTest(char *url)
{
	int ret = 0;
	ret = test_batch_hash(url);
	if(ret == -1)
		return ret;
	return ret;
}


