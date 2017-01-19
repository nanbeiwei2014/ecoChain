/*************************************************************************
    > File Name: testsign.cpp
    > Author: sundongkai
    > Created Time: 2016年12月14日 星期三 10时11分35秒
 ************************************************************************/

#include<iostream>
#include<string.h>
#include<stdio.h>
#include<fstream>
#include "interface.h"
#include "ErrorCode.h"

using namespace std;

int main()
{
	int i = 1;
    //InitCrypt();
	//ofstream fout("result.txt");
    FILE *fp = NULL;
	fp = fopen("lllog_result.txt","a+");
	if ( NULL == fp )
	{
		printf("open file failure!\n");
		return 0;
	}
    char seeds[33];

	char version[10] = {0};
	GetCurrentVersion( version );
	printf("version:%s\n",version);

    char encode[] = "helloworld";
	unsigned char enStr[128] = {0};
	unsigned int  len = 0;

	EncodeBase64( (unsigned char*)encode,strlen(encode),enStr,&len);
    printf("str:%s,len:%d\n",enStr,len);

    unsigned char deStr[128] = {0};
	unsigned int outLen = 0;
	DecodeBase64( (unsigned char*)enStr,len,deStr,&outLen);
	printf("str:%s,len:%d\n\n",deStr,outLen);

	char errmsg[32] = {0};
	while( i<2 )
	{
        sprintf(seeds,"%032x",i);
		fprintf(fp,"seeds:%s\n",seeds);

		char str_prikey[64];// = {0};
		char str_pubkey[64];// = {0};
		int bRet = GetPriPubKey( seeds, str_prikey, str_pubkey,errmsg );
		if ( ERR_CODE_SUCC != bRet )
		{
			fprintf(fp, "errmsg:%s!\n", errmsg);
			//StopCrypt();
			return 0;
		}
    	fprintf(fp,"prikey:%s\n",str_prikey);
		fprintf(fp,"pubkey:%s\n",str_pubkey);
		cout<<"prikey:"<<str_prikey<<endl;
		cout<<"pubkey:"<<str_pubkey<<endl;

		char classdata[64] = {0};
		sprintf( classdata, "hello_%016x", i);

		fprintf( fp, "before sign data:%s\n", classdata);

		char classsign[128] = {0};
		bRet = GetDataSign(str_prikey,classdata, classsign, errmsg);
		if ( ERR_CODE_SUCC != bRet )
		{
			fprintf(fp, "errmsg:%s!\n", errmsg);
			//StopCrypt();
			return 0;
		}

		fprintf(fp,"datasign:%s\n",classsign);
		cout<<"datasign:"<<classsign<<endl;

		fprintf( fp, "after sign data:%s\n", classdata);

		bRet = VerifySign( str_pubkey, classdata, classsign, errmsg );
		if ( ERR_CODE_SUCC != bRet )
		{
			fprintf(fp,"verify sign error\n");
			cout<<"verify sign error"<<endl<<endl;
		}
		else
		{
			fprintf(fp,"verify sign success\n\n");
			cout<<"verify sign success"<<endl<<endl;
		}
        fflush(fp);
		i++;
	}

	fclose(fp);
	//StopCrypt();

	return 0;
}
