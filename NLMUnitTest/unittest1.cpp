#include "stdafx.h"
#include "CppUnitTest.h"

#include <time.h>
#include "testcode.h"



using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NLMUnitTest
{		
	TEST_CLASS(UnitTestDisplay)
	{
	public:
		NLMTest * test;
		UnitTestDisplay::UnitTestDisplay(){
			test = new NLMTest;
		}

		TEST_METHOD(Testblur1)
		{
			Assert* as = new Assert();
			int m = 256;
			int n = 256;
			unsigned char* a = new unsigned char[m*n];
			for(int i=0;i<m;i++)
				for (int j = 0; j < n; j++) {
					a[i*n + j] = rand() % 256;
				}
			int* b = test->blur(25, m, n, a, "abcd");
			for (int i = 0; i < m*n; i++) {
				if (b[i] > 255)
					as->Fail();
			}
			as->AreEqual(2, 2);
		}

		TEST_METHOD(Testblur2) {
			Assert* as = new Assert();
			int m = 0;
			int n = 0;

			unsigned char* a = NULL;
			for (int i = 0; i<m; i++)
				for (int j = 0; j < n; j++) {
					a[i*n + j] = rand() % 256;
				}
			int* b = test->blur(25, m, n, a, "abcd");
			as->IsNull(b);
		}

		TEST_METHOD(Testblur3) {
			Assert* as = new Assert();
			int m = 256;
			int n = 256;
			unsigned char* a = new unsigned char[m*n];
			for (int i = 0; i<m; i++)
				for (int j = 0; j < n; j++) {
						a[i*n + j] = rand() % 256;
				}
			int* b = test->blur(-25, m, n, a, "abcd");
			as->IsNull(b);
		}

		TEST_METHOD(TestGauss1) {
			Assert* as = new Assert();
			float fSigma = -21.4f;
			float temp = 0;
			temp = test->gauss(fSigma);
			as->AreEqual(temp,0.0f);
		}

		TEST_METHOD(TestDenoise1) {
			Assert* as = new Assert();
			int win = 1;
			int bloc = 10;
			float fFiltPar = 0.55f;
			int m = 256;
			int n = 256;
			int d_c = 1;
			int d_wh = m * n;
			int d_whc = m * n*d_c;
			unsigned char* a = new unsigned char[m*n];
			for (int i = 0; i<m; i++)
				for (int j = 0; j < n; j++) {
					a[i*n + j] = rand() % 256;
				}
			unsigned char **fpI = new unsigned char*[d_c];
			int **fpO = new int*[d_c];
			for (int i = 0; i < d_c; i++) {
				fpI[i] = new unsigned char[d_wh];
				fpO[i] = new int[d_wh];
			}

			unsigned char *denoised = new unsigned char[d_whc];
			for (int i = 0; i < d_wh; i++) {
				for (int j = 0; j < d_c; j++) {
					fpI[j][i] = a[3 * i + j];
					fpO[j][i] = 0;
				}
			}

			test->denoise(win, bloc, 25.0f, fFiltPar, fpI, fpO, 1, m, n);
			
			for (int i = 0; i < d_wh; i++) {
				for (int j = 0; j < d_c; j++) {
					if(fpO[j][i]>255||fpO[j][i]<0)
						as->Fail();
				}
			}
			as->AreEqual(2, 2);
		}

		TEST_METHOD(TestDenoise2) {
			Assert* as = new Assert();
			int win = 1;
			int bloc = 10;
			float fFiltPar = 0.55f;
			int m = 0;
			int n = 0;
			int d_c = 1;
			int d_wh = m * n;
			int d_whc = m * n*d_c;
			unsigned char* a = new unsigned char[m*n];
			for (int i = 0; i<m; i++)
				for (int j = 0; j < n; j++) {
					a[i*n + j] = rand() % 256;
				}
			unsigned char **fpI = new unsigned char*[d_c];
			int **fpO = new int*[d_c];
			for (int i = 0; i < d_c; i++) {
				fpI[i] = new unsigned char[d_wh];
				fpO[i] = new int[d_wh];
			}

			unsigned char *denoised = new unsigned char[d_whc];
			for (int i = 0; i < d_wh; i++) {
				for (int j = 0; j < d_c; j++) {
					fpI[j][i] = a[3 * i + j];
					fpO[j][i] = 0;
				}
			}

			test->denoise(win, bloc, 25.0f, fFiltPar, fpI, fpO, 1, m, n);

			for (int i = 0; i < d_wh; i++) {
				for (int j = 0; j < d_c; j++) {
					if (fpO[j][i]>255 || fpO[j][i]<0)
						as->Fail();
				}
			}
			as->AreEqual(2, 2);
		}


		TEST_METHOD(TestDenoise3) {
			Assert* as = new Assert();
			int win = 1;
			int bloc = 10;
			float fFiltPar = 0.55f;
			int m = 10;
			int n = 10;
			int d_c = 3;
			int d_wh = m * n;
			int d_whc = m * n*d_c;
			unsigned char* a = new unsigned char[m*n];
			for (int i = 0; i<m; i++)
				for (int j = 0; j < n; j++) {
					for (int k = 0; k < d_c; k++) {
						a[i*n + j * d_c + k] = rand() % 256;
					}
				}

			unsigned char** fpI = new unsigned char* [d_c];
			int **fpO = new int*[d_c];
			for (int i = 0; i < d_c; i++) {
				fpI[i] = new unsigned char[d_wh];
				fpO[i] = new int[d_wh];
			}

			unsigned char *denoised = new unsigned char[d_whc];
			for (int i = 0; i < d_wh; i++) {
				for (int j = 0; j < d_c; j++) {
					fpI[j][i] = a[3 * i + j];
					fpO[j][i] = 0;
				}
			}

			test->denoise(win, bloc, 25.0f, fFiltPar, fpI, fpO, 1, m, n);

			for (int i = 0; i < d_wh; i++) {
				for (int j = 0; j < d_c; j++) {
					if (fpO[j][i]>255 || fpO[j][i]<0)
						as->Fail();
				}
			}
			as->AreEqual(2, 2);
		}

	};

}