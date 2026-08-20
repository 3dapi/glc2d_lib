#include <algorithm>
#include <climits>
#include <cstdio>
#include <cstring>
#include <vector>

#include <windows.h>

#include "glcInternal.h"
#include "ILcSound.h"

namespace glc
{

int LcDev_DsSoundInit(void* hWnd);
void LcDev_DsSoundDestroy();
int LcDev_DsSoundCreate(ILcSound** pData, int nID, void* p1={}, void* p2={});
int LcDev_MfPlayerCreate(ILcSound** pData, int nID, void* p1={}, void* p2={});

typedef std::vector<ILcSound*> lsLcSound;
static lsLcSound m_vSound;
static int m_nIDSound{};

static CSTR LcDev_SoundExtension(CSTR sFileName)
{
	if(!sFileName)
		return {};

	return std::strrchr(sFileName, '.');
}

int LcDev_SoundInit(void* hWnd)
{
	return LcDev_DsSoundInit(hWnd);
}

void LcDev_SoundDestroy()
{
	for(auto& pSound : m_vSound)
	{
		delete pSound;
		pSound = {};
	}
	m_vSound.clear();

	LcDev_DsSoundDestroy();
}

int LcDev_SoundCreate(ILcSound** pData, void* p1, void* p2)
{
	if(!pData)
		return -1;

	*pData = {};

	CSTR sFileName = (CSTR)p1;
	CSTR sExt = LcDev_SoundExtension(sFileName);

	if(!sExt)
		return -1;

	if(m_nIDSound == INT_MAX)
	{
		printf("Overflow Sound List\n");
		return -1;
	}

	int nID = m_nIDSound + 1;
	int hr = -1;

	if(0 == _stricmp(sExt, ".wav"))
	{
		hr = LcDev_DsSoundCreate(pData, nID, p1, p2);
	}
	else if(0 == _stricmp(sExt, ".mp3"))
	{
		hr = LcDev_MfPlayerCreate(pData, nID, p1, p2);
	}
	else
	{
		printf("Unsupported Sound Format: %s\n", sFileName);
		return -1;
	}

	if(FAILED(hr) || !*pData)
		return -1;

	m_nIDSound = nID;
	return 0;
}

ILcSound* LcDev_SoundFind(int _nID)
{
	auto itr = std::find_if(m_vSound.begin(), m_vSound.end()
		, [_nID](const auto& p){return p && p->GetID() == _nID;});

	return itr != m_vSound.end() ? *itr : nullptr;
}


};// namespace glc
////////////////////////////////////////////////////////////////////////////////


int g2_SoundLoad(CSTR sFileName)
{
	glc::ILcSound* pSound = {};

	if(FAILED(glc::LcDev_SoundCreate(&pSound, (VPTR)sFileName)))
		return -1;

	glc::m_vSound.push_back(pSound);
	return pSound->GetID();
}

int g2_SoundRelease(int _nID)
{
	auto itr = std::find_if(glc::m_vSound.begin(), glc::m_vSound.end()
		, [_nID](const auto& p){return p && p->GetID() == _nID;});

	if(itr == glc::m_vSound.end())
		return -1;

	delete *itr;
	glc::m_vSound.erase(itr);

	return static_cast<int>(glc::m_vSound.size());
}

void g2_SoundPlay(int _nID, bool bLoop)
{
	auto pSound = glc::LcDev_SoundFind(_nID);

	if(pSound)
		pSound->Play(bLoop);
}

void g2_SoundStop(int _nID)
{
	auto pSound = glc::LcDev_SoundFind(_nID);

	if(pSound)
		pSound->Stop();
}

void g2_SoundReset(int _nID)
{
	auto pSound = glc::LcDev_SoundFind(_nID);

	if(pSound)
		pSound->Reset();
}

bool g2_SoundIsPlaying(int _nID)
{
	auto pSound = glc::LcDev_SoundFind(_nID);

	if(!pSound)
		return {};

	return 0 != pSound->GetState();
}
