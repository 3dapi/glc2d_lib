
#include <algorithm>
#include <vector>

#include <windows.h>
#include <stdio.h>

#include <d3d9.h>

#include "ILcTexture.h"

namespace glc2d
{

class CLcTexture : public ILcTexture
{
protected:
	int					m_nID{-1};
	char				m_sFile[128]{};
	LPDIRECT3DTEXTURE9	m_pTex{};
	D3DXIMAGE_INFO		m_pImg{};

	int					m_SrfW{};
	int					m_SrfH{};

public:
	CLcTexture();
	virtual	~CLcTexture();

	virtual	int		Create(void* p1=NULL,void* p2=NULL,void* p3=NULL,void* p4=NULL);
	virtual	void	Destroy();
	virtual	int		GetID();

	virtual	int		GetImageWidth();
	virtual	int		GetImageHeight();
	virtual	int		GetSurfaceWidth();
	virtual	int		GetSurfaceHeight();

	virtual	void*	GetTexture();
	virtual	void	GetImageRect(void* rc);

	CSTR	GetName();

	static	int					m_nIDTex;		// Texture ID
	static	LPDIRECT3DDEVICE9	m_pDevice;		// Direct3DDevice
};

int					CLcTexture::m_nIDTex	= 0;	// Texture ID
LPDIRECT3DDEVICE9	CLcTexture::m_pDevice	= {};	// Direct3DDevice
typedef std::vector<ILcTexture* >	lsPDTX;
static lsPDTX	m_vTex;								// Texture List

CLcTexture::CLcTexture()
{
}

CLcTexture::~CLcTexture()
{
	Destroy();
}

int CLcTexture::GetID()
{
	return m_nID;
}

int CLcTexture::Create(void* p1,void* p2,void* p3,void* p4)
{
	CSTR	sFile	= (CSTR)p1;
	DWORD	dc		= *((DWORD*)p2);
	if(FAILED(D3DXCreateTextureFromFileEx(
		CLcTexture::m_pDevice
		, sFile
		, D3DX_DEFAULT
		, D3DX_DEFAULT
		, D3DX_DEFAULT
		, 0
		, D3DFMT_UNKNOWN
		, D3DPOOL_MANAGED
		, D3DX_FILTER_NONE
		, D3DX_FILTER_NONE
		, dc
		, &m_pImg
		, NULL
		, &m_pTex
		)) )
	{
		printf("Create Texture Failed: %s\n", sFile);
		m_pTex = {};
		return -1;
	}

	D3DSURFACE_DESC dsc;
	m_pTex->GetLevelDesc(0, &dsc);
	m_SrfW = dsc.Width;
	m_SrfH = dsc.Height;

	// 새로운 아이디 부여

	int _nID = CLcTexture::m_nIDTex;
	++_nID;

	// overflow....
	if(_nID<0)
	{
		printf("Overflow Texture List\n");
		m_pTex->Release();
		m_pTex = {};
		return -1;
	}

	CLcTexture::m_nIDTex = _nID;
	m_nID = _nID;

	return 0;
}

void CLcTexture::Destroy()
{
	SAFE_RELEASE(	m_pTex	);
	memset(&m_pImg, 0, sizeof m_pImg);
	memset(m_sFile, 0, sizeof m_sFile);
}

int CLcTexture::GetImageWidth()
{
	return m_pImg.Width;
}

int CLcTexture::GetImageHeight()
{
	return m_pImg.Height;
}


int CLcTexture::GetSurfaceWidth()
{
	return m_SrfW;
}

int CLcTexture::GetSurfaceHeight()
{
	return m_SrfH;
}


void* CLcTexture::GetTexture()
{
	return m_pTex;
}

void CLcTexture::GetImageRect(void* v)
{
	RECT	rc={0,0, (LONG)m_pImg.Width, (LONG)m_pImg.Height};
	memcpy(v, &rc, sizeof(RECT));
}


CSTR CLcTexture::GetName()
{
	return m_sFile;
}

int LcDev_TextureInit(void* pDev)
{
	CLcTexture::m_pDevice = (LPDIRECT3DDEVICE9)pDev;
	return 0;
}

void LcDev_TextureDestroy()
{
	for(auto v : m_vTex)
	{
		SAFE_DELETE( v );
	}
	m_vTex.clear();
}

ILcTexture* LcDev_TextureFind(int _nID)
{
	auto itr = std::find_if(m_vTex.begin(), m_vTex.end()
		, [_nID](const auto& p){return p->GetID() == _nID;});
	return itr != m_vTex.end() ? *itr : nullptr;
}

ILcTexture* LcDev_TextureFind(CSTR sFile)
{
	auto itr = std::find_if(m_vTex.begin(), m_vTex.end()
		, [sFile](const auto& p)
		{
			auto pTex = (CLcTexture*)p;
			return 0 ==:: _stricmp(pTex->GetName(), sFile);
		}
	);
	return itr != glc2d::m_vTex.end() ? *itr : nullptr;
}

int LcDev_TextureCreate(ILcTexture** pData
					, void* p1
					, void* p2
					, void* p3
					, void* p4
					)
{
	*pData = {};
	CLcTexture*	pObj = (CLcTexture*)LcDev_TextureFind((CSTR)p1);	// Texture 를 파일 이름으로 찾는다.
	if(pObj)
	{
		*pData = pObj;
		return 0;
	}
	pObj = new CLcTexture;
	if(FAILED(pObj->Create(p1, p2, p3, p4)))
	{
		delete pObj;
		return -1;
	}
	*pData = pObj;
	return 0;
}

};// namespace glc2d
////////////////////////////////////////////////////////////////////////////////


int	glc2d_TextureLoad(CSTR sFileName, DWORD dc)
{
	glc2d::ILcTexture*	pTexture = {};
	if(FAILED(glc2d::LcDev_TextureCreate(&pTexture, (VPTR)sFileName, &dc)))
		return -1;

	glc2d::m_vTex.push_back(pTexture);

	// ID를 돌려 준다.
	return pTexture->GetID();
}

int glc2d_TextureRelease(int _nID)
{
	auto itr = std::find_if(glc2d::m_vTex.begin(), glc2d::m_vTex.end()
		, [_nID](const auto& p){return p->GetID() == _nID;});
	if(itr == glc2d::m_vTex.end())
	{
		return -1;
	}
	SAFE_DELETE(*itr);
	glc2d::m_vTex.erase(itr);
	return static_cast<int>(glc2d::m_vTex.size());
}

int glc2d_TextureWidth(int _nID)
{
	auto itr = std::find_if(glc2d::m_vTex.begin(), glc2d::m_vTex.end()
		, [_nID](const auto& p){return p->GetID() == _nID;});
	if(itr == glc2d::m_vTex.end())
	{
		return -1;
	}
	auto ret = *itr;
	return ret->GetImageWidth();
}

int glc2d_TextureHeight(int _nID)
{
	auto itr = std::find_if(glc2d::m_vTex.begin(), glc2d::m_vTex.end()
		, [_nID](const auto& p){return p->GetID() == _nID;});
	if(itr == glc2d::m_vTex.end())
	{
		return -1;
	}
	auto ret = *itr;
	return ret->GetImageHeight();
}
