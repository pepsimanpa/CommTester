// TCPClientDlg.cpp: 구현 파일
//

#include "pch.h"
#include "CommTester.h"
#include "TCPClientDlg.h"
#include "afxdialogex.h"


// CTCPClientDlg 대화 상자

IMPLEMENT_DYNAMIC(CTCPClientDlg, CDialogEx)

CTCPClientDlg::CTCPClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TCP_CLIENT, pParent)
{

}

CTCPClientDlg::~CTCPClientDlg()
{
}

void CTCPClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_CLT_IP, m_ctrlAddrCltIP);
	DDX_Control(pDX, IDC_EDIT_CLT_PORT, m_ctrlEditCltPort);
	DDX_Control(pDX, IDC_IPADDRESS_SRV_IP, m_ctrlAddrSrvIP);
	DDX_Control(pDX, IDC_EDIT_SRV_PORT, m_ctrlEditSrvPort);
	DDX_Control(pDX, IDC_EDIT_SEND, m_ctrlEditSendData);
	DDX_Control(pDX, IDC_EDIT_STATUS, m_ctrlEditStatus);
}


BEGIN_MESSAGE_MAP(CTCPClientDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_START, &CTCPClientDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CTCPClientDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_OPT, &CTCPClientDlg::OnBnClickedButtonOpt)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CTCPClientDlg::OnBnClickedButtonSend)
END_MESSAGE_MAP()


// CTCPClientDlg 메시지 처리기


void CTCPClientDlg::OnBnClickedButtonStart()
{
	if (m_ctrlAddrSrvIP.IsBlank() != TRUE && m_ctrlAddrCltIP.IsBlank() != TRUE)
	{
		DWORD dwIP;
		CString strIP;
		CString strPort;

		m_ctrlAddrSrvIP.GetAddress(dwIP);
		strIP.Format(_T("%d.%d.%d.%d"), FIRST_IPADDRESS(dwIP), SECOND_IPADDRESS(dwIP),
			THIRD_IPADDRESS(dwIP), FOURTH_IPADDRESS(dwIP));
		m_ctrlEditSrvPort.GetWindowText(strPort);

		m_tcpClt.SetServerInfo(CT2A(strIP), _ttoi(strPort));

		m_ctrlAddrCltIP.GetAddress(dwIP);
		strIP.Format(_T("%d.%d.%d.%d"), FIRST_IPADDRESS(dwIP), SECOND_IPADDRESS(dwIP),
			THIRD_IPADDRESS(dwIP), FOURTH_IPADDRESS(dwIP));
		m_ctrlEditCltPort.GetWindowText(strPort);

		m_tcpClt.SetClientInfo(CT2A(strIP), _ttoi(strPort));


		m_tcpClt.SetReceiveFunc(&CTCPClientDlg::ReceiveFunc);

		m_tcpClt.CreateSocket();
		m_tcpClt.Init();
		m_tcpClt.Start();
	}
}


void CTCPClientDlg::OnBnClickedButtonStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_tcpClt.Stop();
}


void CTCPClientDlg::OnBnClickedButtonOpt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CTCPClientDlg::OnBnClickedButtonSend()
{
	CString strSend;
	m_ctrlEditSendData.GetWindowText(strSend);

	char chBuff[MAX_BUFF_SIZE];
	strcpy_s(chBuff, CT2A(strSend));

	m_tcpClt.Send(chBuff, strlen(chBuff));
}

void CTCPClientDlg::ReceiveFunc(char* pBuff, int nSize)
{
	if (nSize > 0)
	{
		::SendMessage(HWND_BROADCAST, TCP_CLT_RECV_MSG, (WPARAM)pBuff, (LPARAM)nSize);
	}
}


BOOL CTCPClientDlg::OnInitDialog()
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

	m_ctrlAddrCltIP.SetAddress(htonl(inet_addr(CT2A(strMyIP))));
	m_ctrlEditCltPort.SetWindowTextW(_T("7000"));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


LRESULT CTCPClientDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (message == TCP_CLT_RECV_MSG)
	{
		CString strRecv((char*)wParam);
		m_ctrlEditStatus.SetSel(-2, -1);				// 커서를 에디트박스 끝으로 이동
		m_ctrlEditStatus.ReplaceSel(strRecv);     // 에디트 박스에 글자 추가
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}
