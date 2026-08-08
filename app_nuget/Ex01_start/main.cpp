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

int main()
{
	// SDK 초기화
	glc2d_InitSdk();
	
	printf("Starting ...\n\n");

	// 윈도우 생성
	glc2d_CreateWin(100, 100, 1024, 600, "My First Game Window");

	// 실행
	glc2d_Run();

	// 윈도우 해제
	glc2d_DestroyWin();

	return 0;
}