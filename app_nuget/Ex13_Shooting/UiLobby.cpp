#include "UiLobby.h"

UiLobby::UiLobby()
{
}

UiLobby::~UiLobby()
{
    Destroy();
}

int UiLobby::Destroy()
{
    ReleaseTextures();

    return S_OK;
}

int UiLobby::Init()
{
    Destroy();

    const vector<std::tuple<string, string>> textureList
    {
        {
            "ui/ui_choose_ship",
            ASSET_FOLDER "ui/ui_choose_ship.png"
        },
        {
            "ui/ui_box",
            ASSET_FOLDER "ui/ui_box.png"
        },
        {
            "ui/ui_game_start",
            ASSET_FOLDER "ui/ui_game_start.png"
        },
        {
            EMODEL_SHIP[2],
            ASSET_FOLDER "sprite/" + EMODEL_SHIP[2] + ".png"
        },
        {
            EMODEL_SHIP[3],
            ASSET_FOLDER "sprite/" + EMODEL_SHIP[3] + ".png"
        },
    };

    for(const auto& [name, file] : textureList)
    {
        if(!LoadTexture(name, file))
        {
            return E_FAIL;
        }
    }

    m_blend = 0.0F;
    m_blendDir = 1.0F;

    return S_OK;
}

int UiLobby::Update(float dt)
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

int UiLobby::Draw()
{
    {
        auto found = m_uiTex.find("ui/ui_choose_ship");
        if(found != m_uiTex.end())
        {
            float x = g2_GetScnW() * 0.5F - found->second.width * 0.5F;
            DrawTexture("ui/ui_choose_ship", {x, 120.0F}, {1.0F, 1.0F}, 0xFFFFFF99);
        }
    }

    DrawTexture(EMODEL_SHIP[2], { 75.0F, 350.0F}, {0.6F, 0.6F}, MakeColor(0x00FFFFFF, m_blend) );
    DrawTexture(EMODEL_SHIP[3], {360.0F, 350.0F}, {0.6F, 0.6F}, MakeColor(0x00FFFFFF,    0.4F) );
    DrawTexture("ui/ui_box",    { 50.0F, 300.0F}, {0.7F, 0.7F}, MakeColor(0x00FF00FF, m_blend) );
    DrawTexture("ui/ui_box",    {330.0F, 300.0F}, {0.7F, 0.7F}, MakeColor(0x00004CFF,    0.4F) );
    return S_OK;
}

int UiLobby::DrawFront()
{
    auto found = m_uiTex.find("ui/ui_game_start");
    if(found == m_uiTex.end())
    {
        return E_FAIL;
    }
    float x = g2_GetScnW() * 0.5F - found->second.width * 0.5F;
    DrawTexture("ui/ui_game_start", {x, 600.0F}, {1.0F, 1.0F}, MakeColor(0x00FFFFFF, m_blend) );
    return S_OK;
}
