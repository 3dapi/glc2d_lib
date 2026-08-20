#pragma once
#ifndef _UiEnd_H_
#define _UiEnd_H_

#include "UiDrawable.h"

class UiEnd : public UiDrawable
{
protected:
    inline static int s_fontScore { -1 };

    float m_blend { 0.0F };
    float m_blendDir { 1.0F };

public:
    UiEnd();
    ~UiEnd() override;

    int Init() override;
    int Update(float dt) override;
    int Destroy() override;
    int Draw() override;
    int DrawFront() override;
};

#endif
