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

int		nTx;		// 텍스처 인덱스

// 마우스 위치 읽기
int mouseX = 0;
int mouseY = 0;
int mouseZ = 0;

int Render()
{
	// 이미지 위치를 마우스 위치로 설정한다.
	VEC2	vcPos(mouseX, mouseY);

	glc2d_Draw2D(nTx, NULL, &vcPos);

	return 0;
}


int FrameMove()
{
	// 마우스 위치 읽기
	mouseX = glc2d_GetMouseX();
	mouseY = glc2d_GetMouseY();
	mouseZ = glc2d_GetMouseZ();

	// 윈도우 타이틀 영역에 마우스 위치 출력
	glc2d_SetWindowTitle("%d %d %d", mouseX, mouseY, mouseZ);

	return 0;
}


int main()
{
	glc2d_InitSdk();

	// 화면에 출력하기 위해서 출력 함수를 연결한다.
	glc2d_SetRender(Render);

	// 데이터 갱신을 위한 함수를 연결한다.
	glc2d_SetFrameMove(FrameMove);

	// 윈도우 생성
	glc2d_CreateWin(100, 100, 800, 600, "My First Game Window");

	// 텍스처 로드
	nTx = glc2d_TextureLoad("Texture/lena.png");

	// 실행
	glc2d_Run();

	// 윈도우 해제
	glc2d_DestroyWin();

	return 0;
}