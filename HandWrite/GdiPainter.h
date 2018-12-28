#pragma once
#include "Painter.h"

class GdiPainter :
   public Painter
{
public:
   GdiPainter(int w, int h);
   virtual ~GdiPainter();

   virtual void PenDown(const PPoint& point);
   virtual void PenMove(const std::vector<PPoint>& pts);
   virtual void PenUp();

   Gdiplus::Bitmap* getBitmap() { return m_pBitmap.get(); }

   void drawSoftLine(const PPoint& from, const PPoint& to);
   void directDab(const PPoint& from);

private:
   std::auto_ptr<Gdiplus::Bitmap> m_pBitmap;
   Gdiplus::Graphics* m_pGraphics;
   bool m_pendown;
   bool m_penmoved;
   PPoint m_lastpoint;
   std::auto_ptr < Gdiplus::SolidBrush> m_brush;

};

