
// NLMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NLM.h"
#include "NLMDlg.h"
#include "RDlg.h"
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

#define   WIDTHBYTES(bits) (((bits)+31)/32*4)//用于使图像宽度所占字节数为4byte的倍数
int write_width;
CString strFilePath;
CString strFileName;
BITMAPINFO* pBmpInfo;  //记录图像细节
BYTE* pBmpData;        //图像数据
int iBmpWidth;
int iBmpHeight;



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CNLMDlg dialog



CNLMDlg::CNLMDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NLM_DIALOG, pParent)
	, m_FilePath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNLMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_FilePath);
}

BEGIN_MESSAGE_MAP(CNLMDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, &CNLMDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CNLMDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CNLMDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CNLMDlg message handlers

BOOL CNLMDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNLMDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNLMDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{

		CDialogEx::OnPaint();
		Pain();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNLMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}







void CNLMDlg::OnBnClickedButton2()
{
	BOOL isOpen = TRUE;
	CFileDialog dlg(isOpen, "*.BMP", NULL, NULL, "位图文件(*.BMP)|*.bmp;*.BMP|", this);
	if (!dlg.DoModal() == IDOK) return;
	strFileName = dlg.GetPathName();   //以只读的方式打开文件 
	SetDlgItemText(IDC_EDIT1, strFileName);
	Pain();
}

void CNLMDlg::Pain() {

	BITMAPFILEHEADER bmpHeader;//文件头    
	BITMAPINFOHEADER bmpInfo;//信息头    
	CFile bmpFile;//记录打开文件         
	if (!bmpFile.Open(strFileName, CFile::modeRead | CFile::typeBinary)) return;
	if (bmpFile.Read(&bmpHeader, sizeof(BITMAPFILEHEADER)) != sizeof(BITMAPFILEHEADER))
	{
		AfxMessageBox("read bmp header failed!");
		return;
	}  if (bmpHeader.bfType != 0x4d42)
	{
		AfxMessageBox("invalid file type!");   return;
	}
	if (bmpFile.Read(&bmpInfo, sizeof(BITMAPINFOHEADER)) != sizeof(BITMAPINFOHEADER))
	{
		AfxMessageBox("read bmp infor header failed!");
		return;
	}  if (bmpInfo.biBitCount != 24)
	{
		AfxMessageBox("File is not 24 bit.Application doesn't support this kind of file!");
		return;
	}
	write_width = WIDTHBYTES(bmpInfo.biWidth*bmpInfo.biBitCount);

	pBmpInfo = (BITMAPINFO *)new char[sizeof(BITMAPINFOHEADER)];
	if (!pBmpInfo)
	{
		AfxMessageBox("memory error!");
		return;
	}
	//为图像数据申请空间    
	memcpy(pBmpInfo, &bmpInfo, sizeof(BITMAPINFOHEADER));
	DWORD dataBytes = bmpHeader.bfSize - bmpHeader.bfOffBits;
	pBmpData = (BYTE*)new char[dataBytes];
	if (!pBmpData)
	{
		AfxMessageBox("memory error!");
		delete pBmpData;   return;
	}  if (bmpFile.Read(pBmpData, dataBytes) != dataBytes)
	{
		AfxMessageBox("Read bmp data failed!");
		delete pBmpInfo;
		delete pBmpData;
		return;
	}
	bmpFile.Close();
	CWnd *pWnd = GetDlgItem(IDC_PICTURE);//获得pictrue控件窗口的句柄    
	CRect rect;
	pWnd->GetClientRect(&rect);//获得pictrue控件所在的矩形区域    
	CDC *pDC = pWnd->GetDC();//获得pictrue控件的DC       
							 //显示图片    
	pDC->SetStretchBltMode(COLORONCOLOR);
	StretchDIBits(pDC->GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), 0, 0, bmpInfo.biWidth, bmpInfo.biHeight, pBmpData, pBmpInfo, DIB_RGB_COLORS, SRCCOPY);
	iBmpWidth = bmpInfo.biWidth;
	iBmpHeight = bmpInfo.biHeight; // TODO: Add extra validation here 
}




void CNLMDlg::OnBnClickedButton1()
{
	RDlg rdlg;
	rdlg.DoModal(); 

	strFileName = "noise.bmp";  //以只读的方式打开文件  
	Pain();
}


void CNLMDlg::OnBnClickedButton3()
{
	deblur(BlurRadius);
	// TODO: Add your control notification handler code here
	strFileName = "denoise.bmp";  //以只读的方式打开文件    
	Pain();
}
