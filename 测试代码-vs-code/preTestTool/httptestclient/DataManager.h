#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <string>
using namespace std;

class DataManager
{
public:
	//��ȡ��ʵ��
	static DataManager* instance();
	//�ͷŵ�ʵ��
	static void release();
	//�Ƿ��Ѿ�ʵ����
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