
// HandWriteDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HandWrite.h"
#include "HandWriteDlg.h"
#include "afxdialogex.h"
#include "freehand.h"
#include "pressure.h"
#include "GdiPainter.h"
#include"PPoint.h"
#include"toolcontroller.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHandWriteDlg 对话框



CHandWriteDlg::CHandWriteDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_HANDWRITE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHandWriteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHandWriteDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
   ON_WM_MOUSEMOVE()
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CHandWriteDlg 消息处理程序

BOOL CHandWriteDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
   //CPaintDC dc(this);
  // Gdiplus::Graphics b(dc.m_hDC);
   //m_graphics.reset(new Gdiplus::Graphics(dc.m_hDC)); // 创建图形对象
   CRect rect;
   GetClientRect(&rect);
   m_painter.reset(new GdiPainter(rect.Width(), rect.Height()));
   m_toolController.reset(new ToolController(m_painter.get()));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHandWriteDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
      CPaintDC dc(this);
      Gdiplus::Graphics graph(dc.m_hDC); // 创建图形对象
      graph.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);

      graph.DrawImage(m_painter->getBitmap(), 0, 0 );

	  //for (auto& p : mousePts)
	  //{
		//  Gdiplus::Pen ptPen(Gdiplus::Color::Green, 3);
	//	  graph.DrawEllipse(&ptPen, p.x, p.y, 5, 5);
	  //}
		CDialogEx::OnPaint();
	}  
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CHandWriteDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CHandWriteDlg::OnMouseMove(UINT nFlags, CPoint point)
{
   // TODO: 在此添加消息处理程序代码和/或调用默认值
   if (m_lbuttondown) {
	   mousePts.push_back(point);

      Point p(point.x, point.y);
      m_toolController->continueDrawing(p);

      //Invalidate(false);

   }

   CDialogEx::OnMouseMove(nFlags, point);
}


void CHandWriteDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
   // TODO: 在此添加消息处理程序代码和/或调用默认值
   m_lbuttondown = true;
   mousePts.push_back(point);

   Point p(point.x, point.y);
   m_toolController->startDrawing(p);

   //Invalidate(false);

   CDialogEx::OnLButtonDown(nFlags, point);
}


void CHandWriteDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
   // TODO: 在此添加消息处理程序代码和/或调用默认值
   if (m_lbuttondown) {
	   mousePts.push_back(point);

      m_lbuttondown = false;
      m_toolController->endDrawing();
      Invalidate(false);
   }


   CDialogEx::OnLButtonUp(nFlags, point);
}
