// This is a earthworm game.
//
// Author: 박세연(청강대학교: parkse83@hotmail.com)
// Date  : 2006-06-16
//
////////////////////////////////////////////////////////////////////////////////

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

#include "_StdAfx.h"

int g_GameState = GM_START;
int g_nFont1;
int g_Stage = 0;
int g_Next_Stage = 0;
int g_Level = 0;
int g_ToTal_Grade = 0;
int g_KeyInput;
int g_nCount = 1;
tgFOOD FOOD[MAX_Tail];
int g_ItemCount = 0;
DWORD g_EndTime;
DWORD g_BigenTime;
UserInput g_Input;
int g_Speed;
tgScreen ScreenSrc[Screen_MAX];
tgLINE LINE[MAX_Tail];


int main()
{
	printf("Play Snake Game!\n\n");

	//배경색을 바꾼다.
	g2_SetClearColor(0xFFc8c8c8);

	// 아이콘을 바꾼다.
	g2_SetWindowIcon(IDI_MAIN_ICON);

	// 윈도우를 만든다.
	g2_CreateWin(100, 100, 640, 480, "- Snake Game", true, 1.6F);

	g_nFont1	= g2_FontCreate("굴림", 15, 0);

	// 그림을 프로그램에 로딩
	ScreenSrc[0].nTx = g2_TextureLoad("Texture/Header1.bmp",0xFFFF00FF);
	ScreenSrc[0].iImgW = g2_TextureWidth(ScreenSrc[0].nTx);
	ScreenSrc[0].iImgH = g2_TextureHeight(ScreenSrc[0].nTx);

	ScreenSrc[1].nTx = g2_TextureLoad("Texture/Tail.bmp",0xFFFF00FF);
	ScreenSrc[1].iImgW = g2_TextureWidth(ScreenSrc[1].nTx);
	ScreenSrc[1].iImgH = g2_TextureHeight(ScreenSrc[1].nTx);

	ScreenSrc[2].nTx = g2_TextureLoad("Texture/BG1.bmp",0xFFFF00FF);
	ScreenSrc[2].iImgW = g2_TextureWidth(ScreenSrc[2].nTx);
	ScreenSrc[2].iImgH = g2_TextureHeight(ScreenSrc[2].nTx);

	ScreenSrc[3].nTx = g2_TextureLoad("Texture/BG2.bmp");
	ScreenSrc[3].iImgW = g2_TextureWidth(ScreenSrc[3].nTx);
	ScreenSrc[3].iImgH = g2_TextureHeight(ScreenSrc[3].nTx);

	ScreenSrc[4].nTx = g2_TextureLoad("Texture/rol.bmp");
	ScreenSrc[4].iImgW = g2_TextureWidth(ScreenSrc[4].nTx);
	ScreenSrc[4].iImgH = g2_TextureHeight(ScreenSrc[4].nTx);

	ScreenSrc[5].nTx = g2_TextureLoad("Texture/Food1.bmp",0xFFFF00FF);
	ScreenSrc[5].iImgW = g2_TextureWidth(ScreenSrc[5].nTx);
	ScreenSrc[5].iImgH = g2_TextureHeight(ScreenSrc[5].nTx);

	ScreenSrc[6].nTx = g2_TextureLoad("Texture/item1.bmp",0xFFFF00FF);
	ScreenSrc[6].iImgW = g2_TextureWidth(ScreenSrc[6].nTx);
	ScreenSrc[6].iImgH = g2_TextureHeight(ScreenSrc[6].nTx);

	ScreenSrc[7].nTx = g2_TextureLoad("Texture/item2.bmp",0xFFFF00FF);
	ScreenSrc[7].iImgW = g2_TextureWidth(ScreenSrc[7].nTx);
	ScreenSrc[7].iImgH = g2_TextureHeight(ScreenSrc[7].nTx);

	ScreenSrc[8].nTx = g2_TextureLoad("Texture/bg3.bmp");
	ScreenSrc[8].iImgW = g2_TextureWidth(ScreenSrc[8].nTx);
	ScreenSrc[8].iImgH = g2_TextureHeight(ScreenSrc[8].nTx);

	ScreenSrc[9].nTx = g2_TextureLoad("Texture/bg4.bmp");
	ScreenSrc[9].iImgW = g2_TextureWidth(ScreenSrc[9].nTx);
	ScreenSrc[9].iImgH = g2_TextureHeight(ScreenSrc[9].nTx);

	ScreenSrc[10].nTx = g2_TextureLoad("Texture/side1.bmp");
	ScreenSrc[10].iImgW = g2_TextureWidth(ScreenSrc[10].nTx);
	ScreenSrc[10].iImgH = g2_TextureHeight(ScreenSrc[10].nTx);

	// 화면에 출력하기 위해서 함수를 연결한다.
	g2_SetFrameMove(FrameMove);
	g2_SetRender(Render);

	g_BigenTime = timeGetTime();
	g_EndTime = g_BigenTime;

	g2_Run();


//	g2_TextureRelease(nTx);

	g2_DestroyWin();


	return 0;
}

int FrameMove()
{
	memcpy(g_Input.pKeyOld,   g_Input.pKeyNew,	sizeof g_Input.pKeyOld);
	memset(g_Input.pKeyNew,					0,	sizeof g_Input.pKeyNew);
	memcpy(g_Input.pKeyNew, g2_GetKeyboard(),sizeof g_Input.pKeyNew);

	if(g_Input.pKeyNew[VK_MENU] == EINPUT_PRESS && g_Input.pKeyNew[VK_RETURN] == EINPUT_DOWN)
	{
		g2_ChangeWindow(!g2_GetWindowMode());
	}

	switch(g_GameState)
	{
	case GM_START:
		IntroFrameMove();
		break;
	case GM_PLAY:
		PlayFrameMove();
		break;
	case GM_END:
		EndFrameMove();
		break;
	}

	return 0;
}

int Render()
{
	switch(g_GameState)
	{
	case GM_START:
		IntroRender();
		break;
	case GM_PLAY:
		PlayRender();
		break;
	case GM_END:
		EndRender();
		break;
	}

	return 0;
}

