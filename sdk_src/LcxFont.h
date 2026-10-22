#pragma once
#include <vector>
#include <string>
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "ILcTexture.h"
#include "ILcSpriteX.h"
#include "ILcxFont.h"

namespace glc
{
class CLcxFont: public ILcxFont, public ILcTexture
{
protected:
	std::string m_str;

	HDC		m_dc		{};
	HFONT	m_font		{};
	HGDIOBJ m_oldFont	{};
	VEC2i	m_imageSize	{};
	VEC2i	m_texSize	{};
	int		m_padding	{};
	int		m_textH		{};	// text height, not including padding
	LPDIRECT3DTEXTURE9 m_pTex{};
		
public:
	inline static LPDIRECT3DDEVICE9	m_pDevice	{};		// D3DDevice
	inline static D3DCAPS9			m_devCaps	{};
	inline static ILcSpriteX*		m_pLcSprite	{};

public:
	~CLcxFont() override;
	int		Draw(const RECT& rect, bool clipped, DWORD color) override;
	int		Draw(const RECT& rect, DWORD color=0xFFFFFFFF) override;
	int		Draw(const VEC2i& pos, DWORD color=0xFFFFFFFF) override;
	int		Draw(std::initializer_list<long> coordinates, DWORD color=0xFFFFFFFF)override;
	int		SetText(const char* str) override;
	void*	GetTexture() const override { return m_pTex; }
	VEC2i	GetSize() const override { return m_imageSize; }

	int		GetID() const override { return -1; }
	VEC2i	GetImageSize() const override { return m_imageSize; }
	VEC2i	GetSurfaceSize() const override { return m_texSize; }
	RECTi	GetImageRect() const override   { return { 0, 0, (long)m_imageSize.x, (long)m_imageSize.y}; }

	int		Create(const std::string& fontName, int height, bool italic = false);
protected:
	int		BuildTexture(const std::string& str);
};
} // namespace glc
