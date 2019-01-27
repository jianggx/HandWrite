#pragma once

/**
*画刷，两种：圆形，梯形，SetWidth之后更新画刷缓冲区内的值（用作灰度值存储），边缘模糊+抗锯齿来源于此
**/
class CMyBrush
{
public:
	CMyBrush(int nLength = 100);
	~CMyBrush(void);

public:
	COLORREF SetColor(COLORREF rgb);
	COLORREF GetColor();
	void SetWidthCircleBrush(float fWidth); //设置为圆形刷子，宽度，用于画笔，毛笔
	void SetWidthCircleBrush2(double fWidth, double cntx, double cnty);

	static  RECT empty;
	void SetWidthSuibiBrush(int nWidth, BYTE gray, RECT r =empty); //设置为水笔荧光笔刷子，宽度，透明度
	BYTE DisToGrayLine(float fDis);
	void MergeColor(BYTE rgbOld[3], BYTE gray=255); //gray即为alpha值;

private:
	BYTE DisToGrayCircle(float fDis);

public:
	int		m_nLength; //内部缓冲正方形的长度=101
	int		m_nMid;  //刷子中心点坐标，缓冲区长度/2
	BYTE	*m_pBuf;	//刷子缓冲区
	int		m_nHalfRect; //刷子占用正方形的半径
	int		m_nPenWidthMaobi; //用于毛笔

	int m_nHalfW;
	int m_nHalfH;


	BYTE	m_a256x256[256][256]; //一张快表[i,j]=((i+1)*(j+1)-1)/256  浓淡表，左上角向右下角辐射到最大值
	
	BYTE	m_aRGB[3];  //刷子颜色[R G B] ,暂时放开
private:

	float	m_fWidthHalfCircle; //圆刷子半径
	float	m_fBorderOuter;  //圆刷子模糊外边界宽度1
	float	m_fBorderOuter2;  //圆刷子模糊外边界宽度2
	float	m_fBorderInner;  //圆刷子模糊内边界宽度

	int		m_nWidthHalfSuibi; //水笔荧光笔半径
};

