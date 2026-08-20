#include "SceneEnd.h"

#include "AppUtil.h"
#include "MainApp.h"
#include "UiEnd.h"

SceneEnd::SceneEnd()
{
}

SceneEnd::~SceneEnd()
{
    Destroy();
}

int SceneEnd::Init()
{
    delete m_pUi;
    m_pUi = new UiEnd;

    if(!m_pUi)
    {
        return E_FAIL;
    }

    return m_pUi->Init();
}

int SceneEnd::Destroy()
{
    delete m_pUi;
    m_pUi = nullptr;

    return S_OK;
}

int SceneEnd::Update(float dt)
{
    if(m_pUi)
    {
        m_pUi->Update(dt);
    }

    return S_OK;
}

int SceneEnd::Render()
{
    if(m_pUi)
    {
        m_pUi->Draw();
        m_pUi->DrawFront();
    }

    return S_OK;
}

void SceneEnd::OnMouseUp(int x, int y)
{

    if(PointInRect(x, y, 100, 400, 500, 700))
    {
        MainApp::Instance().RequestSceneChange(
            EAPP_SCENE::EAPP_SCENE_LOBBY
        );
    }
}
