#include "stdafx.h"
#include "GdiPainter.h"
#include"PPoint.h"
#include"MyBrush.h"

#define Color_ByteCount 4  //单个像素点颜色字节数

GdiPainter::GdiPainter()  {
   //m_pBitmap.reset(new Gdiplus::Bitmap(w, h));
   m_pendown = false; 
   m_penmoved = false;

   m_brush.reset(new CMyBrush());

	m_brush->SetColor(RGB(0, 0, 0));

	m_brush->SetWidthCircleBrush(5);
};


GdiPainter::~GdiPainter()
{
}

HBITMAP GdiPainter::CreateGDIBitmap(HDC hdc, int nWid, int nHei, void ** ppBits)
{

	BITMAPINFOHEADER bmheader;
	memset(&bmheader, 0, sizeof(bmheader));
	bmheader.biSize = sizeof(bmheader);
	bmheader.biCompression = BI_RGB;
	bmheader.biBitCount = 8 * Color_ByteCount;//如果要带透明度可能要=32
	bmheader.biPlanes = 1;
	bmheader.biClrUsed = 256;
	bmheader.biClrImportant = 256;
	bmheader.biWidth = nWid;
	bmheader.biHeight = -(nHei + 1);//TODO 为何加1

	HBITMAP hBmp = ::CreateDIBSection(hdc, (BITMAPINFO*)&bmheader,
		DIB_RGB_COLORS, ppBits, NULL, 0);
	ReleaseDC(NULL, hdc);
	return hBmp;
}
void GdiPainter::SetDibTarget(CDC* pdc, int nDibWidth, int nDibHeight)
{
	m_pDC = pdc;
	m_nDibWidth = nDibWidth;
	m_nDibHeight = nDibHeight;
	m_nDibWidthBytes = ((m_nDibWidth*Color_ByteCount - 1) | 3) + 1; //确保32位对齐
	m_hDib = CreateGDIBitmap(m_pDC->m_hDC, m_nDibWidth, m_nDibHeight, (void**)&m_pDibBits);

	HBITMAP hDibOld = (HBITMAP)m_pDC->SelectObject(m_hDib);
}

void GdiPainter::FillPtPixel(const PPoint& point)
{
	ASSERT(m_pDibBits != NULL && m_nDibWidth > 0 && m_nDibHeight > 0);

	int x = point.x();
	int y = point.y();
	BYTE gray = 255;
	int width = m_penwidth * point.p();

	m_brush->SetWidthCircleBrush2(m_penwidth * point.p(), point.x(), point.y());


	int nHalfRect = m_brush->m_nHalfRect;

	if (x + nHalfRect<0 || x - nHalfRect>m_nDibWidth - 1 || y + nHalfRect<0 || y - nHalfRect>m_nDibHeight - 1) //边界检测
	{
		return;
	}
	int nSI = (y - nHalfRect < 0) ? -y : -nHalfRect;
	int nEI = (y + nHalfRect > m_nDibHeight - 1) ? m_nDibHeight - 1 - y : nHalfRect;
	int nSJ = (x - nHalfRect < 0) ? -x : -nHalfRect;
	int nEJ = (x + nHalfRect > m_nDibWidth - 1) ? m_nDibWidth - 1 - x : nHalfRect;

	BYTE *ptr = m_brush->m_a256x256[gray];
	BYTE *pBrushI = m_brush->m_pBuf + (m_brush->m_nMid + nSI)*m_brush->m_nLength + m_brush->m_nMid + nSJ;
	BYTE *pBitsI = m_pDibBits + (y + nSI) * m_nDibWidthBytes + (x + nSJ)*Color_ByteCount;// 核心绘制层 testcode
	for (int i = nSI;
		i <= nEI;
		i++, pBrushI += m_brush->m_nLength, pBitsI += m_nDibWidthBytes) //外层
	{
		BYTE *pBrushIJ = pBrushI;
		BYTE *pBitsIJ = pBitsI;
		for (int j = nSJ;
			j <= nEJ; j++, pBrushIJ++, pBitsIJ += Color_ByteCount) //内层
		{
			BYTE byGray = *pBrushIJ;
			if (byGray != 0 && (byGray = ptr[byGray]) != 0)
			{
				m_brush->MergeColor(pBitsIJ, byGray);//corecode
			}
		}
	}
}


