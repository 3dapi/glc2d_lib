
#pragma warning(disable: 4996)

#include <algorithm>
#include <cmath>
#include <vector>
#include <string>
#include <utility>
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "glcInternal.h"
#include "ILcFont.h"
#include "glc2d.h"

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

struct FontFile
{
	std::wstring path;
	int count{};
};
static std::vector<FontFile> m_vFontFiles;

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
			, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS
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
	// Font objects must release their GDI handles before file registrations.
	for(auto it = m_vFontFiles.rbegin(); it != m_vFontFiles.rend(); ++it)
		RemoveFontResourceExW(it->path.c_str(), FR_PRIVATE, nullptr);
	m_vFontFiles.clear();
	CLcFont::m_pDevice = nullptr;
	CLcFont::m_pLcSprite = nullptr;
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

static int InternalFontDrawText(const int nIdx
			, const RECT& rct, const DWORD flags, const DWORD fontColor
			, const char* format, va_list ap)
{
	glc::ILcFont* pFont{};
	ID3DXFont* pDxFnt{};
	pFont = glc::LcDev_FontFind(nIdx);
	if(!pFont)
		return -1;
	pDxFnt = (ID3DXFont*)pFont->GetFont();
	if(!pDxFnt)
		return -1;
	if(!format)
		return -1;
	std::string s= glc::FormatString(format, ap);
	if(!s.length())
		return -2;

	auto sprite = glc::CLcFont::m_pLcSprite;
	if(!sprite)
		return -3;

	RECT drawRect = rct;
	if(flags & DT_NOCLIP)
	{
		// Position-only calls use the formatted text's actual font metrics.
		// Measure in logical coordinates; the sprite applies screen scaling once.
		const int height = pDxFnt->DrawTextA(nullptr, s.c_str(), -1, &drawRect,
			(flags & ~DT_NOCLIP) | DT_CALCRECT, fontColor);
		if(height == 0)
			return 0;
	}

	// Allow extra space for italic overhang and font-specific glyph extents.
	// Padding is in logical coordinates and scales together with the text.
	TEXTMETRICA metrics{};
	if(!pDxFnt->GetTextMetricsA(&metrics))
		return -1;
	drawRect.right += metrics.tmAveCharWidth * 2;
	const LONG extraHeight = static_cast<LONG>(std::ceil(
		static_cast<double>(metrics.tmHeight) * 0.12));
	drawRect.bottom += extraHeight;

	HRESULT hr = sprite->Begin(D3DXSPRITE_ALPHABLEND);
	if(FAILED(hr))
		return hr;
	hr = sprite->SetTransform(&glc::CLcFont::m_tm);
	int height = 0;
	if(SUCCEEDED(hr))
		height = pDxFnt->DrawTextA(sprite, s.c_str(), -1, &drawRect, flags, fontColor);
	const HRESULT endHr = sprite->End();
	if(FAILED(hr)) return hr;
	if(FAILED(endHr)) return endHr;
	return height;
}
////////////////////////////////////////////////////////////////////////////////



int g2_FontRegisterFile(CSTR fileName)
{
	if(!fileName || !*fileName)
		return -1;
	const std::wstring name = glc::ToWideString(fileName);
	if(name.empty())
		return -1;
	const DWORD length = GetFullPathNameW(name.c_str(), 0, nullptr, nullptr);
	if(!length)
		return -1;
	std::wstring path(length, L'\0');
	const DWORD written = GetFullPathNameW(name.c_str(), length, path.data(), nullptr);
	if(!written || written >= length)
		return -1;
	path.resize(written);
	for(const auto& file : glc::m_vFontFiles)
	{
		if(_wcsicmp(file.path.c_str(), path.c_str()) == 0)
			return file.count;
	}

	// Allocate the tracking entry first so successful registration cannot be untracked.
	glc::m_vFontFiles.push_back({std::move(path), 0});
	auto& file = glc::m_vFontFiles.back();
	file.count = AddFontResourceExW(file.path.c_str(), FR_PRIVATE, nullptr);
	if(file.count == 0)
	{
		glc::m_vFontFiles.pop_back();
		return -1;
	}
	return file.count;
}

int g2_FontCreate(CSTR sName, long iH, long iItalic)
{
	glc::ILcFont* pFont	{};
	if(FAILED(glc::LcDev_FontCreate(&pFont, (VPTR)sName, &iH, &iItalic)))
		return -1;

	glc::m_vFont.push_back(pFont);
	return pFont->GetID();
}

int g2_FontDrawText(const int nIdx, const RECT& rc, const DWORD fontColor, const char *format, ...)
{
	RECT rct = rc;
	va_list ap;
	va_start(ap, format);
	int hr = InternalFontDrawText(nIdx, rct, 0, fontColor, format, ap);
	va_end(ap);
	return hr;
}

int g2_FontDrawText(const int nIdx, const VEC2i& pos, const DWORD fontColor, const char *format, ...)
{
	// The internal helper measures width and height from this origin.
	RECT rct = {pos.x, pos.y, pos.x, pos.y};
	va_list ap;
	va_start(ap, format);
	int hr = InternalFontDrawText(nIdx, rct, DT_LEFT | DT_TOP | DT_NOCLIP, fontColor, format, ap);
	va_end(ap);
	return hr;
}

int g2_FontDrawText(int nIdx, std::initializer_list<LONG> coordinates,
	DWORD fontColor, const char* format, ...)
{
	if(coordinates.size() != 2 && coordinates.size() != 4)
		return -1;

	const LONG* values = coordinates.begin();
	RECT rect = {values[0], values[1], values[0], values[1]};
	DWORD flags = DT_LEFT | DT_TOP | DT_NOCLIP;
	if(coordinates.size() == 4)
	{
		rect.right = values[2];
		rect.bottom = values[3];
		flags = 0;
	}

	va_list ap;
	va_start(ap, format);
	const int result = InternalFontDrawText(nIdx, rect, flags, fontColor, format, ap);
	va_end(ap);
	return result;
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

