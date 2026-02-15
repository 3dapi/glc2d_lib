//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _ILcModel_H_
#define _ILcModel_H_


#ifndef interface
#define interface	struct
#endif

namespace glc2d
{

interface ILcModel
{
	virtual ~ILcModel(){};

	virtual void	Render() =0;

	virtual	int		GetID()=0;
	virtual const char*	GetName() = 0;

	virtual	void	SetPos(const float* =NULL) =0;
	virtual	void	SetColor(const DWORD=0xFFFFFFFF) =0;
	virtual	void	SetRect(void* rc=NULL) =0;

	virtual	int		FindAniIndex(int* pOut/*Out*/, DWORD TimeCur, DWORD TimeBegin) =0;
	virtual	int		FindImageRect(void* pRECT/*Out*/, int nIndex) =0;
};

int	LcDev_ModelCreate(ILcModel** pData
					  , void* p1=NULL	//char* : Model Name
					  , void* p2=NULL
					  , void* p3=NULL
					  , void* p4=NULL
					);

int			LcDev_ModelInit(void* pSprite);	// ID3DXSprite Pointer
ILcModel*	LcDev_ModelFind(int nID);
void		LcDev_ModelDestroy();

}//namespace glc2d

#endif

