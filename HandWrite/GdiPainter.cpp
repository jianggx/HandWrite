#include "stdafx.h"
#include "GdiPainter.h"
#include"PPoint.h"

GdiPainter::GdiPainter(int w, int h)  {
   m_pBitmap.reset(new Gdiplus::Bitmap(w, h));
   m_pendown = false; 
   m_pen.reset(new Gdiplus::Pen(Gdiplus::Color::Black));
   m_pen->SetColor(Gdiplus::Color(255, 0, 0, 255));
   m_pen->SetWidth(5);
   m_penwidth = 5;
};


GdiPainter::~GdiPainter()
{
}


void GdiPainter::PenMove(const std::vector<PPoint>& pts)
{
   for (const PPoint &p : pts) {
      //PPoint p(pp.x() / 4.0, pp.y() / 4.0, pp.pressure() / 65535.0);

      if (m_pendown) {
         Gdiplus::Graphics* pGraphcs = Gdiplus::Graphics::FromImage(m_pBitmap.get());
         m_pen->SetWidth(m_penwidth *(m_lastpoint.pressure() + p.pressure()) / 2.0);
         Gdiplus::PointF f(m_lastpoint.x(), m_lastpoint.y());
         Gdiplus::PointF t(p.x(), p.y());
         pGraphcs->DrawLine(m_pen.get(), f, t);
      }
      else {
         m_pendown = true;
         //gid+ 没有花点函数
      }
      m_lastpoint = p;
   }
}

void GdiPainter::PenUp()
{
   m_pendown = false;
}