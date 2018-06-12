// RDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NLM.h"
#include "RDlg.h"
#include "afxdialogex.h"
#include "NLMDlg.h"
#include "png.h"
#include "readBMP.h"

#define PNG_SIG_LEN 4
#define IO_PNG_U8  0x0001       /*  8bit unsigned integer */
#define IO_PNG_F32 0x0002       /* 32bit float */

// RDlg dialog

IMPLEMENT_DYNAMIC(RDlg, CDialogEx)

RDlg::RDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

RDlg::~RDlg()
{
}

void RDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(RDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &RDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// RDlg message handlers


void RDlg::OnBnClickedButton2()
{
	CString text;
	GetDlgItemText(IDC_EDIT1, text);
	BlurRadius = _ttof(text);
	blur(BlurRadius);
	// TODO: Add your control notification handler code here
}

void RDlg::blur(float BlurRadius) {
	size_t nx, ny, nc;
	//float *d_v = NULL;

	//声明标识符
	USES_CONVERSION;
	//调用函数，T2A和W2A均支持ATL和MFC中的字符转换
	//char * pFileName = T2A(strFilePath);
	//char * pFileName = W2A(str); //也可实现转换
	//d_v = readPNG(strFilePath, &nx, &ny, &nc);


	if (!pBmpData) {
		printf("error :: %s not found  or not a correct png image \n",strFilePath);
		exit(-1);
	}

	// variables
	//int d_w = (int)nx;
	//int d_h = (int)ny;
	//int d_c = (int)nc;
	int d_w = iBmpWidth;
	int d_h = iBmpHeight;
	int d_c = 3;

	if (d_c == 2) {
		d_c = 1;    // we do not use the alpha channel
	}
	if (d_c > 3) {
		d_c = 3;    // we do not use the alpha channel
	}

	int d_wh = d_w * d_h;
	int d_whc = d_c * d_w * d_h;

	// test if image is really a color image even if it has more than one channel
	if (d_c > 1) {
		// dc equals 3
		int i = 0;
		/*while (i < d_wh && d_v[i] == d_v[d_wh + i] && d_v[i] == d_v[2 * d_wh + i]) {
			i++;
		}*/

		while (i < d_wh && pBmpData[i] == pBmpData[d_wh + i] && pBmpData[i] == pBmpData[2 * d_wh + i]) {
			i++;
		}

		if (i == d_wh) d_c = 1;

	}

	// add noise
	float fSigma = BlurRadius;
	byte *noisy = new byte[d_whc];

	for (int i = 0; i < d_c; i++) {

	}


}

double RDlg::generateGaussianNoise(double sigma)
{
	static double V1, V2, S;
	static int phase = 0;
	double X;
	double U1, U2;
	if (phase == 0) {
		do {
			U1 = (double)rand() / RAND_MAX;
			U2 = (double)rand() / RAND_MAX;

			V1 = 2 * U1 - 1;
			V2 = 2 * U2 - 1;
			S = V1 * V1 + V2 * V2;
		} while (S >= 1 || S == 0);

		X = V1 * sqrt(-2 * log(S) / S);
	}
	else {
		X = V2 * sqrt(-2 * log(S) / S);
	}
	phase = 1 - phase;
	return sigma * X;
}



