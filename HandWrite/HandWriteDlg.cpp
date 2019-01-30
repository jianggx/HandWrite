
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
#include"toolcontroller.h"
#include "log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define LOG_POINT 1

// CHandWriteDlg �Ի���


typedef BOOL(WINAPI *fnGetTouchInputInfo)(HTOUCHINPUT, UINT, PTOUCHINPUT, int);
fnGetTouchInputInfo g_fnGetTouchInputInfo;
typedef BOOL(WINAPI *fnCloseTouchInputHandle)(HTOUCHINPUT);
fnCloseTouchInputHandle g_fnCloseTouchInputHandle;
typedef BOOL(WINAPI *fnRegisterTouchWindow)(HWND, ULONG);
fnRegisterTouchWindow g_fnRegisterTouchWindow;


CHandWriteDlg::CHandWriteDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_HANDWRITE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_lbuttondown = false;
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
	ON_MESSAGE(WM_TOUCH, &CHandWriteDlg::OnTouch)
END_MESSAGE_MAP()


// CHandWriteDlg ��Ϣ�������

BOOL CHandWriteDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	{
		g_fnGetTouchInputInfo = (fnGetTouchInputInfo)GetProcAddress(
			GetModuleHandle(_T("User32.dll")),
			"GetTouchInputInfo");
		if (NULL == g_fnGetTouchInputInfo)
		{
			TRACE(_T("GetTouchInputInfo"));
			return FALSE;
		}
		g_fnCloseTouchInputHandle = (fnCloseTouchInputHandle)GetProcAddress(
			GetModuleHandle(_T("User32.dll")),
			"CloseTouchInputHandle");
		if (NULL == g_fnCloseTouchInputHandle)
		{
			TRACE(_T("CloseTouchInputHandle"));
			return FALSE;
		}
		g_fnRegisterTouchWindow = (fnRegisterTouchWindow)GetProcAddress(
			GetModuleHandle(_T("User32.dll")),
			"RegisterTouchWindow");
		if (NULL == g_fnRegisterTouchWindow)
		{
			TRACE(_T("RegisterTouchWindow"));
			return FALSE;
		}
		BYTE digitizerStatus = (BYTE)GetSystemMetrics(SM_DIGITIZER);
		if ((digitizerStatus & (0x80 + 0x40)) != 0) //Stack Ready + MultiTouch
		{
			//		MessageBox(0, _T("No touch support is currently availible"), _T("Error"), MB_OK);
			unsigned int  nInputs = (BYTE)GetSystemMetrics(SM_MAXIMUMTOUCHES);
			if (nInputs >= 2)
			{
				//MessageBox(_T("��������ģʽ"), _T("INFO"), MB_OK);
				m_bMultiTouch = TRUE;
				if (g_fnRegisterTouchWindow != NULL)
				{
					// Register the application window for receiving multi-touch input.
					if (!g_fnRegisterTouchWindow(m_hWnd, 0))
					{
						MessageBox(_T("Cannot register application window for touch input"), _T("Error"), MB_OK);
					}
					else {
						Logs::logInfo("register touch ok");
					}
				}
			}
		}
	}

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


	CDC *dc = GetDC();
	m_memDC.CreateCompatibleDC(dc);
	m_membmp.CreateCompatibleBitmap(dc, rect.Width(), rect.Height());
	m_memDC.SelectObject(&m_membmp);

	m_painter.reset(new GdiPainter());
	m_painter->SetDibTarget(&m_memDC, rect.Width(), rect.Height());
	m_toolController.reset(new ToolController(m_painter.get()));
	m_memDC.FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(255, 255, 255));

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

		CRect rect;
		GetClientRect(&rect);
		dc.BitBlt(0, 0, rect.Width(), rect.Height(), &m_memDC, 0, 0, SRCCOPY);

		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CHandWriteDlg::OnQueryDragIcon()
{
	FILE_LOG(logINFO) << "OnQueryDragIcon";
	return static_cast<HCURSOR>(m_hIcon);
}


void CHandWriteDlg::handDown(CPoint point) {
	m_lastMoveTime = NowTime();

	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_lbuttondown = true;
	mousePts.push_back(point);

	Point p(point.x, point.y);
	m_toolController->startDrawing(p);

}

