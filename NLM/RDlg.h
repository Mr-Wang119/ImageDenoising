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
	afx_msg
	void deblur(float BlurRadius);
	void OnBnClickedButton2();
	void blur(float BlurRadius);
	void denoise(int iDWin, int iDBloc, float fSigma, float fFiltPar, BYTE ** fpI, int ** fpO, int iChannels, int iWidth, int iHeight);
	float wxSLUT(float dif, float * lut);
	int fiL2FloatDist(BYTE ** u0, BYTE ** u1, int i0, int j0, int i1, int j1, int radius, int channels, int width0, int width1);
	int fiL2FloatDist(BYTE * u0, BYTE * u1, int i0, int j0, int i1, int j1, int radius, int width0, int width1);
	void fpClear(int * fpI, int fValue, int iLength);
	void wxFillExpLut(float * lut, int size);
	BYTE * floatToByte(float * noisy);
	float gauss(float fSigma);
	void saveImg(BYTE * data, CString strFileName);
	double generateGaussianNoise(double sigma);


};
