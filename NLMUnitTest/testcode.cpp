#include<stdlib.h>
#include "stdafx.h"
#include "testcode.h"

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

#define   WIDTHBYTES(bits) (((bits)+31)/32*4)//����ʹͼ������ռ�ֽ���Ϊ4byte�ı���

using namespace std;

void wxFillExpLut(float *lut, int size);
void fpClear(int *fpI, int fValue, int iLength);
int fiL2FloatDist(unsigned char **u0, unsigned char **u1, int i0, int j0, int i1, int j1, int radius, int channels, int width0, int width1);
int fiL2FloatDist(unsigned char *u0, unsigned char *u1, int i0, int j0, int i1, int j1, int radius, int width0, int width1);
float wxSLUT(float dif, float *lut);




float NLMTest::gauss(float fSigma) {
	if (fSigma<0) {
		printf("error :: The variance is less than 0. \n");
		return NULL;
	}

	double a = rand() / (double(RAND_MAX));
	double b = rand() / (double(RAND_MAX));
	double z = (double)(fSigma)*sqrt(-2.0*log(a))*cos(2.0*3.14159*b);
	return (float)z;
}

int* NLMTest::blur(float BlurRadius, int iBmpWidth, int iBmpHeight, unsigned char* pBmpData, char* strFilePath) {
	size_t nx, ny, nc;


	if (!pBmpData) {
		printf("error :: %s not found  or not a correct png image \n", strFilePath);
		return NULL;
	}
	if (BlurRadius<0.0f) {
		printf("error :: The variance is less than 0. \n");
		return NULL;
	}


	// variables
	int d_w = iBmpWidth;
	int d_h = iBmpHeight;
	int d_c = 1;

	if (d_c == 2) {
		d_c = 1;
	}
	if (d_c > 3) {
		d_c = 3;
	}

	int d_wh = d_w * d_h;
	int d_whc = d_c * d_w * d_h;

	if (d_c > 1) {

		int i = 0;

		while (i < d_wh && pBmpData[i] == pBmpData[d_wh + i] && pBmpData[i] == pBmpData[2 * d_wh + i]) {
			i++;
		}

		if (i == d_wh) d_c = 1;

	}

	// ��Ӹ�˹����
	float fSigma = BlurRadius;
	int a = 0;
	int b = 1;
	int* noisy = new int[d_whc];


	if (d_c == 3)
		for (int i = 0; i < d_whc; i += 3) {
			int re = gauss(fSigma);
			int a = floor(pBmpData[i] + re);
			int b = floor(pBmpData[i + 1] + re);
			int c = floor(pBmpData[i + 2] + re);
			if (a > 255)
				noisy[i] = 255;
			else if (a < 0)
				noisy[i] = 0;
			else
				noisy[i] = a;

			if (b > 255)
				noisy[i + 1] = 255;
			else if (b < 0)
				noisy[i + 1] = 0;
			else
				noisy[i + 1] = b;

			if (c > 255)
				noisy[i + 2] = 255;
			else if (c < 0)
				noisy[i + 2] = 0;
			else
				noisy[i + 2] = c;


		}
	else
		for (int i = 0; i < d_whc; i += 1) {
			int re = gauss(fSigma);
			int a = floor(pBmpData[i] + re);
			if (a > 255)
				noisy[i] = 255;
			else if (a < 0)
				noisy[i] = 0;
			else
				noisy[i] = a;

		}
	return noisy;

}

