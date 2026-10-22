/**
 * @file    GaLic (Game Library Collection)
 * @brief   A lightweight 2D Game Library built with DirectX.
 * * Version History:
 * 1. Initial development using DirectX SDK (Summer Update 2003).
 * 2. Updated for compatibility with DirectX SDK (June 2010) and Visual Studio 2008.
 * 3. Current Build Environment: 
 * - IDE: Visual Studio 2022
 * - SDK: Microsoft.DXSDK.D3DX (Version 9.29.952.8)
 */

#pragma warning(disable: 4096)
#pragma warning(disable: 4099)

#pragma once
#ifndef _glcInternal_H_
#define _glcInternal_H_

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#include <string>
#include <vector>
#include <vadefs.h>

struct D3DXMATRIX;

namespace glc
{

// Sampling and blending are independent of the drawing source (image/font).
// The current sprite backend does not implement grayscale conversion.
enum class SPRITE_FILTER { Point, Linear };
enum class SPRITE_BLEND { Alpha, DestinationAlpha };
int LcDev_SpritePrepare(SPRITE_FILTER filter, SPRITE_BLEND blend);
int LcDev_SpriteTransform(const D3DXMATRIX& transform);

std::wstring ToWideString(const std::string& str);
int FormatLength(const char* format, va_list va);
std::string FormatString(const char* format, va_list va);

} // namespace glc

#endif
