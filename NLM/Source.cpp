#include <string.h> 
#include <math.h>   
#include <stdio.h>   
#include <stdlib.h>   
#include <malloc.h>

#include<time.h>//ʱ�����ͷ�ļ����������к�������ͼ�����ٶ�

#define   WIDTHBYTES(bits) (((bits)+31)/32*4)//����ʹͼ������ռ�ֽ���Ϊ4byte�ı���

#define MYDRAW_HEIGHT 1054	//Ŀ��ͼ��߶�
#define MYDRAW_WIDTH 1500  //Ŀ��ͼ����

#define pi 3.1415926535

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef long LONG;

//λͼ�ļ�ͷ��Ϣ�ṹ����
//���в������ļ�������Ϣ�����ڽṹ����ڴ�ṹ������Ҫ�Ǽ��˵Ļ���������ȷ��ȡ�ļ���Ϣ��

typedef struct tagBITMAPFILEHEADER {
	DWORD  bfSize;          //�ļ���С
	WORD   bfReserved1; 	//�����֣�������
	WORD   bfReserved2; 	//�����֣�ͬ��
	DWORD  bfOffBits;       //ʵ��λͼ���ݵ�ƫ���ֽ�������ǰ�������ֳ���֮��
} BITMAPFILEHEADER;

//��ϢͷBITMAPINFOHEADER��Ҳ��һ���ṹ���䶨�����£�

typedef struct tagBITMAPINFOHEADER {
	//public:
	DWORD   biSize;         	//ָ���˽ṹ��ĳ��ȣ�Ϊ40
	LONG    biWidth;       		//λͼ��
	LONG    biHeight;       	//λͼ��
	WORD    biPlanes;       	//ƽ������Ϊ1
	WORD    biBitCount;     	//������ɫλ����������1��2��4��8��16��24���µĿ�����32
	DWORD   biCompression;  	//ѹ����ʽ��������0��1��2������0��ʾ��ѹ��
	DWORD   biSizeImage;    	//ʵ��λͼ����ռ�õ��ֽ���
	LONG    biXPelsPerMeter;	//X����ֱ���
	LONG    biYPelsPerMeter;	//Y����ֱ���
	DWORD   biClrUsed;      	//ʹ�õ���ɫ�������Ϊ0�����ʾĬ��ֵ(2^��ɫλ��)
	DWORD   biClrImportant; 	//��Ҫ��ɫ�������Ϊ0�����ʾ������ɫ������Ҫ��
} BITMAPINFOHEADER;

