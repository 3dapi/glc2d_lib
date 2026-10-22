#pragma warning(disable: 4996)

#include <algorithm>
#include <memory>
#include <string.h>
#include <string>
#include <utility>

#include "LcxFont.h"
#include "ILcTexture.h"
#include "glcInternal.h"
#include "glc2d.h"

namespace glc
{
namespace
{
	inline constexpr size_t MAX_BITMAP_DIMENSION = 4096;
	inline constexpr size_t FONT_BUFFER_SIZE = 4*1024*1024;
	unsigned char fontBuffer[FONT_BUFFER_SIZE]; // Shared scratch buffer, as TXR_TempBuf1().
}
struct FontBitmap
{
    VEC2i size      {};
    int padding     {};
    int textH       {};
    uint8_t* pixels {};
};
FontBitmap RasterizeFontBitmap(HDC dc, const std::string& text)
{
	if(!dc || text.empty())
		return {};

	TEXTMETRICA metrics{};
	if(!GetTextMetricsA(dc, &metrics))
		return {};

	const int padding = std::max(2L, metrics.tmHeight / 3 + metrics.tmOverhang + 2);
	const UINT flags = DT_LEFT | DT_TOP | DT_NOPREFIX | DT_NOCLIP;
		
	RECT measure{};
	if(!DrawTextA(dc, text.c_str(), (int)(text.size()), &measure, flags | DT_CALCRECT))
		return {};

	int width = measure.right;
	int height = measure.bottom;
	if(width <= 0)
		width = 1; // Newlines and empty visible lines still have height.
	if(height <= 0)
		height = metrics.tmHeight;

	const int bmpW  = width + padding * 2;
	const int bmpH = height + padding * 2;

	// Bound both DIB allocation and future texture allocation before multiplying.
	if(	bmpW > MAX_BITMAP_DIMENSION ||
		bmpH > MAX_BITMAP_DIMENSION ||
		bmpW * bmpH > FONT_BUFFER_SIZE)
		return {};

	FontBitmap result;
	result.size.x = int(bmpW);
	result.size.y = int(bmpH);
	result.padding = padding;
	result.pixels=fontBuffer;

	BITMAPINFO info{};
	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth = LONG(bmpW);
	info.bmiHeader.biHeight = -LONG(bmpH);
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = 32;
	info.bmiHeader.biCompression = BI_RGB;

	uint8_t* pixels = nullptr;
	HBITMAP dib = CreateDIBSection(dc, &info, DIB_RGB_COLORS, (void**)&pixels, nullptr, 0);
	if(!dib)
		return {};

	const HGDIOBJ oldBitmap = SelectObject(dc, dib);
	if(!oldBitmap || oldBitmap == HGDI_ERROR)
	{
		DeleteObject(dib);
		return {};
	}

	memset(pixels, 0, bmpW * bmpH * 4);

	SetTextColor(dc, RGB(255, 255, 255));
	SetBkColor(dc, RGB(0, 0, 0));
	SetBkMode(dc, TRANSPARENT);

	RECT rect{padding, padding, padding + width, padding + height};
	const int drawn = DrawTextA(dc, text.c_str(), (int)(text.size()), &rect, flags);

	// GDI batches writes; synchronize before reading DIB memory on the CPU.
	GdiFlush();

	result.textH = drawn;

	for(int i=0; i< bmpW * bmpH; ++i)
		fontBuffer[i] = pixels[i * 4]; // Grayscale AA; GDI's alpha byte is unused.

	SelectObject(dc, oldBitmap);
	DeleteObject(dib);
	if(!drawn)
		return {};

	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

CLcxFont::~CLcxFont()
{
	SAFE_RELEASE(m_pTex);
	if(m_dc && m_oldFont)
		SelectObject(m_dc, m_oldFont);
	if(m_font)
		DeleteObject(m_font);
	if(m_dc)
		DeleteDC(m_dc);
}

int CLcxFont::Draw(const RECT& rect, DWORD color)
{
	return CLcxFont::Draw(rect, true, color);
}

int CLcxFont::Draw(const VEC2i& pos, DWORD color)
{
	const RECT rect{pos.x, pos.y, pos.x, pos.y};
	return CLcxFont::Draw(rect, false, color);
}

int CLcxFont::Draw(std::initializer_list<long> coords, DWORD color)
{
	const auto* v=coords.begin();
	if(coords.size()==2)
		return Draw(VEC2i{v[0], v[1]}, color);
	if(coords.size()==4)
		return Draw(RECT{v[0], v[1], v[2], v[3]}, color);
	return E_INVALIDARG;
}

int CLcxFont::Create(const std::string& fontName, int height, bool italic)
{
	if(fontName.empty() || !height || height < -512 || height > 512)
		return E_INVALIDARG;

	LOGFONTA lfont
	{
		height						, // lfHeight;
		0							, // lfWidth;
		0							, // lfEscapement;
		0							, // lfOrientation;
		FW_EXTRABOLD				, // lfWeight;
		(BYTE)italic				, // lfItalic;
		FALSE						, // lfUnderline;
		FALSE						, // lfStrikeOut;
		DEFAULT_CHARSET				, // lfCharSet;
		OUT_DEFAULT_PRECIS			, // lfOutPrecision;
		CLIP_DEFAULT_PRECIS			, // lfClipPrecision;
		ANTIALIASED_QUALITY			, // lfQuality;
		DEFAULT_PITCH|FF_DONTCARE	, // lfPitchAndFamily;
	};
	::strcpy(lfont.lfFaceName, fontName.c_str());
	auto font = CreateFontIndirectA(&lfont);
	if(!font)
		return E_FAIL;

	m_font = font;
	m_dc = CreateCompatibleDC(nullptr);
	if(!m_dc)
	{
		goto ERR;
	}

	auto oldFont = SelectObject(m_dc, m_font);
	if(!oldFont || oldFont==HGDI_ERROR)
	{
		goto ERR;
	}
	m_oldFont = oldFont;
	return S_OK;

ERR:
	if(m_oldFont)
		SelectObject(m_dc, m_oldFont);
	if(m_font)
		DeleteObject(m_font);
	if(m_dc)
		DeleteDC(m_dc);
	return E_FAIL;
}

int CLcxFont::SetText(const char* str)
{
	if(!str)
		return E_INVALIDARG;
	if(0 == m_str.compare(str))
		return S_OK;
	
	if(!*str)
	{
		SAFE_RELEASE(m_pTex);
		m_texSize   = {};
		m_imageSize = {};
		m_padding=m_textH=0;
		m_str.clear();
		return S_OK;
	}
	std::string str_t(str);
	int hr= BuildTexture(str_t);
	if(SUCCEEDED(hr))
		m_str = str_t;
	return hr;
}

int CLcxFont::Draw(const RECT& rect, bool clipped, DWORD color)
{
	if(!m_pTex)
		return 0;

	const int padding=m_padding;
	RECT source{};
	if(clipped)
	{
		const int w = int(rect.right -rect.left);
		const int h = int(rect.bottom - rect.top);
		if(w<=0 || h<=0)
			return 0;
		
		source =
		{
			padding, padding,
			padding + std::min(w, m_imageSize.x - 2*padding),
			padding + std::min(h, m_imageSize.y - 2*padding)
		};
	}

	const VEC2 position(float(rect.left - (clipped?0:padding)), float(rect.top - (clipped?0:padding)));
	// Drawing only submits the prepared texture. No UTF-8 conversion,
	// formatting, GDI calls, allocations, texture creation or uploads here.
	const int hr=LcDev_TextureDraw(static_cast<ILcTexture*>(this), clipped?&source:nullptr, &position,
		nullptr, nullptr, 0, color, 1, true, 0);
	return FAILED(hr)?hr:m_textH;
}

int CLcxFont::BuildTexture(const std::string& str)
{
	int hr = 0;
	FontBitmap bitmap = RasterizeFontBitmap(m_dc, str);
	if(!bitmap.pixels)
		return E_FAIL;

	int w = std::max(m_texSize.x, bitmap.size.x);
	int h = std::max(m_texSize.y, bitmap.size.y);

	if(m_devCaps.TextureCaps & D3DPTEXTURECAPS_POW2)
	{
		int tw=1, th=1;
		while(tw<w)
			tw*=2;
		while(th<h)
			th*=2;

		w=tw;
		h=th;
	}
	if(m_devCaps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY)
		w=h=(std::max)(w, h);

	if(w>(int)m_devCaps.MaxTextureWidth || h>(int)m_devCaps.MaxTextureHeight)
		return D3DERR_NOTAVAILABLE;

	LPDIRECT3DTEXTURE9 texture=m_pTex;
	const bool create=!m_pTex || w!=m_texSize.x || h!=m_texSize.y;
	if(create)
	{
		hr= m_pDevice->CreateTexture(w, h, 1, 0, D3DFMT_A8, D3DPOOL_MANAGED, &texture, nullptr);
		if(FAILED(hr))
			return hr;
	}

	// Update texture pixels.
	D3DLOCKED_RECT lock{};
	hr=texture->LockRect(0, &lock, nullptr, 0);
	if(SUCCEEDED(hr))
	{
		unsigned char* pixel0 = (unsigned char*)(lock.pBits);
		for(int y=0;y<h;++y)
		{
			auto* row = pixel0 + y*lock.Pitch;
			memset(row, 0, w);
			if(y< bitmap.size.y)
				memcpy(row, bitmap.pixels + y*bitmap.size.x, bitmap.size.x);
		}
		hr=texture->UnlockRect(0);
	}
	if(FAILED(hr))
	{
		if(create)
			texture->Release();
		return hr;
	}
	if(create)
	{
		SAFE_RELEASE(m_pTex);
		m_pTex=texture;
		m_texSize = {w, h};
	}
	m_imageSize  = bitmap.size;
	m_padding    = bitmap.padding;
	m_textH = bitmap.textH;
	return S_OK;
}

} // namespace glc

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

void* g2_LcxFontCreate(const char* fontFace, long height, long italic)
{
	auto font =std::make_unique<glc::CLcxFont>();

	if(FAILED(font->Create(fontFace, height, italic)))
		return nullptr;
	return font.release();
}

int g2_LcxFontRelease(void*& font)
{
	auto pFont = (glc::ILcxFont*)font;
	if(pFont)
	{
		delete pFont;
		font=nullptr;
	}
	return 0;
}

int g2_LcxFontSetText(void* font, const char* text)
{
	auto pFont = (glc::ILcxFont*)font;
	if( pFont)
		return pFont->SetText(text);
	return E_FAIL;
}

int g2_LcxFontDraw(void* font, const char* text)
{
	auto pFont = (glc::ILcxFont*)font;
	if(pFont)
		return ((glc::ILcxFont*)pFont)->SetText(text);
	return E_FAIL;
}
int g2_LcxFontDraw(void* font, const VEC2i& pos, const DWORD color)
{
	auto pFont = (glc::ILcxFont*)font;
	if(!pFont)
		return E_FAIL;
	return pFont->Draw(pos, color);
}