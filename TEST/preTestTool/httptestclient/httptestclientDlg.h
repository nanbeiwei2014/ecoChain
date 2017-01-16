
// httptestclientDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

#include <string>
using namespace std;

#define WM_MYMESSAGE (WM_USER+100)

// ChttptestclientDlg 对话框
class ChttptestclientDlg : public CDialogEx
{
// 构造
public:
	ChttptestclientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HTTPTESTCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	BOOL MByteToWChar(LPCSTR lpcszStr, LPWSTR lpwszStr, DWORD dwSize);
	BOOL WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize);

	std::string UTF8_To_string(const std::string & str);
	std::string string_To_UTF8(const std::string & str);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CEdit	m_urlEdit;
	CString m_strUrl;
	int		m_nThreadCount;
	CString m_strSendData;

	bool	m_bIsStartSend;			//开始发送测试

	afx_msg void OnBnClickedOk();
	afx_msg void OnEnKillfocusEdit1();
	afx_msg void OnEnKillfocusEdit3();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnEnChangeEdit2();

	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnStop();

	afx_msg LRESULT OnMyMessage(WPARAM w,LPARAM l);
	static unsigned __stdcall ThreadSendData(void* param);
	

	CString m_strGetDataUrl;
	CString m_strGetDataParam;
	string  m_strUnicodeData;

	afx_msg void OnBnClickedBtnGetdata();
	static unsigned __stdcall ThreadGetData(void* param);
	afx_msg void OnEnChangeGetdataEdit();
	afx_msg void OnEnChangeEdit5();
	afx_msg void OnBnClickedButton1();
};
