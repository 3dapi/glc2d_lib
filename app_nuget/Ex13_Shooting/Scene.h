#pragma once
#ifndef _Scene_H_
#define _Scene_H_

#include "AppCommon.h"

class Scene
{
public:
    virtual ~Scene() = default;

    virtual int Type() const = 0;
    virtual int Init() = 0;
    virtual int Destroy() = 0;
    virtual int Update(float dt) = 0;
    virtual int Render() = 0;

    virtual void OnMouseUp(int x, int y)
    {
    }
};

#endif
