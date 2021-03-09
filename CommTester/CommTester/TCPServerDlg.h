#pragma once

#include "../CommBase/TCP/TCPServer.h"

#define WM_TCP_SRV_RECV_MSG (WM_USER+101)
#define WM_TCP_SRV_ACPT_MSG (WM_USER+102)

// CTCPServerDlg 대화 상자

class CTCPServerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTCPServerDlg)

public:
	CTCPServerDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTCPServerDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TCP_SERVER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnTcpSrvRecvMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTcpSrvAcptMsg(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonOpt();
	afx_msg void OnBnClickedButtonSend();

	void ReceiveFunc(char* pBuff, int nSize);
	void EventFunc(int nEventNum, char* pEventString);
	void PrintStatus(CString str);

	CIPAddressCtrl m_ctrlAddrSrvIP;
	CEdit m_ctrlEditSrvPort;
	CEdit m_ctrlEditSendData;
	CEdit m_ctrlEditStatus;
	CListBox m_ctrlListClt;

	CTCPServer* m_pTcpSrv;
};
