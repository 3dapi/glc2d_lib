#include "MainApp.h"

#include "GameInfo.h"
#include "SceneBegin.h"
#include "SceneEnd.h"
#include "SceneLobby.h"
#include "ScenePlay.h"

MainApp& MainApp::Instance()
{
    static MainApp instance;
    return instance;
}

int MainApp::Init()
{
    GameInfo::instance();

    m_prevTime = g2_TimeGetTime();

    m_bgm = g2_SoundLoad(ASSET_FOLDER "sound/fight-for-the-future-336841.mp3");
    g2_SoundPlay(m_bgm, true);

    ChangeScene(EAPP_SCENE::EAPP_SCENE_BEGIN);

    return S_OK;
}

int MainApp::Destroy()
{
    for(auto& scene : m_scene)
    {
        if(scene)
        {
            scene->Destroy();
            scene.reset();
        }
    }

    GameInfo::deleteInstance();

    m_prevTime = {};
    m_changeScene = false;

    return S_OK;
}

float MainApp::UpdateDeltaTime()
{
    long long currentTime = g2_TimeGetTime();

    if(m_prevTime == 0)
    {
        m_prevTime = currentTime;
        return 0.0F;
    }
    float dt = static_cast<float>(currentTime - m_prevTime) / 1000.0F;
    m_prevTime = currentTime;

    return dt;
}

int MainApp::Update()
{
    float dt = UpdateDeltaTime();

    const KEYCODE* key = g2_GetKeyboard();

    if(key && key[VK_F2] == EINPUT_UP)
    {
        GameInfo::IsCheat()? GameInfo::SetCheat(false): GameInfo::SetCheat(true);
    }

    if(g2_GetMouseEvent(0) == EINPUT_UP)
    {
        size_t index = static_cast<size_t>(m_sceneIdxCur);

        if(index < m_scene.size() && m_scene[index])
        {
            m_scene[index]->OnMouseUp(
                g2_GetMouseX(),
                g2_GetMouseY()
            );
        }
    }

    if(m_changeScene)
    {
        ChangeScene(m_sceneIdxNew);
    }

    size_t index = static_cast<size_t>(m_sceneIdxCur);

    if(index < m_scene.size() && m_scene[index])
    {
        return m_scene[index]->Update(dt);
    }

    return S_OK;
}

int MainApp::Render()
{
    size_t index = static_cast<size_t>(m_sceneIdxCur);

    if(index < m_scene.size() && m_scene[index])
    {
        return m_scene[index]->Render();
    }

    return S_OK;
}

void MainApp::RequestSceneChange(EAPP_SCENE target)
{
    if(target < EAPP_SCENE::EAPP_SCENE_BEGIN || target >= EAPP_SCENE::EAPP_SCENE_COUNT)
    {
        return;
    }

    if(m_sceneIdxCur == target)
    {
        return;
    }

    m_sceneIdxNew = target;
    m_changeScene = true;
}

void MainApp::ChangeScene(EAPP_SCENE target)
{
    if(target < EAPP_SCENE::EAPP_SCENE_BEGIN || target >= EAPP_SCENE::EAPP_SCENE_COUNT)
    {
        return;
    }

    size_t currentIndex = static_cast<size_t>(m_sceneIdxCur);

    if(currentIndex < m_scene.size() && m_scene[currentIndex])
    {
        m_scene[currentIndex]->Destroy();
    }

    m_sceneIdxCur = target;
    m_sceneIdxNew = target;
    m_changeScene = false;

    size_t index = static_cast<size_t>(m_sceneIdxCur);

    if(!m_scene[index])
    {
        switch(target)
        {
        case EAPP_SCENE::EAPP_SCENE_BEGIN:
            m_scene[index] = std::make_unique<SceneBegin>();
            break;

        case EAPP_SCENE::EAPP_SCENE_LOBBY:
            m_scene[index] = std::make_unique<SceneLobby>();
            break;

        case EAPP_SCENE::EAPP_SCENE_PLAY:
            m_scene[index] = std::make_unique<ScenePlay>();
            break;

        case EAPP_SCENE::EAPP_SCENE_END:
            m_scene[index] = std::make_unique<SceneEnd>();
            break;

        default:
            return;
        }
    }

    m_scene[index]->Init();
}
