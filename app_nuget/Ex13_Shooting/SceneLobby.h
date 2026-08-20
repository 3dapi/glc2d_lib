#pragma once
#ifndef _SceneLobby_H_
#define _SceneLobby_H_

#include "Scene.h"

class UiDrawable;

class SceneLobby : public Scene
{
protected:
    UiDrawable* m_pUi {};

public:
    SceneLobby();
    ~SceneLobby() override;

    int Type() const override
    {
        return static_cast<int>(EAPP_SCENE::EAPP_SCENE_LOBBY);
    }

    int Init() override;
    int Destroy() override;
    int Update(float dt) override;
    int Render() override;

    void OnMouseUp(int x, int y) override;

protected:
    void CheckChooseShip(int x, int y);
};

#endif