void main()
{
	long now = 0;
	now = clock();//�洢ͼ����ʼʱ��

	BITMAPFILEHEADER bitHead, writebitHead;
	BITMAPINFOHEADER bitInfoHead, writebitInfoHead;
	FILE* pfile;//�����ļ�
	FILE* wfile;//����ļ�

	char strFile[50] = "15.bmp";//��ͼ��·����BMPͼ�����Ϊ24λ���ɫ��ʽ
	char strFilesave[50] = "16.bmp";//�����ͼ��洢·��
	fopen_s(&pfile, strFile, "rb");//�ļ���ͼ��
	fopen_s(&wfile, strFilesave, "wb");//���ļ�Ϊ�洢�޸ĺ�ͼ����׼��
									   //��ȡλͼ�ļ�ͷ��Ϣ
	WORD fileType;
	fread(&fileType, 1, sizeof(WORD), pfile);
	fwrite(&fileType, 1, sizeof(WORD), wfile);
	if (fileType != 0x4d42)
	{
		printf("file is not .bmp file!");
		return;
	}
	//��ȡλͼ�ļ�ͷ��Ϣ
	fread(&bitHead, 1, sizeof(tagBITMAPFILEHEADER), pfile);
	writebitHead = bitHead;//���ڽ�ȡͼ��ͷ��Դ�ļ�ͷ���ƣ������Ƚ�Դ�ļ�ͷ���ݸ����ȡ�ļ�ͷ
						   //��ȡλͼ��Ϣͷ��Ϣ
	fread(&bitInfoHead, 1, sizeof(BITMAPINFOHEADER), pfile);
	writebitInfoHead = bitInfoHead;//ͬλͼ�ļ�ͷ����

	writebitInfoHead.biHeight = MYDRAW_HEIGHT;//Ϊ��ȡ�ļ���дλͼ�߶�
	writebitInfoHead.biWidth = MYDRAW_WIDTH;//Ϊ��ȡ�ļ���дλͼ���
	int mywritewidth = WIDTHBYTES(writebitInfoHead.biWidth*writebitInfoHead.biBitCount);//BMPͼ��ʵ��λͼ�������Ŀ��Ϊ4byte�ı������ڴ˼���ʵ�����������
	writebitInfoHead.biSizeImage = mywritewidth * writebitInfoHead.biHeight;//����λͼʵ����������С

	writebitHead.bfSize = 54 + writebitInfoHead.biSizeImage;//λͼ�ļ�ͷ��СΪλͼ��������С����54byte
	fwrite(&writebitHead, 1, sizeof(tagBITMAPFILEHEADER), wfile);//д��λͼ�ļ�ͷ��Ϣ������ļ�
	fwrite(&writebitInfoHead, 1, sizeof(BITMAPINFOHEADER), wfile);//д��λͼ��Ϣͷ��Ϣ������ļ�

	int width = bitInfoHead.biWidth;
	int height = bitInfoHead.biHeight;
	//�����ڴ�ռ��Դͼ�����ڴ�   
	int l_width = WIDTHBYTES(width*bitInfoHead.biBitCount);//����λͼ��ʵ�ʿ�Ȳ�ȷ����Ϊ4byte�ı���
	int write_width = WIDTHBYTES(writebitInfoHead.biWidth*writebitInfoHead.biBitCount);//����дλͼ��ʵ�ʿ�Ȳ�ȷ����Ϊ4byte�ı���

	BYTE    *pColorData = (BYTE *)malloc(height*l_width);//�����ڴ�ռ�洢ͼ������
	memset(pColorData, 0, height*l_width);

	BYTE    *pColorDataMid = (BYTE *)malloc(mywritewidth*MYDRAW_HEIGHT);//�����ڴ�ռ�洢ͼ����֮������
	memset(pColorDataMid, 0, mywritewidth*MYDRAW_HEIGHT);

	long nData = height * l_width;
	long write_nData = mywritewidth * MYDRAW_HEIGHT;//��ȡ��λͼ���������ȶ���

													//��λͼ������Ϣ����������   
	fread(pColorData, 1, nData, pfile);//ͼ�����ͨ�������ⲿ�����ݼ���ʵ��

									   /*******************ͼ������******************/
									   /*******************ͼ����ת******************/
	int centerofrotation_x = MYDRAW_WIDTH / 2;//��ת����x����
	int centerofrotation_y = MYDRAW_HEIGHT / 2;//��ת����y����
	double degree = 60;//��ʱ����ת�Ƕ�
	double radian = degree / 180 * pi;//�Ƕ�ת����
	for (int hnum = 0; hnum < MYDRAW_HEIGHT; hnum++)
		for (int wnum = 0; wnum < MYDRAW_WIDTH; wnum++)
		{
			int pixel_point = hnum * write_width + wnum * 3;//ӳ��߶ȱ任ͼ������λ��ƫ����

			double d_original_img_wnum = (wnum - centerofrotation_x)*cos(radian) - (hnum - centerofrotation_y)*sin(radian) + centerofrotation_x;
			double d_original_img_hnum = (wnum - centerofrotation_x)*sin(radian) + (hnum - centerofrotation_y)*cos(radian) + centerofrotation_y;

			if (d_original_img_wnum<0 || d_original_img_wnum>width || d_original_img_hnum<0 || d_original_img_hnum>height)//�Ҳ�����ԭͼ��Ķ��ڹ�ϵ
			{
				pColorDataMid[pixel_point] = pColorDataMid[pixel_point + 1] = pColorDataMid[pixel_point + 2] = 0;
				continue;
			}

			int i_original_img_hnum = d_original_img_hnum;
			int i_original_img_wnum = d_original_img_wnum;
			double distance_to_a_x = d_original_img_wnum - i_original_img_wnum;//��ԭͼ������a���ˮƽ����
			double distance_to_a_y = d_original_img_hnum - i_original_img_hnum;//��ԭͼ������a��Ĵ�ֱ����

			int original_point_a = i_original_img_hnum * l_width + i_original_img_wnum * 3;//����λ��ƫ��������Ӧ��ͼ��ĸ����ص�RGB�����,�൱�ڵ�A  
			int original_point_b = i_original_img_hnum * l_width + (i_original_img_wnum + 1) * 3;//����λ��ƫ��������Ӧ��ͼ��ĸ����ص�RGB�����,�൱�ڵ�B
			int original_point_c = (i_original_img_hnum + 1)*l_width + i_original_img_wnum * 3;//����λ��ƫ��������Ӧ��ͼ��ĸ����ص�RGB�����,�൱�ڵ�C 
			int original_point_d = (i_original_img_hnum + 1)*l_width + (i_original_img_wnum + 1) * 3;//����λ��ƫ��������Ӧ��ͼ��ĸ����ص�RGB�����,�൱�ڵ�D 
			if (i_original_img_hnum == MYDRAW_HEIGHT - 1)
			{
				original_point_c = original_point_a;
				original_point_d = original_point_b;
			}
			if (i_original_img_wnum == MYDRAW_WIDTH - 1)
			{
				original_point_a = original_point_b;
				original_point_c = original_point_d;
			}

			pColorDataMid[pixel_point] =
				pColorData[original_point_a] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
				pColorData[original_point_b] * distance_to_a_x*(1 - distance_to_a_y) +
				pColorData[original_point_c] * distance_to_a_y*(1 - distance_to_a_x) +
				pColorData[original_point_c] * distance_to_a_y*distance_to_a_x;
			pColorDataMid[pixel_point + 1] =
				pColorData[original_point_a + 1] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
				pColorData[original_point_b + 1] * distance_to_a_x*(1 - distance_to_a_y) +
				pColorData[original_point_c + 1] * distance_to_a_y*(1 - distance_to_a_x) +
				pColorData[original_point_c + 1] * distance_to_a_y*distance_to_a_x;
			pColorDataMid[pixel_point + 2] =
				pColorData[original_point_a + 2] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
				pColorData[original_point_b + 2] * distance_to_a_x*(1 - distance_to_a_y) +
				pColorData[original_point_c + 2] * distance_to_a_y*(1 - distance_to_a_x) +
				pColorData[original_point_c + 2] * distance_to_a_y*distance_to_a_x;

		}
	/*******************ͼ����ת******************/
	/*******************ͼ������******************/

	fwrite(pColorDataMid, 1, write_nData, wfile);   //��������ͼ��������д���ļ�
	fclose(pfile);
	fclose(wfile);

	printf("ͼ�������\n");
	printf("����ʱ��Ϊ��%dms\n", int(((double)(clock() - now)) / CLOCKS_PER_SEC * 1000));//���ͼ������ʱ����Ϣ
}