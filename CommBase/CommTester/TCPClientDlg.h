#pragma once


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

	DECLARE_MESSAGE_MAP()
};
