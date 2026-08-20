#include "SceneNone.h"

SceneNone::SceneNone()
{
}

SceneNone::~SceneNone()
{
    Destroy();
}

int SceneNone::Init()
{
    return S_OK;
}

int SceneNone::Destroy()
{
    return S_OK;
}

int SceneNone::Update(float dt)
{
    return S_OK;
}

int SceneNone::Render()
{
    return S_OK;
}
