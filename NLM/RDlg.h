#pragma once

extern float wxSLUT(float dif, float * lut);

extern int fiL2FloatDist(BYTE ** u0, BYTE ** u1, int i0, int j0, int i1, int j1, int radius, int channels, int width0, int width1);
extern int fiL2FloatDist(BYTE * u0, BYTE * u1, int i0, int j0, int i1, int j1, int radius, int width0, int width1);

extern void wxFillExpLut(float * lut, int size);

extern void fpClear(int * fpI, int fValue, int iLength);

extern void denoise(int iDWin, int iDBloc, float fSigma, float fFiltPar, BYTE ** fpI, int ** fpO, int iChannels, int iWidth, int iHeight);

extern void deblur(float BlurRadius);

extern void saveImg(BYTE * data, CString strFileName);
extern float BlurRadius;

// RDlg dialog

class RDlg : public CDialogEx
{
	DECLARE_DYNAMIC(RDlg)

public:
	RDlg(CWnd* pParent = nullptr);   // standard constructor
	BYTE * floatToByte(float * noisy);
	float gauss(float fSigma);

	virtual ~RDlg();


// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg
	void OnBnClickedButton2();
	void blur(float BlurRadius);

};
