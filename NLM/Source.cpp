#include <string.h> 
#include <math.h>   
#include <stdio.h>   
#include <stdlib.h>   
#include <malloc.h>

#include<time.h>//时间相关头文件，可用其中函数计算图像处理速度

#define   WIDTHBYTES(bits) (((bits)+31)/32*4)//用于使图像宽度所占字节数为4byte的倍数

#define MYDRAW_HEIGHT 1054	//目标图像高度
#define MYDRAW_WIDTH 1500  //目标图像宽度

#define pi 3.1415926535

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef long LONG;

//位图文件头信息结构定义
//其中不包含文件类型信息（由于结构体的内存结构决定，要是加了的话将不能正确读取文件信息）

typedef struct tagBITMAPFILEHEADER {
	DWORD  bfSize;          //文件大小
	WORD   bfReserved1; 	//保留字，不考虑
	WORD   bfReserved2; 	//保留字，同上
	DWORD  bfOffBits;       //实际位图数据的偏移字节数，即前三个部分长度之和
} BITMAPFILEHEADER;

//信息头BITMAPINFOHEADER，也是一个结构，其定义如下：

typedef struct tagBITMAPINFOHEADER {
	//public:
	DWORD   biSize;         	//指定此结构体的长度，为40
	LONG    biWidth;       		//位图宽
	LONG    biHeight;       	//位图高
	WORD    biPlanes;       	//平面数，为1
	WORD    biBitCount;     	//采用颜色位数，可以是1，2，4，8，16，24，新的可以是32
	DWORD   biCompression;  	//压缩方式，可以是0，1，2，其中0表示不压缩
	DWORD   biSizeImage;    	//实际位图数据占用的字节数
	LONG    biXPelsPerMeter;	//X方向分辨率
	LONG    biYPelsPerMeter;	//Y方向分辨率
	DWORD   biClrUsed;      	//使用的颜色数，如果为0，则表示默认值(2^颜色位数)
	DWORD   biClrImportant; 	//重要颜色数，如果为0，则表示所有颜色都是重要的
} BITMAPINFOHEADER;

