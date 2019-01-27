#pragma once

/**
*��ˢ�����֣�Բ�Σ����Σ�SetWidth֮����»�ˢ�������ڵ�ֵ�������Ҷ�ֵ�洢������Եģ��+�������Դ�ڴ�
**/
class CMyBrush
{
public:
	CMyBrush(int nLength = 100);
	~CMyBrush(void);

public:
	COLORREF SetColor(COLORREF rgb);
	COLORREF GetColor();
	void SetWidthCircleBrush(float fWidth); //����ΪԲ��ˢ�ӣ���ȣ����ڻ��ʣ�ë��
	void SetWidthCircleBrush2(double fWidth, double cntx, double cnty);

	static  RECT empty;
	void SetWidthSuibiBrush(int nWidth, BYTE gray, RECT r =empty); //����Ϊˮ��ӫ���ˢ�ӣ���ȣ�͸����
	BYTE DisToGrayLine(float fDis);
	void MergeColor(BYTE rgbOld[3], BYTE gray=255); //gray��Ϊalphaֵ;

private:
	BYTE DisToGrayCircle(float fDis);

public:
	int		m_nLength; //�ڲ����������εĳ���=101
	int		m_nMid;  //ˢ�����ĵ����꣬����������/2
	BYTE	*m_pBuf;	//ˢ�ӻ�����
	int		m_nHalfRect; //ˢ��ռ�������εİ뾶
	int		m_nPenWidthMaobi; //����ë��

	int m_nHalfW;
	int m_nHalfH;


	BYTE	m_a256x256[256][256]; //һ�ſ��[i,j]=((i+1)*(j+1)-1)/256  Ũ�������Ͻ������½Ƿ��䵽���ֵ
	
	BYTE	m_aRGB[3];  //ˢ����ɫ[R G B] ,��ʱ�ſ�
private:

	float	m_fWidthHalfCircle; //Բˢ�Ӱ뾶
	float	m_fBorderOuter;  //Բˢ��ģ����߽���1
	float	m_fBorderOuter2;  //Բˢ��ģ����߽���2
	float	m_fBorderInner;  //Բˢ��ģ���ڱ߽���

	int		m_nWidthHalfSuibi; //ˮ��ӫ��ʰ뾶
};

