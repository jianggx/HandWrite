#include "stdafx.h"
#include "GdiPainter.h"
#include"PPoint.h"


GdiPainter::GdiPainter(int w, int h)  {
   m_pBitmap.reset(new Gdiplus::Bitmap(w, h));
   m_pendown = false; 
   m_penmoved = false;
   m_brush.reset(new Gdiplus::SolidBrush(Gdiplus::Color::Black));

   m_pGraphics = Gdiplus::Graphics::FromImage(m_pBitmap.get());
   m_pGraphics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
};


GdiPainter::~GdiPainter()
{
}


void GdiPainter::directDab(const PPoint& pt) {
	m_pGraphics->FillEllipse(m_brush.get(), Gdiplus::REAL(pt.x()), pt.y(), m_penwidth*pt.pressure(), m_penwidth*pt.pressure());
}

void GdiPainter::drawSoftLine(const PPoint& from, const PPoint& to)
{
	double dx = to.x() - from.x();
	double dy = to.y() - from.y();
	const double dist = hypot(dx, dy);
	dx = dx / dist;
	dy = dy / dist;
	const double dp = (to.pressure() - from.pressure()) / dist;

	const double spacing_base = max(1.0, m_penwidth/2);

	double i=0;

	PPoint p(from.x() + dx * i, from.y() + dy * i,  from.pressure() + dp * i);

	while (i < dist) {
		const double spacing = max(1.0, spacing_base*p.pressure());
		directDab(p);
		p.rx() += dx * spacing;
		p.ry() += dy * spacing;
		p.setPressure(min(max(0.0, p.pressure() + dp * spacing), 1.0));
		i += spacing;
	}
}


void GdiPainter::PenMove(const std::vector<PPoint>& pts)
{
   for (const PPoint &p : pts) {
      //PPoint p(pp.x() / 4.0, pp.y() / 4.0, pp.pressure() / 65535.0);

      if (m_pendown) {

		  m_penmoved = true;
		  drawSoftLine(m_lastpoint, p);
      }
      else {
         m_pendown = true;
		 m_penmoved = false;
	  }
      m_lastpoint = p;
   }
}

void GdiPainter::PenUp()
{
	if (!m_penmoved && m_pendown)
	{
		//gid+ 没有花点函数
		m_pGraphics->FillEllipse(m_brush.get(), Gdiplus::REAL(m_lastpoint.x()), m_lastpoint.y(), m_penwidth, m_penwidth);
	}
   m_pendown = false;
   m_penmoved = false;
}