void GdiPainter::FillPtPixel2(const PPoint& point)
{
	ASSERT(m_pDibBits != NULL && m_nDibWidth > 0 && m_nDibHeight > 0);

	int x = point.x();
	int y = point.y();
	BYTE gray = 255;
	double width = max(m_penwidth * point.p(),1);

	int nHalfRect = (width+3)/2;

	if (x + nHalfRect<0 || x - nHalfRect>m_nDibWidth - 1 || y + nHalfRect<0 || y - nHalfRect>m_nDibHeight - 1) //边界检测
	{
		return;
	}
	int nSI = (y - nHalfRect < 0) ? -y : -nHalfRect;
	int nEI = (y + nHalfRect > m_nDibHeight - 1) ? m_nDibHeight - 1 - y : nHalfRect;
	int nSJ = (x - nHalfRect < 0) ? -x : -nHalfRect;
	int nEJ = (x + nHalfRect > m_nDibWidth - 1) ? m_nDibWidth - 1 - x : nHalfRect;

	BYTE *pBitsI = m_pDibBits + (y + nSI) * m_nDibWidthBytes + (x + nSJ)*Color_ByteCount;// 核心绘制层 testcode
	for (int i = nSI;
		i <= nEI;
		i++,  pBitsI += m_nDibWidthBytes) //外层
	{
		BYTE *pBitsIJ = pBitsI;
		for (int j = nSJ;
			j <= nEJ; j++, pBitsIJ += Color_ByteCount) //内层
		{
			double dist = hypot(i+y+0.5 - point.y(), j+x+0.5 - point.x());
			if (dist > width)
				continue;


			double hev = max(cos(3.14159265 * dist / width), 0);
			if (hev <= 0)
				continue;


			BYTE byGray = 255*(1- hev);

			pBitsIJ[0] = min(byGray, pBitsIJ[0]);
			pBitsIJ[1] = min(byGray, pBitsIJ[1]);
			pBitsIJ[2] = min(byGray, pBitsIJ[2]);
		}
	}
}


void GdiPainter::directDab(const PPoint& pt) {
	FillPtPixel2(pt);
	//m_pGraphics->FillEllipse(m_brush.get(), Gdiplus::REAL(pt.x()), pt.y(), m_penwidth*pt.p(), m_penwidth*pt.p());
}

void GdiPainter::drawSoftLine(const PPoint& from, const PPoint& to)
{
	double dx = to.x() - from.x();
	double dy = to.y() - from.y();
	double dist = hypot(dx, dy);
	dist *= 2;
	dx = dx / dist;
	dy = dy / dist;
	const double dp = (to.p() - from.p()) / dist;

	double i=0;
	PPoint p(from.x() + dx * i, from.y() + dy * i,  from.p() + dp * i);
	while (i < dist) {
		directDab(p);
		p.rx() += dx ;
		p.ry() += dy ;
		p.rp() = min(max(0.0, p.p() + dp ), 1.0);
		i += 1;
	}
}

void GdiPainter::PenDown(const PPoint& point)
{
	m_pendown = true;
	m_penmoved = false;
	m_lastpoint = point;

}


void GdiPainter::PenMove(const std::vector<PPoint>& pts)
{
   for (const PPoint &p : pts) {
      //PPoint p(pp.x() / 4.0, pp.y() / 4.0, pp.pressure() / 65535.0);

      if (m_pendown) {

		  m_penmoved = true;
		  drawSoftLine(m_lastpoint, p);
      }
      m_lastpoint = p;
   }
}

void GdiPainter::PenUp()
{
	if (!m_penmoved && m_pendown)
	{
		//gid+ 没有花点函数
		PPoint point = m_lastpoint;
		point.rp() = 1;
		directDab(point);
	}
   m_pendown = false;
   m_penmoved = false;
}