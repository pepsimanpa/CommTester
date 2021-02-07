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

}

CTCPServerDlg::~CTCPServerDlg()
{
}

void CTCPServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_SRV_IP, m_ctrlIP);
	DDX_Control(pDX, IDC_EDIT_SRV_PORT, m_ctrlPort);
}


BEGIN_MESSAGE_MAP(CTCPServerDlg, CDialogEx)
END_MESSAGE_MAP()


// CTCPServerDlg 메시지 처리기
