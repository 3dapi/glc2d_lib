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

// include the 2d game header file
#include "glc2d.h"
#include <stdio.h>

int FrameMove();
int Render();

// Lena
int		nTx1;
int		iImgW1;
int		iImgH1;

void*	pFont1{};
void*	pFont2{};
void*	pFont3{};

int		mouseX;
int		mouseY;
int		mouseZ;

const KEYCODE* pKey;


int main()
{
	g2_InitSdk();

	g2_SetClearColor(0xFF006699);
	g2_SetRender(Render);
	g2_SetFrameMove(FrameMove);

	g2_CreateWin(100, 100, 800, 600, "2D Game", true, 1.60F);

	nTx1	= g2_TextureLoad("Texture/lena.png");
	iImgW1	= g2_TextureWidth(nTx1);
	iImgH1	= g2_TextureHeight(nTx1);

	g2_FontRegisterFile("Font/nanum_pen.ttf");
	g2_FontRegisterFile("Font/NEXON Lv2 Gothic Bold.ttf");
	g2_FontRegisterFile("Font/netmarbleM.ttf");
	
	pFont1 = g2_LcxFontCreate("넥슨Lv2고딕 Bold", 20, 0);
	pFont2 = g2_LcxFontCreate("netmarble Medium", 25, 0);
	pFont3 = g2_LcxFontCreate("나눔손글씨 펜", 40, 1);

	g2_LcxFontSetText(pFont1, "마우스의 왼쪽 버튼 클릭");

	g2_Run();

	g2_LcxFontRelease(pFont1);
	g2_LcxFontRelease(pFont2);
	g2_LcxFontRelease(pFont3);
	g2_DestroyWin();

	return 0;
}

int FrameMove()
{
	mouseX = g2_GetMouseX();
	mouseY = g2_GetMouseY();
	mouseZ = g2_GetMouseZ();
	pKey = g2_GetKeyboard();
    if(g2_GetMouseEvent(1))
    {
        char text[128];
        snprintf(text, sizeof text, "Mouse 위치: %d %d %d ", mouseX, mouseY, mouseZ);
        // The setter compares UTF-8 bytes and uploads only when the text changes.
		return g2_LcxFontSetText(pFont3, text);
    }

	return 0;
}

int Render()
{
	RECT	rt1 = {0,0,iImgW1, iImgH1};
	g2_Draw2D(nTx1, &rt1);
	g2_LcxFontDraw(pFont1, {10, 10}, 0xffffFF00);

	if( g2_GetMouseEvent(1))		// R button
	{
		int c= g2_LcxFontDraw(pFont3, {mouseX, mouseY}, 0xffffaaff);
	}

	return 0;
}
