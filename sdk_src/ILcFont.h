//
////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef _ILcFont_H_
#define _ILcFont_H_

#include "glcType.h"
#include "glcInternal.h"

namespace glc
{

interface	ILcFont
{
	virtual	~ILcFont(){};

	virtual	int		GetID()=0;
	virtual	void*	GetFont()=0;
	virtual	int		OnLostDevice()=0;
	virtual	int		OnResetDevice()=0;
};

int LcDev_FontCreate(ILcFont** pData
					, void* p1={}		// CSTR: fong Name
					, void* p2={}		// long: Height
					, void* p3={}		// long: Italic?
					, void* p4={}
					);

int			LcDev_FontInit(void* pd3dDevice, void* pd3dSprite, void* pTm);
ILcFont*	LcDev_FontFind(int _nID);
void		LcDev_FontDestroy();
int			LcDev_FontOnLostDevice();
int			LcDev_FontOnResetDevice();

}//namespace glc

#endif

