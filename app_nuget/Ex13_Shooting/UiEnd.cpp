#include "UiEnd.h"

#include "GameInfo.h"

UiEnd::UiEnd()
{
}

UiEnd::~UiEnd()
{
    Destroy();
}

int UiEnd::Destroy()
{
    ReleaseTextures();

    return S_OK;
}

int UiEnd::Init()
{
    Destroy();

    if(!LoadTexture("ui/ui_touch_the_screen", ASSET_FOLDER "ui/ui_touch_the_screen.png"))
    {
        return E_FAIL;
    }

    if(s_fontScore < 0)
    {
        // 원본 32px Font Texture를 1.5배로 출력했으므로
        // glc2d에서는 48px Font로 직접 출력한다.
        s_fontScore =
            
            g2_FontCreate(
                "netmarble Bold",
                48
            );

        if(s_fontScore < 0)
        {
            return E_FAIL;
        }
    }

    m_blend = 0.0F;
    m_blendDir = 1.0F;

    return S_OK;
}

int UiEnd::Update(float dt)
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

int UiEnd::Draw()
{
    const GameInfo* gameInfo = GameInfo::instance();
    if(s_fontScore >= 0)
    {
        g2_FontDrawText(s_fontScore, {70, 300}, 0xFFFFFF00, "Score: %d", gameInfo->m_gameScore);
    }

    return S_OK;
}

int UiEnd::DrawFront()
{
    auto found = m_uiTex.find("ui/ui_touch_the_screen");
    if(found == m_uiTex.end())
    {
        return E_FAIL;
    }

    constexpr float scale = 0.6F;
    float x = g2_GetScnW() * 0.5F - found->second.width * scale * 0.5F;
    DrawTexture("ui/ui_touch_the_screen", {x, 520.0F}, {scale, scale}, MakeColor(0x00FFFFFF, m_blend) );

    return S_OK;
}
