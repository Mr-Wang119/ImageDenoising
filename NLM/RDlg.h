#pragma once



// RDlg dialog

class RDlg : public CDialogEx
{
	DECLARE_DYNAMIC(RDlg)

public:
	RDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~RDlg();
	float BlurRadius;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton2();
	void blur(float BlurRadius);
	double generateGaussianNoise(double sigma);
};
