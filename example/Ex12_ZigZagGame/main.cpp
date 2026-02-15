#include <stdio.h>
#include <Stdlib.h>
#include "glc2d.h"

#include "GameHeader.h"

int main()
{
	glc2d_InitSdk();

	int scnW = GameGetScreenWidth();
	int scnH = GameGetScreenHeight();

	glc2d_SetClearColor(0xFF000000);
	//glc2d_SetWindowStyle(WS_OVERLAPPED | WS_CAPTION| WS_VISIBLE);
	glc2d_SetStateShow(false);
	glc2d_SetRender(GameRender);
	glc2d_SetFrameMove(GameFrameMove);

	// create window
	glc2d_CreateWin(0, 0, scnW, scnH, "Simple zigzag 게임", true, 1080.0F/scnH);

	GameInit();

	glc2d_Run();
	GameDestroy();

	glc2d_DestroyWin();
	return 0;
}
