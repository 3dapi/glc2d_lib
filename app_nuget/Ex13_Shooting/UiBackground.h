#pragma once
#ifndef _UiBackground_H_
#define _UiBackground_H_

#include "UiDrawable.h"

class UiBackground : public UiDrawable
{
protected:
    float m_scrollBegin {};
    float m_scrollY {};
    float m_scrollSpeed {};

public:
    UiBackground();
    ~UiBackground() override;

    int Init() override;
    int Update(float dt) override;
    int Destroy() override;
    int Draw() override;
    int DrawFront() override;
};

#endif
