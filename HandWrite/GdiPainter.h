#pragma once
#include "Painter.h"

class CMyBrush;

class GdiPainter :
   public Painter
{
public:
   GdiPainter();
   virtual ~GdiPainter();

   virtual void PenDown(const PPoint& point);
   virtual void PenMove(const std::vector<PPoint>& pts);
   virtual void PenUp();

   Gdiplus::Bitmap* getBitmap() { return m_pBitmap.get(); }

   void drawSoftLine(const PPoint& from, const PPoint& to);
   void directDab(const PPoint& from);

   void SetDibTarget(CDC* pdc, int nDibWidth, int nDibHeight);
   HBITMAP CreateGDIBitmap(HDC hdc, int nWid, int nHei, void ** ppBits);
   void FillPtPixel(int x, int y, BYTE gray);
private:
   std::auto_ptr<Gdiplus::Bitmap> m_pBitmap;
   Gdiplus::Graphics* m_pGraphics;
   bool m_pendown;
   bool m_penmoved;
   PPoint m_lastpoint;


   std::auto_ptr <CMyBrush> m_brush;
   int m_nDibWidth, m_nDibHeight; //DIBλͼ�Ŀ��
   int m_nDibWidthBytes; //DIBһ�е��ֽ���
   BYTE *m_pDibBits;  //������24λDIBͼ������-���豸�޹ص�λͼ��DIB��
   int m_StepSize;//add by ljm ��Խ�� ����Խ�󣬼�С����

   CDC* m_pDC;//���ڱ���DC,�û���ͼ���ڴ�cdc, ��ͬDrawProView��m_MemDC,SetDibTarget������
   COLORREF m_bkColor;//ͼƬ�ı���ɫ
   HBITMAP m_hDib;
};

