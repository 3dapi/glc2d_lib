#include "SceneLobby.h"

#include "AppUtil.h"
#include "GameInfo.h"
#include "MainApp.h"
#include "UiLobby.h"

SceneLobby::SceneLobby()
{
}

SceneLobby::~SceneLobby()
{
    Destroy();
}

int SceneLobby::Init()
{
    delete m_pUi;
    m_pUi = new UiLobby;

    if(!m_pUi)
    {
        return E_FAIL;
    }

    return m_pUi->Init();
}

int SceneLobby::Destroy()
{
    delete m_pUi;
    m_pUi = nullptr;

    return S_OK;
}

int SceneLobby::Update(float dt)
{
    if(m_pUi)
    {
        m_pUi->Update(dt);
    }

    return S_OK;
}

int SceneLobby::Render()
{
    if(m_pUi)
    {
        m_pUi->Draw();
        m_pUi->DrawFront();
    }

    return S_OK;
}

void SceneLobby::OnMouseUp(int x, int y)
{
    CheckChooseShip(x, y);
}

void SceneLobby::CheckChooseShip(int x, int y)
{
    if(PointInRect(x, y, 60, 310, 250, 500))
    {
        GameInfo::instance()->MainPlayer()->Model(EMODEL_SHIP[0]);
        return;
    }

    if(PointInRect(x, y, 100, 590, 500, 670))
    {
        MainApp::Instance().RequestSceneChange(
            EAPP_SCENE::EAPP_SCENE_PLAY
        );
    }
    else
    {
        GameInfo::instance()->MainPlayer()->Model("");
    }
}
