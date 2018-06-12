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
#define MIN(i,j) ( (i)<(j) ? (i):(j) )
#define MAX(i,j) ( (i)<(j) ? (j):(i) )

#define PNG_SIG_LEN 4
#define IO_PNG_U8  0x0001       /*  8bit unsigned integer */
#define IO_PNG_F32 0x0002       /* 32bit float */
///// LUT tables
#define LUTMAX 30.0
#define LUTMAXM1 29.0
#define LUTPRECISION 1000.0
#define fTiny 0.00000001f

#define   WIDTHBYTES(bits) (((bits)+31)/32*4)//用于使图像宽度所占字节数为4byte的倍数

// RDlg dialog

IMPLEMENT_DYNAMIC(RDlg, CDialogEx)

using namespace std;

BYTE *noisy;
float BlurRadius;

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


void deblur(float fSigma) {
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


	// denoise
	BYTE **fpI = new BYTE*[d_c];
	int **fpO = new int*[d_c];
	for (int i = 0; i < d_c; i++) {
		fpI[i] = new BYTE[d_wh];
		fpO[i] = new int[d_wh];
	}

	BYTE *denoised = new BYTE[d_whc];
	for (int i = 0; i < d_wh; i++) {
		for (int j = 0; j < d_c; j++) {
			fpI[j][i] = noisy[3 * i + j];
			fpO[j][i] = 0;
		}
	}
	fpI[0][0] = 30;



	/*
	for (int ii = 0; ii < d_c; ii++) {

	fpI[ii] = &noisy[ii * d_wh];
	fpO[ii] = &denoised[ii * d_wh];

	}*/




	int bloc, win;
	float fFiltPar;

	if (d_c == 1) {

		if (fSigma > 0.0f && fSigma <= 15.0f) {
			win = 1;
			bloc = 10;
			fFiltPar = 0.4f;

		}
		else if (fSigma > 15.0f && fSigma <= 30.0f) {
			win = 2;
			bloc = 10;
			fFiltPar = 0.4f;

		}
		else if (fSigma > 30.0f && fSigma <= 45.0f) {
			win = 3;
			bloc = 17;
			fFiltPar = 0.35f;

		}
		else if (fSigma > 45.0f && fSigma <= 75.0f) {
			win = 4;
			bloc = 17;
			fFiltPar = 0.35f;

		}
		else if (fSigma <= 100.0f) {

			win = 5;
			bloc = 17;
			fFiltPar = 0.30f;

		}
		else {
			printf("error :: algorithm parametrized only for values of sigma less than 100.0\n");
			exit(-1);
		}





	}
	else {


		if (fSigma > 0.0f && fSigma <= 25.0f) {
			win = 1;
			bloc = 10;
			fFiltPar = 0.55f;

		}
		else if (fSigma > 25.0f && fSigma <= 55.0f) {
			win = 2;
			bloc = 17;
			fFiltPar = 0.4f;

		}
		else if (fSigma <= 100.0f) {
			win = 3;
			bloc = 17;
			fFiltPar = 0.35f;

		}
		else {
			printf("error :: algorithm parametrized only for values of sigma less than 100.0\n");
			exit(-1);
		}



	}

	denoise(win, bloc, fSigma, fFiltPar, fpI, fpO, d_c, d_w, d_h);

	for (int i = 0; i < d_wh; i++) {
		for (int j = 0; j < d_c; j++) {
			denoised[d_c * i + j] = fpO[j][i];
		}
	}

	saveImg(denoised, "denoise.bmp");

}