void main()
{
	long now = 0;
	now = clock();//存储图像处理开始时间

	BITMAPFILEHEADER bitHead, writebitHead;
	BITMAPINFOHEADER bitInfoHead, writebitInfoHead;
	FILE* pfile;//输入文件
	FILE* wfile;//输出文件

	char strFile[50] = "15.bmp";//打开图像路径，BMP图像必须为24位真彩色格式
	char strFilesave[50] = "16.bmp";//处理后图像存储路径
	fopen_s(&pfile, strFile, "rb");//文件打开图像
	fopen_s(&wfile, strFilesave, "wb");//打开文件为存储修改后图像做准备
									   //读取位图文件头信息
	WORD fileType;
	fread(&fileType, 1, sizeof(WORD), pfile);
	fwrite(&fileType, 1, sizeof(WORD), wfile);
	if (fileType != 0x4d42)
	{
		printf("file is not .bmp file!");
		return;
	}
	//读取位图文件头信息
	fread(&bitHead, 1, sizeof(tagBITMAPFILEHEADER), pfile);
	writebitHead = bitHead;//由于截取图像头和源文件头相似，所以先将源文件头数据赋予截取文件头
						   //读取位图信息头信息
	fread(&bitInfoHead, 1, sizeof(BITMAPINFOHEADER), pfile);
	writebitInfoHead = bitInfoHead;//同位图文件头相似

	writebitInfoHead.biHeight = MYDRAW_HEIGHT;//为截取文件重写位图高度
	writebitInfoHead.biWidth = MYDRAW_WIDTH;//为截取文件重写位图宽度
	int mywritewidth = WIDTHBYTES(writebitInfoHead.biWidth*writebitInfoHead.biBitCount);//BMP图像实际位图数据区的宽度为4byte的倍数，在此计算实际数据区宽度
	writebitInfoHead.biSizeImage = mywritewidth * writebitInfoHead.biHeight;//计算位图实际数据区大小

	writebitHead.bfSize = 54 + writebitInfoHead.biSizeImage;//位图文件头大小为位图数据区大小加上54byte
	fwrite(&writebitHead, 1, sizeof(tagBITMAPFILEHEADER), wfile);//写回位图文件头信息到输出文件
	fwrite(&writebitInfoHead, 1, sizeof(BITMAPINFOHEADER), wfile);//写回位图信息头信息到输出文件

	int width = bitInfoHead.biWidth;
	int height = bitInfoHead.biHeight;
	//分配内存空间把源图存入内存   
	int l_width = WIDTHBYTES(width*bitInfoHead.biBitCount);//计算位图的实际宽度并确保它为4byte的倍数
	int write_width = WIDTHBYTES(writebitInfoHead.biWidth*writebitInfoHead.biBitCount);//计算写位图的实际宽度并确保它为4byte的倍数

	BYTE    *pColorData = (BYTE *)malloc(height*l_width);//开辟内存空间存储图像数据
	memset(pColorData, 0, height*l_width);

	BYTE    *pColorDataMid = (BYTE *)malloc(mywritewidth*MYDRAW_HEIGHT);//开辟内存空间存储图像处理之后数据
	memset(pColorDataMid, 0, mywritewidth*MYDRAW_HEIGHT);

	long nData = height * l_width;
	long write_nData = mywritewidth * MYDRAW_HEIGHT;//截取的位图数据区长度定义

													//把位图数据信息读到数组里   
	fread(pColorData, 1, nData, pfile);//图像处理可通过操作这部分数据加以实现

									   /*******************图像处理部分******************/
									   /*******************图像旋转******************/
	int centerofrotation_x = MYDRAW_WIDTH / 2;//旋转中心x坐标
	int centerofrotation_y = MYDRAW_HEIGHT / 2;//旋转中心y坐标
	double degree = 60;//逆时针旋转角度
	double radian = degree / 180 * pi;//角度转弧度
	for (int hnum = 0; hnum < MYDRAW_HEIGHT; hnum++)
		for (int wnum = 0; wnum < MYDRAW_WIDTH; wnum++)
		{
			int pixel_point = hnum * write_width + wnum * 3;//映射尺度变换图像数组位置偏移量

			double d_original_img_wnum = (wnum - centerofrotation_x)*cos(radian) - (hnum - centerofrotation_y)*sin(radian) + centerofrotation_x;
			double d_original_img_hnum = (wnum - centerofrotation_x)*sin(radian) + (hnum - centerofrotation_y)*cos(radian) + centerofrotation_y;

			if (d_original_img_wnum<0 || d_original_img_wnum>width || d_original_img_hnum<0 || d_original_img_hnum>height)//找不到与原图像的对于关系
			{
				pColorDataMid[pixel_point] = pColorDataMid[pixel_point + 1] = pColorDataMid[pixel_point + 2] = 0;
				continue;
			}

			int i_original_img_hnum = d_original_img_hnum;
			int i_original_img_wnum = d_original_img_wnum;
			double distance_to_a_x = d_original_img_wnum - i_original_img_wnum;//在原图像中与a点的水平距离
			double distance_to_a_y = d_original_img_hnum - i_original_img_hnum;//在原图像中与a点的垂直距离

			int original_point_a = i_original_img_hnum * l_width + i_original_img_wnum * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点,相当于点A  
			int original_point_b = i_original_img_hnum * l_width + (i_original_img_wnum + 1) * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点,相当于点B
			int original_point_c = (i_original_img_hnum + 1)*l_width + i_original_img_wnum * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点,相当于点C 
			int original_point_d = (i_original_img_hnum + 1)*l_width + (i_original_img_wnum + 1) * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点,相当于点D 
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
	/*******************图像旋转******************/
	/*******************图像处理部分******************/

	fwrite(pColorDataMid, 1, write_nData, wfile);   //将处理完图像数据区写回文件
	fclose(pfile);
	fclose(wfile);

	printf("图像处理完成\n");
	printf("运行时间为：%dms\n", int(((double)(clock() - now)) / CLOCKS_PER_SEC * 1000));//输出图像处理花费时间信息
}