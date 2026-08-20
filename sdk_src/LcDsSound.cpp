#include <string>
#include <cstdio>
#include "DsUtil.h"

#include "glcInternal.h"
#include "ILcSound.h"

namespace glc
{

class LcDsSound : public ILcSound
{
protected:
	int					m_nID{-1};
	std::string			m_sFile;
	CSound*				m_pSnd{};

public:
	LcDsSound();
	virtual ~LcDsSound();

	int		Create(int nID, void* p1=nullptr, void* p2=nullptr);
	void	Destroy();

	int		GetID() override;
	int		Play(bool bLoop=false) override;
	int		Stop() override;
	int		Reset() override;
	int		GetState() override;

	inline static HWND				m_hWnd{};
	inline static CSoundManager*	m_pSndMn{};
};


LcDsSound::LcDsSound()
{
}

LcDsSound::~LcDsSound()
{
	Destroy();
}

void LcDsSound::Destroy()
{
	SAFE_DELETE(m_pSnd);

	m_sFile.clear();
	m_nID = -1;
}

int LcDsSound::Create(int nID, void* p1, void* p2)
{
	CSTR sFile = (CSTR)p1;
	CSound* pSnd = {};

	if(!sFile || !sFile[0] || !LcDsSound::m_pSndMn)
		return -1;

	if(FAILED(LcDsSound::m_pSndMn->Create(&pSnd, (LPSTR)sFile, 0, GUID_NULL, 1)))
	{
		printf("Create DS Sound Failed: %s\n", sFile);
		return -1;
	}

	if(!pSnd)
		return -1;

	m_pSnd = pSnd;
	m_sFile = sFile;
	m_nID = nID;

	return 0;
}

int LcDsSound::GetID()
{
	return m_nID;
}

int LcDsSound::Play(bool bLoop)
{
	if(!m_pSnd)
		return -1;
	DWORD flags = bLoop ? DSBPLAY_LOOPING : 0;
	return m_pSnd->Play(0, flags);
}

int LcDsSound::Stop()
{
	if(!m_pSnd)
		return -1;

	return m_pSnd->Stop();
}

int LcDsSound::Reset()
{
	if(!m_pSnd)
		return -1;

	return m_pSnd->Reset();
}

int LcDsSound::GetState()
{
	if(!m_pSnd)
		return 0;

	return m_pSnd->IsSoundPlaying();
}


int LcDev_DsSoundInit(void* hWnd)
{
	if(LcDsSound::m_pSndMn)
		return 0;

	LcDsSound::m_hWnd = (HWND)hWnd;
	LcDsSound::m_pSndMn = new CSoundManager;

	if(FAILED(LcDsSound::m_pSndMn->Initialize(LcDsSound::m_hWnd, DSSCL_PRIORITY)))
	{
		printf("Error: DS Sound Manager Initialize Failed\n");
		SAFE_DELETE(LcDsSound::m_pSndMn);
		return -1;
	}

	if(FAILED(LcDsSound::m_pSndMn->SetPrimaryBufferFormat(2, 22050, 16)))
	{
		printf("Error: SetPrimaryBufferFormat Failed\n");
		SAFE_DELETE(LcDsSound::m_pSndMn);
		return -1;
	}

	return 0;
}

void LcDev_DsSoundDestroy()
{
	SAFE_DELETE(LcDsSound::m_pSndMn);
	LcDsSound::m_hWnd = {};
}

int LcDev_DsSoundCreate(ILcSound** pData, int nID, void* p1, void* p2)
{
	if(!pData)
		return -1;

	*pData = {};

	LcDsSound* pObj = new(std::nothrow) LcDsSound;

	if(!pObj)
		return -1;

	if(FAILED(pObj->Create(nID, p1, p2)))
	{
		delete pObj;
		return -1;
	}

	*pData = pObj;
	return 0;
}


};// namespace glc
////////////////////////////////////////////////////////////////////////////////
