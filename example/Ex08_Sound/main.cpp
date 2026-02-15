// link the 2d game library
#if defined(_DEBUG)
  #pragma comment(lib, "glc2d_.lib")
#else
  #pragma comment(lib, "glc2d.lib")
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

unsigned char*	pKey;


int		nSound1;
int		nSound2;


int main()
{
	glc2d_SetClearColor(0xFF006699);
	glc2d_SetWindowStyle(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU| WS_VISIBLE);

	glc2d_CreateWin(100, 100, 800, 600, "Sound");
	glc2d_SetRender(Render);
	glc2d_SetFrameMove(FrameMove);

	glc2d_SetStateShow(false);// State 보여주는 것을 감춘다.

	nTx1	= glc2d_TextureLoad("Texture/lena.png");
	iImgW1	= glc2d_TextureWidth(nTx1);
	iImgH1	= glc2d_TextureHeight(nTx1);

	nFont1 = glc2d_FontCreate("Arial", 30, 0);

	nSound1 = glc2d_SoundLoad("sound/bounce.wav");
	nSound2 = glc2d_SoundLoad("sound/move3.wav");




	glc2d_Run();


	glc2d_DestroyWin();


	return 0;
}


int FrameMove()
{
	mouseX = glc2d_GetMouseX();
	mouseY = glc2d_GetMouseY();
	mouseZ = glc2d_GetMouseZ();

	pKey = (unsigned char*)glc2d_GetKeyboard();


	if(pKey[VK_LEFT] && !glc2d_SoundIsPlaying(nSound1))
	{
		glc2d_SoundReset(nSound1);
		glc2d_SoundPlay(nSound1);
	}

	if(pKey[VK_RIGHT] && !glc2d_SoundIsPlaying(nSound2))
	{
		glc2d_SoundReset(nSound2);
		glc2d_SoundPlay(nSound2);
	}


	return 0;
}

int Render()
{
	RECT	rt1 = {0,0,iImgW1, iImgH1};

	glc2d_Draw2D(nTx1, &rt1);


	int c= glc2d_FontDrawText(nFont1, 20, 300, 500, 340
			, 0xffFFAA44, "Tri to Press Left or Right Key");



	return 0;
}