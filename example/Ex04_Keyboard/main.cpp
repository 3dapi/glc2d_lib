// link the 2d game library
#if defined(_DEBUG)
  #pragma comment(lib, "glc2d_.lib")
#else
  #pragma comment(lib, "glc2d.lib")
#endif

// include the 2d game header file
#include "glc2d.h"
#include <stdio.h>

int		iImgW;
int		iImgH;
int		nTx;

unsigned char	oldKey[256];

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

	unsigned char* pKey = (unsigned char*)glc2d_GetKeyboard();


	for(int i=0; i<256; ++i)
	{
		if( pKey[i])
			printf("You Presed %d key!!!\n", i);
	}


	return 0;
}


int main()
{
	glc2d_SetClearColor(0xFF006699);
	glc2d_SetWindowStyle(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU| WS_VISIBLE);

	glc2d_CreateWin(100, 100, 800, 600, "McUtil Keyboard Test");

	nTx = glc2d_TextureLoad("Texture/lena.png");
	iImgW = glc2d_TextureWidth(nTx);
	iImgH = glc2d_TextureHeight(nTx);


	glc2d_SetRender(Render);
	glc2d_SetFrameMove(FrameMove);

	glc2d_Run();

	glc2d_DestroyWin();


	return 0;
}