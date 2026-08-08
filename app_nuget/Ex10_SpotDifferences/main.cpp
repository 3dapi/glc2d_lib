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

#include "resource.h"
#include "GameHeader.h"

int main()
{
	glc2d_InitSdk();

	int scnW = GameGetScreenWidth();
	int scnH = GameGetScreenHeight();

	glc2d_SetWindowIcon(IDI_MAIN_ICON);
	glc2d_SetClearColor(0xFF000000);
	glc2d_SetStateShow(false);

	glc2d_SetRender(GameRender);
	glc2d_SetFrameMove(GameFrameMove);

	glc2d_CreateWin(0, 0, scnW, scnH, "Spot The Difference");

	GameInit();
	glc2d_Run();
	GameDestroy();

	glc2d_DestroyWin();
	return 0;
}
