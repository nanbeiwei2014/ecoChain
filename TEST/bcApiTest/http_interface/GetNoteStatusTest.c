#include "common.h"
#include "interface.h"

typedef struct
{
	char	note_id[8];
	char 	result[512];
	char 	expect_result[512];
}TestNoteStatusCase;


TestNoteStatusCase test_note_status_case[] = {
	{
		"0", 
		"null",
		"{\"result\":{\"nodeVersion\":0,\"protVersion\":0,\"linkNode\":0,\"netState\":true,\"blockHeight\":0,\"generateTime\":0},\"error\":null,\"id\":1}"
	},
	{
		"-1",
		"null",
		"{\"result\":null,\"error\":{\"code\":-5,\"message\":\"Nodes can not found\"},\"id\":1}"
	},
	{
		"\"0\"",
		"",
		"{\"result\":null,\"error\":{\"code\":-1,\"message\":\"JSON value is not an integer as expected\"},\"id\":1}"
	}	
};	


int GetNoteStatusTest(char *url)
{
	int i = 0;
	int ret = 0;
	char sendurl[1024];
	json_object *post_data = NULL;
	char *res = NULL;
	
	sprintf(sendurl, "%s/GetNodeStatus", url);
	
	for(i = 0; i < sizeof(test_note_status_case)/sizeof(test_note_status_case[0]); i++)
	{
		res = http_post(sendurl, test_note_status_case[i].note_id);
		if(res != NULL)
		{
			//printf("%s\n",res);
			sprintf(test_note_status_case[i].result, "%s", res);
			free(res);
		}
		else
		{
			ret = -1;
		}
	}

	for(i = 0; i < sizeof(test_note_status_case)/sizeof(test_note_status_case[0]); i++)
	{
		if(i == 0)
		{
			json_object *json_obj = NULL;
			json_obj = json_tokener_parse(test_note_status_case[i].result);
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
								if(strncasecmp(key1, "nodeVersion", strlen("nodeVersion")) == 0)
								{
									//printf("%s %s\n", key1, json_object_to_json_string(val1));
								}
								else if(strncasecmp(key1, "protVersion", strlen("protVersion")) == 0)
								{
									//printf("%s %s\n", key1, json_object_to_json_string(val1));
								}
								else if(strncasecmp(key1, "linkNode", strlen("linkNode")) == 0)
								{
									//printf("%s %s\n", key1, json_object_to_json_string(val1));
								}
								else if(strncasecmp(key1, "netState", strlen("netState")) == 0)
								{
									//printf("%s %s\n", key1, json_object_to_json_string(val1));
								}
								else if(strncasecmp(key1, "blockHeight", strlen("blockHeight")) == 0)
								{
									//printf("%s %s\n", key1, json_object_to_json_string(val1));
								}
								else if(strncasecmp(key1, "generateTime", strlen("generateTime")) == 0)
								{
									//printf("%s %s\n", key1, json_object_to_json_string(val1));
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
		}
		else
		{
			if(strncmp(test_note_status_case[i].result, test_note_status_case[i].expect_result, strlen(test_note_status_case[i].result) - 1) != 0)
			{
				printf("1 %s\n",test_note_status_case[i].result);
				printf("2 %s\n",test_note_status_case[i].expect_result);
				ret = -1;
				break;
			}
		}
	}
	

	return ret;
}
