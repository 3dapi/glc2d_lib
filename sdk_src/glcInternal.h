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

#endif
