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
}


BEGIN_MESSAGE_MAP(CTCPClientDlg, CDialogEx)
END_MESSAGE_MAP()


// CTCPClientDlg 메시지 처리기
