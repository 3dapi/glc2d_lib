#pragma once
#ifndef _UiBegin_H_
#define _UiBegin_H_

#include "UiDrawable.h"

class UiBegin : public UiDrawable
{
protected:
    float m_blend { 0.0F };
    float m_blendDir { 1.0F };

public:
    UiBegin();
    ~UiBegin() override;

    int Init() override;
    int Update(float dt) override;
    int Destroy() override;
    int Draw() override;
    int DrawFront() override;
};

#endif
