
// HandWriteDlg.h : 头文件
//

#pragma once

class Freehand;
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

   std::auto_ptr<Freehand> m_freehand;
   std::auto_ptr<GdiPainter> m_painter;

   CDC m_dcMemory;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
