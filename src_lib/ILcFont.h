//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _ILcFont_H_
#define _ILcFont_H_

#ifndef interface
#define interface	struct
#endif

namespace glc2d
{

interface	ILcFont
{
	virtual	~ILcFont(){};

	virtual	int		GetID()=0;
	virtual	void*	GetFont()=0;
};


int LcDev_FontCreate(ILcFont** pData
					, void* p1={}		// char*: fong Name
					, void* p2={}		// long: Height
					, void* p3={}		// long: Italic?
					, void* p4={}
					);

int			LcDev_FontInit(void* pd3dDevice);
ILcFont*	LcDev_FontFind(int _nID);
void		LcDev_FontDestroy();

}//namespace glc2d

#endif

