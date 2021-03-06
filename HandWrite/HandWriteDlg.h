
// HandWriteDlg.h : 头文件
//

#pragma once

class ToolController;
class GdiPainter;

// CHandWriteDlg 对话框
class CHandWriteDlg : public CDialogEx
{
// 构造
public:
	CHandWriteDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HANDWRITE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
   bool m_lbuttondown;
   std::auto_ptr<ToolController> m_toolController;
   std::auto_ptr<GdiPainter> m_painter;

   std::vector<CPoint> mousePts;


   CDC m_memDC;
   CBitmap m_membmp;

   CDC m_dcMemory;
   long long m_lastMoveTime;
   BOOL m_bMultiTouch;//是否触屏模式

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	void handDown(CPoint point);
	void handMove(CPoint point);
	void handUp(CPoint point);

   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

   afx_msg LRESULT OnTouch(WPARAM wParam, LPARAM lParam);
   void OnTouchDownHandler(UINT nFlags, CPoint point, DWORD dwId = 0);
   void OnTouchMoveHandler(UINT nFlags, CPoint point, DWORD dwId = 0);
   void OnTouchUpHandler(UINT nFlags, CPoint point, DWORD dwId = 0);

};
