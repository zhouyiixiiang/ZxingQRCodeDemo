//#include<windef.h>
#include <string>
#include <fstream>
#include <afx.h>
#define PAGE_END	1
#define SCAN_END	2
#include "defqueue.h"

extern CString m_FilePath_i;

class Scanner
{
public:
	//Scanner(CWnd* pParent=NULL);
	CString	m_FilePath;
	int iDeviceNum;
public:
	//static int InitScanner();
	virtual int InitScanner();
	static void StartScan();
	//static void __stdcall ScanFinish(int nFlag);
	//DefQueue queue1;
protected:
	//virtual int InitScanner();
};