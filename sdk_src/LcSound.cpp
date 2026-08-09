
#include <algorithm>
#include <vector>

#include <windows.h>
#include <d3d9.h>
#include "DxErrShim.h"
#include <dsound.h>
#include <stdio.h>

#include "DsUtil.h"
#include "ILcSound.h"

namespace glc2d
{

class CLcSound : public ILcSound
{
protected:
	int					m_nID{-1};
	char				m_sFile[128]{};
	CSound*				m_pSnd{};

public:
	CLcSound();
	virtual	~CLcSound();

	virtual	int		Create(void* p1=NULL,void* p2=NULL,void* p3=NULL,void* p4=NULL);
	virtual	void	Destroy();

	int		GetID() override;
	int		Play() override;
	int		Stop() override;
	int		Reset() override;
	int		GetState() override;


	static int				m_nIDSound	;		// Sound ID
	static HWND				m_hWnd		;		// Window Handle
	static CSoundManager*	m_pSndMn	;		//	Sound Manager
};


int				CLcSound::m_nIDSound= {};		// Sound ID
HWND			CLcSound::m_hWnd	= {};		// Window Handle
CSoundManager*	CLcSound::m_pSndMn	= {};		//	Sound Manager

typedef std::vector<ILcSound* >	lsLcSound;
static lsLcSound	m_vSound			;		// Sound List

CLcSound::CLcSound()
{
}

CLcSound::~CLcSound()
{
	Destroy();
}

void CLcSound::Destroy()
{
	SAFE_DELETE(	m_pSnd	);
	memset(m_sFile, 0, sizeof m_sFile);
}


int CLcSound::GetID()
{
	return m_nID;
}

int CLcSound::Create(void* p1,void* p2,void* p3,void* p4)
{
	CSTR	sFile	= (CSTR)p1;
	CSound*	pSnd	= {};

	if(FAILED(CLcSound::m_pSndMn->Create( &pSnd, (LPSTR)sFile, 0, GUID_NULL, 1 )))
	{
		printf("Create Sound Failed: %s\n", sFile);
		m_pSnd = {};
		return -1;
	}

	if(NULL ==pSnd)
		return -1;

	m_pSnd = pSnd;

	// 새로운 아이디 부여
	int _nID = CLcSound::m_nIDSound;

	++_nID;

	// overflow....
	if(_nID<0)
	{
		printf("Overflow Sound List\n");
		delete pSnd;
		pSnd = {};
		return -1;
	}


	CLcSound::m_nIDSound = _nID;
	m_nID = _nID;

	return 0;
}

int CLcSound::Play()
{
	return m_pSnd->Play();
}

int CLcSound::Stop()
{
	return m_pSnd->Stop();
}

int CLcSound::Reset()
{
	return m_pSnd->Reset();
}

int CLcSound::GetState()
{
	return	m_pSnd->IsSoundPlaying();
}

int LcDev_SoundCreate(ILcSound** pData
					, void* p1
					, void* p2
					, void* p3
					, void* p4
					)
{
	*pData = {};
	CLcSound* pObj=new CLcSound;
	if(FAILED(pObj->Create(p1, p2, p3, p4)))
	{
		delete pObj;
		return -1;
	}
	*pData = pObj;
	return 0;
}

int LcDev_SoundInit(void* hWnd)
{
	CLcSound::m_hWnd = (HWND)hWnd;

	CLcSound::m_pSndMn = new CSoundManager;

	if( FAILED(CLcSound::m_pSndMn->Initialize( CLcSound::m_hWnd, DSSCL_PRIORITY ) ) )
	{
		printf("Error: Sound Manager Initialize Failed\n");
		return -1;
	}

	if( FAILED( CLcSound::m_pSndMn->SetPrimaryBufferFormat( 2, 22050, 16 ) ) )
	{
		printf("Error: SetPrimaryBufferFormat Failed\n");
		return -1;
	}

	return 0;
}

void LcDev_SoundDestroy()
{
	size_t iSize = glc2d::m_vSound.size();
	for(size_t i=0; i<iSize; ++i)
	{
		SAFE_DELETE( m_vSound[i]	);
	}
	m_vSound.clear();
	SAFE_DELETE(	CLcSound::m_pSndMn	);
}

ILcSound* LcDev_SoundFind(int _nID)
{
	auto itr = std::find_if(m_vSound.begin(), m_vSound.end()
	, [_nID](const auto& p){return p->GetID() == _nID;});
	return itr != m_vSound.end() ? *itr : nullptr;
}


};// namespace glc2d
////////////////////////////////////////////////////////////////////////////////


int glc2d_SoundLoad(CSTR sFileName)
{
	glc2d::ILcSound*	pSound = {};
	if(FAILED(glc2d::LcDev_SoundCreate(&pSound, (VPTR)sFileName)))
		return -1;

	glc2d::m_vSound.push_back(pSound);
	// ID를 돌려 준다.
	return pSound->GetID();
}


int glc2d_SoundRelease(int _nID)
{
	auto itr = std::find_if(glc2d::m_vSound.begin(), glc2d::m_vSound.end()
		, [_nID](const auto& p){return p->GetID() == _nID;});

	if(itr == glc2d::m_vSound.end())
		return -1;

	glc2d::m_vSound.erase(itr);
	return static_cast<int>(glc2d::m_vSound.size());
}

void glc2d_SoundPlay(int _nID)
{
	auto itr = std::find_if(glc2d::m_vSound.begin(), glc2d::m_vSound.end()
		, [_nID](const auto& p){return p->GetID() == _nID;});
	
	if(itr == glc2d::m_vSound.end())
		return;
	(*itr)->Play();
}

void glc2d_SoundStop(int _nID)
{
	auto itr = std::find_if(glc2d::m_vSound.begin(), glc2d::m_vSound.end()
		, [_nID](const auto& p){return p->GetID() == _nID;});
	
	if(itr == glc2d::m_vSound.end())
		return;
	(*itr)->Stop();
}

void glc2d_SoundReset(int _nID)
{
	auto itr = std::find_if(glc2d::m_vSound.begin(), glc2d::m_vSound.end()
		, [_nID](const auto& p){return p->GetID() == _nID;});

	if(itr == glc2d::m_vSound.end())
		return;
	(*itr)->Reset();
}

bool glc2d_SoundIsPlaying(int _nID)
{
	auto itr = std::find_if(glc2d::m_vSound.begin(), glc2d::m_vSound.end()
		, [_nID](const auto& p){return p->GetID() == _nID;});

	if(itr == glc2d::m_vSound.end())
		return {};
	auto ret = (*itr)->GetState();
	return ret;
}
