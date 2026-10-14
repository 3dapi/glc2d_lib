#include "UiPlay.h"

#include "GameInfo.h"
#include "ScenePlay.h"

UiPlay::UiPlay()
{
}

UiPlay::~UiPlay()
{
    Destroy();
}

int UiPlay::Destroy()
{
    ReleaseTextures();

    m_pScenePlay = nullptr;

    return S_OK;
}

int UiPlay::Init()
{
    Destroy();

    const vector<std::tuple<string, string>> textureList
    {
        {
            "ui/ui_rect",
            ASSET_FOLDER "ui/ui_rect.png"
        },
        {
            "ui/ui_gameover",
            ASSET_FOLDER "ui/ui_gameover.png"
        },
        {
            "ui/ui_touch_the_screen",
            ASSET_FOLDER "ui/ui_touch_the_screen.png"
        },
        {
            "ui/ui_stage_cleared",
            ASSET_FOLDER "ui/ui_stage_cleared.png"
        },
    };

    for(const auto& [name, file] : textureList)
    {
        if(!LoadTexture(name, file))
        {
            return E_FAIL;
        }
    }

    if(s_fontStatus < 0)
    {
        // 원본 UI에서 실제 사용한 글꼴 크기는 20.
        s_fontStatus = g2_FontCreate("netmarble Bold", 32);
        if(s_fontStatus < 0)
        {
            return E_FAIL;
        }
    }

    m_blend = 0.0F;
    m_blendDir = 1.0F;

    return S_OK;
}

int UiPlay::Update(float dt)
{
    m_blend += dt * m_blendDir;

    if(1.0F < m_blend)
    {
        m_blendDir = -1.0F;
        m_blend = 1.0F;
    }
    else if(m_blend < 0.0F)
    {
        m_blendDir = 1.0F;
        m_blend = 0.0F;
    }

    return S_OK;
}

int UiPlay::Draw()
{
    GameInfo* gameInfo = GameInfo::instance();
    if(s_fontStatus >= 0)
    {
        g2_FontDrawText(s_fontStatus, { 5,  5}, 0xFFFFFF00, "Score: %d", gameInfo->m_gameScore);
        g2_FontDrawText(s_fontStatus, { 5, 35}, 0xFFFF0000, "HP: %d", static_cast<int>(gameInfo->MainPlayer()->HP() ));
    }

    {
        float hp = gameInfo-> MainPlayer()-> HP();
        DrawTexture("ui/ui_rect", {150.0F, 38.0F}, {(hp + 0.4F) * 0.7F /100.0F, 0.25F}, 0xFFFF0000);
    }

    if(m_pScenePlay &&
       m_pScenePlay->m_droneBoss &&
       m_pScenePlay->m_playState == ScenePlay::PLAY_STATE::BOSS)
    {
        float hp = m_pScenePlay-> m_droneBoss-> HP();
        if(s_fontStatus >= 0)
        {
            g2_FontDrawText(s_fontStatus, {5, 70}, 0xFFB300B3, "BOSS: %d", static_cast<int>(hp) );
        }

        DrawTexture("ui/ui_rect", {150.0F, 73.0F}, { (hp + 0.4F) * 0.7F /100.0F, 0.25F}, 0xFFB300B3);
    }

    if(!gameInfo->m_enablePlay)
    {
        auto found = m_uiTex.find("ui/ui_gameover");
        if(found != m_uiTex.end())
        {
            float x = g2_GetScnW() * 0.5F - found->second.width * 0.5F;
            DrawTexture("ui/ui_gameover", {x, 200.0F}, {1.0F, 1.0F}, 0xFFFF00FF);
        }
    }

    return S_OK;
}

int UiPlay::DrawFront()
{
    GameInfo* gameInfo = GameInfo::instance();
    if(!gameInfo->m_enablePlay && gameInfo-> MainPlayer()-> Alive())
    {
        DrawTexture("ui/ui_stage_cleared", {0.0F, 0.0F}, {1.0F, 1.0F}, MakeColor(0x00FFFFFF, m_blend) );
    }

    return S_OK;
}
