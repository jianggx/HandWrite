#pragma once
#include "Painter.h"

class GdiPainter :
   public Painter
{
public:
   GdiPainter(int w, int h);
   virtual ~GdiPainter();

   virtual void PenMove(const std::vector<PPoint>& pts);
   virtual void PenUp();

   Gdiplus::Bitmap* getBitmap() { return m_pBitmap.get(); }

private:
   std::auto_ptr<Gdiplus::Bitmap> m_pBitmap;
   Gdiplus::Graphics* m_pGraphics;
   bool m_pendown;
   int m_penwidth;
   PPoint m_lastpoint;
   std::auto_ptr<Gdiplus::Pen> m_pen;
};

