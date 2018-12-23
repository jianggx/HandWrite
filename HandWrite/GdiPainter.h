#pragma once
#include "Painter.h"
class GdiPainter :
   public Painter
{
public:
   GdiPainter(Gdiplus::Graphics& g);
   virtual ~GdiPainter();

   virtual void PenMove(const std::vector<PPoint>& pts);
   virtual void PenUp();

private:
   Gdiplus::Graphics& m_graphcs;
   bool m_pendown;
   int m_penwidth;
   PPoint m_lastpoint;
   std::auto_ptr<Gdiplus::Pen> m_pen;
};

