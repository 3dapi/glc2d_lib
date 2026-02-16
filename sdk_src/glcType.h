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
#ifndef _glcType_H_
#define _glcType_H_

#include <stdint.h>
#include <d3dx9.h>

//------------------------------------------------------------------------------------------------------------------------------------------
//
enum
{
	GLC_MAX_KEY = 256,
};

//------------------------------------------------------------------------------------------------------------------------------------------

typedef D3DXVECTOR2							VEC2;
typedef D3DXVECTOR3							VEC3;
typedef D3DXCOLOR							DCOL;

typedef	LPDIRECT3D9							PD3D;
typedef LPDIRECT3DDEVICE9					PDEV;
typedef	LPD3DXSPRITE						PDSP;

typedef LPDIRECT3DTEXTURE9					PDTX;
typedef LPDIRECT3DSURFACE9					PDSF;
typedef	D3DXIMAGE_INFO						DIMG;

typedef	const char*							CSTR;
typedef	void*								VPTR;
typedef	uint8_t								KEYCODE;

typedef	struct VEC2i
{
	int x, y;
} VEC2i;

typedef	struct VEC3i
{
	int x, y, z;
} VEC3i;

#endif
