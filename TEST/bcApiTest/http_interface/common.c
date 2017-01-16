#include "common.h"


#define    COLOR_NONE           "\033[0m"
#define    FONT_COLOR_RED       "\033[0;31m"

char *test_failed_list[1024];
int test_failed_num = 0;
int test_success_num = 0;
FILE  *fp = NULL;

#define WLOG_TEST "blockchainApiTest.log"

int init_check()
{
	test_failed_num = 0;
	test_success_num = 0;
	
	memset(test_failed_list, 0, sizeof(test_failed_list));
	memset(&global_block_info, 0, sizeof(global_block_info));

	init_dataset();
	
	fp = fopen(WLOG_TEST, "wb");
	if (fp == NULL)
		return -1;
	return 0;
}

void check(int *err, const char *msg)
{
	assert(msg != NULL);
	if (0 == *err)
	{
		printf("%s......", msg);
		printf(FONT_COLOR_RED"PASS\n"COLOR_NONE);
		test_success_num++;
		fprintf(fp, "%s ... PASS\n", msg);
	}
	else
	{
		printf("%s......", msg);
		printf(FONT_COLOR_RED"FAILED\n"COLOR_NONE);
		if (test_failed_num == sizeof(test_failed_list))
		{
			printf("sizeof(test_failed_list is) too small.\n");
			exit(1);
		}
		fprintf(fp, "%s ... FAILED\n", msg);
		
		test_failed_list[test_failed_num++] = strdup(msg);
		*err = -1;
	}
}

void destory_check()
{
	int i = 0;
	for(i = 0; i < (test_failed_num - 1); i++)
	{
		if(test_failed_list[i] != NULL)
		{
			free(test_failed_list[i]);
		}
	}

	if(fp != NULL)
	{
		fclose(fp);
	}
}


void getNowTime(char *systime)
{
    struct  timespec time;
    clock_gettime(CLOCK_REALTIME, &time);  //获取相对于1970到现在的秒数
    struct tm nowTime;
    localtime_r(&time.tv_sec, &nowTime);

	if(systime != NULL)
	{
    	sprintf(systime, "%04d-%02d-%02d %02d:%02d:%02d", nowTime.tm_year + 1900, nowTime.tm_mon + 1, nowTime.tm_mday, \
      	nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec);
	}
}


void init_dataset()
{
	static int seedsum = 0;
	int i = 0;
	char randomStr[50];
	unsigned int seed = 0;
	time_t now;
	
	memset(&global_data_set, 0, sizeof(global_data_set));
	global_data_set.result_num = 100;

	for(i = 0; i < global_data_set.result_num; i++)
	{
		snprintf(global_data_set.dataResSet[i].param_for_create_key,33,"%p%d%p%s",i + 1, time(&now), i + 2, "qwertyuiopasdfghjklzxcvbnm0123456789");
		//printf("%s\n",global_data_set.dataResSet[i].param_for_create_key);
		sprintf(global_data_set.dataResSet[i].data,"%p%d%p%d",i + 1, time(&now), i + 2, seedsum++);
	}
}

