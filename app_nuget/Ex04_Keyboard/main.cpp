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
	glc2d_Draw2D(nTx, &rt1);

	return 0;
}

int FrameMove()
{
	glc2d_SetWindowTitle("Change Window Mode. Try to Press Space Bar");

	int mouseX = glc2d_GetMouseX();
	int mouseY = glc2d_GetMouseY();
	int mouseZ = glc2d_GetMouseZ();

	// keyboard 포인터.
	const KEYCODE* pKeyboard = glc2d_GetKeyboard();
	for(int i=9; i<128; ++i)
	{
		if( pKeyboard[i])
			printf("You Pressed %d key!!!\n", i);
	}

	return 0;
}


int main()
{
	glc2d_InitSdk();

	glc2d_SetClearColor(0xFF006699);

	glc2d_SetRender(Render);
	glc2d_SetFrameMove(FrameMove);

	glc2d_CreateWin(100, 100, 800, 600, "McUtil Keyboard Test");

	nTx = glc2d_TextureLoad("Texture/lena.png");
	iImgW = glc2d_TextureWidth(nTx);
	iImgH = glc2d_TextureHeight(nTx);

	glc2d_Run();

	glc2d_DestroyWin();

	return 0;
}