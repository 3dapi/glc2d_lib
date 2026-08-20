#pragma once
#ifndef _UiPlay_H_
#define _UiPlay_H_

#include "UiDrawable.h"

class ScenePlay;

class UiPlay : public UiDrawable
{
    friend class ScenePlay;

protected:
    inline static int s_fontStatus { -1 };

    float m_blend { 0.0F };
    float m_blendDir { 1.0F };

    ScenePlay* m_pScenePlay {};

public:
    UiPlay();
    ~UiPlay() override;

    int Init() override;
    int Update(float dt) override;
    int Destroy() override;
    int Draw() override;
    int DrawFront() override;
};

#endif
