#include "glc2d.h"
#include "GameHeader.h"
#include <vector>

//--------------------------------------
// Global
//--------------------------------------
int g_GamePhase = GAME_INIT;

int m_screenW = 800;
int m_screenH = 480;

// texture
int texLeft = -1;
int texRight = -1;
int texCheck = -1;

// font
int fontMain = -1;

// rect (틀린 위치)
struct CheckRect
{
    RECT rc;
};

std::vector<CheckRect> g_rects;
std::vector<int> g_checked;

// time
DWORD g_timeStart = 0;
int g_timeRemain = 200;

//--------------------------------------
// Utils
//--------------------------------------
bool IsInRect(int x, int y, RECT& r)
{
    return (x > r.left && x < r.right &&
            y > r.top  && y < r.bottom);
}

void AddCheck(int idx)
{
    for(int v : g_checked)
    {
        if(v == idx) return;
    }
    g_checked.push_back(idx);
}

//--------------------------------------
// Game
//--------------------------------------
int GameGetScreenWidth(void){ return m_screenW; }
int GameGetScreenHeight(void){ return m_screenH; }

int GameInit(void)
{
    texLeft  = glc2d_TextureLoad("Texture/smurf11.png");
    texRight = glc2d_TextureLoad("Texture/smurf12.png");
    texCheck = glc2d_TextureLoad("Texture/ui.png");

    fontMain = glc2d_FontCreate("Arial", 24, 0);

    // 틀린 위치 수동 등록 (샘플)
    g_rects.clear();

    CheckRect r1 = { {50, 50, 120, 120} };
    CheckRect r2 = { {200, 100, 260, 160} };
    CheckRect r3 = { {300, 200, 360, 260} };

    g_rects.push_back(r1);
    g_rects.push_back(r2);
    g_rects.push_back(r3);

    g_checked.clear();

    g_timeStart = glc2d_TimeGetTime();
    g_timeRemain = 200;

    g_GamePhase = GAME_PLAY;
    return 0;
}

int GameDestroy(void)
{
    return 0;
}

//--------------------------------------
// Update
//--------------------------------------
int GameFrameMove(void)
{
    if(g_GamePhase == GAME_PLAY)
    {
        int mx = glc2d_GetMouseX();
        int my = glc2d_GetMouseY();

        int click = glc2d_GetMouseEvent(0);

        if(click)
        {
            for(int i=0; i<g_rects.size(); ++i)
            {
                if(IsInRect(mx, my, g_rects[i].rc))
                {
                    AddCheck(i);
                }
            }
        }

        // time
        DWORD cur = glc2d_TimeGetTime();
        if(cur - g_timeStart > 1000)
        {
            g_timeStart = cur;
            g_timeRemain--;

            if(g_timeRemain <= 0)
            {
                g_GamePhase = GAME_END;
            }
        }

        if(g_checked.size() == g_rects.size())
        {
            g_GamePhase = GAME_END;
        }
    }

    return 0;
}

//--------------------------------------
// Render
//--------------------------------------
int GameRender(void)
{
    // left
    glc2d_Draw2D(texLeft, nullptr);

    // right
    VEC2 pos = {400, 0};
    glc2d_Draw2D(texRight, nullptr, &pos);

    // check 표시
    for(int idx : g_checked)
    {
        RECT r = g_rects[idx].rc;

        VEC2 p;
        p.x = (r.left + r.right) * 0.5f - 16 + 400;
        p.y = (r.top  + r.bottom)* 0.5f - 16;

        glc2d_Draw2D(texCheck, &r, &p);
    }

    // UI
    char buf[64];
    sprintf(buf, "Time: %d", g_timeRemain);
    glc2d_FontDrawText(fontMain, 10, 440, 300, 470, 0xFFFFFFFF, buf);

    if(g_GamePhase == GAME_END)
    {
        glc2d_FontDrawText(fontMain, 250, 200, 600, 300, 0xFFFF0000, "Game Over");
    }

    return 0;
}
