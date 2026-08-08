#pragma once
#include "glc2d.h"

int GameGetScreenWidth(void);
int GameGetScreenHeight(void);
int GameRender(void);
int GameFrameMove(void);
int GameInit(void);
int GameDestroy(void);

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

enum
{
	GAME_INIT = 0,
	GAME_PLAY = 1,
	//GAME_END  = 2,
};