void RDlg::OnBnClickedButton2()
{
	CString text;
	GetDlgItemText(IDC_EDIT1, text);
	BlurRadius = _ttof(text);

	blur(BlurRadius);
	EndDialog(0);
	

	//CFile bmpFile;//记录打开文件        
	//CString strFileName;//记录选择文件路径    
	//BOOL isOpen = TRUE;
	//BITMAPFILEHEADER bmpHeader;//文件头    
	//BITMAPINFOHEADER bmpInfo;//信息头  

	//strFileName = "noise.bmp";
	//if (!bmpFile.Open(strFileName, CFile::modeRead | CFile::typeBinary)) return;
	//if (bmpFile.Read(&bmpHeader, sizeof(BITMAPFILEHEADER)) != sizeof(BITMAPFILEHEADER))
	//{
	//	AfxMessageBox("read bmp header failed!");
	//	return;
	//}  if (bmpHeader.bfType != 0x4d42)
	//{
	//	AfxMessageBox("invalid file type!");   return;
	//}
	//if (bmpFile.Read(&bmpInfo, sizeof(BITMAPINFOHEADER)) != sizeof(BITMAPINFOHEADER))
	//{
	//	AfxMessageBox("read bmp infor header failed!");
	//	return;
	//}  if (bmpInfo.biBitCount != 24)
	//{
	//	AfxMessageBox("File is not 24 bit.Application doesn't support this kind of file!");
	//	return;
	//}
	//write_width = WIDTHBYTES(bmpInfo.biWidth*bmpInfo.biBitCount);

	//pBmpInfo = (BITMAPINFO *)new char[sizeof(BITMAPINFOHEADER)];
	//if (!pBmpInfo)
	//{
	//	AfxMessageBox("memory error!");
	//	return;
	//}

	//CWnd *pWnd = GetDlgItem(IDC_PICTURE);//获得pictrue控件窗口的句柄    
	//CRect rect;
	//pWnd->GetClientRect(&rect);//获得pictrue控件所在的矩形区域    
	//CDC *pDC = pWnd->GetDC();//获得pictrue控件的DC       
	//						 //显示图片    
	//pDC->SetStretchBltMode(COLORONCOLOR);
	//StretchDIBits(pDC->GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), 0, 0, bmpInfo.biWidth, bmpInfo.biHeight, pBmpData, pBmpInfo, DIB_RGB_COLORS, SRCCOPY);
	//iBmpWidth = bmpInfo.biWidth;
	//iBmpHeight = bmpInfo.biHeight; // TODO: Add extra validation here 
	//							   // TODO: Add your control notification handler code here
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
	noisy = new BYTE[d_whc];


	for (int i = 0; i < d_whc; i+=3) {
		int re = gauss(fSigma);
		int a = floor(pBmpData[i] + re);
		int b = floor(pBmpData[i+1] + re);
		int c = floor(pBmpData[i+2] + re);
		if (a > 255)
			noisy[i] = 255;
		else if (a < 0)
			noisy[i] = 0;
		else
			noisy[i] = a;

		if (b > 255)
			noisy[i+1] = 255;
		else if (b < 0)
			noisy[i+1] = 0;
		else
			noisy[i+1] = b;

		if (c > 255)
			noisy[i+2] = 255;
		else if (c < 0)
			noisy[i+2] = 0;
		else
			noisy[i+2] = c;
		

	}


	cout << "noise added" << endl;

	saveImg(noisy,"noise.bmp");
	


	

}

void denoise(int iDWin,            // Half size of patch
	int iDBloc,           // Half size of research window
	float fSigma,         // Noise parameter
	float fFiltPar,       // Filtering parameter
	BYTE **fpI,          // Input
	int **fpO,          // Output
	int iChannels, int iWidth, int iHeight) {




	// length of each channel
	int iwxh = iWidth * iHeight;


	//  length of comparison window
	int ihwl = (2 * iDWin + 1);
	int iwl = (2 * iDWin + 1) * (2 * iDWin + 1);
	int icwl = iChannels * iwl;



	// filtering parameter
	float fSigma2 = fSigma * fSigma;
	float fH = fFiltPar * fSigma;
	float fH2 = fH * fH;

	// multiply by size of patch, since distances are not normalized
	fH2 *= (float)icwl;



	// tabulate exp(-x), faster than using directly function expf
	int iLutLength = (int)rintf((float)LUTMAX * (float)LUTPRECISION);
	float *fpLut = new float[iLutLength];
	wxFillExpLut(fpLut, iLutLength);




	// auxiliary variable
	// number of denoised values per pixel
	int *fpCount = new int[iwxh];
	fpClear(fpCount, 0, iwxh);




	// clear output
	//for (int ii = 0; ii < iChannels; ii++) fpClear(fpO[ii], 0, iwxh);




	// PROCESS STARTS
	// for each pixel (x,y)
#pragma omp parallel shared(fpI, fpO)
	{


#pragma omp for schedule(dynamic) nowait

		for (int y = 0; y < iHeight; y++) {


			// auxiliary variable
			// denoised patch centered at a certain pixel
			int **fpODenoised = new int*[iChannels];
			for (int ii = 0; ii < iChannels; ii++) fpODenoised[ii] = new int[iwl];





			for (int x = 0; x < iWidth; x++) {

				// reduce the size of the comparison window if we are near the boundary
				int iDWin0 = MIN(iDWin, MIN(iWidth - 1 - x, MIN(iHeight - 1 - y, MIN(x, y))));


				// research zone depending on the boundary and the size of the window
				int imin = MAX(x - iDBloc, iDWin0);
				int jmin = MAX(y - iDBloc, iDWin0);

				int imax = MIN(x + iDBloc, iWidth - 1 - iDWin0);
				int jmax = MIN(y + iDBloc, iHeight - 1 - iDWin0);



				//  clear current denoised patch
				for (int ii = 0; ii < iChannels; ii++) fpClear(fpODenoised[ii], 0.0f, iwl);



				// maximum of weights. Used for reference patch
				float fMaxWeight = 0.0f;


				// sum of weights
				float fTotalWeight = 0.0f;


				for (int j = jmin; j <= jmax; j++)
					for (int i = imin; i <= imax; i++)
						if (i != x || j != y) {

							double fDif = fiL2FloatDist(fpI, fpI, x, y, i, j, iDWin0, iChannels, iWidth, iWidth);

							// dif^2 - 2 * fSigma^2 * N      dif is not normalized
							fDif = MAX(fDif - 2 * icwl *  fSigma2, 0.0f);
							fDif = fDif / fH2;

							float fWeight = wxSLUT(fDif, fpLut);

							if (fWeight > fMaxWeight) fMaxWeight = fWeight;

							fTotalWeight += fWeight;


							for (int is = -iDWin0; is <= iDWin0; is++) {
								int aiindex = (iDWin + is) * ihwl + iDWin;
								int ail = (j + is)*iWidth + i;

								for (int ir = -iDWin0; ir <= iDWin0; ir++) {

									int iindex = aiindex + ir;
									int il = ail + ir;

									for (int ii = 0; ii < iChannels; ii++)
										fpODenoised[ii][iindex] += fWeight * fpI[ii][il];

								}

							}


						}




				// current patch with fMaxWeight
				for (int is = -iDWin0; is <= iDWin0; is++) {
					int aiindex = (iDWin + is) * ihwl + iDWin;
					int ail = (y + is)*iWidth + x;

					for (int ir = -iDWin0; ir <= iDWin0; ir++) {

						int iindex = aiindex + ir;
						int il = ail + ir;

						for (int ii = 0; ii < iChannels; ii++)
							fpODenoised[ii][iindex] += fMaxWeight * fpI[ii][il];

					}
				}



				fTotalWeight += fMaxWeight;







				// normalize average value when fTotalweight is not near zero
				if (fTotalWeight > fTiny) {



					for (int is = -iDWin0; is <= iDWin0; is++) {
						int aiindex = (iDWin + is) * ihwl + iDWin;
						int ail = (y + is)*iWidth + x;

						for (int ir = -iDWin0; ir <= iDWin0; ir++) {
							int iindex = aiindex + ir;
							int il = ail + ir;

							fpCount[il]++;

							for (int ii = 0; ii < iChannels; ii++) {
								fpO[ii][il] += fpODenoised[ii][iindex] / fTotalWeight;

							}

						}
					}


				}






			}



			for (int ii = 0; ii < iChannels; ii++) delete[] fpODenoised[ii];
			delete[] fpODenoised;


		}




	}









	for (int ii = 0; ii < iwxh; ii++)
		if (fpCount[ii] > 0.0) {
			for (int jj = 0; jj < iChannels; jj++)  
				fpO[jj][ii] /= fpCount[ii];

		}
		else {

			for (int jj = 0; jj < iChannels; jj++)  
				fpO[jj][ii] = fpI[jj][ii];
		}




		// delete memory
		delete[] fpLut;
		delete[] fpCount;



}

