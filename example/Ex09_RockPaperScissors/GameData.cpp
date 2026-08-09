#include "GameHeader.h"

//--------------------------------------------------------------------------
// Resource File / Layout Data
//--------------------------------------------------------------------------

const char* g_texBgFile = "tex_ui/ui_bg.png";
const char* g_texScoreFile = "tex_ui/ui_score.png";
const char* g_texStartFile = "tex_ui/ui_start.png";

HandResourceData g_handResource[]
{
	// Rock
	{
		"tex_play/img_l_r.png",
		"tex_play/img_r_r.png",
		{
			"tex_ui/ui_rock.png",
			"tex_ui/ui_rock_o.png",
			{20, 330, 128, 128}
		}
	},

	// Scissor
	{
		"tex_play/img_l_s.png",
		"tex_play/img_r_s.png",
		{
			"tex_ui/ui_scissor.png",
			"tex_ui/ui_scissor_o.png",
			{150, 330, 128, 128}
		}
	},

	// Paper
	{
		"tex_play/img_l_p.png",
		"tex_play/img_r_p.png",
		{
			"tex_ui/ui_paper.png",
			"tex_ui/ui_paper_o.png",
			{280, 330, 128, 128}
		}
	},
};

GameButtonData g_btnContinueData
{
	"tex_ui/ui_continue.png",
	"tex_ui/ui_continue_o.png",
	{455, 365, 128, 64}
};

GameButtonData g_btnEndData
{
	"tex_ui/ui_end.png",
	"tex_ui/ui_end_o.png",
	{565, 400, 64, 64}
};

GL_RECT g_rcStart = {256, 200, 128, 64};

const char* g_fontNameScore = "Arial";
const char* g_fontNameMsg = "Arial";

int g_fontSizeScore = 40;
int g_fontSizeMsg = 24;

//--------------------------------------------------------------------------
// Runtime Data
//--------------------------------------------------------------------------

int g_screenW = 640;
int g_screenH = 480;

EGameState g_gameState = GAME_READY;

int g_userWin = 0;
int g_comWin = 0;

EHand g_handUser = HAND_ROCK;
EHand g_handCom = HAND_ROCK;
bool g_hasResult = false;

int g_texBg = -1;
int g_texScore = -1;
int g_texStart = -1;

HandData g_hand[HAND_MAX];

GameButton g_btnContinue;
GameButton g_btnEnd;

int g_fontScore = -1;
int g_fontMsg = -1;

std::mt19937 g_random{std::random_device{}()};

//--------------------------------------------------------------------------
// Load
//--------------------------------------------------------------------------

void LoadButton(GameButton& button, const GameButtonData& data)
{
	button.texNormal = g2_TextureLoad(data.texNormalFile);
	button.texOver = g2_TextureLoad(data.texOverFile);
	button.rc = data.rc;
}

int GameLoadData(void)
{
	g_texBg = g2_TextureLoad(g_texBgFile);
	g_texScore = g2_TextureLoad(g_texScoreFile);
	g_texStart = g2_TextureLoad(g_texStartFile);

	for(int i = 0; i < HAND_MAX; ++i)
	{
		const auto& src = g_handResource[i];
		auto& dst = g_hand[i];

		dst.texLeft = g2_TextureLoad(src.texLeftFile);
		dst.texRight = g2_TextureLoad(src.texRightFile);

		LoadButton(dst.button, src.button);
	}

	LoadButton(g_btnContinue, g_btnContinueData);
	LoadButton(g_btnEnd, g_btnEndData);

	g_fontScore = g2_FontCreate(g_fontNameScore, g_fontSizeScore);
	g_fontMsg = g2_FontCreate(g_fontNameMsg, g_fontSizeMsg);

	return 0;
}

EHand GetRandomHand(void)
{
	static std::uniform_int_distribution<int> dist(0, HAND_MAX - 1);
	return static_cast<EHand>(dist(g_random));
}
