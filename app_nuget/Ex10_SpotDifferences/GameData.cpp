
#include "GameHeader.h"

//--------------------------------------
// Game Data
//--------------------------------------

std::vector<std::vector<GL_RECT> > g_rects
{
	{
		{   0, 112,  32,  32, },
		{ 224,  62,  32,  32, },
		{  91,   2,  37, 102, },
		{ 280, 212,  30,  30, },
		{ 194, 267,  14,  67, },
		{ 107, 165,  17,  17, },
		{   0, 306,  40,  40, },
		{  50, 261,  20,  24, },
		{ 243, 336,  28,  20, },
		{ 338, 305,  18,  18, },
	},
	{
		{  27, 122, 116,  43, },
		{ 228,  74,  20,  20, },
		{ 374, 110,  20,  20, },
		{ 204, 117,  37,  23, },
		{  11, 181,  32,  52, },
		{  50, 330,  42,  28, },
		{ 162, 262,  43,  23, },
		{ 231, 314,  41,  12, },
		{ 323, 252,  16,  14, },
		{ 364, 215,  21,  19, },
	},
	{
		{  50,  38,  24,  28, },
		{ 378,  10,  20,  14, },
		{  18,  96,  52,  98, },
		{  83, 176,  23,  14, },
		{  63, 254,  24,  24, },
		{  94, 300,  20,  27, },
		{   1, 332,  24,  42, },
		{ 222, 238,  15,  39, },
		{ 240, 347,  34,  26, },
		{ 353, 332,  28,  18, },
	},
};

std::vector<std::vector<std::string>> g_texNames
{
	{
		"Texture/smurf11.png",
		"Texture/smurf12.png",
	},
	{
		"Texture/smurf21.png",
		"Texture/smurf22.png",
	},
	{
		"Texture/smurf31.png",
		"Texture/smurf32.png",
	},
};

std::string g_checkFile = "Texture/ui.png";
std::string g_fontNameMain = "Pretendard ExtraBold";
std::string g_fontNameTimer = "Arial";


//--------------------------------------
// Button
//--------------------------------------

// ui.png
//
// Next
// Normal : 128 ~ 218
// Over   : 218 ~ 308
//
// Exit
// Normal : 312 ~ 408
// Over   : 408 ~ 504

GameButton g_btnNext
{
	{128, 0, 218, 86},
	{218, 0, 308, 86},
	{550.0f, 385.0f},
};

GameButton g_btnExit
{
	{312, 0, 408, 94},
	{408, 0, 504, 94},
	{670.0f, 385.0f},
};


//--------------------------------------
// Game Stage
//--------------------------------------

void GameStage::DrawCheckRect(int checkImage) const
{
	const auto rcImage = RECT{0, 0, 64, 64};
	// check 표시
	for(const auto& r : *chckRc)
	{
		VEC2 pLeft;
		pLeft.x = r.x + r.w * 0.5f - 32.0f;
		pLeft.y = r.y + r.h * 0.5f - 32.0f;
		VEC2 pRight = pLeft;
		pRight.x += IMAGE_WIDTH;
		g2_Draw2D(checkImage, &rcImage, &pLeft);
		g2_Draw2D(checkImage, &rcImage, &pRight);
	}
}

