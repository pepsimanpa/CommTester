// UDPDlg.cpp: 구현 파일
//

#include "pch.h"
#include "CommTester.h"
#include "UDPDlg.h"
#include "afxdialogex.h"


// CUDPDlg 대화 상자
CUDPDlg* g_pThis = NULL;

IMPLEMENT_DYNAMIC(CUDPDlg, CDialogEx)

CUDPDlg::CUDPDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_UDP, pParent)
{
	g_pThis = this;
	m_pUdp = new CUDP;
}

CUDPDlg::~CUDPDlg()
{
	delete m_pUdp;
}

void CUDPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_MY_IP, m_ctrlAddrMyIP);
	DDX_Control(pDX, IDC_IPADDRESS_TGT_IP, m_ctrlAddrTgtIP);
	DDX_Control(pDX, IDC_EDIT_MY_PORT, m_ctrlEditMyPort);
	DDX_Control(pDX, IDC_EDIT_TGT_PORT, m_ctrlEditTgtPort);
	DDX_Control(pDX, IDC_EDIT_SEND, m_ctrlEditSendData);
	DDX_Control(pDX, IDC_EDIT_STATUS, m_ctrlEditStatus);
}


BEGIN_MESSAGE_MAP(CUDPDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_START, &CUDPDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CUDPDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_OPT, &CUDPDlg::OnBnClickedButtonOpt)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CUDPDlg::OnBnClickedButtonSend)
	ON_MESSAGE(WM_UDP_RECV_MSG, &CUDPDlg::OnUdpRecvMsg)
END_MESSAGE_MAP()


// CUDPDlg 메시지 처리기


void CUDPDlg::OnBnClickedButtonStart()
{
	if (m_ctrlAddrMyIP.IsBlank() != TRUE && m_ctrlAddrTgtIP.IsBlank() != TRUE)
	{
		DWORD dwIP;
		CString strIP;
		CString strPort;

		m_ctrlAddrTgtIP.GetAddress(dwIP);
		strIP.Format(_T("%d.%d.%d.%d"), FIRST_IPADDRESS(dwIP), SECOND_IPADDRESS(dwIP),
			THIRD_IPADDRESS(dwIP), FOURTH_IPADDRESS(dwIP));
		m_ctrlEditTgtPort.GetWindowText(strPort);

		m_pUdp->SetTargetAddr(CT2A(strIP), _ttoi(strPort));

		m_ctrlAddrMyIP.GetAddress(dwIP);
		strIP.Format(_T("%d.%d.%d.%d"), FIRST_IPADDRESS(dwIP), SECOND_IPADDRESS(dwIP),
			THIRD_IPADDRESS(dwIP), FOURTH_IPADDRESS(dwIP));
		m_ctrlEditMyPort.GetWindowText(strPort);

		m_pUdp->SetMyAddr(CT2A(strIP), _ttoi(strPort));


		m_pUdp->SetReceiveFunc(&CUDPDlg::ReceiveFunc);

		if (m_pUdp->CreateSocket() == UDP_OK)
			if (m_pUdp->Bind() == UDP_OK)
				if (m_pUdp->Connect() == UDP_OK)
					if (m_pUdp->Start() == UDP_OK)
						PrintStatus(_T("Success Start UDP Service"));
					else
						PrintStatus(_T("[ERROR] Start UDP Service"));
				else
					PrintStatus(_T("[ERROR] Connect"));
			else
				PrintStatus(_T("[ERROR] Bind"));
		else
			PrintStatus(_T("[ERROR] Create Socket"));
	}
	else
		PrintStatus(_T("[ERROR] Incorrect IP and Port"));
}


void CUDPDlg::OnBnClickedButtonStop()
{
	m_pUdp->Stop();

	PrintStatus(_T("Stop UDP Service"));
}


void CUDPDlg::OnBnClickedButtonOpt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CUDPDlg::OnBnClickedButtonSend()
{
	CString strSend;
	m_ctrlEditSendData.GetWindowText(strSend);

	char chBuff[MAX_BUFF_SIZE];
	strcpy_s(chBuff, CT2A(strSend));

	if (m_pUdp->IsConnect())
	{
		if (m_pUdp->Send(chBuff, strlen(chBuff)) != UDP_ERROR)
			PrintStatus(_T("[SEND]: ") + strSend);
		else
			PrintStatus(_T("[ERROR] Send"));
	}
	else
	{
		DWORD dwIP;
		CString strIP;
		CString strPort;

		m_ctrlAddrTgtIP.GetAddress(dwIP);
		strIP.Format(_T("%d.%d.%d.%d"), FIRST_IPADDRESS(dwIP), SECOND_IPADDRESS(dwIP),
			THIRD_IPADDRESS(dwIP), FOURTH_IPADDRESS(dwIP));
		m_ctrlEditTgtPort.GetWindowText(strPort);


		if (m_pUdp->Send(CT2A(strIP), _ttoi(strPort), chBuff, strlen(chBuff)) != UDP_ERROR)
			PrintStatus(_T("[SEND]: ") + strSend);
		else
			PrintStatus(_T("[ERROR] Send"));
	}
}


void CUDPDlg::ReceiveFunc(char* pBuff, int nSize)
{
	if (nSize > 0)
	{
		::SendMessage(g_pThis->GetSafeHwnd(), WM_UDP_RECV_MSG, (WPARAM)pBuff, (LPARAM)nSize);
	}
}

void CUDPDlg::PrintStatus(CString str)
{
	str += "\r\n";
	m_ctrlEditStatus.SetSel(-2, -1);		 // 커서를 에디트박스 끝으로 이동
	m_ctrlEditStatus.ReplaceSel(str);     // 에디트 박스에 글자 추가
}


BOOL CUDPDlg::OnInitDialog()
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

	m_ctrlAddrMyIP.SetAddress(htonl(inet_addr(CT2A(strMyIP))));
	m_ctrlEditMyPort.SetWindowTextW(_T("5000"));

	m_ctrlAddrTgtIP.SetAddress(htonl(inet_addr(CT2A(strMyIP))));
	m_ctrlEditTgtPort.SetWindowTextW(_T("7000"));


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

LRESULT CUDPDlg::OnUdpRecvMsg(WPARAM wParam, LPARAM lParam)
{
	CString strRecv((char*)wParam);

	PrintStatus(_T("[RECV]: ") + strRecv);

	return 0;
}
