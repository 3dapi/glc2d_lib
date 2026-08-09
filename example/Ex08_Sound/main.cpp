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

int		nFont1;

int		mouseX;
int		mouseY;
int		mouseZ;

const KEYCODE* pKey;


int		nSound1;
int		nSound2;


int main()
{
	g2_InitSdk();
	g2_SetClearColor(0xFF006699);
	g2_SetRender(Render);
	g2_SetFrameMove(FrameMove);

	g2_CreateWin(100, 100, 800, 600, "Sound");
	nTx1	= g2_TextureLoad("Texture/lena.png");
	iImgW1	= g2_TextureWidth(nTx1);
	iImgH1	= g2_TextureHeight(nTx1);

	nFont1 = g2_FontCreate("Arial", 30, 0);
	nSound1 = g2_SoundLoad("sound/bounce.wav");
	nSound2 = g2_SoundLoad("sound/move3.wav");

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


	if(pKey[VK_LEFT] && !g2_SoundIsPlaying(nSound1))
	{
		g2_SoundReset(nSound1);
		g2_SoundPlay(nSound1);
	}

	if(pKey[VK_RIGHT] && !g2_SoundIsPlaying(nSound2))
	{
		g2_SoundReset(nSound2);
		g2_SoundPlay(nSound2);
	}


	return 0;
}

int Render()
{
	RECT	rt1 = {0,0,iImgW1, iImgH1};

	g2_Draw2D(nTx1, &rt1);


	int c= g2_FontDrawText(nFont1, 20, 300, 500, 340
			, 0xffFFAA44, "Tri to Press Left or Right Key");



	return 0;
}