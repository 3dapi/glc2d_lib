#pragma once
#ifndef _ScenePlay_H_
#define _ScenePlay_H_

#include "GameInfo.h"
#include "Scene.h"

class UiDrawable;
class UiPlay;

struct TextureResource
{
    int id { -1 };
    int width {};
    int height {};
};

class ScenePlay : public Scene
{
    friend class UiPlay;

public:
    enum class PLAY_STATE : int
    {
        BEGIN = 0,
        LOW,
        MIDDLE,
        HI,
        BOSS,
        END,
    };

protected:
    inline static const int MAX_SND_EFFECT     { 15 };
    inline static const int MAX_DRONE          { 1000 };
    inline static const int MAX_BULLET_PLAYER  { 1000 };
    inline static const int MAX_BULLET_ENEMY   { 1000 };
    inline static const int MAX_MISSLE_ENEMY   { 2000 };

    map<string, TextureResource> m_texture;

    GamePlayer*             m_mainPlayer {};
    vector<EnemyDrone*>     m_vecDrone;
    EnemyBoss*              m_droneBoss {};

    vector<GameBullet*>     m_vecBulletEnemy;
    vector<GameBullet*>     m_vecBulletPlayer;
    vector<GameMissile*>    m_vecMissileEnemy;

    vector<int>             m_sndLaser;
    vector<int>             m_sndBoom;

    UiDrawable*             m_pUi {};
    UiDrawable*             m_pUiBg {};

    PLAY_STATE              m_playState {};
    float                   m_timeStored {};
    float                   m_timeDrone {};

public:
    ScenePlay();
    ~ScenePlay() override;

    int Type() const override
    {
        return static_cast<int>(EAPP_SCENE::EAPP_SCENE_PLAY);
    }

    int Init() override;
    int Destroy() override;
    int Update(float dt) override;
    int Render() override;

    void OnMouseUp(int x, int y) override;

    void SetStateStart();
    void SetStateEnd();

    int  UpdateEnemy(float dt);

    void BulletFire(GameObject* obj, bool isPlayer);
    void BulletUpdate(float dt);

    void MissileFire(GameObject* obj);
    void MissileUpdate(float dt);

    void DroneBossSetup();
    void DroneBossUpdate(float dt);

    void BulletSoundPlay();
    void BoomSoundPlay();

protected:
    bool LoadTexture(const string& name, const string& file);
    void ReleaseTextures();

    void DrawObject(const string& modelName, const VEC2& gamePosition, float scale = 1.0F, float angle = 0.0F);
};

#endif
