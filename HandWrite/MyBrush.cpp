#include "StdAfx.h"
#include "MyBrush.h"
#include <math.h>

#pragma warning (disable : 4244)
CMyBrush::CMyBrush(int nLength)
{
	ASSERT(nLength >= 5 && nLength < 200);

	m_nLength = nLength | 1;  //确保长度为奇数
	m_nMid = m_nLength / 2;
	m_pBuf = new BYTE [m_nLength * m_nLength];
	memset(m_pBuf, 0, m_nLength*m_nLength);
	
	for (int i=0; i<256; i++)
	{
		for (int j=0; j<256; j++)
		{
			m_a256x256[i][j] = ((i+1) * (j+1) - 1) / 256;
		}
	}

	m_fWidthHalfCircle = 0.f;
	m_nWidthHalfSuibi = 0;
	m_aRGB[0] = m_aRGB[1] = m_aRGB[2] = 0; 
}

CMyBrush::~CMyBrush(void)
{
	if (m_pBuf != NULL)
		delete [] m_pBuf, m_pBuf = NULL;
}

COLORREF CMyBrush::SetColor( COLORREF rgb )
{
	COLORREF clrOld = RGB(m_aRGB[0], m_aRGB[1], m_aRGB[2]);
	m_aRGB[0] = GetRValue(rgb);
	m_aRGB[1] = GetGValue(rgb);
	m_aRGB[2] = GetBValue(rgb);
	return clrOld;
}

COLORREF CMyBrush::GetColor()
{
	COLORREF clrOld = RGB(m_aRGB[0], m_aRGB[1], m_aRGB[2]);
	return clrOld;
}

RECT CMyBrush::empty = { 0,0,0,0 };
void CMyBrush::SetWidthSuibiBrush( int nWidth, BYTE gray, RECT empty)
{
	if (nWidth < 2)
		nWidth = 2;
	else if (nWidth > m_nLength)
		nWidth = m_nLength;

	m_nWidthHalfSuibi = nWidth/2;
	m_nHalfRect = m_nWidthHalfSuibi;
	m_fWidthHalfCircle = 0.f; //将圆刷子宽度清0，确保与水笔刷子互斥

	memset(m_pBuf, 0, m_nLength*m_nLength);

	BYTE *pi = m_pBuf+(m_nMid-m_nHalfRect)*m_nLength;
	for (int i = -m_nHalfRect; i<=m_nHalfRect; i++, pi += m_nLength) //初始化
	{
		BYTE *pij = pi + (m_nMid - m_nHalfRect);
		for (int j=-m_nHalfRect; j<=m_nHalfRect; j++, pij++)
		{
			int sum = j - i;
			if (sum >= 0 && sum <= m_nWidthHalfSuibi) //两条平行线之间
			{
				*pij = gray;
			}
		}
	}



}

int DisToGrayCircle2(double distance, double fWidth)
{
	int rv = 255 * (fWidth - distance) / fWidth;
	return min(255, max(rv, 0));
}

void CMyBrush::SetWidthCircleBrush2(double fWidth, double cntx, double cnty)
{
	if (fWidth < 1.f)
	{
		fWidth = 1.f;
	}
	else if (fWidth > m_nLength - 6.f)
	{
		fWidth = m_nLength - 6.f;
	}

	double half_w = fWidth / 2;
	int x0 = int(cntx - half_w);
	int x1 = int(ceil(cntx + half_w));
	if ((x1 - x0) % 2 == 1)
		x0 -= 1;
	m_nHalfRect = (x1 - x0) / 2;

	int y0 = int(cnty - half_w);
	int y1 = y0 + x1 - x0;

	memset(m_pBuf, 0, m_nLength*m_nLength);

	BYTE *pi = m_pBuf + (m_nMid - m_nHalfRect)*m_nLength;
	for (int i = -m_nHalfRect; i <= m_nHalfRect; i++, pi += m_nLength) //初始化圆刷子
	{
		BYTE *pij = pi + (m_nMid - m_nHalfRect);
		for (int j = -m_nHalfRect; j <= m_nHalfRect; j++, pij++)
		{
			double dis = sqrt(pow(x0 + i+ m_nHalfRect - cntx, 2) + pow(y0 + j + m_nHalfRect - cnty, 2));

			*pij = DisToGrayCircle2(dis, half_w);
		}
	}

}

