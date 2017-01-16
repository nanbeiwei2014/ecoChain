#include "testbatchcase.h"
using namespace std;
int main(int argc, char *argv[])
{	
	int ret = 0;
	cout<<"**************************************************"<<endl;
	cout<<"\tBlockChain C/C++ INTERFACE API TEST CASE\t\t"<<endl;
	cout<<"**************************************************"<<endl;
	unsigned long test_case_num = 100;
	
	if(argc != 3)
	{
		printf("input test case number, such as 100 http://192.168.100.129:8332\n");
		exit(0);
	}
	else
	{
		test_case_num = atol(argv[1]);
	}
	
	TestBatchCase batch_test(test_case_num,argv[2]);
	
	ret = batch_test.BatchGetPriPubKey();
	if(ret != 0)
	{
		cout<<"BatchGetPriPubKey ERROR"<<endl;
	}
	
	ret = batch_test.BatchGetDataSign();
	if(ret != 0)
	{
		cout<<"BatchGetDataSign ERROR"<<endl;
	}
		
	ret = batch_test.BatchVerifyDataSign();
	if(ret != 0)
	{
		cout<<"BatchVerifyDataSign ERROR"<<endl;
	}
	
	batch_test.displayData();
	
	ret = batch_test.SendDataToNet();
	if(ret != 0)
	{
		cout<<"SendDataToNet ERROR"<<endl;
	}
	
	ret = batch_test.RecvDataFromNet();
	if(ret != 0)
	{
		cout<<"RecvDataFromNet ERROR"<<endl;
	}
	
	ret = batch_test.AbortBatchTestCase();
	if(ret != 0)
	{
		cout<<"AbortBatchTestCase ERROR"<<endl;
	}
	
	cout<<"  INTERFACE API TEST "<<test_case_num<<" NUM"<<endl;
	return 0;
}