void NLMTest::denoise(int iDWin,            // patch�İ뾶
	int iDBloc,           // �������ڵİ뾶
	float fSigma,         // Sigma������������ʵĲ�����
	float fFiltPar,       // �˲�����
	unsigned char **fpI,          // ����������
	int **fpO,          // �������
	int iChannels, int iWidth, int iHeight) {




	// ÿһ��channel�Ĵ�С
	int iwxh = iWidth * iHeight;


	//  �Ƚϴ��ڵĴ�С
	int ihwl = (2 * iDWin + 1);
	int iwl = (2 * iDWin + 1) * (2 * iDWin + 1);
	int icwl = iChannels * iwl;



	// �˲��Ĳ���
	float fSigma2 = fSigma * fSigma;
	float fH = fFiltPar * fSigma;
	float fH2 = fH * fH;


	fH2 *= (float)icwl;


	int iLutLength = (int)rintf((float)LUTMAX * (float)LUTPRECISION);
	float *fpLut = new float[iLutLength];
	wxFillExpLut(fpLut, iLutLength);




	//��ʱ����
	int *fpCount = new int[iwxh];
	fpClear(fpCount, 0, iwxh);



	//����ÿ�����ر���
#pragma omp parallel shared(fpI, fpO)
	{
#pragma omp for schedule(dynamic) nowait
		for (int y = 0; y < iHeight; y++) {

			int **fpODenoised = new int*[iChannels];
			for (int ii = 0; ii < iChannels; ii++) fpODenoised[ii] = new int[iwl];

			for (int x = 0; x < iWidth; x++) {

				// ��С�Ƚϴ��ڵĴ�С
				int iDWin0 = MIN(iDWin, MIN(iWidth - 1 - x, MIN(iHeight - 1 - y, MIN(x, y))));

				// �߽紦��
				int imin = MAX(x - iDBloc, iDWin0);
				int jmin = MAX(y - iDBloc, iDWin0);

				int imax = MIN(x + iDBloc, iWidth - 1 - iDWin0);
				int jmax = MIN(y + iDBloc, iHeight - 1 - iDWin0);

				//  ��ʼ������
				for (int ii = 0; ii < iChannels; ii++) fpClear(fpODenoised[ii], 0.0f, iwl);

				// weights�����ֵ
				float fMaxWeight = 0.0f;

				// weights�ĺ�
				float fTotalWeight = 0.0f;

				for (int j = jmin; j <= jmax; j++)
					for (int i = imin; i <= imax; i++)
						if (i != x || j != y) {

							double fDif = fiL2FloatDist(fpI, fpI, x, y, i, j, iDWin0, iChannels, iWidth, iWidth);

							// dif^2 - 2 * fSigma^2 * N 
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

				// ���Ȩ�صĵ�ǰpatch
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







				// ��һ��
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



		delete[] fpLut;
		delete[] fpCount;



}

void fpClear(int *fpI, int fValue, int iLength) {
	for (int ii = 0; ii < iLength; ii++) fpI[ii] = fValue;
}

void wxFillExpLut(float *lut, int size) {
	for (int i = 0; i < size; i++)   lut[i] = expf(-(float)i / LUTPRECISION);
}

float wxSLUT(float dif, float *lut) {

	if (dif >= (float)LUTMAXM1) return 0.0;

	int  x = (int)floor((double)dif * (float)LUTPRECISION);

	float y1 = lut[x];
	float y2 = lut[x + 1];

	return y1 + (y2 - y1)*(dif*LUTPRECISION - x);
}


int fiL2FloatDist(unsigned char **u0, unsigned char **u1, int i0, int j0, int i1, int j1, int radius, int channels, int width0, int width1) {

	int dif = 0;


	for (int ii = 0; ii < channels; ii++) {

		dif += fiL2FloatDist(u0[ii], u1[ii], i0, j0, i1, j1, radius, width0, width1);

	}


	return dif;
}

int fiL2FloatDist(unsigned char *u0, unsigned char *u1, int i0, int j0, int i1, int j1, int radius, int width0, int width1) {

	int dist = 0;
	for (int s = -radius; s <= radius; s++) {

		int l = (j0 + s)*width0 + (i0 - radius);
		unsigned char *ptr0 = &u0[l];

		l = (j1 + s)*width1 + (i1 - radius);
		unsigned char *ptr1 = &u1[l];

		for (int r = -radius; r <= radius; r++, ptr0++, ptr1++) {
			int dif = (*ptr0 - *ptr1);
			dist += (dif*dif);
		}

	}

	return dist;
}