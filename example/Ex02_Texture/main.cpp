// link the 2d game library
#if defined(_DEBUG)
  #pragma comment(lib, "glc2d_.lib")
#else
  #pragma comment(lib, "glc2d.lib")
#endif

// include the 2d game header file
#include "glc2d.h"
#include <stdio.h>

int		nTx;					// 이미지 인덱스

VEC2	g_vcVcl(1, 0);			// Object Velocity
VEC2	g_vcPos(400, 200);		// Object Position

FLOAT	g_fSpeed  = 4;			// Object Speed
FLOAT	g_fRadius = 0;			// Radius
FLOAT	g_dRadius = 0.1F;		// Delta Radius
FLOAT	g_dDir    = 1.0F;		// Right or left rotation direction


int Render()
{
	// test....
	Sleep(40);



	FLOAT fAngle = acosf(g_vcVcl.x);
	if(0 > g_vcVcl.y)
		fAngle *= -1.F;

	FLOAT fCos = cosf(fAngle);
	FLOAT fSin = sinf(fAngle);
	VEC2  vDir = VEC2(-fSin, fCos)  * g_dDir;

	g_vcVcl = g_fRadius * g_vcVcl + vDir;		// setup the the velocity
	D3DXVec2Normalize(&g_vcVcl, &g_vcVcl);		// Normalize the velocity

	g_vcPos += g_vcVcl * g_fSpeed;				// accumulate the position

	g_fRadius += g_dRadius;

	// Draw Object
	glc2d_DrawAlphaOption(1);
	glc2d_Draw2D(nTx, NULL, &g_vcPos);
	glc2d_DrawAlphaOption(0);

	return 0;
}


int main()
{
	printf("그림 올리기.......................\n\n");

	//배경색을 바꾼다.
	glc2d_SetClearColor(0xFF336699);
	glc2d_CreateWin(100, 100, 800, 600, "My First Game Window");


	// 그림을 프로그램에 로딩
	nTx = glc2d_TextureLoad("Texture/tst.png");


	// 화면에 출력하기 위해서 함수를 연결한다.
	glc2d_SetRender(Render);


	// 실행
	glc2d_Run();


	// 텍스처 해제
	glc2d_TextureRelease(nTx);

	// 윈도우 해제
	glc2d_DestroyWin();


	return 0;
}