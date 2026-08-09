#pragma once

#include <random>

#include "glc2d.h"

//--------------------------------------------------------------------------
// Constants
//--------------------------------------------------------------------------

constexpr int GAME_WIN_COUNT = 3;

enum EGameState
{
	GAME_READY = 0,
	GAME_SELECT,
	GAME_RESULT,
	GAME_OVER,
};

enum EHand
{
	HAND_ROCK = 0,
	HAND_SCISSOR,
	HAND_PAPER,
	HAND_MAX,
};

//--------------------------------------------------------------------------
// Common Data
//--------------------------------------------------------------------------

struct GL_RECT
{
	int x = 0, y = 0, w = 0, h = 0;

	bool Contains(int px, int py) const
	{
		return x <= px && px < x + w &&
			   y <= py && py < y + h;
	}
};

//--------------------------------------------------------------------------
// Resource Data
//--------------------------------------------------------------------------

struct GameButtonData
{
	const char* texNormalFile {};
	const char* texOverFile {};
	GL_RECT rc{};
};

struct HandResourceData
{
	const char* texLeftFile {};
	const char* texRightFile {};
	GameButtonData button{};
};

//--------------------------------------------------------------------------
// Runtime Data
//--------------------------------------------------------------------------

struct GameButton
{
	int texNormal = -1;
	int texOver = -1;
	GL_RECT rc{};

	bool IsMouseOver(int mouse_x, int mouse_y) const
	{
		return rc.Contains(mouse_x, mouse_y);
	}

	void Draw(int mouse_x, int mouse_y) const
	{
		int tex = texNormal;
		if(texOver >= 0 && IsMouseOver(mouse_x, mouse_y))
		{
			tex = texOver;
		}
		VEC2 pos{float(rc.x), float(rc.y)};
		g2_Draw2D(tex, nullptr, &pos);
	}
};

struct HandData
{
	int texLeft = -1;
	int texRight = -1;
	GameButton button{};
};

//--------------------------------------------------------------------------
// Game Data
//--------------------------------------------------------------------------

extern int g_screenW;
extern int g_screenH;

extern EGameState g_gameState;

extern int g_userWin;
extern int g_comWin;

extern EHand g_handUser;
extern EHand g_handCom;
extern bool g_hasResult;

extern int g_texBg;
extern int g_texScore;
extern int g_texStart;

extern HandData g_hand[HAND_MAX];

extern GameButton g_btnContinue;
extern GameButton g_btnEnd;

extern GL_RECT g_rcStart;

extern int g_fontScore;
extern int g_fontMsg;

extern std::mt19937 g_random;

//--------------------------------------------------------------------------
// Game Data Load
//--------------------------------------------------------------------------

int GameLoadData(void);
EHand GetRandomHand(void);

//--------------------------------------------------------------------------
// Game
//--------------------------------------------------------------------------

int GameInit(void);
int GameDestroy(void);
int GameFrameMove(void);
int GameRender(void);
