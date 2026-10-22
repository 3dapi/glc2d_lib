//
////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef _ILcTexture_H_
#define _ILcTexture_H_

#include "glcType.h"

namespace glc
{

interface	ILcTexture
{
	virtual	~ILcTexture()=default;

	virtual	int		GetID() const =0;
	virtual	VEC2i	GetImageSize() const =0;
	virtual	VEC2i	GetSurfaceSize() const =0;
	virtual	void*	GetTexture() const =0;
	virtual	RECTi	GetImageRect() const =0;
};

// Internal draw path shared by registered textures and model-owned textures.
int LcDev_TextureDraw(ILcTexture* texture, const RECT* rect,
	const VEC2* position, const VEC2* scale, const VEC2* pivot,
	float angle, DWORD color, int mono, bool linear = false, int alphaMethod = -1);

int	LcDev_TextureCreate(ILcTexture** pData
					, void* p1=NULL
					, void* p2=NULL
					, void* p3=NULL
					, void* p4=NULL
					);


int			LcDev_TextureInit(void* pd3dDevice);
ILcTexture*	LcDev_TextureFind(int nID);
void		LcDev_TextureDestroy();

}//namespace glc

#endif
