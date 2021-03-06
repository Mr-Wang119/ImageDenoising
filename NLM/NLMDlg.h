
// NLMDlg.h : header file
//

#pragma once

extern CString strFilePath;
extern CString strFileName;
extern BITMAPINFO* pBmpInfo;  //记录图像细节
extern BYTE* pBmpData;        //图像数据
extern int iBmpWidth;
extern int iBmpHeight;
extern int write_width;
extern BYTE* noisy;


// CNLMDlg dialog
class CNLMDlg : public CDialogEx
{
// Construction
public:
	CNLMDlg(CWnd* pParent = nullptr);	// standard constructor
	LONG ImageWidth;
	LONG ImageHeight;
	BYTE *lpBmpData;
	BITMAPINFO* lpBmpPalett;



// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NLM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton2();
	void Pain();


	
	CString m_FilePath;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
};
