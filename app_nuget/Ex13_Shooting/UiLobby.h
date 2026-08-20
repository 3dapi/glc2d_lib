#pragma once
#ifndef _UiLobby_H_
#define _UiLobby_H_

#include "UiDrawable.h"

class UiLobby : public UiDrawable
{
protected:
    float m_blend { 0.0F };
    float m_blendDir { 1.0F };

public:
    UiLobby();
    ~UiLobby() override;

    int Init() override;
    int Update(float dt) override;
    int Destroy() override;
    int Draw() override;
    int DrawFront() override;
};

#endif
