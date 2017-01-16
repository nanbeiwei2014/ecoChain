#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <string>
using namespace std;

class DataManager
{
public:
	//获取单实例
	static DataManager* instance();
	//释放单实例
	static void release();
	//是否已经实例化
	static bool isInstance();

	DataManager();
	~DataManager();
	
	//bool	getUrl(char *filename);		//
	//bool	postUrl(char *filename);	//


public:
	static DataManager	*m_pInstance;

	int		m_sendCount;
	int		m_recvCount;
	int		m_errorCount;

	bool	m_bIsPrintLog;
};

extern DataManager *g_pGlobal_Data;

#endif //DATAMANAGER_H