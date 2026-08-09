// Interface for the ILcSpriteX class.
//
////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef _ILcSpriteX_H_
#define _ILcSpriteX_H_

#include "glcType.h"
#include "glcInternal.h"

namespace glc
{

interface ILcSpriteX
{
	virtual ~ILcSpriteX(){};

	virtual	int		Begin(DWORD=0)=0;
	virtual	int		End(DWORD=0)=0;

	virtual	int		OnResetDevice()=0;
	virtual	int		OnLostDevice()=0;

	virtual	int	    Draw( const void* pTex				// Texture
						, const RECT* pRct				// Draw Region
						, const void* pScl={}			// Scaling
						, const void* pTrn={}			// Position
						, const DWORD dColor=0xFFFFFFFF	// color
						, const int   bMono=0			// Monotone
						)=0;

	virtual	int	  DrawEx( const void* pTex				// Texture
						, const RECT* pRct				// Draw Region
						, const void* pScl				// Scaling
						, const void* pTrn				// Position
						, const void* pRot				// Rotation Center
						, const float fAngle			// Rotation Angle(Radian)
						, const DWORD dColor=0xFFFFFFFF	// color
						, const int   bMono=0			// Monotone
						)=0;

	virtual	void	AlphaOption(int opt=0)=0;
};

int	LcDev_SpriteCreate(ILcSpriteX** pData
					, void* p1={}
					, void* p2={}
					, void* p3={}
					, void* p4={}
					);


}//namespace glc

#endif
