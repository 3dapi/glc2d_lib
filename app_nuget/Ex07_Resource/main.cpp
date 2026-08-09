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
#include "resource.h"

int FrameMove();
int Render();

// Lena
int		nTx1;
int		iImgW1;
int		iImgH1;

int		nFont1;
int		nFont2;
int		nFont3;

int		mouseX;
int		mouseY;
int		mouseZ;

const KEYCODE*	pKey;


int main()
{
	g2_InitSdk();
	g2_SetClearColor(0xFF006699);
	// 아이콘 설정.
	g2_SetWindowIcon(IDI_MAIN_ICON);

	g2_SetRender(Render);
	g2_SetFrameMove(FrameMove);

	g2_CreateWin(100, 100, 800, 600, "Resource", false);

	nTx1	= g2_TextureLoad("Texture/lena.png");
	iImgW1	= g2_TextureWidth(nTx1);
	iImgH1	= g2_TextureHeight(nTx1);

	nFont1 = g2_FontCreate("굴림", 20, 0);
	nFont2 = g2_FontCreate("Arial", 25, 1);
	nFont3 = g2_FontCreate("궁서", 20, 1);

	g2_Run();

	g2_DestroyWin();

	return 0;
}

int FrameMove()
{
	mouseX = g2_GetMouseX();
	mouseY = g2_GetMouseY();
	mouseZ = g2_GetMouseZ();

	pKey = g2_GetKeyboard();
	if(pKey[VK_SPACE])
	{
		g2_ChangeWindow(true);
	}
	if(pKey[VK_RCONTROL])
	{
		g2_ChangeWindow(false);
	}

	return 0;
}

int Render()
{
	RECT	rt1 = {0,0,iImgW1, iImgH1};

	g2_Draw2D(nTx1, &rt1);

	g2_FontDrawText(nFont2, 10, 10, 500, 40, 0xffffFF00, "마우스의 오른쪽 버튼 클릭");

	if( g2_GetMouseEvent(1))		// R button
	{
		int c= g2_FontDrawText(nFont3, mouseX, mouseY, mouseX+500, mouseY+ 40
			, 0xFF99AAFF
			, "Mouse 위치: %d %d %d ", mouseX, mouseY, mouseZ);
	}

	return 0;
}