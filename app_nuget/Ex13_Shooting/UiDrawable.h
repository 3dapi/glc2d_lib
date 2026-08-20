#pragma once
#ifndef _UiDrawable_H_
#define _UiDrawable_H_

#include "AppCommon.h"

struct UI_TEXTURE
{
    int id { -1 };
    int width {};
    int height {};
};

class UiDrawable
{
public:
    virtual ~UiDrawable() = default;

    virtual int Init() = 0;
    virtual int Update(float deltaTime) = 0;
    virtual int Destroy() = 0;
    virtual int Draw() = 0;
    virtual int DrawFront() = 0;

protected:
    bool LoadTexture(
        const string& name,
        const string& file
    );

    void ReleaseTextures();
    int DrawTexture(const string& name, const VEC2& pos, const VEC2& scale={1.0F, 1.0F}, DWORD color = 0xFFFFFFFF, float angle = 0.0F) const;
    static DWORD MakeColor(DWORD rgb, float alpha);

protected:
    map<string, UI_TEXTURE> m_uiTex;
};

#endif
