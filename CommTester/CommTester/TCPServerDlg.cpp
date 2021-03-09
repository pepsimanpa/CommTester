// TCPServerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "CommTester.h"
#include "TCPServerDlg.h"
#include "afxdialogex.h"

// CTCPServerDlg 대화 상자

IMPLEMENT_DYNAMIC(CTCPServerDlg, CDialogEx)

CTCPServerDlg::CTCPServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TCP_SERVER, pParent)
{
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
	DDX_Control(pDX, IDC_EDIT_STATUS, m_ctrlEditStatus);
	DDX_Control(pDX, IDC_LIST_CLT, m_ctrlListClt);
}


BEGIN_MESSAGE_MAP(CTCPServerDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_START, &CTCPServerDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CTCPServerDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_OPT, &CTCPServerDlg::OnBnClickedButtonOpt)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CTCPServerDlg::OnBnClickedButtonSend)
	ON_MESSAGE(WM_TCP_SRV_RECV_MSG, &CTCPServerDlg::OnTcpSrvRecvMsg)
	ON_MESSAGE(WM_TCP_SRV_ACPT_MSG, &CTCPServerDlg::OnTcpSrvAcptMsg)
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

		m_pTcpSrv->SetServerAddr(CT2A(strIP), _ttoi(strPort));

		m_pTcpSrv->SetReceiveFunc(std::bind(&CTCPServerDlg::ReceiveFunc, this, std::placeholders::_1, std::placeholders::_2));
		m_pTcpSrv->SetEventFunc(std::bind(&CTCPServerDlg::EventFunc, this, std::placeholders::_1, std::placeholders::_2));

		if(m_pTcpSrv->CreateSocket() == TCP_SERVER_OK)
			if(m_pTcpSrv->Listen() == TCP_SERVER_OK)
				if(m_pTcpSrv->Start() == TCP_SERVER_OK)
					PrintStatus(_T("Success Start TCP Server"));
				else
					PrintStatus(_T("[ERROR] Start TCP Server"));
			else
				PrintStatus(_T("[ERROR] Listen"));
		else
			PrintStatus(_T("[ERROR] Create Socket"));

	}
	else
		PrintStatus(_T("[ERROR] Incorrect IP and Port"));
}

void CTCPServerDlg::ReceiveFunc(char* pBuff, int nSize)
{
	if (nSize > 0)
	{
		::SendMessage(this->GetSafeHwnd(), WM_TCP_SRV_RECV_MSG, (WPARAM)pBuff, (LPARAM)nSize);
	}
}

void CTCPServerDlg::EventFunc(int nEventNum, char* pEventString)
{
	switch (nEventNum)
	{
	case eTCP_SRV_EVENT_ERROR:
	{
		break;
	}
	case eTCP_SRV_EVENT_ACCEPT:
	{
		::SendMessage(this->GetSafeHwnd(), WM_TCP_SRV_ACPT_MSG, (WPARAM)pEventString, (LPARAM)0);
		break;
	}
	default:
		break;
	}
}

void CTCPServerDlg::PrintStatus(CString str)
{
	str += "\r\n";
	m_ctrlEditStatus.SetSel(-2, -1);		 // 커서를 에디트박스 끝으로 이동
	m_ctrlEditStatus.ReplaceSel(str);     // 에디트 박스에 글자 추가
}

void CTCPServerDlg::OnBnClickedButtonStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pTcpSrv->Stop();

	PrintStatus(_T("Stop TCP Server"));
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

	if (m_ctrlListClt.GetCurSel() > -1)
	{
		if(m_pTcpSrv->Send(m_ctrlListClt.GetCurSel(), chBuff, strlen(chBuff)) != TCP_SERVER_ERROR)
			PrintStatus(_T("[SEND]: ") + strSend);
		else
			PrintStatus(_T("[ERROR] Send"));
	}
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

	PrintStatus(_T("[RECV]: ") + strRecv);

	return 0;
}

afx_msg LRESULT CTCPServerDlg::OnTcpSrvAcptMsg(WPARAM wParam, LPARAM lParam)
{
	CString strAddr((char*)wParam);

	m_ctrlListClt.InsertString(-1, strAddr);

	PrintStatus(_T("[ACCEPT]: ") + strAddr);

	return 0;
}
