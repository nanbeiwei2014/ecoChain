#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include "interface.h"
#include "charset_gb2312.h"
#include "charset_iso.h"
#include "charset_utf8.h"
#include "charset_gbk.h"


#define TEST_CASE_DATA_SIZE  4

typedef struct
{
	char 	charset[30];
	int 	date_length;
	char 	data[160];
	char 	sign[150];
	char 	dataHash[160];
}SingleDataRecord;

typedef struct
{
	char 	seed[60];
	char	priKey[160];
	char 	pubkey[160];
	SingleDataRecord dataResSet[TEST_CASE_DATA_SIZE];
}ResultDataSet;

ResultDataSet global_data_set;

//CKeyAndSign *ckey;

#endif 
