
// HandWrite.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CHandWriteApp: 
// �йش����ʵ�֣������ HandWrite.cpp
//

class CHandWriteApp : public CWinApp
{
public:
	CHandWriteApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
   ULONG_PTR gdiplusToken;
   Gdiplus::GdiplusStartupInput gdiplusStartupInput;

	DECLARE_MESSAGE_MAP()
   virtual int ExitInstance();
};

extern CHandWriteApp theApp;