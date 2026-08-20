#pragma once
#ifndef _SceneEnd_H_
#define _SceneEnd_H_

#include "Scene.h"

class UiDrawable;

class SceneEnd : public Scene
{
protected:
    UiDrawable* m_pUi {};

public:
    SceneEnd();
    ~SceneEnd() override;

    int Type() const override { return static_cast<int>(EAPP_SCENE::EAPP_SCENE_END); }
    int Init() override;
    int Destroy() override;
    int Update(float dt) override;
    int Render() override;

    void OnMouseUp(int x, int y) override;
};

#endif