void CHandWriteDlg::handMove(CPoint point) {

	if (m_lbuttondown) {

		if (LOG_POINT) {
			long long t = NowTime();
			FILE_LOG(logINFO) << "dTime=" << t - m_lastMoveTime;
			m_lastMoveTime = t;
			FILE_LOG(logINFO) << "Move x=" << (int)point.x << ",y=" << (int)point.y;
		}

		mousePts.push_back(point);
		Point p(point.x, point.y);
		m_toolController->continueDrawing(p);

		Invalidate(false);
	}

}

void CHandWriteDlg::handUp(CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_lbuttondown) {
		mousePts.push_back(point);

		m_lbuttondown = false;
		m_toolController->endDrawing();
		Invalidate(false);
	}


}

void CHandWriteDlg::OnMouseMove(UINT nFlags, CPoint point)
{

	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	handMove(point);
	//CDialogEx::OnMouseMove(nFlags, point);
}




void CHandWriteDlg::OnLButtonDown(UINT nFlags, CPoint point)
{

	//Invalidate(false);
	handDown(point);
	//CDialogEx::OnLButtonDown(nFlags, point);
}


void CHandWriteDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	handUp(point);
	//CDialogEx::OnLButtonUp(nFlags, point);
}


LRESULT CHandWriteDlg::OnTouch(WPARAM wParam, LPARAM lParam)
{
	// A WM_TOUCH message can contain several messages from different contacts
	// packed together.
	unsigned int numInputs = (int)wParam; //Number of actual contact messages
	TOUCHINPUT* ti = new TOUCHINPUT[numInputs]; // Allocate the storage for
	//the parameters of the per-
	//contact messages

	// Unpack message parameters into the array of TOUCHINPUT structures, each
	// representing a message for one single contact.
	if (NULL == g_fnGetTouchInputInfo ||
		NULL == g_fnCloseTouchInputHandle)
	{
		return 0;
	}
	if (g_fnGetTouchInputInfo((HTOUCHINPUT)lParam, numInputs, ti,
		sizeof(TOUCHINPUT)))
	{
		// For each contact, dispatch the message to the appropriate message
		// handler.

#ifdef IS_SUPORT_MTOUCH
		for (unsigned int i = 0; i < numInputs; ++i)
#else
		unsigned int i = 0;
#endif
		{
			POINT pt;
			pt.x = ti[i].x / 100;
			pt.y = ti[i].y / 100;
			ScreenToClient(&pt);
			CPoint point;
			point.x = pt.x;
			point.y = pt.y;
			if (ti[i].dwFlags & TOUCHEVENTF_DOWN)
			{
				//TRACE("down (%4d,%4d,%4d)\n", ti[i].dwID, pt.x, pt.y);
				//m_myPen.PushStart(point, ti[i].dwID);
				OnTouchDownHandler(0, point, ti[i].dwID);
			}
			else if (ti[i].dwFlags & TOUCHEVENTF_MOVE)
			{
				//TRACE("move (%4d,%4d,%4d)\n", ti[i].dwID, pt.x, pt.y);
				//m_myPen.Push(point, ti[i].dwID);
				//Invalidate(FALSE);
				OnTouchMoveHandler(MK_LBUTTON, point, ti[i].dwID);
			}
			else if (ti[i].dwFlags & TOUCHEVENTF_UP)
			{
				//TRACE("up   (%4d,%4d,%4d)\n", ti[i].dwID, pt.x, pt.y);
				//m_myPen.PushEnd(point, ti[i].dwID);
				//Invalidate(FALSE);
				OnTouchUpHandler(0, point, ti[i].dwID);
			}
		}
	}
	g_fnCloseTouchInputHandle((HTOUCHINPUT)lParam);
	delete[] ti;

	return TRUE;
}

void CHandWriteDlg::OnTouchUpHandler(UINT nFlags, CPoint point, DWORD dwId)
{
	handUp(point);
}

void CHandWriteDlg::OnTouchDownHandler(UINT nFlags, CPoint point, DWORD dwId)
{
	handDown(point);
}

void CHandWriteDlg::OnTouchMoveHandler(UINT nFlags, CPoint point, DWORD dwId)
{
	handMove(point);
}
