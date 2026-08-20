#pragma once
#ifndef _MainApp_H_
#define _MainApp_H_

#include "AppCommon.h"
#include "Scene.h"

class MainApp
{
public:
    static MainApp& Instance();

public:
    int Init();
    int Destroy();
    int Update();
    int Render();

    void RequestSceneChange(EAPP_SCENE target);

private:
    MainApp() = default;
    ~MainApp() = default;

    MainApp(const MainApp&) = delete;
    MainApp& operator=(const MainApp&) = delete;

    void ChangeScene(EAPP_SCENE target);
    float UpdateDeltaTime();

private:
    EAPP_SCENE m_sceneIdxCur { EAPP_SCENE::EAPP_SCENE_BEGIN };
    EAPP_SCENE m_sceneIdxNew { EAPP_SCENE::EAPP_SCENE_BEGIN };

    vector<unique_ptr<Scene>> m_scene
    {
        static_cast<size_t>(EAPP_SCENE::EAPP_SCENE_COUNT)
    };

    bool        m_changeScene {};
    long long   m_prevTime {};
    int         m_bgm{-1};
};

#endif
