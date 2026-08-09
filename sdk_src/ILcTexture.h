//
////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef _ILcTexture_H_
#define _ILcTexture_H_

#include "glcType.h"
#include "glcInternal.h"

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
