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

int		iImgW;
int		iImgH;
int		nTx;

int Render()
{
	RECT	rt1 = {0,0,iImgW, iImgH};
	g2_Draw2D(nTx, &rt1);

	return 0;
}

int FrameMove()
{
	g2_SetWindowTitle("Change Window Mode. Try to Press Space Bar");

	int mouseX = g2_GetMouseX();
	int mouseY = g2_GetMouseY();
	int mouseZ = g2_GetMouseZ();

	// keyboard 포인터.
	const KEYCODE* pKeyboard = g2_GetKeyboard();
	for(int i=9; i<128; ++i)
	{
		if( pKeyboard[i])
			printf("You Pressed %d key!!!\n", i);
	}

	return 0;
}


int main()
{
	g2_InitSdk();

	g2_SetClearColor(0xFF006699);

	g2_SetRender(Render);
	g2_SetFrameMove(FrameMove);

	g2_CreateWin(100, 100, 800, 600, "McUtil Keyboard Test");

	nTx = g2_TextureLoad("Texture/lena.png");
	iImgW = g2_TextureWidth(nTx);
	iImgH = g2_TextureHeight(nTx);

	g2_Run();

	g2_DestroyWin();

	return 0;
}