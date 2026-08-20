#include "UiBackground.h"

UiBackground::UiBackground()
{
}

UiBackground::~UiBackground()
{
    Destroy();
}

int UiBackground::Destroy()
{
    ReleaseTextures();

    return S_OK;
}

int UiBackground::Init()
{
    Destroy();

    m_scrollBegin = -960.0F;
    m_scrollY = m_scrollBegin;
    m_scrollSpeed = 200.0F;

    if(!LoadTexture(EMODEL_BG[0], ASSET_FOLDER "sprite/" + EMODEL_BG[0] + ".png"))
    {
        return E_FAIL;
    }

    return S_OK;
}

int UiBackground::Update(float dt)
{
    m_scrollY +=
        dt *
        m_scrollSpeed;

    if(0.0F < m_scrollY)
    {
        m_scrollY +=
            m_scrollBegin;
    }

    return S_OK;
}

int UiBackground::Draw()
{
    auto found =
        m_uiTex.find(EMODEL_BG[0]);

    if(found == m_uiTex.end())
    {
        return E_FAIL;
    }

    const UI_TEXTURE& texture = found->second;

    float x =
        g2_GetScnW() * 0.5F -
        texture.width * 0.5F;

    DrawTexture(EMODEL_BG[0],  {x, m_scrollY});

    return S_OK;
}

int UiBackground::DrawFront()
{
    return S_OK;
}
