#include "GameInfo.h"

namespace GAME
{
	static GameInfo* g_gameInfo = new GameInfo;
	static bool M_CHEAT = false;
}

GameInfo* GameInfo::instance()
{
    return GAME::g_gameInfo;
}

void GameInfo::deleteInstance()
{
    delete GAME::g_gameInfo;
    GAME::g_gameInfo = nullptr;
}

bool GameInfo::IsCheat()
{
    return GAME::M_CHEAT;
}
void GameInfo::SetCheat(bool cheat)
{
    GAME::M_CHEAT = cheat;
}

GameInfo::GameInfo()
{
    m_player = new GamePlayer;
}

GameInfo::~GameInfo()
{
    delete m_player;
    m_player = nullptr;
}

GamePlayer* GameInfo::MainPlayer()
{
    return m_player;
}

void GameInfo::IncreaseScore(int score)
{
    m_gameScore += score;
}
