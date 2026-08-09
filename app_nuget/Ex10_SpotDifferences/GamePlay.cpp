#include <string>
#include <vector>

#include "glc2d.h"
#include "GameHeader.h"

//--------------------------------------
// Global
//--------------------------------------
int g_gamePhase = GAME_INIT;
bool g_isVictory = false;

// screen size
int g_screenW = 800;
int g_screenH = 480;

// check / ui texture
int g_texCheck = -1;

// font
int g_fontMain = -1;
int g_fontTimer = -1;

// check index
std::vector<int> g_checked;

// current stage
int g_curStage = 0;

// game stage
std::vector<GameStage> g_gameStage(GAME_MAX_STAGE);

// timer
long long g_timeStart = 0;
int g_timeRemain = 200;

//--------------------------------------
// Utils
//--------------------------------------

void AddCheck(int idx)
{
	for(int v : g_checked)
	{
		if(v == idx)
		{
			return;
		}
	}

	g_checked.push_back(idx);
}

bool IsMouseOver(const GameButton& button)
{
	int mx = g2_GetMouseX();
	int my = g2_GetMouseY();

	int width =
		button.rcNormal.right - button.rcNormal.left;

	int height =
		button.rcNormal.bottom - button.rcNormal.top;

	return
		mx >= button.pos.x &&
		mx <  button.pos.x + width &&
		my >= button.pos.y &&
		my <  button.pos.y + height;
}

void NextStage()
{
	g_curStage++;
	if(g_curStage >= GAME_MAX_STAGE)
	{
		g_curStage = 0;
	}

	g_checked.clear();

	g_timeStart = g2_TimeGetTime();
	g_timeRemain = 200;

	g_isVictory = false;
	g_gamePhase = GAME_PLAY;
}

//--------------------------------------
// Game
//--------------------------------------

int GameGetScreenWidth(void)
{
	return g_screenW;
}

int GameGetScreenHeight(void)
{
	return g_screenH;
}

int GameInit(void)
{
	g_curStage = 0;

	g_texCheck = g2_TextureLoad(g_checkFile.c_str());
	g_fontMain = g2_FontCreate(g_fontNameMain.c_str(), 64, 0);
	g_fontTimer = g2_FontCreate(g_fontNameTimer.c_str(), 32, 0);

	g_gameStage.clear();
	for(size_t i = 0; i < GAME_MAX_STAGE; ++i)
	{
		GameStage gameStage{};
		auto& tex = g_texNames[i];
		gameStage.texLeft = g2_TextureLoad(tex[0].c_str());
		gameStage.texRight = g2_TextureLoad(tex[1].c_str());
		gameStage.chckRc = &g_rects[i];
		g_gameStage.push_back(gameStage);
	}


	g_checked.clear();

	g_timeStart = g2_TimeGetTime();
	g_timeRemain = 200;

	g_isVictory = false;
	g_gamePhase = GAME_PLAY;

	return 0;
}


int GameDestroy(void)
{
	return 0;
}

//--------------------------------------
// Update
//--------------------------------------

int GameFrameMove(void)
{
	const auto& curStage =
		g_gameStage[g_curStage];

	int mx = g2_GetMouseX();
	int my = g2_GetMouseY();

	int click = g2_GetMouseEvent(0);


	//----------------------------------
	// Button
	//----------------------------------

	if(click == EINPUT_DOWN)
	{
		if(IsMouseOver(g_btnNext))
		{
			NextStage();
			return 0;
		}

		if(IsMouseOver(g_btnExit))
		{
			PostMessage(g2_GetHwnd(), WM_CLOSE, 0, 0);
			return 0;
		}
	}

	//----------------------------------
	// Game Play
	//----------------------------------

	if(g_gamePhase == GAME_PLAY)
	{
		if(click == EINPUT_DOWN)
		{
			int imageX = mx;

			// 오른쪽 그림이면
			// 왼쪽 그림 기준 좌표로 변환
			if(mx >= curStage.IMAGE_WIDTH)
			{
				imageX -= curStage.IMAGE_WIDTH;
			}

			const auto& gameCheckRect =
				g_rects[g_curStage];

			for(int i = 0;
				i < static_cast<int>(gameCheckRect.size());
				++i)
			{
				if(IsInRect(
					imageX,
					my,
					gameCheckRect[i]))
				{
					AddCheck(i);
					break;
				}
			}
		}

		//----------------------------------
		// Timer
		//----------------------------------

		long long cur = g2_TimeGetTime();

		if(cur - g_timeStart >= 1000)
		{
			g_timeStart = cur;
			g_timeRemain--;

			if(g_timeRemain <= 0)
			{
				g_timeRemain = 0;
				g_isVictory = false;
				g_gamePhase = GAME_END;
			}
		}

		//----------------------------------
		// Victory
		//----------------------------------

		if(g_checked.size() == 1)//			curStage.chckRc->size())
		{
			g_isVictory = true;
			g_gamePhase = GAME_END;
		}
	}

	return 0;
}

//--------------------------------------
// Render
//--------------------------------------

void DrawBackground()
{
	const auto& curStage = g_gameStage[g_curStage];
	// left
	g2_Draw2D(curStage.texLeft, nullptr);

	// right
	VEC2 pos = {(float)curStage.IMAGE_WIDTH, 0.0f};
	g2_Draw2D(curStage.texRight, nullptr, &pos);
}

void DrawChecked()
{
	auto rcImage = RECT{0, 0, 64, 64};
	const auto& curStage = g_gameStage[g_curStage];
	for(int idx : g_checked)
	{
		const auto& r = curStage.chckRc->at(idx);

		VEC2 pLeft;
		pLeft.x = r.x + r.w * 0.5f - 32.0f;
		pLeft.y = r.y + r.h * 0.5f - 32.0f;
		VEC2 pRight = pLeft;
		pRight.x += curStage.IMAGE_WIDTH;
		g2_Draw2D(g_texCheck, &rcImage, &pLeft);
		g2_Draw2D(g_texCheck, &rcImage, &pRight);
	}
}

void DrawButton(const GameButton& button)
{
	bool over = IsMouseOver(button);
	RECT rc = over ? button.rcOver : button.rcNormal;
	VEC2 pos = button.pos;
	g2_Draw2D(g_texCheck, &rc, &pos);
}

void DrawGameUI()
{
	if(g_isVictory)
		DrawButton(g_btnNext);

	DrawButton(g_btnExit);

	char buf[64];
	sprintf(buf, "Time: %d", g_timeRemain);

	g2_FontDrawText(g_fontTimer, 10, 440, 300, 470, 0xFFFFFFFF, buf);
}

int GameRender(void)
{
	DrawBackground();

	// Check 표시
#if TRUE
	DrawChecked();
#endif


	// Debug: 모든 check rect
#if FALSE
	const auto& curStage =
		g_gameStage[g_curStage];

	curStage.DrawCheckRect(
		g_texCheck
	);
#endif

	DrawGameUI();

	if(g_gamePhase == GAME_END)
	{
		if(g_isVictory)
		{
			g2_FontDrawText(g_fontMain, 250, 200, 800, 400, 0xFFFF00FF, "You Win!!!");
		}
		else
		{
			g2_FontDrawText(g_fontMain, 250, 200, 800, 400, 0xFFFF0000, "Game Over");
		}
	}

	return 0;
}
