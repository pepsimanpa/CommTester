#pragma once

#include "../CommBase/TCP/TCPServer.h"

#define WM_TCP_SRV_RECV_DATA (WM_USER+101)

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
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonOpt();
	afx_msg void OnBnClickedButtonSend();

	static void ReceiveFunc(char* pBuff, int nSize);

	CIPAddressCtrl m_ctrlAddrSrvIP;
	CEdit m_ctrlEditSrvPort;
	CEdit m_ctrlEditSendData;
	CEdit m_ctrlEditCltList;
	CEdit m_ctrlEditStatus;

	CTCPServer m_tcpSrv;
protected:
	afx_msg LRESULT OnTcpSrvRecvData(WPARAM wParam, LPARAM lParam);
};
