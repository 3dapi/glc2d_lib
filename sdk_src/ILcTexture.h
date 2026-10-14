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
	virtual	~ILcTexture(){};

	virtual	int		GetID()=0;

	virtual	int		GetImageWidth()=0;
	virtual	int		GetImageHeight()=0;
	virtual	int		GetSurfaceWidth()=0;
	virtual	int		GetSurfaceHeight()=0;
	virtual	void*	GetTexture()=0;
	virtual	void	GetImageRect(void* rc)=0;
};

// Internal draw path shared by registered textures and model-owned textures.
int LcDev_TextureDraw(ILcTexture* texture, const RECT* rect,
	const VEC2* position, const VEC2* scale, const VEC2* pivot,
	float angle, DWORD color, int mono);

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
