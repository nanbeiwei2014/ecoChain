#include <iostream>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <random>
#include "interface.h"


typedef struct
{
	bool    use_seed;
	char 	seed[60];
	char	priKey[60];
	char 	pubkey[60];
	char 	data[160];
	char 	sign[150];
	char 	dataHash[160];
}SingleDataRecord;


class TestBatchCase
{
public:
    TestBatchCase(unsigned long test_num,char *url);
    ~TestBatchCase();
	void displayData();
	int BatchGetPriPubKey();
	int BatchGetDataSign();
	int BatchVerifyDataSign();
	int SendDataToNet();
	int RecvDataFromNet();
	
	int AbortBatchTestCase();
	
private:
	//CKeyAndSign *ckey;
	int init_test_case();
	int destory_test_case();

	char m_url[1024];
	unsigned long  dataSetNum;
	unsigned long  auto_create_key;
	unsigned long  seed_create_key;
	SingleDataRecord *dataResSet;
};


