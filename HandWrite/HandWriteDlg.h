
// HandWriteDlg.h : ͷ�ļ�
//

#pragma once

class ToolController;
class GdiPainter;

// CHandWriteDlg �Ի���
class CHandWriteDlg : public CDialogEx
{
// ����
public:
	CHandWriteDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HANDWRITE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
   bool m_lbuttondown = false;
   std::auto_ptr<ToolController> m_toolController;
   std::auto_ptr<GdiPainter> m_painter;

   std::vector<CPoint> mousePts;


   CDC m_memDC;
   CBitmap m_membmp;

   CDC m_dcMemory;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
