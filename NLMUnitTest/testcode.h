#pragma once
class NLMTest
{
public:

	float gauss(float fSigma);


	int* blur(float BlurRadius, int iBmpWidth, int iBmpHeight, unsigned char *pBmpData, char* strFilePath);

	void denoise(int iDWin, int iDBloc, float fSigma, float fFiltPar, unsigned char ** fpI, int ** fpO, int iChannels, int iWidth, int iHeight);


};