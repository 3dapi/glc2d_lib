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
	glc2d_InitSdk();
	glc2d_SetClearColor(0xFF006699);
	// 아이콘 설정.
	glc2d_SetWindowIcon(IDI_MAIN_ICON);

	glc2d_SetRender(Render);
	glc2d_SetFrameMove(FrameMove);

	glc2d_CreateWin(100, 100, 800, 600, "Resource", false);

	nTx1	= glc2d_TextureLoad("Texture/lena.png");
	iImgW1	= glc2d_TextureWidth(nTx1);
	iImgH1	= glc2d_TextureHeight(nTx1);

	nFont1 = glc2d_FontCreate("굴림", 20, 0);
	nFont2 = glc2d_FontCreate("Arial", 25, 1);
	nFont3 = glc2d_FontCreate("궁서", 20, 1);

	glc2d_Run();

	glc2d_DestroyWin();

	return 0;
}

int FrameMove()
{
	mouseX = glc2d_GetMouseX();
	mouseY = glc2d_GetMouseY();
	mouseZ = glc2d_GetMouseZ();

	pKey = glc2d_GetKeyboard();
	if(pKey[VK_SPACE])
	{
		glc2d_ChangeWindow(true);
	}

	return 0;
}

int Render()
{
	RECT	rt1 = {0,0,iImgW1, iImgH1};

	glc2d_Draw2D(nTx1, &rt1);

	glc2d_FontDrawText(nFont2, 10, 10, 500, 40, 0xffffFF00, "마우스의 오른쪽 버튼을 눌러봐봐!!!");

	if( glc2d_GetMouseEvent(1))		// R button
	{
		int c= glc2d_FontDrawText(nFont3, mouseX, mouseY, mouseX+500, mouseY+ 40
			, 0xFF99AAFF
			, "Mouse 위치: %d %d %d ", mouseX, mouseY, mouseZ);
	}

	return 0;
}