
// httptestclientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "httptestclient.h"
#include "httptestclientDlg.h"
#include "afxdialogex.h"

#include "GlobalInfo.h"

#include <fstream>
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// ChttptestclientDlg 对话框



ChttptestclientDlg::ChttptestclientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ChttptestclientDlg::IDD, pParent)
	, m_strUrl(_T(""))
	, m_nThreadCount(1)
	, m_strSendData(_T(""))
	, m_strGetDataUrl(_T(""))
	, m_strGetDataParam(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_strUrl = L"http://192.168.101.115:8332/send_data_to_sys";
	m_strSendData = L"{\"address\":\"abcde\",\"data\":\"test\",\"sign\":\"123456\"}";
	m_bIsStartSend = false;

	m_strGetDataUrl = L"http://192.168.101.115:8332/get_data_from_sys";
	m_strGetDataParam = L"";
	GlobalInfo::GetInstance();

	//
	std::ifstream t("C:\\Users\\Administrator\\Desktop\\unicode.txt");  
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>()); 
	int nLen = str.length();
	m_strUnicodeData = str;

	wchar_t *pwchar = new wchar_t[1024];
	memset(pwchar, 0, 1024);
	memcpy(pwchar, str.c_str(), str.length());
}

void ChttptestclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_urlEdit);
	DDX_Text(pDX, IDC_EDIT1, m_strUrl);
	DDX_Text(pDX, IDC_EDIT2, m_nThreadCount);
	DDX_Text(pDX, IDC_EDIT3, m_strSendData);
	DDX_Text(pDX, IDC_GETDATA_EDIT, m_strGetDataUrl);
	DDX_Text(pDX, IDC_EDIT5, m_strGetDataParam);
	
}

BEGIN_MESSAGE_MAP(ChttptestclientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &ChttptestclientDlg::OnBnClickedOk)
	ON_EN_KILLFOCUS(IDC_EDIT1, &ChttptestclientDlg::OnEnKillfocusEdit1)
	ON_BN_CLICKED(IDC_BTN_START, &ChttptestclientDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, &ChttptestclientDlg::OnBnClickedBtnStop)
	ON_EN_KILLFOCUS(IDC_EDIT3, &ChttptestclientDlg::OnEnKillfocusEdit3)
	ON_EN_CHANGE(IDC_EDIT1, &ChttptestclientDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT3, &ChttptestclientDlg::OnEnChangeEdit3)
	ON_EN_CHANGE(IDC_EDIT2, &ChttptestclientDlg::OnEnChangeEdit2)
	ON_MESSAGE(WM_MYMESSAGE, &ChttptestclientDlg::OnMyMessage)
	ON_BN_CLICKED(IDC_BTN_GETDATA, &ChttptestclientDlg::OnBnClickedBtnGetdata)
	ON_EN_CHANGE(IDC_GETDATA_EDIT, &ChttptestclientDlg::OnEnChangeGetdataEdit)
	ON_EN_CHANGE(IDC_EDIT5, &ChttptestclientDlg::OnEnChangeEdit5)
	ON_BN_CLICKED(IDC_BUTTON1, &ChttptestclientDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// ChttptestclientDlg 消息处理程序

BOOL ChttptestclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void ChttptestclientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void ChttptestclientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR ChttptestclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL ChttptestclientDlg::MByteToWChar(LPCSTR lpcszStr, LPWSTR lpwszStr, DWORD dwSize)
{
	// Get the required size of the buffer that receives the Unicode string. 
	DWORD dwMinSize;
	dwMinSize = MultiByteToWideChar (CP_ACP, 0, lpcszStr, -1, NULL, 0);

	if(dwSize < dwMinSize)
	{
		return FALSE;
	}

	// Convert headers from ASCII to Unicode.
	MultiByteToWideChar (CP_ACP, 0, lpcszStr, -1, lpwszStr, dwMinSize);  
	return TRUE;
}
BOOL ChttptestclientDlg::WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize)
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, NULL, 0, NULL, FALSE);
	if(dwSize < dwMinSize)
	{
		return FALSE;
	}
	WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, lpszStr, dwSize, NULL, FALSE);
	return TRUE;
}

std::string ChttptestclientDlg::UTF8_To_string(const std::string & str) 
{ 
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0); 

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
	memset(pwBuf, 0, nwLen * 2 + 2); 

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen); 

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL); 

	char * pBuf = new char[nLen + 1]; 
	memset(pBuf, 0, nLen + 1); 

	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL); 

	std::string retStr = pBuf; 

	delete []pBuf; 
	delete []pwBuf; 

	pBuf = NULL; 
	pwBuf = NULL; 

	return retStr; 
} 
std::string ChttptestclientDlg::string_To_UTF8(const std::string & str) 
{ 
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0); 

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
	ZeroMemory(pwBuf, nwLen * 2 + 2); 

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen); 

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL); 

	char * pBuf = new char[nLen + 1]; 
	ZeroMemory(pBuf, nLen + 1); 

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL); 

	std::string retStr(pBuf); 

	delete []pwBuf; 
	delete []pBuf; 

	pwBuf = NULL; 
	pBuf  = NULL; 

	return retStr; 
} 


void ChttptestclientDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();
}


void ChttptestclientDlg::OnEnKillfocusEdit1()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(true);
}
void ChttptestclientDlg::OnEnChangeEdit1()
{
	UpdateData(true);
}

void ChttptestclientDlg::OnEnKillfocusEdit3()
{
	//
	UpdateData(true);
}
void ChttptestclientDlg::OnEnChangeEdit3()
{
	UpdateData(true);
}
void ChttptestclientDlg::OnEnChangeEdit2()
{
	UpdateData(true);
}

void ChttptestclientDlg::OnEnChangeGetdataEdit()
{
	UpdateData(true);
}
void ChttptestclientDlg::OnEnChangeEdit5()
{
	UpdateData(true);
}
LRESULT ChttptestclientDlg::OnMyMessage(WPARAM w,LPARAM l)
{
	CString str1,str2,str3,str4;
	str1.Format(L"%d", g_pGlobalInfo->m_nSendTimes);
	str2.Format(L"%d", g_pGlobalInfo->m_nSuccCount);
	str3.Format(L"%d", g_pGlobalInfo->m_nReqDataErrCount);
	str4.Format(L"%d", g_pGlobalInfo->m_nReqErrCount);

	SetDlgItemText(IDC_STATIC_SENDTIMES, str1);
	SetDlgItemText(IDC_STATIC_SUCCTIMES, str2);
	SetDlgItemText(IDC_STATIC_SCDETIMES, str3);
	SetDlgItemText(IDC_STATIC_REQERRTIMES, str4);

	UpdateData(false);

	return 1;
}

//发送数据线程
unsigned __stdcall ChttptestclientDlg::ThreadSendData(void* param)
{
	ChttptestclientDlg *pTmpDlg = (ChttptestclientDlg*)param;
	if(pTmpDlg == NULL)
	{
		return -1;
	}

	while(pTmpDlg->m_bIsStartSend)
	{
		//生成发送数据
		string strNewData = g_pGlobalInfo->ChangeSendData();

		int nLen = strNewData.length();
//		if(nLen <= 1024*512)
//		{
//			continue;
//		}

		//EnterCriticalSection(&g_pGlobalInfo->m_logCS);
		//GlobalInfo::OutputLog("sendReqData.log", strNewData);
		//LeaveCriticalSection(&g_pGlobalInfo->m_logCS);

		//将新数据格式化成UTF8编码
		string strNewUtf8Data = pTmpDlg->string_To_UTF8(strNewData);

		string	strRes;
		bool bRes = g_pGlobalInfo->SendHttpRequest(g_pGlobalInfo->m_strUrl, strNewUtf8Data, strNewData);
		//请求失败的情况
		g_pGlobalInfo->fvalid=bRes;
		if(bRes == false)
		{

		}

		//
		pTmpDlg->PostMessage(WM_MYMESSAGE, NULL, NULL);
	}

	return 0;
}

//开始测试
void ChttptestclientDlg::OnBnClickedBtnStart()
{
	g_pGlobalInfo->m_nSendTimes = 0;
	g_pGlobalInfo->m_nSuccCount = 0;
	g_pGlobalInfo->m_nReqDataErrCount = 0;
	g_pGlobalInfo->m_nReqErrCount = 0;
	g_pGlobalInfo->m_nodeRecvdCountMap.clear();

	//
	char szData[2048] = "";
	memset(szData, 0, 2048);

	//SendUrl
	BOOL bRes = WCharToMByte((LPCWSTR)m_strUrl, szData, 20480);
	if(bRes == FALSE || m_strUrl.GetLength() <= 0)
	{
		MessageBox(L"Url error !", L"Tip", MB_OK);
	}
	g_pGlobalInfo->m_strUrl = szData;
	

	//SendData
	char szData1[1024*10] = "";
	memset(szData1, 0, 1024*10);
	BOOL bRes1 = WCharToMByte((LPCWSTR)m_strSendData, szData1, 1024*10);
	if(bRes1 == FALSE || m_strUrl.GetLength() <= 0)
	{
		MessageBox(L"Send Data error !", L"Tip", MB_OK);
	}
	g_pGlobalInfo->m_strSendData = szData1;

	//
	m_bIsStartSend = true;

	for(int i = 0; i < m_nThreadCount; i++)
	{
		//启动业务线程
		HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, ThreadSendData, this, NULL, NULL);
		if (handle != NULL)
		{
			CloseHandle(handle);
		}
	}
}

