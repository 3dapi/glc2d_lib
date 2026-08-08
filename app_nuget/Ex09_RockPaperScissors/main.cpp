// link the 2d game library
// link the 2d game library
#if defined(_DEBUG)
  #if defined(_M_X64) // 64-bit 아키텍처
    #pragma comment(lib, "glc2d_x64_debug.lib")
  #elif defined(_M_IX86) // 32-bit 아키텍처
    #pragma comment(lib, "glc2d_win32_debug.lib")
  #endif
#else
  #if defined(_M_X64)
    #pragma comment(lib, "glc2d_x64_release.lib")
  #elif defined(_M_IX86)
    #pragma comment(lib, "glc2d_win32_release.lib")
  #endif
#endif
#include "glc2d.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "resource.h"


// ------------------------------
// Game Data
// ------------------------------

#define PLAY_R 1
#define PLAY_S 2
#define PLAY_P 3

int g_screenW = 640, g_screenH = 480;

int g_handUsr = -1;
int g_handCom = -1;
int g_score   = 0;
int g_win     = 1;

int g_mouseX, g_mouseY;
int g_mouseClick;

// textures
int texUIBg;
int texScoreBg;
int texBtn[3];
int texHand[6];

// font
int fontScore;
int fontMsg;

// ------------------------------
// 
int GameGetScreenWidth(void);
int GameGetScreenHeight(void);
// game play 함수.
int GameFrameMove(void);
int GameInit(void);
int GameRender(void);
// ------------------------------

int main()
{
	glc2d_InitSdk();
	int scnW = GameGetScreenWidth();
	int scnH = GameGetScreenHeight();

	glc2d_SetClearColor(0xFF006699);
	glc2d_SetCursorShow(1);
	glc2d_SetStateShow(0);

	glc2d_SetRender(GameRender);
	glc2d_SetFrameMove(GameFrameMove);

	glc2d_CreateWin(0, 0, scnW, scnH, "Rock Paper Scissors");

	GameInit();

	glc2d_Run();

	glc2d_DestroyWin();

	return 0;
}

int GameGetScreenWidth(void)
{
	return g_screenW;
}

int GameGetScreenHeight()
{
	return g_screenH;
}

int GameInit(void)
{
	srand(0);

	texUIBg   = glc2d_TextureLoad("tex_ui/ui_bg.png");
	texScoreBg= glc2d_TextureLoad("tex_ui/ui_score.png");

	texBtn[0] = glc2d_TextureLoad("tex_ui/ui_rock.png");
	texBtn[1] = glc2d_TextureLoad("tex_ui/ui_scissor.png");
	texBtn[2] = glc2d_TextureLoad("tex_ui/ui_paper.png");

	texHand[0] = glc2d_TextureLoad("tex_play/img_l_r.png");
	texHand[1] = glc2d_TextureLoad("tex_play/img_l_s.png");
	texHand[2] = glc2d_TextureLoad("tex_play/img_l_p.png");
	texHand[3] = glc2d_TextureLoad("tex_play/img_r_r.png");
	texHand[4] = glc2d_TextureLoad("tex_play/img_r_s.png");
	texHand[5] = glc2d_TextureLoad("tex_play/img_r_p.png");

	fontScore = glc2d_FontCreate("Arial", 40);
	fontMsg   = glc2d_FontCreate("Arial", 18);

	return 0;
}

// ------------------------------

int GameFrameMove(void)
{
	g_mouseX = glc2d_GetMouseX();
	g_mouseY = glc2d_GetMouseY();
	g_mouseClick = glc2d_GetMouseEvent(0);

	int play = 0;

	if(g_win > 0)
	{
		// Rock
		if(30 < g_mouseX && g_mouseX < 130 &&
			330 < g_mouseY && g_mouseY < 450)
		{
			if(g_mouseClick)
			{
				play = 1;
				g_handUsr = PLAY_R;
				g_handCom = rand() % 3 + 1;
			}
		}

		// Scissor
		if(170 < g_mouseX && g_mouseX < 260 &&
			330 < g_mouseY && g_mouseY < 450)
		{
			if(g_mouseClick)
			{
				play = 1;
				g_handUsr = PLAY_S;
				g_handCom = rand() % 3 + 1;
			}
		}

		// Paper
		if(290 < g_mouseX && g_mouseX < 400 &&
			330 < g_mouseY && g_mouseY < 450)
		{
			if(g_mouseClick)
			{
				play = 1;
				g_handUsr = PLAY_P;
				g_handCom = rand() % 3 + 1;
			}
		}

		if(play && g_handUsr > 0)
		{
			if(g_handUsr == g_handCom)
			{
				// draw message only
			}
			else if((g_handUsr == PLAY_R && g_handCom == PLAY_S) ||
					 (g_handUsr == PLAY_S && g_handCom == PLAY_P) ||
					 (g_handUsr == PLAY_P && g_handCom == PLAY_R))
			{
				g_score += 1000;
			}
			else
			{
				g_win = -1;
			}
		}
	}

	if(g_win < 0)
		g_win = 0;

	return 0;
}

// ------------------------------

int GameRender(void)
{
	VEC2 pos;

	// background
	pos = VEC2{0, 0};
	glc2d_Draw2D(texUIBg, NULL, &pos);

	pos = VEC2{10, 10};
	glc2d_Draw2D(texScoreBg, NULL, &pos);

	// buttons
	if(g_win > 0)
	{
		pos = VEC2{20, 330};
		glc2d_Draw2D(texBtn[0], NULL, &pos);

		pos = VEC2{150, 330};
		glc2d_Draw2D(texBtn[1], NULL, &pos);

		pos = VEC2{280, 330};
		glc2d_Draw2D(texBtn[2], NULL, &pos);
	}

	// hands
	if(g_handUsr > 0)
	{
		pos = VEC2{20, 60};
		glc2d_Draw2D(texHand[g_handUsr-1], NULL, &pos);

		pos = VEC2{350, 60};
		glc2d_Draw2D(texHand[g_handCom+2], NULL, &pos);
	}

	// score
	glc2d_FontDrawText(fontScore, 120, 5, 300, 50, 0xFFFF0000, "%d", g_score);

	// message
	if(g_handUsr > 0)
	{
		if(g_handUsr == g_handCom)
		{
			glc2d_FontDrawText(fontMsg, 100, 300, 500, 340, 0xFFFFFF00, "비겼습니다");
		}
		else if(g_win == 0)
		{
			glc2d_FontDrawText(fontMsg, 100, 300, 500, 340, 0xFFFF0000, "졌습니다");
		}
		else
		{
			glc2d_FontDrawText(fontMsg, 100, 300, 500, 340, 0xFF00FF00, "이겼습니다");
		}
	}

	return 0;
}