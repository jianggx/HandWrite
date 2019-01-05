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
   int m_nDibWidth, m_nDibHeight; //DIB位图的宽高
   int m_nDibWidthBytes; //DIB一行的字节数
   BYTE *m_pDibBits;  //操作的24位DIB图像数据-与设备无关的位图（DIB）
   int m_StepSize;//add by ljm 线越宽 步长越大，减小计算

   CDC* m_pDC;//用于保存DC,用户绘图，内存cdc, 等同DrawProView的m_MemDC,SetDibTarget中设置
   COLORREF m_bkColor;//图片的背景色
   HBITMAP m_hDib;
};

