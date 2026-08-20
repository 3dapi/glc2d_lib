#pragma once
#ifndef _SceneNone_H_
#define _SceneNone_H_

#include "Scene.h"

class SceneNone : public Scene
{
public:
    SceneNone();
    ~SceneNone() override;

    int Type() const override
    {
        return static_cast<int>(EAPP_SCENE::EAPP_SCENE_NONE);
    }

    int Init() override;
    int Destroy() override;
    int Update(float dt) override;
    int Render() override;
};

#endif
