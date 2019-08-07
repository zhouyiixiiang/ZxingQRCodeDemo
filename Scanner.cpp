#include "stdafx.h"

# include <afx.h>
#include <string>
#include <fstream>
#include <qmessagebox.h>
# include "Scanner.h"
# include"zxing-QRcode.h"
//#include "stdafx.h"
#include <queue>
#include <mutex>

#include <zxing/LuminanceSource.h>
#include <zxing/common/Counted.h>
#include <zxing/Reader.h>
#include <zxing/aztec/AztecReader.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/common/HybridBinarizer.h>
#include <zxing/DecodeHints.h>
#include <zxing/datamatrix/DataMatrixReader.h>
#include <zxing/MultiFormatReader.h>
#include <zxing/pdf417/PDF417Reader.h>
#include <zxing/qrcode/QRCodeReader.h>
#include <zxing/oned/CodaBarReader.h>
#include <zxing/oned/Code39Reader.h>
#include <zxing/oned/Code93Reader.h>
#include <zxing/oned/Code128Reader.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include "MatSource.h"

#include "defqueue.h"
#include "Demo.h"
using namespace std;
//#include "StdAfx.h"
#define GetModuleHandle  GetModuleHandleA
#define GetModuleFileName  GetModuleFileNameA
#include"logFile.h"
//INITIALIZE_EASYLOGGINGPP



////定义队列-----------------------------------------------------------
//
//queue<CString> QueueOfImages; //定义一个队列来放Image的name
//mutex mtx; //定义一个互斥锁来保证线程对队列进行入队、出队操作加锁
//void pushImage(CString ImageName)
//{
//		//if QueueOfImages == 0;
//		mtx.lock();
//		QueueOfImages.push(ImageName);
//		mtx.unlock();
//}
//CString popImage()
//{
//	while (!QueueOfImages.empty())
//	{
//		mtx.lock();
//		CString ImageName;
//		ImageName = QueueOfImages.front();
//		QueueOfImages.pop();
//		mtx.unlock();
//		return ImageName;
//	}
//}
//
////--------------------------------------------------------------------


HMODULE hModule;
HMODULE hLibrary;


//*************************************************************************************************
// 回调函数声明
typedef void(_stdcall* SCAN_INFO_CALLBACK)(int, char*);
typedef void(_stdcall* SCAN_FINISH_CALLBACK)(int);

//*************************************************************************************************
// 图像信息结构体
typedef struct ImageDataAndInfo
{
	unsigned char* PicBuffer;
	long  lPicSize;
	int  iPicType;
} *PImageDataAndInfo;

//*************************************************************************************************
// 通信动态库导出函数声明
typedef int(_stdcall* pOpenScanner)(SCAN_INFO_CALLBACK, SCAN_FINISH_CALLBACK);
typedef int(_stdcall* pOpenScannerEx)(SCAN_INFO_CALLBACK, SCAN_FINISH_CALLBACK, int);
typedef int(_stdcall* pCloseScanner)();
typedef int(_stdcall* pStartScan)(unsigned int);
typedef int(_stdcall* pAbortScan)();
typedef int(_stdcall* pGetImages)(PImageDataAndInfo, PImageDataAndInfo);
typedef int(_stdcall* pSetIniFilePath)(char*);
typedef int(_stdcall* pGetDecode)(char**, int*, int*);
//*************************************************************************************************
// 定义通讯库导出函数
pOpenScanner f_OpenScanner = NULL;
pOpenScannerEx f_OpenScannerEx = NULL;
pCloseScanner f_CloseScanner = NULL;
pStartScan f_StartScan = NULL;
pAbortScan f_AbortScan = NULL;
pGetImages f_GetImages = NULL;
pSetIniFilePath f_SetIniFilePath = NULL;
pGetDecode f_GetDecode = NULL;
//*************************************************************************************************
// 图像张数
int g_nNumberOfImage;
//*************************************************************************************************
// 是否正在扫描
BOOL bIsScanning = FALSE;
CString m_FilePath_i;



