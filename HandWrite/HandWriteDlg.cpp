
// HandWriteDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "HandWrite.h"
#include "HandWriteDlg.h"
#include "afxdialogex.h"
#include "freehand.h"
#include "pressure.h"
#include "GdiPainter.h"
#include"PPoint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHandWriteDlg �Ի���



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


// CHandWriteDlg ��Ϣ�������

BOOL CHandWriteDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
   //CPaintDC dc(this);
  // Gdiplus::Graphics b(dc.m_hDC);
   //m_graphics.reset(new Gdiplus::Graphics(dc.m_hDC)); // ����ͼ�ζ���
   CRect rect;
   GetClientRect(&rect);
   m_painter.reset(new GdiPainter(rect.Width(), rect.Height()));
   m_freehand.reset(new Freehand(*m_painter));

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CHandWriteDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
      CPaintDC dc(this);
      Gdiplus::Graphics graph(dc.m_hDC); // ����ͼ�ζ���
      graph.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);

      graph.DrawImage(m_painter->getBitmap(), 0, 0 );

      Gdiplus::Pen bluePen(Gdiplus::Color(255, 0, 0, 255)); // ������ɫ��
      Gdiplus::Pen redPen(Gdiplus::Color(255, 255, 0, 0)); // ������ɫ��

      Gdiplus::RectF rect(100, 100, 20.4, 30.5);
      graph.DrawEllipse(&bluePen, rect);

		CDialogEx::OnPaint();
	}  
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CHandWriteDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CHandWriteDlg::OnMouseMove(UINT nFlags, CPoint point)
{
   // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
   PPoint p(point.x, point.y, 1);
   m_freehand->begin(p);

   Invalidate();

   CDialogEx::OnMouseMove(nFlags, point);
}


void CHandWriteDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
   // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
   PPoint p(point.x, point.y, 1);
   m_freehand->motion(p);

   Invalidate();

   CDialogEx::OnLButtonDown(nFlags, point);
}


void CHandWriteDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
   // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
   m_freehand->end();
   Invalidate();

   CDialogEx::OnLButtonUp(nFlags, point);
}
