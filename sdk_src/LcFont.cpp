
#pragma warning(disable: 4996)

#include <algorithm>
#include <vector>
#include <string>
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "ILcFont.h"

namespace glc
{

class CLcFont : public ILcFont
{
protected:
	int					m_nID	{-1};
	D3DXFONT_DESC		m_Dsc	{};
	ID3DXFont*			m_pFnt	{};

public:
	CLcFont();
	virtual	~CLcFont();

	virtual int		Create(void* p1={},void* p2={},void* p3={},void* p4={});
	virtual void	Destroy();

	int		GetID() override;
	void*	GetFont() override;
	int		OnLostDevice() override;
	int		OnResetDevice() override;

	inline static int				m_nIDFnt    {};		// Font ID
	inline static LPDIRECT3DDEVICE9	m_pDevice   {};		// D3DDevice
	inline static LPD3DXSPRITE		m_pLcSprite {};		// DXSprite
	inline static D3DXMATRIX		m_tm        {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1, };
};

typedef std::vector<ILcFont* >	lsLcFont;
static lsLcFont	m_vFont;		// Font List

CLcFont::CLcFont()
{
}

CLcFont::~CLcFont()
{
	Destroy();
}

void CLcFont::Destroy()
{
	SAFE_RELEASE(	m_pFnt	);
	memset(&m_Dsc, 0, sizeof m_Dsc);
}

int CLcFont::GetID()
{
	return m_nID;
}

void* CLcFont::GetFont()
{
	return m_pFnt;
}

int CLcFont::OnResetDevice()
{
	int hr = m_pFnt->OnResetDevice();
	return hr;
}

int CLcFont::OnLostDevice()
{
	int hr = m_pFnt->OnLostDevice();
	return hr;
}


int CLcFont::Create(void* p1,void* p2,void* p3,void* p4)
{
	ID3DXFont*	pDXFont	= {};
	CSTR		sName	= (CSTR)p1;
	long		iH		= *((long*)p2);
	long		iItalic	= *((long*)p3);

	D3DXFONT_DESC hFont =
	{
		iH, 0
			, FW_EXTRABOLD, 1
			, iItalic
			, HANGUL_CHARSET, OUT_DEFAULT_PRECIS
			, ANTIALIASED_QUALITY, FF_DONTCARE, "Arial"
	};
	strcpy(hFont.FaceName, sName);


    if( FAILED(D3DXCreateFontIndirect( CLcFont::m_pDevice, &hFont, &pDXFont ) ) )
        return -1;


	if(NULL ==pDXFont)
		return -1;

	memcpy(&m_Dsc, &hFont, sizeof m_Dsc);
	m_pFnt = pDXFont;

	// 새로운 아이디 부여
	int _nID = CLcFont::m_nIDFnt;

	++_nID;

	// overflow....
	if(_nID<0)
	{
		printf("Overflow Font List\n");
		SAFE_RELEASE(	m_pFnt	);
		return -1;
	}


	CLcFont::m_nIDFnt = _nID;
	m_nID = _nID;

	return 0;
}

int LcDev_FontCreate(ILcFont** pData
					, void* p1
					, void* p2
					, void* p3
					, void* p4
					)
{
	*pData = {};
	CLcFont* pObj=new CLcFont;
	if(FAILED(pObj->Create(p1, p2, p3, p4)))
	{
		delete pObj;
		return -1;
	}
	*pData = pObj;
	return 0;
}

int LcDev_FontInit(void* pd3dDevice, void* pd3dSprite, void* pTm)
{
	CLcFont::m_pDevice	 = (LPDIRECT3DDEVICE9)pd3dDevice;
	CLcFont::m_pLcSprite = (LPD3DXSPRITE)pd3dSprite;
	D3DXMATRIX* tm       = (D3DXMATRIX*)pTm;
	if(!pd3dDevice || !pd3dSprite || !tm)
		return -1;

	CLcFont::m_tm = *tm;
	return 0;
}

void LcDev_FontDestroy()
{
	for(auto& v : m_vFont)
	{
		SAFE_DELETE( v	);
	}
	m_vFont.clear();
}

ILcFont* LcDev_FontFind(int _nID)
{
	auto itr = std::find_if(m_vFont.begin(), m_vFont.end()
		, [_nID](const auto& p){ return p ->GetID() == _nID;});
	return itr != m_vFont.end() ? *itr : nullptr;
}

int LcDev_FontOnLostDevice()
{
	for(auto & font : m_vFont)
	{
		int hr = font->OnLostDevice();
		if(FAILED(hr))
			return hr;
	}
	return S_OK;
}
int LcDev_FontOnResetDevice()
{
	for(auto& font : m_vFont)
	{
		int hr = font->OnResetDevice();
		if(FAILED(hr))
			return hr;
	}
	return S_OK;
}

};// namespace glc
////////////////////////////////////////////////////////////////////////////////



int g2_FontCreate(CSTR sName, long iH, long iItalic)
{
	glc::ILcFont* pFont	{};
	if(FAILED(glc::LcDev_FontCreate(&pFont, (VPTR)sName, &iH, &iItalic)))
		return -1;

	glc::m_vFont.push_back(pFont);
	return pFont->GetID();
}

int g2_FontDrawText(const int nIdx
					, const long lLeft
					, const long lTop
					, const long lRight
					, const long lBottom
					, const DWORD fontColor
					, const char *format, ...)
{
	glc::ILcFont* pFont	{};
	ID3DXFont*	pDxFnt		{};

	pFont = glc::LcDev_FontFind(nIdx);
	if(!pFont)
		return -1;

	pDxFnt = (ID3DXFont*)pFont->GetFont();
	if(!pDxFnt)
		return -1;

	va_list ap;
	std::string s="";
	if(!format)
		return -1;
	va_start(ap, format);
	int r = vsnprintf(nullptr, 0, format, ap);
	if(0<r)
	{
		s.resize(r);
		r = vsnprintf(&s[0], r+1, format, ap);
	}
	va_end(ap);
	
	if(!s.length())
		return -2;

    RECT rc;
	rc.left		= lLeft;
	rc.top		= lTop;
    rc.right	= lRight+20;
	rc.bottom	= lBottom;

	auto sprite = glc::CLcFont::m_pLcSprite;
	if(sprite)
		sprite->Begin(D3DXSPRITE_ALPHABLEND);
	sprite->SetTransform(&glc::CLcFont::m_tm);
	int hr = pDxFnt->DrawText(sprite, s.c_str(), -1, &rc, 0, fontColor);
	if(sprite)
		sprite->End();

	return hr;
}

//int g2_FontRelease(int _nID)
//{
//	auto itr = glc::m_vFont.end();
//	for(auto it = glc::m_vFont.begin(); it != glc::m_vFont.end(); ++it)
//	{
//		if((*it)->GetID() == _nID)
//		{
//			itr = it;
//			break;
//		}
//	}
//	if(itr == glc::m_vFont.end())
//		return -1;
//
//	SAFE_DELETE(*itr);
//	glc::m_vFont.erase(itr);
//	return static_cast<int>(glc::m_vFont.size());
//}

int g2_FontRelease(int _nID)
{
	auto itr = std::find_if(glc::m_vFont.begin(), glc::m_vFont.end()
	, [_nID](const auto& p){ return p ->GetID() == _nID;});
	if(itr == glc::m_vFont.end())
	{
		return -1;
	}
	SAFE_DELETE(*itr);
	glc::m_vFont.erase(itr);
	return static_cast<int>(glc::m_vFont.size());
}

