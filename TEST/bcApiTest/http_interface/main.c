#include "common.h"
#include "interface.h"

int main(int argc, char *argv[])
{
	int err = -1;
	char *url = NULL;
	
	
	printf("**************************************************\n");
	printf("\tBitCoin INTERFACE API TEST CASE\t\t\n");
	printf("**************************************************\n");

	if(argc != 2)
	{
		printf("input parameter such as 'http://192.168.1.140:8332'\n");
		exit(0);
	}
	else
	{
		url = argv[1];
	}
	init_check();
	
	err = GetNewKeyTest(url);
	check(&err,"GetNewKeyTest");
	
	err = SendDataForSignTest(url);
	check(&err,"SendDataForSignTest");
	
	err = SendDataToSysTest(url);
	check(&err,"SendDataToSysTest");

	err = GetDataFromSysTest(url);
	check(&err,"GetDataFromSysTest");

	err = GetDataListTest(url);
	check(&err,"GetDataListTest");

	err = GetBlockListTest(url);
	check(&err,"GetBlockListTest");

	err = GetDataLastNewTest(url);
	check(&err,"GetDataLastNewTest");
	
	err = GetBlockByDateTest(url);
	check(&err,"GetBlockByDateTest");

	
	err = GetNoteStatusTest(url);
	check(&err,"GetNoteStatusTest");
	
	err = GetBlockDetailTest(url);
	check(&err,"GetBlockDetailTest");

	
	err = GetBlockHashByTxTest(url);
	check(&err,"GetBlockHashByTxTest");

	err = GetBlockHeightTest(url);
	check(&err,"GetBlockHeightTest");

	err = GetBlockHashByHeTest(url);
	check(&err,"GetBlockHashByHeTest");
	

	printf("**************************************************\n");	
	if (test_failed_num != 0)	
	{		
		int i = 0;
		printf("There are %d Number of tests failed\n", test_failed_num);
		for (; i < test_failed_num; i++)
		{
			printf("%s\n", test_failed_list[i]);
		}
	}	
	else	
	{		
		printf("ALL %d test case SUCCESS\n",test_success_num);	
	}	
	printf("**************************************************\n");
	destory_check();
	return 0;
}

