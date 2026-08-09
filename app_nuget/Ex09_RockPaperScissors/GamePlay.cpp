#include "GameHeader.h"

//--------------------------------------------------------------------------
// Utils
//--------------------------------------------------------------------------

bool IsUserWin(EHand user, EHand com)
{
	if(user == HAND_ROCK && com == HAND_SCISSOR)
	{
		return true;
	}

	if(user == HAND_SCISSOR && com == HAND_PAPER)
	{
		return true;
	}

	if(user == HAND_PAPER && com == HAND_ROCK)
	{
		return true;
	}

	return false;
}

void ResetGame()
{
	g_userWin = 0;
	g_comWin = 0;

	g_handUser = HAND_ROCK;
	g_handCom = GetRandomHand();		// 컴퓨터가 먼저 선택
	g_hasResult = false;

	g_gameState = GAME_SELECT;
}

void PlayHand(EHand hand)
{
	g_handUser = hand;
	g_hasResult = true;

	if(g_handUser != g_handCom)
	{
		if(IsUserWin(g_handUser, g_handCom))
		{
			++g_userWin;
		}
		else
		{
			++g_comWin;
		}
	}

	if(g_userWin >= GAME_WIN_COUNT || g_comWin >= GAME_WIN_COUNT)
	{
		g_gameState = GAME_OVER;
	}
	else
	{
		g_gameState = GAME_RESULT;
	}
}

void ContinueGame()
{
	g_handCom = GetRandomHand();		// 다음 판의 컴퓨터 선택
	g_hasResult = false;
	g_gameState = GAME_SELECT;
}

void ExitGame()
{
	PostMessage(g2_GetHwnd(), WM_CLOSE, 0, 0);
}

//--------------------------------------------------------------------------
// Game
//--------------------------------------------------------------------------

int GameInit(void)
{
	GameLoadData();
	g_gameState = GAME_READY;
	return 0;
}

int GameDestroy(void)
{
	return 0;
}

//--------------------------------------------------------------------------
// Update
//--------------------------------------------------------------------------

int GameFrameMove(void)
{
	if(g2_GetMouseEvent(0) != EINPUT_DOWN)
	{
		return 0;
	}

	int mx = g2_GetMouseX();
	int my = g2_GetMouseY();

	if(g_gameState == GAME_READY)
	{
		if(g_rcStart.Contains(mx, my))
		{
			ResetGame();
		}

		return 0;
	}

	if(g_gameState == GAME_SELECT)
	{
		for(int i = 0; i < HAND_MAX; ++i)
		{
			if(g_hand[i].button.IsMouseOver(mx, my))
			{
				PlayHand(static_cast<EHand>(i));
				return 0;
			}
		}

		if(g_btnEnd.IsMouseOver(mx, my))
		{
			ExitGame();
			return 0;
		}
	}

	if(g_gameState == GAME_RESULT)
	{
		if(g_btnContinue.IsMouseOver(mx, my))
		{
			ContinueGame();
			return 0;
		}

		if(g_btnEnd.IsMouseOver(mx, my))
		{
			ExitGame();
			return 0;
		}
	}

	if(g_gameState == GAME_OVER)
	{
		if(g_btnContinue.IsMouseOver(mx, my))
		{
			ResetGame();
			return 0;
		}

		if(g_btnEnd.IsMouseOver(mx, my))
		{
			ExitGame();
			return 0;
		}
	}

	return 0;
}

//--------------------------------------------------------------------------
// Render
//--------------------------------------------------------------------------

void DrawBackground()
{
	VEC2 pos = {0.0f, 0.0f};
	g2_Draw2D(g_texBg, nullptr, &pos);
	pos = {10.0f, 10.0f};
	g2_Draw2D(g_texScore, nullptr, &pos);
}

void DrawScore()
{
	g2_FontDrawText(g_fontScore, 120, 5, 350, 55, 0xFFFF0000, "%d : %d  COM:%d", g_userWin, g_comWin, g_handCom);
}

void DrawHands()
{
	if(!g_hasResult)
	{
		return;
	}
	VEC2 posLeft = {20.0f, 60.0f};
	VEC2 posRight = {350.0f, 60.0f};
	g2_Draw2D(g_hand[g_handUser].texLeft, nullptr, &posLeft );
	g2_Draw2D(g_hand[g_handCom].texRight, nullptr, &posRight);
}

void DrawSelectButtons()
{
	int mx = g2_GetMouseX();
	int my = g2_GetMouseY();

	for(int i = 0; i < HAND_MAX; ++i)
	{
		g_hand[i].button.Draw(mx, my);
	}

	g_btnEnd.Draw(mx, my);
}

void DrawResultMessage()
{
	if(!g_hasResult)
	{
		return;
	}

	if(g_handUser == g_handCom)
	{
		g2_FontDrawText(g_fontMsg, 240, 290, 430, 330, 0xFFFFFF00, "Draw");
		return;
	}

	if(IsUserWin(g_handUser, g_handCom))
	{
		g2_FontDrawText(g_fontMsg, 240, 290, 430, 330, 0xFF00FF00, "You Win");
	}
	else
	{
		g2_FontDrawText(g_fontMsg, 240, 290, 430, 330, 0xFFFF0000, "You Lose");
	}
}

void DrawReady()
{
	VEC2 pos{ float(g_rcStart.x), float(g_rcStart.y) };
	g2_Draw2D(g_texStart, nullptr, &pos);
}

void DrawGameOverMessage()
{
	if(g_userWin >= GAME_WIN_COUNT)
	{
		g2_FontDrawText(g_fontScore, 180, 270, 500, 325, 0xFF00FF00, "YOU WIN!");
	}
	else
	{
		g2_FontDrawText(g_fontScore, 180, 270, 500, 325, 0xFFFF0000, "YOU LOSE");
	}
}

int GameRender(void)
{
	int mx = g2_GetMouseX();
	int my = g2_GetMouseY();

	DrawBackground();
	DrawScore();

	if(g_gameState == GAME_READY)
	{
		DrawReady();
		return 0;
	}

	DrawHands();

	if(g_gameState == GAME_SELECT)
	{
		DrawSelectButtons();
	}
	else if(g_gameState == GAME_RESULT)
	{
		DrawResultMessage();
		g_btnContinue.Draw(mx, my);
		g_btnEnd.Draw(mx, my);
	}
	else if(g_gameState == GAME_OVER)
	{
		DrawResultMessage();
		DrawGameOverMessage();
		g_btnContinue.Draw(mx, my);
		g_btnEnd.Draw(mx, my);
	}

	return 0;
}

