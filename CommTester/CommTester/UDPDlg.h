#pragma once

#include "../CommBase/UDP/UDP.h"

#define WM_UDP_RECV_MSG (WM_USER+301)

// CUDPDlg 대화 상자

class CUDPDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUDPDlg)

public:
	CUDPDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CUDPDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_UDP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnUdpRecvMsg(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonOpt();
	afx_msg void OnBnClickedButtonSend();

	static void ReceiveFunc(char* pBuff, int nSize);
	void PrintStatus(CString str);

	CIPAddressCtrl m_ctrlAddrMyIP;
	CIPAddressCtrl m_ctrlAddrTgtIP;
	CEdit m_ctrlEditMyPort;
	CEdit m_ctrlEditTgtPort;
	CEdit m_ctrlEditSendData;
	CEdit m_ctrlEditStatus;

	CUDP* m_pUdp;
};
