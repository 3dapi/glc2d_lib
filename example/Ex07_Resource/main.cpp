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
int		nFont2;
int		nFont3;


int		mouseX;
int		mouseY;
int		mouseZ;

unsigned char*	pKey;


int main()
{
	glc2d_SetClearColor(0xFF006699);
	glc2d_SetWindowStyle(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU| WS_VISIBLE);

	glc2d_CreateWin(100, 100, 800, 600, "Resource", false);
	glc2d_SetRender(Render);
	glc2d_SetFrameMove(FrameMove);

	glc2d_SetStateShow(false);// State 보여주는 것을 감춘다.

	glc2d_SetStateShow(false);// State 보여주는 것을 감춘다.


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

	pKey = (unsigned char*)glc2d_GetKeyboard();

	if(pKey[VK_SPACE])
	{
		glc2d_ChangeWindow();
	}

	return 0;
}

int Render()
{
	RECT	rt1 = {0,0,iImgW1, iImgH1};

	glc2d_Draw2D(nTx1, &rt1);



	int c= glc2d_FontDrawText(nFont2, 10, 300, 500, 40
	, 0xffAAFF00
	, "Alt + Enter 해봐!!! ");


	if( glc2d_GetMouseEvent(1))		// R button
	{
		int c= glc2d_FontDrawText(nFont3, mouseX, mouseY, mouseX+500, mouseY+ 40
			, 0xFF99AAFF
			, "Mouse 위치: %d %d %d ", mouseX, mouseY, mouseZ);
	}

	return 0;
}