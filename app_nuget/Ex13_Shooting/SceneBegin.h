#pragma once
#ifndef _SceneBegin_H_
#define _SceneBegin_H_

#include "Scene.h"

class UiDrawable;

class SceneBegin : public Scene
{
protected:
    UiDrawable* m_pUi {};

public:
    SceneBegin();
    ~SceneBegin() override;

    int Type() const override { return static_cast<int>(EAPP_SCENE::EAPP_SCENE_BEGIN); }
    int Init() override;
    int Destroy() override;
    int Update(float dt) override;
    int Render() override;

    void OnMouseUp(int x, int y) override;
};

#endif
