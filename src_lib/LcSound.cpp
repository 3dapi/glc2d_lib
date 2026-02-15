

#include <vector>

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "DxErrShim.h"
#include <dsound.h>
#include <stdio.h>

#include "DsUtil.h"

#include "ILcSound.h"


#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }



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
	virtual	int		GetID();

	virtual	int		Play();
	virtual	int		Stop();
	virtual	int		Reset();
	virtual	int		GetState();


	static int				m_nIDSound	;		// Sound ID
	static HWND				m_hWnd		;		// Window Handle
	static CSoundManager*	m_pSndMn	;		//	Sound Manager
};




int				CLcSound::m_nIDSound= 0;		// Sound ID
HWND			CLcSound::m_hWnd	= NULL;		// Window Handle
CSoundManager*	CLcSound::m_pSndMn	= NULL;		//	Sound Manager

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
	char*	sFile	= (char*)p1;
	CSound*	pSnd	= NULL;

	if(FAILED(CLcSound::m_pSndMn->Create( &pSnd, sFile, 0, GUID_NULL, 1 )))
	{
		printf("Create Sound Failed: %s\n", sFile);
		m_pSnd = NULL;
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
		pSnd = NULL;
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
	*pData = NULL;

	CLcSound* pObj=new CLcSound;

	if(FAILED(pObj->Create(p1, p2, p3, p4)))
	{
		delete pObj;
		return -1;
	}

	*pData = pObj;
	return 0;
}


typedef std::vector<ILcSound* >	lsLcSound;
typedef lsLcSound::iterator		itLcSound;


lsLcSound	m_vSound;			// Sound List


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
	int iSize = glc2d::m_vSound.size();

	for(int i=0; i<iSize; ++i)
	{
		SAFE_DELETE( m_vSound[i]	);
	}

	m_vSound.clear();


	SAFE_DELETE(	CLcSound::m_pSndMn	);
}


ILcSound* LcDev_SoundFind(int _nID)
{
	int iSize = m_vSound.size();
	int	nIdx=-1;

	for(int i=0; i<iSize; ++i)
	{
		if(m_vSound[i]->GetID() == _nID)
		{
			nIdx = i;
			break;
		}
	}

	if(nIdx<0 || nIdx>=iSize)
	{
		return 0;
	}

	return m_vSound[nIdx];
}


};// namespace glc2d
////////////////////////////////////////////////////////////////////////////////









int glc2d_SoundLoad(char* sFileName)
{
	glc2d::ILcSound*	p = NULL;

	if(FAILED(glc2d::LcDev_SoundCreate(&p, sFileName)))
		return -1;

	glc2d::m_vSound.push_back(p);

	// ID를 돌려 준다.
	return p->GetID();
}


int glc2d_SoundRelease(int _nID)
{
	int iSize = glc2d::m_vSound.size();
	int	nIdx=-1;

	for(int i=0; i<iSize; ++i)
	{
		if(glc2d::m_vSound[i]->GetID() == _nID)
		{
			nIdx = i;
			break;
		}
	}

	if(nIdx<0 || nIdx>=iSize)
		return -1;


	glc2d::lsLcSound::iterator	itSound;

	itSound = glc2d::m_vSound.begin() + nIdx;
	SAFE_DELETE( glc2d::m_vSound[nIdx]		);
	glc2d::m_vSound.erase(itSound);

	iSize = glc2d::m_vSound.size();

	return iSize;
}



void	glc2d_SoundPlay(int _nID)
{
	int iSize = glc2d::m_vSound.size();
	int	nIdx=-1;

	for(int i=0; i<iSize; ++i)
	{
		if(glc2d::m_vSound[i]->GetID() == _nID)
		{
			nIdx = i;
			break;
		}
	}

	if(nIdx<0 || nIdx>=iSize)
		return;

	glc2d::m_vSound[nIdx]->Play();
}

void	glc2d_SoundStop(int _nID)
{
	int iSize = glc2d::m_vSound.size();
	int	nIdx=-1;

	for(int i=0; i<iSize; ++i)
	{
		if(glc2d::m_vSound[i]->GetID() == _nID)
		{
			nIdx = i;
			break;
		}
	}

	if(nIdx<0 || nIdx>=iSize)
		return;

	glc2d::m_vSound[nIdx]->Stop();
}

void	glc2d_SoundReset(int _nID)
{
	int iSize = glc2d::m_vSound.size();
	int	nIdx=-1;

	for(int i=0; i<iSize; ++i)
	{
		if(glc2d::m_vSound[i]->GetID() == _nID)
		{
			nIdx = i;
			break;
		}
	}

	if(nIdx<0 || nIdx>=iSize)
		return;

	glc2d::m_vSound[nIdx]->Reset();
}

BOOL	glc2d_SoundIsPlaying(int _nID)
{
	int iSize = glc2d::m_vSound.size();
	int	nIdx=-1;

	for(int i=0; i<iSize; ++i)
	{
		if(glc2d::m_vSound[i]->GetID() == _nID)
		{
			nIdx = i;
			break;
		}
	}

	if(nIdx<0 || nIdx>=iSize)
		return 0;

	return glc2d::m_vSound[nIdx]->GetState();
}