//将LPCWSTR转换为LPCSTR
char* wtoc(wchar_t* wText)
{
	DWORD dwNum = WideCharToMultiByte(CP_ACP, NULL, wText, -1, NULL, 0, NULL, FALSE);//把第五个参数设成NULL的到宽字符串的长度包括结尾符
	char* psText = NULL;
	psText = new char[dwNum];
	if (!psText)
	{
		delete[]psText;
		psText = NULL;
	}
	WideCharToMultiByte(CP_ACP, NULL, wText, -1, psText, dwNum, NULL, FALSE);
	return psText;
}

Scanner* pDemoDlg;

// 回调函数过程
void _stdcall ScanInfoCallBack(int nPos, char* pInfo)
{
	if (pInfo != "")
	{
		//wndStatusBar.SetText(pInfo, 0, 0);
	}
}

/**************************************************************************************************
* ScanFinish()
* 参数:
	[in] nFlag扫描标志位
* 返回值:
	无
* 说明:
	扫描完成回调函数
**************************************************************************************************/
void _stdcall ScanFinish(int nFlag)
{
	ImageDataAndInfo PicFront, PicRear;
	CString strImageNameF, strImageNameR;
	DWORD dwRelWrite;
	HANDLE hFile;
	// 译码结果
	char* pDecodeResult;
	int DecodeLength;
	int DecodeType;

	CString strTemp;

	g_nNumberOfImage++;


	//*************************************************************************************************
	// 检验参数合法性
	if (nFlag < PAGE_END || nFlag > SCAN_END)
	{
		QMessageBox::warning(NULL, "warning", "Invalid page parameter!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		FlagFinished = 1;
		return;
	}
	//**************************************************************************************************
	// 扫描结束
	if (nFlag == SCAN_END)
	{
		f_CloseScanner();
		// 设置扫描标志位
		bIsScanning = FALSE;
		FlagFinished = 1;
		g_nNumberOfImage--;//由于当扫描结束的时候，在scanfinish一开始的时候有一个g_nNumberOfImage++的操作，而如果判定为扫描结束，则不应该g_nNumberOfImage++，所以在这里减去多加的g_nNumberOfImage
		ELOGI("Scan over");
		return;
	}

	//*************************************************************************************************
	// 获取图像
	if (!f_GetImages(&PicFront, &PicRear))
	{
		QMessageBox::warning(NULL, "warning", "Invalid page parameter!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		FlagFinished = 1;
		return;
	}

	//*************************************************************************************************
	// 保存正面
		//ZxingQRcode::recognize();
	if (PicFront.lPicSize > 0)
	{
		//strImageNameF = pDemoDlg->m_FilePath;
		ELOGI("save front page");
		strImageNameF = m_FilePath_i;
		if (PicFront.iPicType == 0)//bmp
		{
			strTemp.Format(L"\\Image_F%d.%s", g_nNumberOfImage, L"bmp");

		}
		else if (PicFront.iPicType == 1)//tif
		{
			strTemp.Format(L"\\Image_F%d.%s", g_nNumberOfImage, L"jpg");
		}
		else//jpg
		{
			strTemp.Format(L"\\Image_F%d.%s", g_nNumberOfImage, L"tif");
		}
		strImageNameF += strTemp;

		hFile = CreateFile(strImageNameF, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if (hFile == NULL)
		{
			QMessageBox::warning(NULL, "warning", "Save Iamge Error!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
			FlagFinished = 1;
			return;
		}
		if (!WriteFile(hFile, PicFront.PicBuffer, PicFront.lPicSize, &dwRelWrite, NULL))
		{
			QMessageBox::warning(NULL, "warning", "Save Iamge Error!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
			FlagFinished = 1;
			CloseHandle(hFile);
			return;
		}

		if (dwRelWrite != (DWORD)PicFront.lPicSize)
		{
			QMessageBox::warning(NULL, "warning", "Save Iamge Error!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
			FlagFinished = 1;
			CloseHandle(hFile);
			return;
		}
		//DefQueue queue1;
		CloseHandle(hFile);
		ELOGI("pushing image: " << strImageNameF << " into queue");
		pushImage(strImageNameF); //将反面的照片名字放入要识别的二维码队列中
	}

	//*************************************************************************************************
	// 保存反面
	if (PicRear.lPicSize > 0)
	{
		ELOGI("saving reverse pic");
		//strImageNameR = pDemoDlg->m_FilePath;
		strImageNameR = m_FilePath_i;
		if (PicRear.iPicType == 0)
		{
			strTemp.Format(L"\\Image_R%d.%s", g_nNumberOfImage, L"bmp");
		}
		else if (PicRear.iPicType == 1)
		{
			strTemp.Format(L"\\Image_R%d.%s", g_nNumberOfImage, L"jpg");
		}
		else
		{
			strTemp.Format(L"\\Image_R%d.%s", g_nNumberOfImage, L"tif");
		}
		strImageNameR += strTemp;

		hFile = CreateFile(strImageNameR, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if (hFile == NULL)
		{
			QMessageBox::warning(NULL, "warning", "Save Iamge Error!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
			FlagFinished = 1;
			return;
		}
		if (!WriteFile(hFile, PicRear.PicBuffer, PicRear.lPicSize, &dwRelWrite, NULL))
		{
			QMessageBox::warning(NULL, "warning", "Save Iamge Error!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
			FlagFinished = 1;
			CloseHandle(hFile);
			return;
		}

		if (dwRelWrite != (DWORD)PicRear.lPicSize)
		{
			QMessageBox::warning(NULL, "warning", "Save Iamge Error!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
			FlagFinished = 1;
			CloseHandle(hFile);
			return;
		}
		CloseHandle(hFile);
	}
	//************************************************************************************************
	// 一维条码识别
	ELOGI("saved one side of image: No. " << to_string(g_nNumberOfImage));
}
int Scanner::InitScanner()
{
	ELOGI("initting scanner");
	pDemoDlg = this;
	//加载动态库
	char szDLLPath[255];
	memset(szDLLPath, 255, 0);
	//CString	m_FilePath;
	char cConfigureIni[255];
	hModule = GetModuleHandle("Demo.exe");
	GetModuleFileName(hModule, szDLLPath, 255);
	char* p = strrchr(szDLLPath, '\\');
	*(p + 1) = 0;
	m_FilePath = szDLLPath;
	m_FilePath += "Images";
	CString	m_FilePath1;
	m_FilePath1= m_FilePath;
	m_FilePath1 += "Bad";
	LPCSTR s2;
	LPWSTR s1= m_FilePath.GetBuffer(0);
	s2 = wtoc(s1);
	LPCSTR s3;
	LPWSTR s4 = m_FilePath1.GetBuffer(0);
	s3 = wtoc(s4);
	CreateDirectoryA(s2, NULL);	
	CreateDirectoryA(s3, NULL);
	//下一行是加入的代码，即把动态库DLL文件夹放入 SDK/Demo/VCDemo/Debug/文件下
	strcat(szDLLPath, "DLL\\");
	strcpy(cConfigureIni, szDLLPath);
	strcat(cConfigureIni, "IniFile\\Configure.ini");
	strcat(szDLLPath, "ScanDll.dll");
	m_FilePath_i = pDemoDlg->m_FilePath;
	//hLibrary = LoadLibrary(L"E:\\草稿\\Demo\\Win32\\Debug\\DLL\\ScanDll.dll");
	LPCTSTR s5;
	s5 = (LPCTSTR)szDLLPath;
	//将szDLLPath从char类型转换为LPCTSTR
	int num = MultiByteToWideChar(0, 0, szDLLPath, -1, NULL, 0);
	wchar_t* wide = new wchar_t[num];
	MultiByteToWideChar(0, 0, szDLLPath, -1, wide, num);
	//
	hLibrary = LoadLibrary(wide);
	if (hLibrary == NULL)
	{
		QMessageBox::warning(NULL, "warning", "Get Library address Error", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		FlagFinished = 1;
		return FALSE;
	}
	//加载动态库中的函数
	f_OpenScanner = (pOpenScanner)GetProcAddress(hLibrary, "OpenScanner");
	if (f_OpenScanner == NULL)
	{
		QMessageBox::warning(NULL, "warning", "Get address Error", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		FlagFinished = 1;
		return FALSE;
	}

	f_OpenScannerEx = (pOpenScannerEx)GetProcAddress(hLibrary, "OpenScannerEx");
	if (f_OpenScannerEx == NULL)
	{
		QMessageBox::warning(NULL, "warning", "Get address Error", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		FlagFinished = 1;
		return FALSE;
	}


	f_CloseScanner = (pCloseScanner)GetProcAddress(hLibrary, "CloseScanner");
	if (f_CloseScanner == NULL)
	{
		QMessageBox::warning(NULL, "warning", "Get address Error", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		FlagFinished = 1;
		return FALSE;
	}

	f_StartScan = (pStartScan)GetProcAddress(hLibrary, "StartScan");
	if (f_StartScan == NULL)
	{
		QMessageBox::warning(NULL, "warning", "Get address Error", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		FlagFinished = 1;
		return FALSE;
	}

	f_AbortScan = (pAbortScan)GetProcAddress(hLibrary, "AbortScan");
	if (f_AbortScan == NULL)
	{
		QMessageBox::warning(NULL, "warning", "Get address Error", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		FlagFinished = 1;
		return FALSE;
	}

	f_GetImages = (pGetImages)GetProcAddress(hLibrary, "GetImages");
	if (f_GetImages == NULL)
	{
		QMessageBox::warning(NULL, "warning", "Get address Error", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		FlagFinished = 1;
		return FALSE;
	}

	f_GetDecode = (pGetDecode)GetProcAddress(hLibrary, "GetDecode");
	if (f_GetDecode == NULL)
	{
		QMessageBox::warning(NULL, "warning", "Get address Error", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		FlagFinished = 1;
		return FALSE;
	}

	f_SetIniFilePath = (pSetIniFilePath)GetProcAddress(hLibrary, "SetIniFilePath");
	if (f_SetIniFilePath == NULL)
	{
		QMessageBox::warning(NULL, "warning", "Get address Error", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		FlagFinished = 1;
		return FALSE;
	}
	//*************************************************************************************************
// 设置配置文件路径
	int nReturnCode;
	nReturnCode = f_SetIniFilePath(cConfigureIni);
	if (nReturnCode == FALSE)
	{
		QMessageBox::warning(NULL, "warning", "Set Initpath Error", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		FlagFinished = 1;
		return FALSE;
	}
	return TRUE;
}

void Scanner::StartScan()
{
	int bResult;
	//*************************************************************************************************
// 打开端口
	//bResult = f_OpenScanner(&ScanInfoCallBack,&ScanFinish);
	ELOGI("Open Scanner");
	int iDeviceNum = 1;
	m_FilePath_i = pDemoDlg->m_FilePath;
	bResult = f_OpenScannerEx(&ScanInfoCallBack, &ScanFinish, iDeviceNum);
	if (bResult == 0)
	{
		QMessageBox::warning(NULL, "warning", "Open Scanner Error!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		FlagFinished = 1;
		return;
	}
	//**m**********************************************************************************************
	// 启动扫描
	bResult = f_StartScan(0);
	ELOGI("start Scanner");
	if (bResult == FALSE)
	{
		QMessageBox::warning(NULL, "warning", "start Scanner Error!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		FlagFinished = 1;
		return;
	}

	//*************************************************************************************************

}

