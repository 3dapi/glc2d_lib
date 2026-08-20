#include "SceneBegin.h"

#include "AppUtil.h"
#include "MainApp.h"
#include "UiBegin.h"

SceneBegin::SceneBegin()
{
}

SceneBegin::~SceneBegin()
{
    Destroy();
}

int SceneBegin::Init()
{
    delete m_pUi;
    m_pUi = new UiBegin;

    if(!m_pUi)
    {
        return E_FAIL;
    }

    return m_pUi->Init();
}

int SceneBegin::Destroy()
{
    delete m_pUi;
    m_pUi = nullptr;

    return S_OK;
}

int SceneBegin::Update(float dt)
{
    if(m_pUi)
    {
        m_pUi->Update(dt);
    }

    return S_OK;
}

int SceneBegin::Render()
{
    if(m_pUi)
    {
        // 원본 SceneBegin의 호출 순서를 그대로 유지한다.
        m_pUi->Draw();
        m_pUi->Draw();
        m_pUi->DrawFront();
    }

    return S_OK;
}

void SceneBegin::OnMouseUp(int x, int y)
{

    if(PointInRect(x, y, 120, 440, 480, 650))
    {
        MainApp::Instance().RequestSceneChange(
            EAPP_SCENE::EAPP_SCENE_LOBBY
        );
    }
}
