#include <glc2d.h>

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

#include "MainApp.h"

namespace
{
int GameFrameMove()
{
    return MainApp::Instance().Update();
}

int GameRender()
{
    return MainApp::Instance().Render();
}
}

int main()
{
    g2_InitSdk();

    g2_SetClearColor(0xFF000000);
    g2_SetCursorShow(1);
    g2_SetStateShow(0);

    g2_SetFrameMove(GameFrameMove);
    g2_SetRender(GameRender);

    if(FAILED(
        g2_CreateWin(
            0,
            0,
            600,
            960,
            "glc2d Shooting",
            true
        )
    ))
    {
        return 0;
    }

    // 2D Sprite의 PNG alpha와 색상 alpha를 사용한다.
    //g2_DrawAlphaOption(1);

    MainApp::Instance().Init();

    g2_Run();

    MainApp::Instance().Destroy();
    g2_DestroyWin();

    return 0;
}
