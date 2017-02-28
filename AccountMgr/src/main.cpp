#include "stdio.h"
#include "AccountMgr.h"

int main()
{
    //printf("Hello World!\n");
    
    //启动RPC服务
    StartAccMgrSvr("192.168.101.146", 18332);

    return 0;
}

