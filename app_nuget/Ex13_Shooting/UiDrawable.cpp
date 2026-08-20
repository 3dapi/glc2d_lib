#include "UiDrawable.h"

bool UiDrawable::LoadTexture(
    const string& name,
    const string& file)
{
    int id = g2_TextureLoad(file.c_str());

    if(id < 0)
    {
        return false;
    }

    UI_TEXTURE texture;
    texture.id = id;
    texture.width = g2_TextureWidth(id);
    texture.height = g2_TextureHeight(id);

    m_uiTex[name] = texture;

    return true;
}

void UiDrawable::ReleaseTextures()
{
    for(auto& [name, texture] : m_uiTex)
    {
        if(texture.id >= 0)
        {
            g2_TextureRelease(texture.id);
            texture.id = -1;
        }
    }

    m_uiTex.clear();
}

int UiDrawable::DrawTexture(const string& name, const VEC2& pos, const VEC2& scale, DWORD color, float angle) const
{
    auto found = m_uiTex.find(name);

    if(found == m_uiTex.end())
    {
        return E_FAIL;
    }

    const UI_TEXTURE& texture = found->second;

    if(texture.id < 0)
    {
        return E_FAIL;
    }

    if(angle == 0.0F)
    {
        return g2_Draw2D(texture.id, {}, &pos, &scale, nullptr, 0.0F, color);
    }

    VEC2 rotationCenter
    {
        texture.width * 0.5F,
        texture.height * 0.5F
    };
    return g2_Draw2D(texture.id, {}, &pos, &scale, &rotationCenter, angle, color);
}

DWORD UiDrawable::MakeColor(DWORD rgb, float alpha)
{
    alpha = std::clamp(alpha, 0.0F, 1.0F );
    auto a = static_cast<DWORD>(alpha * 255.0F + 0.5F);
    return (a << 24) | (rgb & 0x00FFFFFF);
}
