//����ÿ�����ر���
void denoise(int iDWin, 
	int iDBloc,           // �������ڵİ뾶
	float fSigma,         // Sigma������������ʵĲ�����
	float fFiltPar,       // �˲�����
	BYTE **fpI,          // ����������
	int **fpO,          // �������
	int iChannels, int iWidth, int iHeight) {
	for (int y = 0; y < iHeight; y++) {
		for (int x = 0; x < iWidth; x++) {
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
						float fWeight = wxSLUT(fDif, fpLut)��
							if (fWeight > fMaxWeight) fMaxWeight = fWeight;
						fTotalWeight += fWeight��
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
		}
	}
}
