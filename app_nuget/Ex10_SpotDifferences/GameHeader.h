#pragma once
#include <string>
#include <vector>

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
	GAME_INIT		= 0,
	GAME_PLAY		= 1,
	GAME_END		= 2,
	GAME_MAX_STAGE	= 3,
};

// Game Data
// rect (체크 영역)
struct GL_RECT
{
	int x = 0, y = 0;
	int w = 0, h = 0;
};

inline bool IsInRect(int x, int y, const GL_RECT& r)
{
	return (x > r.x && x < r.x + r.w &&
			y > r.y && y < r.y + r.h);
}


// Game Button
struct GameButton
{
	RECT rcNormal{};
	RECT rcOver{};
	VEC2 pos{};
};


// Game Data
extern std::vector<std::vector<GL_RECT> > g_rects;
extern std::vector<std::vector<std::string>> g_texNames;

extern std::string g_checkFile;
extern std::string g_fontNameMain;
extern std::string g_fontNameTimer;

extern GameButton g_btnNext;
extern GameButton g_btnExit;


// Game Stage
struct GameStage
{
	int texLeft = -1;
	int texRight = -1;

	const std::vector<GL_RECT>* chckRc = nullptr;

	int IMAGE_WIDTH = 400;

	void DrawCheckRect(int checkImage) const;
};
