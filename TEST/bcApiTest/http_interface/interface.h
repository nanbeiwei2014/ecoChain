#ifndef _BTC_INTERFACE_H
#define _BTC_INTERFACE_H

int GetBlockByDateTest(char *url);
int GetBlockDetailTest(char *url);
int GetBlockHashByHeTest(char *url);
int GetBlockHashByTxTest(char *url);
int GetBlockHeightTest(char *url);
int GetBlockListTest(char *url);
int GetDataLastNewTest(char *url);
int GetDataListTest(char *url);
int GetNewKeyTest(char *url);
int GetNoteStatusTest(char *url);
int SendDataForSignTest(char *url);
int SendDataToSysTest(char *url);

#endif  
