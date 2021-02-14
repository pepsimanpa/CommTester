#pragma once

#include "../CommBase/TCP/TCPClient.h"

#define WM_TCP_CLT_RECV_MSG (WM_USER+102)

// CTCPClientDlg 대화 상자

class CTCPClientDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTCPClientDlg)

public:
	CTCPClientDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTCPClientDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TCP_CLIENT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnTcpCltRecvMsg(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonOpt();
	afx_msg void OnBnClickedButtonSend();

	static void ReceiveFunc(char* pBuff, int nSize);

	CIPAddressCtrl m_ctrlAddrCltIP;
	CIPAddressCtrl m_ctrlAddrSrvIP;
	CEdit m_ctrlEditCltPort;
	CEdit m_ctrlEditSrvPort;
	CEdit m_ctrlEditSendData;
	CEdit m_ctrlEditStatus;

	CTCPClient* m_pTcpClt;
};