void CMyBrush::SetWidthCircleBrush(float fWidth)
{
	if (fWidth < 1.f)
	{
		fWidth = 1.f;
	}
	else if (fWidth > m_nLength - 6.f)
	{
		fWidth = m_nLength - 6.f;
	}

	if (abs(fWidth - 2*m_fWidthHalfCircle) < 0.001f)
		return;

	m_nWidthHalfSuibi = 0; //将水笔刷子宽度清0
	m_fWidthHalfCircle = fWidth / 2.f;
	if (m_fWidthHalfCircle >= 3.f)
	{
		m_fBorderInner = 1.0f;
		m_fBorderOuter = 1.0f;
		m_fBorderOuter2 = 1.0f;
	}
	if (m_fWidthHalfCircle >= 2.5f)
	{
		m_fBorderInner = 0.75f;
		m_fBorderOuter = 1.0f;
		m_fBorderOuter2 = 1.25f;
	}
	else if (m_fWidthHalfCircle >= 2.0f)
	{
		m_fBorderInner = 0.75f;
		m_fBorderOuter = 1.0f;
		m_fBorderOuter2 = 1.0f;
	}
	else if (m_fWidthHalfCircle >= 1.25f)
	{
		m_fBorderInner = 0.75f;
		m_fBorderOuter = 0.75f;
		m_fBorderOuter2 = 1.0f;
	}
	else if (m_fWidthHalfCircle >= 0.75f)
	{
		m_fBorderInner = 0.5f;
		m_fBorderOuter = 0.75f;
		m_fBorderOuter2 = 1.0f;
	}
	else if (m_fWidthHalfCircle >= 0.5f)
	{
		m_fBorderInner = 0.5f;
		m_fBorderOuter = 0.75f;
		m_fBorderOuter2 = 0.75f;
	}
	else
	{
		ASSERT(FALSE);
	}

	m_nHalfRect =int(m_fWidthHalfCircle + m_fBorderOuter + m_fBorderOuter2) + 1;
	ASSERT(m_nHalfRect <= m_nMid);

	memset(m_pBuf, 0, m_nLength*m_nLength);

	//Logs::logInfo("SetWidthCircleBrush");
	BYTE *pi = m_pBuf+(m_nMid-m_nHalfRect)*m_nLength;
	for (int i = -m_nHalfRect; i<=m_nHalfRect; i++, pi += m_nLength) //初始化圆刷子
	{
		float i2 = i*i;

		BYTE *pij = pi + (m_nMid - m_nHalfRect);
		for (int j=-m_nHalfRect; j<=m_nHalfRect; j++, pij++)
		{
			float sum2 = i2 + j*j;
			float fDis = sqrt(sum2); //计算到圆心距离

			*pij = DisToGrayCircle(fDis);
		}
	}
}

BYTE CMyBrush::DisToGrayCircle(float fDis)
{
	BYTE byNode1 = 176;
	BYTE byNode2 = 32;

	if (fDis <= m_fWidthHalfCircle - m_fBorderInner)
	{
		return 255;
	}
	else if (fDis < m_fWidthHalfCircle) //内层模糊圈
	{
		float fRatio = (m_fWidthHalfCircle - fDis)/m_fBorderInner;
		return int(fRatio*(255-byNode1))+byNode1;
	}
	else if (fDis < m_fWidthHalfCircle + m_fBorderOuter) ////外层模糊圈1
	{
		float fRatio = (m_fWidthHalfCircle + m_fBorderOuter - fDis)/m_fBorderOuter;
		return int(fRatio*(byNode1-byNode2))+byNode2;
	}
	else if (fDis < m_fWidthHalfCircle + m_fBorderOuter + m_fBorderOuter2) ////外层模糊圈2
	{
		float fRatio = (m_fWidthHalfCircle + m_fBorderOuter + m_fBorderOuter2 - fDis)/m_fBorderOuter2;
		return int(fRatio*byNode2);
	}
	else
	{
		return 0;
	}
}

BYTE CMyBrush::DisToGrayLine(float fDis)
{
	fDis = abs(fDis);

	float fk = 1.0f;
	if (fDis > fk)
	{
		return 0;
	}
	else
	{
		float fRatio = (fk - fDis)/fk;
		return BYTE(fRatio*255);
	}
}

//当前画刷的颜色合并上rgbOld， 按照透明度为gray混色，透明度是当前画刷的透明度
//A*a+(1-a)*B
void CMyBrush::MergeColor( BYTE rgbOld[3], BYTE gray )
{

	BYTE *ptr1 = m_a256x256[gray], *ptr2 = m_a256x256[255-gray];
	rgbOld[2] = ptr1[m_aRGB[0]] + ptr2[rgbOld[2]];  //r 原始代码
	rgbOld[1] = ptr1[m_aRGB[1]] + ptr2[rgbOld[1]];  //g
	rgbOld[0] = ptr1[m_aRGB[2]] + ptr2[rgbOld[0]];  //b
}

