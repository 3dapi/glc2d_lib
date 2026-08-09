// link the 2d game library
#if defined(_DEBUG)
	#if defined(_M_X64)
		#pragma comment(lib, "glc2d_x64_debug.lib")
	#elif defined(_M_IX86)
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
	g2_InitSdk();

	int scnW = g_screenW;
	int scnH = g_screenH;

	g2_SetWindowIcon(IDI_MAIN_ICON);
	g2_SetClearColor(0xFF006699);
	g2_SetCursorShow(1);
	g2_SetStateShow(0);

	g2_SetRender(GameRender);
	g2_SetFrameMove(GameFrameMove);
	g2_CreateWin(0, 0, scnW, scnH, "Rock Paper Scissors", true, 2.0F);

	GameInit();
	g2_Run();

	GameDestroy();
	g2_DestroyWin();

	return 0;
}