float wxSLUT(float dif, float *lut) {

	if (dif >= (float)LUTMAXM1) return 0.0;

	int  x = (int)floor((double)dif * (float)LUTPRECISION);

	float y1 = lut[x];
	float y2 = lut[x + 1];

	return y1 + (y2 - y1)*(dif*LUTPRECISION - x);
}


int fiL2FloatDist(BYTE **u0, BYTE **u1, int i0, int j0, int i1, int j1, int radius, int channels, int width0, int width1) {

	int dif = 0;


	for (int ii = 0; ii < channels; ii++) {

		dif += fiL2FloatDist(u0[ii], u1[ii], i0, j0, i1, j1, radius, width0, width1);

	}


	return dif;
}

int fiL2FloatDist(BYTE *u0, BYTE *u1, int i0, int j0, int i1, int j1, int radius, int width0, int width1) {

	int dist = 0;
	for (int s = -radius; s <= radius; s++) {

		int l = (j0 + s)*width0 + (i0 - radius);
		BYTE *ptr0 = &u0[l];

		l = (j1 + s)*width1 + (i1 - radius);
	    BYTE *ptr1 = &u1[l];

		for (int r = -radius; r <= radius; r++, ptr0++, ptr1++) {
			int dif = (*ptr0 - *ptr1);
			dist += (dif*dif);
		}

	}

	return dist;
}

void fpClear(int *fpI, int fValue, int iLength) {
	for (int ii = 0; ii < iLength; ii++) fpI[ii] = fValue;
}

void wxFillExpLut(float *lut, int size) {
	for (int i = 0; i < size; i++)   lut[i] = expf(-(float)i / LUTPRECISION);
}

BYTE* RDlg::floatToByte(float* noisy) {
	return 0;
}

float RDlg::gauss(float fSigma) {
	double a = rand() / (double(RAND_MAX));
	double b = rand() / (double(RAND_MAX));
	double z = (double)(fSigma)*sqrt(-2.0*log(a))*cos(2.0*3.14159*b);
	return (float)z;
}

void saveImg(BYTE* data, CString strFileName) {

	BITMAPFILEHEADER bmpf;//文件头    
	BITMAPINFOHEADER bmpi;//信息头    
	int Widthbytes;
	CFile bmpFile;
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

