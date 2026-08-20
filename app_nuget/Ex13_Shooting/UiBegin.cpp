#include "UiBegin.h"

UiBegin::UiBegin()
{
}

UiBegin::~UiBegin()
{
    Destroy();
}

int UiBegin::Destroy()
{
    ReleaseTextures();

    return S_OK;
}

int UiBegin::Init()
{
    Destroy();

    if(!LoadTexture("ui/ui_big_title", ASSET_FOLDER "ui/ui_big_title.png"))
    {
        return E_FAIL;
    }

    if(!LoadTexture("ui/ui_touch_the_screen", ASSET_FOLDER "ui/ui_touch_the_screen.png"))
    {
        return E_FAIL;
    }

    m_blend = 0.0F;
    m_blendDir = 1.0F;

    return S_OK;
}

int UiBegin::Update(float dt)
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

int UiBegin::Draw()
{
    auto found =
        m_uiTex.find("ui/ui_big_title");

    if(found == m_uiTex.end())
    {
        return E_FAIL;
    }

    const UI_TEXTURE& texture = found->second;
    float x = g2_GetScnW() * 0.5F - texture.width * 0.5F;
    DrawTexture("ui/ui_big_title", {x, 100.0F}, {1.0F,1.0F}, MakeColor(0x00FFFF00, m_blend) );
    return S_OK;
}

int UiBegin::DrawFront()
{
    auto found = m_uiTex.find("ui/ui_touch_the_screen");
    if(found == m_uiTex.end())
    {
        return E_FAIL;
    }

    const UI_TEXTURE& texture = found->second;

    constexpr float scale = 0.6F;
    float x = g2_GetScnW() * 0.5F - texture.width * scale * 0.5F;
    DrawTexture("ui/ui_touch_the_screen", {x, 620.0F}, {scale, scale}, MakeColor(0x00FFFFFF, m_blend) );

    return S_OK;
}
