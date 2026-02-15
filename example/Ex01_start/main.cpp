// link the 2d game library
#if defined(_DEBUG)
  #pragma comment(lib, "glc2d_.lib")
#else
  #pragma comment(lib, "glc2d.lib")
#endif

// include the 2d game header file
#include "glc2d.h"
#include <stdio.h>

int main()
{
	printf("Starting ...\n\n");

	// 윈도우 생성
	glc2d_CreateWin(100, 100, 1024, 600, "My First Game Window");

	// 실행
	glc2d_Run();

	// 윈도우 해제
	glc2d_DestroyWin();

	return 0;
}