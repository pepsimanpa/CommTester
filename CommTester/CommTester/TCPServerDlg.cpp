﻿// TCPServerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "CommTester.h"
#include "TCPServerDlg.h"
#include "afxdialogex.h"

// CTCPServerDlg 대화 상자
CTCPServerDlg* g_pThis = NULL;

IMPLEMENT_DYNAMIC(CTCPServerDlg, CDialogEx)

CTCPServerDlg::CTCPServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TCP_SERVER, pParent)
{
	g_pThis = this;
	m_pTcpSrv = new CTCPServer;
}

CTCPServerDlg::~CTCPServerDlg()
{
	delete m_pTcpSrv;
}

void CTCPServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_SRV_IP, m_ctrlAddrSrvIP);
	DDX_Control(pDX, IDC_EDIT_SRV_PORT, m_ctrlEditSrvPort);
	DDX_Control(pDX, IDC_EDIT_SEND, m_ctrlEditSendData);
	DDX_Control(pDX, IDC_EDIT_CLIENT_LIST, m_ctrlEditCltList);
	DDX_Control(pDX, IDC_EDIT_STATUS, m_ctrlEditStatus);
}


BEGIN_MESSAGE_MAP(CTCPServerDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_START, &CTCPServerDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CTCPServerDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_OPT, &CTCPServerDlg::OnBnClickedButtonOpt)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CTCPServerDlg::OnBnClickedButtonSend)
	ON_MESSAGE(WM_TCP_SRV_RECV_MSG, &CTCPServerDlg::OnTcpSrvRecvMsg)
END_MESSAGE_MAP()


// CTCPServerDlg 메시지 처리기


void CTCPServerDlg::OnBnClickedButtonStart()
{
	if (m_ctrlAddrSrvIP.IsBlank() != TRUE)
	{
		DWORD dwIP;
		m_ctrlAddrSrvIP.GetAddress(dwIP);

		CString strIP;
		strIP.Format(_T("%d.%d.%d.%d"), FIRST_IPADDRESS(dwIP), SECOND_IPADDRESS(dwIP),
			THIRD_IPADDRESS(dwIP), FOURTH_IPADDRESS(dwIP));

		CString strPort;
		m_ctrlEditSrvPort.GetWindowText(strPort);

		m_pTcpSrv->SetServerInfo(CT2A(strIP), _ttoi(strPort));
		m_pTcpSrv->SetReceiveFunc(&CTCPServerDlg::ReceiveFunc);

		m_pTcpSrv->CreateSocket();
		m_pTcpSrv->Init();
		m_pTcpSrv->Start();
	}
}

void CTCPServerDlg::ReceiveFunc(char* pBuff, int nSize)
{
	if (nSize > 0)
	{
		::SendMessage(g_pThis->GetSafeHwnd(), WM_TCP_SRV_RECV_MSG, (WPARAM)pBuff, (LPARAM)nSize);
	}
}

void CTCPServerDlg::OnBnClickedButtonStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pTcpSrv->Stop();
}


void CTCPServerDlg::OnBnClickedButtonOpt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//::SendMessage(g_pThis->GetSafeHwnd(), WM_TCP_SRV_RECV_MSG, (WPARAM)NULL, (LPARAM)NULL);
}


void CTCPServerDlg::OnBnClickedButtonSend()
{
	CString strSend;
	m_ctrlEditSendData.GetWindowText(strSend);

	char chBuff[MAX_BUFF_SIZE];
	strcpy_s(chBuff, CT2A(strSend));

	m_pTcpSrv->Send(0, chBuff, strlen(chBuff));
}


BOOL CTCPServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	char myaddr[256];
	PHOSTENT  phostinfo;
	IN_ADDR  inaddr;
	CString strMyIP;

	gethostname(myaddr, 256);
	phostinfo = gethostbyname(myaddr);

	if (phostinfo != NULL)
	{
		for (int i = 0; phostinfo->h_addr_list[i]; i++)
		{
			memcpy(&inaddr, phostinfo->h_addr_list[i], 4);
			strMyIP = inet_ntoa(inaddr);

			break;
		}
	}

	m_ctrlAddrSrvIP.SetAddress(htonl(inet_addr(CT2A(strMyIP))));
	m_ctrlEditSrvPort.SetWindowTextW(_T("5000"));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


afx_msg LRESULT CTCPServerDlg::OnTcpSrvRecvMsg(WPARAM wParam, LPARAM lParam)
{
	CString strRecv((char*)wParam);
	strRecv += "\r\n";
	m_ctrlEditStatus.SetSel(-2, -1);				// 커서를 에디트박스 끝으로 이동
	m_ctrlEditStatus.ReplaceSel(strRecv);     // 에디트 박스에 글자 추가

	return 0;
}