//停止测试
void ChttptestclientDlg::OnBnClickedBtnStop()
{
	m_bIsStartSend = false;

	Sleep(1);

	//写入测试结果
	std::stringstream ss;  
	ss << "发送次数：" << g_pGlobalInfo->m_nSendTimes << "=====" \
		<< "成功次数：" << g_pGlobalInfo->m_nSuccCount << "=====" \
		<< "请求成功返回数据错误：" << g_pGlobalInfo->m_nReqDataErrCount << "=====" \
		<< "请求失败次数：" << g_pGlobalInfo->m_nReqErrCount << endl ;  
	string strTestRes = ss.str();
	GlobalInfo::OutputLog("testResult.log", strTestRes);

	//写入节点接收结果
	map<string, int>::iterator iIter;
	for(iIter = g_pGlobalInfo->m_nodeRecvdCountMap.begin();
		iIter != g_pGlobalInfo->m_nodeRecvdCountMap.end();
		iIter++)
	{
		std::stringstream ss;
		ss << "接收节点Mac ：" << iIter->first << "； 接收次数：" <<  iIter->second << endl;
		string strTestRes = ss.str();
		GlobalInfo::OutputLog("testResult.log", strTestRes);
	}
}


//获取数据线程
unsigned __stdcall ChttptestclientDlg::ThreadGetData(void* param)
{
	ChttptestclientDlg *pTmpDlg = (ChttptestclientDlg*)param;
	if(pTmpDlg == NULL)
	{
		return -1;
	}

	//if(g_pGlobalInfo->m_strGetDataParam.length() > 0)
	//{
		//将新数据格式化成UTF8编码
		//string strNewUtf8Data = pTmpDlg->string_To_UTF8(g_pGlobalInfo->m_strGetDataParam);
		//bool bRes = g_pGlobalInfo->SendGetDataHttpRequest(g_pGlobalInfo->m_strGetDataUrl, strNewUtf8Data, g_pGlobalInfo->m_strGetDataParam);

		bool bRes = g_pGlobalInfo->SendGetDataHttpRequest(g_pGlobalInfo->m_strGetDataUrl, g_pGlobalInfo->m_strGetDataParam, g_pGlobalInfo->m_strGetDataParam);
	//}
	//else
	//{
	//	string strFileName = g_pGlobalInfo->m_strCurrPath + "log\\dataHash.log";
	//	ifstream   readFile;
	//	readFile.open(strFileName, ios::in);
	//	if(!readFile)
	//	{
	//		return -1;
	//	}

	//	char nextline[2048];
	//	while(readFile.getline(nextline, sizeof(nextline)))
	//	{
	//		string tempStr = nextline;
	//		tempStr = tempStr.substr(22, tempStr.length() - 21);

	//		//将新数据格式化成UTF8编码
	//		string strNewUtf8Data = pTmpDlg->string_To_UTF8(tempStr);
	//		bool bRes = g_pGlobalInfo->SendGetDataHttpRequest(g_pGlobalInfo->m_strGetDataUrl, strNewUtf8Data, tempStr);

	//		memset(nextline, 0, 2018);
	//	}

	//	readFile.close();
	//}

	return 0;
}

void ChttptestclientDlg::OnBnClickedBtnGetdata()
{
	UpdateData(false);

	char szData[2048] = "";
	memset(szData, 0, 2048);

	//SendUrl
	BOOL bRes = WCharToMByte((LPCWSTR)m_strGetDataUrl, szData, 2048);
	if(bRes == FALSE || m_strGetDataUrl.GetLength() <= 0)
	{
		MessageBox(L"Url error !", L"Tip", MB_OK);
	}
	g_pGlobalInfo->m_strGetDataUrl = szData;

	//SendData
	//wchar_t *pwChar = (wchar_t*)m_strGetDataParam.GetBuffer(m_strGetDataParam.GetLength());
	//char *pChar = new char[m_strGetDataParam.GetLength() * 2 + 1];

	//memcpy(pChar, (char*)pwChar, m_strGetDataParam.GetLength() * 2);
	//char *pchar = (char*)pwChar;

	//memset(szData, 0, 2048);
	//BOOL bRes1 = WCharToMByte((LPCWSTR)m_strGetDataParam, szData, 2048);
	//if(bRes1 == FALSE || m_strGetDataParam.GetLength() <= 0)
	//{
	//	//MessageBox(L"Send Data error !", L"Tip", MB_OK);
	//}
	//g_pGlobalInfo->m_strGetDataParam = szData;

	//g_pGlobalInfo->m_strGetDataParam = m_strGetDataParam.GetBuffer(m_strGetDataParam.GetLength());
	g_pGlobalInfo->m_strGetDataParam = m_strUnicodeData;
	

	//启动业务线程
	HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, ThreadGetData, this, NULL, NULL);
	if (handle != NULL)
	{
		CloseHandle(handle);
	}
}


void ChttptestclientDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
}
