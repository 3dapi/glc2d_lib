#include <new>
#include <cstdio>
#include <string>

#include <windows.h>
#include <mfplay.h>

#include "glcInternal.h"
#include "ILcSound.h"

#pragma comment(lib, "mfplay.lib")

namespace glc
{

class LcMfPlayerCallback : public IMFPMediaPlayerCallback
{
public:
	LONG				m_cRef{1};
	class LcMfPlayer*	m_owner{};

public:
	explicit LcMfPlayerCallback(LcMfPlayer* owner): m_owner(owner) {}
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override
	{
		if(!ppv)
			return E_POINTER;
		*ppv = nullptr;
		if(riid == __uuidof(IUnknown) || riid == __uuidof(IMFPMediaPlayerCallback))
		{
			*ppv = static_cast<IMFPMediaPlayerCallback*>(this);
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}
	ULONG STDMETHODCALLTYPE AddRef() override
	{
		return InterlockedIncrement(&m_cRef);
	}
	ULONG STDMETHODCALLTYPE Release() override
	{
		ULONG count = InterlockedDecrement(&m_cRef);
		if(count == 0)
			delete this;
		return count;
	}

	void STDMETHODCALLTYPE OnMediaPlayerEvent(MFP_EVENT_HEADER* event) override;
	bool IsLoop() const;
};

class LcMfPlayer: public ILcSound
{
protected:
	int					m_nID{-1};
	std::string			m_sFile;
	IMFPMediaPlayer*	m_player{};
	bool				m_bLoop{};
	LcMfPlayerCallback*	m_mfCb {};

public:
	LcMfPlayer();
	virtual ~LcMfPlayer();

	int		Create(int nID, void* p1=nullptr, void* p2=nullptr);
	void	Destroy();

	int		GetID() override;
	int		Play(bool bLoop=false) override;
	int		Stop() override;
	int		Reset() override;
	int		GetState() override;
	bool	IsLoop() const
	{
		return m_bLoop;
	}
};


LcMfPlayer::LcMfPlayer()
{
}

LcMfPlayer::~LcMfPlayer()
{
	Destroy();
}

void LcMfPlayer::Destroy()
{
	if(m_player)
	{
		m_player->Shutdown();
		m_player->Release();
		m_player = {};
	}
	if(m_mfCb)
	{
		m_mfCb->Release();
		m_mfCb = {};
	}
	if(!m_sFile.empty())
		m_sFile.clear();
	m_nID = -1;
}

int LcMfPlayer::Create(int nID, void* p1, void* p2)
{
	auto sFile = (CSTR)p1;
	std::wstring wsFile;
	IMFPMediaPlayer* player	{};
	LcMfPlayerCallback* mfCb{};
	HRESULT hr {-1};
	if(!sFile || !sFile[0])
	{
		printf("Err::LcMfPlayer::Create\n");
		return -1;
	}
	wsFile = ToWideString(sFile);
	if(wsFile.empty())
	{
		goto ERR_END;
	}
	mfCb = new (std::nothrow) LcMfPlayerCallback{this};
	if(!mfCb)
	{
		goto ERR_END;
	}
	hr = MFPCreateMediaPlayer(wsFile.c_str(), FALSE, 0, mfCb, nullptr, &player);
	if(FAILED(hr) || !player)
	{
		goto ERR_END;
	}
	m_mfCb   = mfCb;
	m_player = player;
	m_sFile = sFile;
	m_nID = nID;
	return 0;
ERR_END:
	if(player)
	{
		player->Shutdown();
		player->Release();
	}
	if(mfCb)
	{
		mfCb->Release();
	}
	if(!wsFile.empty())
	{
		wsFile.clear();
	}
	printf("Create MF Player Failed: %s\n", sFile);
	return -1;
}

int LcMfPlayer::GetID()
{
	return m_nID;
}

int LcMfPlayer::Play(bool bLoop)
{
	if(!m_player)
		return -1;
	m_bLoop = bLoop;
	return static_cast<int>(m_player->Play());
}

int LcMfPlayer::Stop()
{
	if(!m_player)
		return -1;
	m_bLoop = false;
	return static_cast<int>(m_player->Stop());
}

int LcMfPlayer::Reset()
{
	return Stop();
}

int LcMfPlayer::GetState()
{
	if(!m_player)
		return 0;

	MFP_MEDIAPLAYER_STATE state = MFP_MEDIAPLAYER_STATE_EMPTY;

	if(FAILED(m_player->GetState(&state)))
		return 0;

	return state == MFP_MEDIAPLAYER_STATE_PLAYING ? 1 : 0;
}


void STDMETHODCALLTYPE LcMfPlayerCallback::OnMediaPlayerEvent(MFP_EVENT_HEADER* pEventHeader)
{
	if(!pEventHeader || FAILED(pEventHeader->hrEvent))
		return; // 에러 처리.

	switch(pEventHeader->eEventType)
	{
		//case MFP_EVENT_TYPE_MEDIAITEM_CREATED:
		//{
		//	MFP_MEDIAITEM_CREATED_EVENT* pEvent = MFP_GET_MEDIAITEM_CREATED_EVENT(pEventHeader);
		//	// pEvent->pMediaItem 사용.
		//	break;
		//}
		case MFP_EVENT_TYPE_PLAYBACK_ENDED:
		{
			if(IsLoop())
				pEventHeader->pMediaPlayer->Stop();
			break;
		}

		case MFP_EVENT_TYPE_STOP:
		{
			if(IsLoop())
				pEventHeader->pMediaPlayer->Play();
			break;
		}
	}
}

bool LcMfPlayerCallback::IsLoop() const
{
	if(m_owner)
		return m_owner->IsLoop();
	return false;
}

int LcDev_MfPlayerCreate(ILcSound** pData, int nID, void* p1, void* p2)
{
	if(!pData)
		return -1;

	*pData = {};

	LcMfPlayer* pObj = new(std::nothrow) LcMfPlayer;

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
