#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include "json.h" 

extern char *test_failed_list[1024];
extern int test_failed_num;
extern int test_success_num;
extern FILE  *fp;

int init_check();
void check(int *err, const char *msg);
void destory_check();
void getNowTime(char *systime);
void init_dataset();

typedef struct
{
	char	blockHeight[60];
	char 	generateTime[60];
	char 	records[60];
	char 	generateNode[160];
	char 	blockSize[60];
	char 	blockHash[160];
}BlockChainInfo;

typedef struct
{
	int 	blockSum;
	BlockChainInfo	BlockChainInfo[101];
}BlockInfo;

BlockInfo global_block_info;


typedef struct
{
	char 	param_for_create_key[60];
	char	priKey[60];
	char 	pubkey[60];
	char 	data[160];
	char 	sign[150];
	char 	dataHash[160];
}SingleDataRecord;

typedef struct
{
	int 			 result_num;
	SingleDataRecord dataResSet[100];
}ResultDataSet;

ResultDataSet global_data_set;


#endif  