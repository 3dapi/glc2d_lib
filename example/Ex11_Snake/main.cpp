// This is a earthworm game.
//
// Author: 박세연(청강대학교: parkse83@hotmail.com)
// Date  : 2006-06-16
//
////////////////////////////////////////////////////////////////////////////////

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
	glc2d_SetClearColor(0xFFc8c8c8);
	glc2d_CreateWin(100, 100, 640, 480, "- Snake Game", false);
	g_nFont1	= glc2d_FontCreate("굴림", 15, 0);

	// 그림을 프로그램에 로딩
	ScreenSrc[0].nTx = glc2d_TextureLoad("Texture/Header1.bmp",0xFFFF00FF);
	ScreenSrc[0].iImgW = glc2d_TextureWidth(ScreenSrc[0].nTx);
	ScreenSrc[0].iImgH = glc2d_TextureHeight(ScreenSrc[0].nTx);

	ScreenSrc[1].nTx = glc2d_TextureLoad("Texture/Tail.bmp",0xFFFF00FF);
	ScreenSrc[1].iImgW = glc2d_TextureWidth(ScreenSrc[1].nTx);
	ScreenSrc[1].iImgH = glc2d_TextureHeight(ScreenSrc[1].nTx);

	ScreenSrc[2].nTx = glc2d_TextureLoad("Texture/BG1.bmp",0xFFFF00FF);
	ScreenSrc[2].iImgW = glc2d_TextureWidth(ScreenSrc[2].nTx);
	ScreenSrc[2].iImgH = glc2d_TextureHeight(ScreenSrc[2].nTx);

	ScreenSrc[3].nTx = glc2d_TextureLoad("Texture/BG2.bmp");
	ScreenSrc[3].iImgW = glc2d_TextureWidth(ScreenSrc[3].nTx);
	ScreenSrc[3].iImgH = glc2d_TextureHeight(ScreenSrc[3].nTx);

	ScreenSrc[4].nTx = glc2d_TextureLoad("Texture/rol.bmp");
	ScreenSrc[4].iImgW = glc2d_TextureWidth(ScreenSrc[4].nTx);
	ScreenSrc[4].iImgH = glc2d_TextureHeight(ScreenSrc[4].nTx);

	ScreenSrc[5].nTx = glc2d_TextureLoad("Texture/Food1.bmp",0xFFFF00FF);
	ScreenSrc[5].iImgW = glc2d_TextureWidth(ScreenSrc[5].nTx);
	ScreenSrc[5].iImgH = glc2d_TextureHeight(ScreenSrc[5].nTx);

	ScreenSrc[6].nTx = glc2d_TextureLoad("Texture/item1.bmp",0xFFFF00FF);
	ScreenSrc[6].iImgW = glc2d_TextureWidth(ScreenSrc[6].nTx);
	ScreenSrc[6].iImgH = glc2d_TextureHeight(ScreenSrc[6].nTx);

	ScreenSrc[7].nTx = glc2d_TextureLoad("Texture/item2.bmp",0xFFFF00FF);
	ScreenSrc[7].iImgW = glc2d_TextureWidth(ScreenSrc[7].nTx);
	ScreenSrc[7].iImgH = glc2d_TextureHeight(ScreenSrc[7].nTx);

	ScreenSrc[8].nTx = glc2d_TextureLoad("Texture/bg3.bmp");
	ScreenSrc[8].iImgW = glc2d_TextureWidth(ScreenSrc[8].nTx);
	ScreenSrc[8].iImgH = glc2d_TextureHeight(ScreenSrc[8].nTx);

	ScreenSrc[9].nTx = glc2d_TextureLoad("Texture/bg4.bmp");
	ScreenSrc[9].iImgW = glc2d_TextureWidth(ScreenSrc[9].nTx);
	ScreenSrc[9].iImgH = glc2d_TextureHeight(ScreenSrc[9].nTx);

	ScreenSrc[10].nTx = glc2d_TextureLoad("Texture/side1.bmp");
	ScreenSrc[10].iImgW = glc2d_TextureWidth(ScreenSrc[10].nTx);
	ScreenSrc[10].iImgH = glc2d_TextureHeight(ScreenSrc[10].nTx);

	// 화면에 출력하기 위해서 함수를 연결한다.
	glc2d_SetFrameMove(FrameMove);
	glc2d_SetRender(Render);

	g_BigenTime = timeGetTime();
	g_EndTime = g_BigenTime;

	glc2d_SetStateShow(false);// State 보여주는 것을 감춘다.


	glc2d_Run();



//	glc2d_TextureRelease(nTx);

	glc2d_DestroyWin();


	return 0;
}

int FrameMove()
{
	memcpy(g_Input.pKeyOld,   g_Input.pKeyNew,	sizeof g_Input.pKeyOld);
	memset(g_Input.pKeyNew,					0,	sizeof g_Input.pKeyNew);
	memcpy(g_Input.pKeyNew, glc2d_GetKeyboard(),sizeof g_Input.pKeyNew);

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

