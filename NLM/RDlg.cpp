// RDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NLM.h"
#include "RDlg.h"
#include "afxdialogex.h"
#include "NLMDlg.h"
#include "png.h"
#include "readBMP.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>

#define PNG_SIG_LEN 4
#define IO_PNG_U8  0x0001       /*  8bit unsigned integer */
#define IO_PNG_F32 0x0002       /* 32bit float */

#define   WIDTHBYTES(bits) (((bits)+31)/32*4)//用于使图像宽度所占字节数为4byte的倍数

// RDlg dialog

IMPLEMENT_DYNAMIC(RDlg, CDialogEx)

using namespace std;

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
		printf("error :: %s not found  or not a correct png image \n", strFilePath);
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
	int a = 0;
	int b = 1;
	BYTE *noisy = new BYTE[d_whc];

	for (int i = 0; i < d_w; i++) {
		for (int j = 0; j < d_h; j++) {
			int p = i * write_width + j * 3;

			
			noisy[p] = floor(pBmpData[p] + gauss(fSigma));
			noisy[p+1] = floor(pBmpData[p+1] + gauss(fSigma));
			noisy[p+2] = floor(pBmpData[p+2] + gauss(fSigma));

		}
	}

	cout << "noise added" << endl;


	saveImg(noisy);





}

BYTE* RDlg::floatToByte(float* noisy) {
	return 0;
}

float RDlg::gauss(float fSigma) {
	double a = rand() / double(RAND_MAX);
	double b = rand() / double(RAND_MAX);
	double z = (double)(fSigma)*sqrt(-2.0*log(a))*cos(2.0*3.14159*b);
	return (float)z;
}

void RDlg::saveImg(BYTE* data) {

	BITMAPFILEHEADER bmpf;//文件头    
	BITMAPINFOHEADER bmpi;//信息头    
	int Widthbytes;
	CFile bmpFile;
	CString strFileName = "noise.bmp";
	if (bmpFile.Open(strFileName, CFile::modeCreate | CFile::modeReadWrite) == 0)return;
	Widthbytes = (iBmpWidth * 3 + 3) / 4 * 4;//位图对齐，确保为4的倍数     
											 /* BITMAPFILEHEADER结构填写 */
	bmpf.bfType = 0x4d42;
	bmpf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmpf.bfSize = bmpf.bfOffBits + Widthbytes * iBmpHeight;
	bmpf.bfReserved1 = 0;
	bmpf.bfReserved2 = 0;
	/* BITMAPINFOHEADER结构填写 */
	bmpi.biSize = sizeof(BITMAPINFOHEADER);
	bmpi.biWidth = iBmpWidth;
	bmpi.biHeight = iBmpHeight;
	bmpi.biPlanes = 1;
	bmpi.biBitCount = 24;//24位    
	bmpi.biCompression = BI_RGB;
	bmpi.biSizeImage = 0;
	bmpi.biXPelsPerMeter = 0;
	bmpi.biYPelsPerMeter = 0;
	bmpi.biClrUsed = 0;
	bmpi.biClrImportant = 0;
	bmpFile.Write(&bmpf, sizeof(bmpf));//写文件头；    
	bmpFile.Write(&bmpi, sizeof(bmpi));//写信息头；    


	bmpFile.Write(data, Widthbytes*iBmpHeight);//写图像数据信息；     
	bmpFile.Close();
	// TODO: Add your control notification handler code here  
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



