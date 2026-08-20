#include "ScenePlay.h"

#include <algorithm>
#include <new>
#include <random>

#include "AppUtil.h"
#include "MainApp.h"
#include "UiBackground.h"
#include "UiPlay.h"

namespace
{
template<typename T>
void DeleteObjectVector(vector<T*>& objects)
{
    for(auto* object : objects)
    {
        delete object;
    }
    objects.clear();
}

float RandomRange(float minValue, float maxValue)
{
    static std::mt19937 generator{ std::random_device{}() };
    std::uniform_real_distribution<float> distribution(minValue, maxValue);
    return distribution(generator);
}

int RandomRange(int minValue, int maxValue)
{
    static std::mt19937 generator{ std::random_device{}() };
    std::uniform_int_distribution<int> distribution(minValue, maxValue);
    return distribution(generator);
}
}

ScenePlay::ScenePlay()
{
}

ScenePlay::~ScenePlay()
{
    Destroy();
}

bool ScenePlay::LoadTexture(const string& name, const string& file)
{
    int id = g2_TextureLoad(file.c_str());
    if(id < 0)
    {
        return false;
    }
    TextureResource resource;
    resource.id = id;
    resource.width = g2_TextureWidth(id);
    resource.height = g2_TextureHeight(id);
    m_texture[name] = resource;
    return true;
}

void ScenePlay::ReleaseTextures()
{
    for(auto& [name, texture] : m_texture)
    {
        if(texture.id >= 0)
        {
            g2_TextureRelease(texture.id);
            texture.id = -1;
        }
    }
    m_texture.clear();
}

int ScenePlay::Init()
{
    Destroy();
    vector<std::tuple<string, string>> textureList
    {
        {
            EMODEL_SHIP[0],
            ASSET_FOLDER "sprite/" + EMODEL_SHIP[0] + ".png"
        },
        {
            EMODEL_SHIP[1],
            ASSET_FOLDER "sprite/" + EMODEL_SHIP[1] + ".png"
        },
    };

    for(const auto& model : EMODEL_DRONE)
    {
        textureList.push_back(
            {
                model,
                ASSET_FOLDER "sprite/" + model + ".png"
            }
        );
    }

    for(const auto& model : EMODEL_MISSILE)
    {
        textureList.push_back(
            {
                model,
                ASSET_FOLDER "sprite/" + model + ".png"
            }
        );
    }

    for(const auto& model : EMODEL_BULLET)
    {
        textureList.push_back(
            {
                model,
                ASSET_FOLDER "sprite/" + model + ".png"
            }
        );
    }

    for(const auto& model : EMODEL_BOSS)
    {
        textureList.push_back(
            {
                model,
                ASSET_FOLDER "sprite/" + model + ".png"
            }
        );
    }

    for(const auto& [name, file] : textureList)
    {
        if(!LoadTexture(name, file))
        {
            return E_FAIL;
        }
    }

    m_vecDrone.resize(MAX_DRONE, nullptr);
    std::generate(m_vecDrone.begin(), m_vecDrone.end(), []() { return new EnemyDrone; } );
    m_vecBulletEnemy.resize(MAX_BULLET_ENEMY, nullptr);
    std::generate(m_vecBulletEnemy.begin(), m_vecBulletEnemy.end(), []() { return new GameBullet; } );
    m_vecBulletPlayer.resize(MAX_BULLET_PLAYER, nullptr);
    std::generate(m_vecBulletPlayer.begin(), m_vecBulletPlayer.end(), []() { return new GameBullet; } );
    m_vecMissileEnemy.resize(MAX_MISSLE_ENEMY, nullptr);
    std::generate(m_vecMissileEnemy.begin(), m_vecMissileEnemy.end(),[]() { return new GameMissile; });

    m_sndLaser.reserve(MAX_SND_EFFECT);
    m_sndBoom.reserve(MAX_SND_EFFECT);

    for(int i = 0; i < MAX_SND_EFFECT; ++i)
    {
        m_sndLaser.push_back(g2_SoundLoad(ASSET_FOLDER "sound/laser_02.wav"));
        m_sndBoom.push_back(g2_SoundLoad(ASSET_FOLDER "sound/bakuhatu29.wav"));
    }

    SetStateStart();

    m_pUi = new UiPlay;
    if(!m_pUi)
    {
        return E_FAIL;
    }

    if(FAILED(m_pUi->Init()))
    {
        return E_FAIL;
    }

    if(auto* ui = dynamic_cast<UiPlay*>(m_pUi))
    {
        ui->m_pScenePlay = this;
    }

    m_pUiBg = new UiBackground;

    if(!m_pUiBg)
    {
        return E_FAIL;
    }

    if(FAILED(m_pUiBg->Init()))
    {
        return E_FAIL;
    }

    return S_OK;
}

int ScenePlay::Destroy()
{
    ReleaseTextures();

    DeleteObjectVector(m_vecDrone);

    delete m_droneBoss;
    m_droneBoss = nullptr;

    DeleteObjectVector(m_vecBulletEnemy);
    DeleteObjectVector(m_vecBulletPlayer);
    DeleteObjectVector(m_vecMissileEnemy);

    for(int sound : m_sndLaser)
    {
        if(sound >= 0)
        {
            g2_SoundRelease(sound);
        }
    }

    for(int sound : m_sndBoom)
    {
        if(sound >= 0)
        {
            g2_SoundRelease(sound);
        }
    }

    m_sndLaser.clear();
    m_sndBoom.clear();

    delete m_pUi;
    m_pUi = nullptr;

    delete m_pUiBg;
    m_pUiBg = nullptr;

    m_mainPlayer = nullptr;

    m_playState = {};
    m_timeStored = {};
    m_timeDrone = {};

    return S_OK;
}

int ScenePlay::Update(float dt)
{
    auto* gameInfo = GameInfo::instance();
    if(!m_mainPlayer)
    {
        return E_FAIL;
    }

    m_timeStored += dt;
    m_timeDrone += dt;

    if(gameInfo->m_enablePlay &&
       0 >= m_mainPlayer->HP())
    {
        SetStateEnd();
        return S_OK;
    }

    if(gameInfo->m_enablePlay)
    {
        float newHp = m_mainPlayer->HP() + dt * 10.0F;
        if(500.0F < newHp)
        {
            newHp = 500.0F;
        }
        m_mainPlayer->HP(newHp);
    }

    if(1.0F < m_timeStored && PLAY_STATE::BEGIN == m_playState)
    {
        m_playState = PLAY_STATE::LOW;
    }
    else if(10.0F < m_timeStored && PLAY_STATE::LOW == m_playState)
    {
        m_playState = PLAY_STATE::MIDDLE;
    }
    else if(20.0F < m_timeStored && PLAY_STATE::MIDDLE == m_playState)
    {
        m_playState = PLAY_STATE::HI;
    }
    else if(40.0F < m_timeStored && PLAY_STATE::HI == m_playState)
    {
        m_playState = PLAY_STATE::BOSS;
        DroneBossSetup();
    }
    else if(m_droneBoss && !m_droneBoss->Alive() && PLAY_STATE::BOSS == m_playState)
    {
        m_playState = PLAY_STATE::END;
        SetStateEnd();
    }

    UpdateEnemy(dt);
    DroneBossUpdate(dt);
    BulletUpdate(dt);
    MissileUpdate(dt);

    const KEYCODE* key = g2_GetKeyboard();
    if(key)
    {
        bool updatedPos = false;
        if(key[VK_LEFT] == EINPUT_PRESS)
        {
            m_mainPlayer->MoveLeft(dt);
            updatedPos = true;
        }
        if(key[VK_RIGHT] == EINPUT_PRESS)
        {
            m_mainPlayer->MoveRight(dt);
            updatedPos = true;
        }
        if(key[VK_UP] == EINPUT_PRESS)
        {
            m_mainPlayer->MoveUp(dt);
            updatedPos = true;
        }
        if(key[VK_DOWN] == EINPUT_PRESS)
        {
            m_mainPlayer->MoveDown(dt);
            updatedPos = true;
        }
        if(updatedPos)
        {
            SIZE screenSize { g2_GetScnW(), g2_GetScnH() };
            m_mainPlayer->PositionScreenBoundary(screenSize);
        }

        if(gameInfo->m_enablePlay)
        {
            if(key[VK_SPACE] == EINPUT_UP)
            {
                BulletFire(m_mainPlayer, true);
            }
            if(GameInfo::IsCheat() && key[VK_F3] == EINPUT_UP)
            {
                m_mainPlayer->HP(500.0F);
            }
        }
    }

    m_mainPlayer->Update(dt);

    if(m_pUi)
    {
        m_pUi->Update(dt);
    }

    if(m_pUiBg)
    {
        m_pUiBg->Update(dt);
    }

    return S_OK;
}

void ScenePlay::DrawObject(const string& modelName, const VEC2& gamePosition, float scale, float angle)
{
    auto found = m_texture.find(modelName);
    if(found == m_texture.end())
    {
        return;
    }

    const TextureResource& texture = found->second;

    VEC2 screen =
        GameCoordToScreen(gamePosition);

    VEC2 position
    {
        screen.x - texture.width * scale * 0.5F,
        screen.y - texture.height * scale * 0.5F
    };

    VEC2 scaling
    {
        scale,
        scale
    };

    if(angle == 0.0F)
    {
        g2_Draw2D(texture.id, nullptr, &position, &scaling);
        return;
    }

    VEC2 rotationCenter
    {
        position.x + texture.width * 0.5F,
        position.y + texture.height * 0.5F
    };

      g2_Draw2D(texture.id, nullptr, &position, &scaling, &rotationCenter, angle);
}

int ScenePlay::Render()
{
    if(m_pUiBg)
    {
        m_pUiBg->Draw();
    }

    if(m_droneBoss && m_droneBoss->Alive())
    {
        DrawObject(m_droneBoss->Model(), m_droneBoss->Position(), m_droneBoss->Scale());
    }

    for(auto* drone : m_vecDrone)
    {
        if(!drone || !drone->Alive())
        {
            continue;
        }
        DrawObject(drone->Model(), drone->Position(), drone->Scale());
    }

    for(auto* bullet : m_vecBulletPlayer)
    {
        if(!bullet || !bullet->alive)
        {
            continue;
        }

        DrawObject( bullet->m_model, bullet->pos, bullet->scale);
    }

    if(m_mainPlayer)
    {
        DrawObject( m_mainPlayer->Model(), m_mainPlayer->Position(), m_mainPlayer->Scale() );
    }

    for(auto* bullet : m_vecBulletEnemy)
    {
        if(!bullet || !bullet->alive)
        {
            continue;
        }

        DrawObject(bullet->m_model, bullet->pos, bullet->scale, 0);
    }

    for(auto* missile : m_vecMissileEnemy)
    {
        if(!missile || !missile->alive)
        {
            continue;
        }

        DrawObject(missile->m_model, missile->pos, missile->scale, missile->rot);
    }

    if(m_pUi)
    {
        m_pUi->Draw();
        m_pUi->DrawFront();
    }

    return S_OK;
}

void ScenePlay::OnMouseUp(int x, int y)
{
    if(m_playState == PLAY_STATE::END)
    {
        MainApp::Instance().RequestSceneChange(
            EAPP_SCENE::EAPP_SCENE_END
        );
    }
}

void ScenePlay::SetStateStart()
{
    GameInfo::instance()->m_enablePlay = true;

    m_mainPlayer =
        GameInfo::instance()->MainPlayer();

    m_mainPlayer->Init();

    auto found =
        m_texture.find(m_mainPlayer->Model());

    if(found != m_texture.end())
    {
        VEC2 box
        {
            static_cast<float>(found->second.width),
            static_cast<float>(found->second.height)
        };

        m_mainPlayer->Box(box);
    }

    m_playState = PLAY_STATE::BEGIN;
    m_timeStored = {};
    m_timeDrone = {};
}

void ScenePlay::SetStateEnd()
{
    GameInfo::instance()->m_enablePlay = false;
    m_playState = PLAY_STATE::END;
}

int ScenePlay::UpdateEnemy(float dt)
{
    auto spawnDrone =
        [&](int modelBegin,
            int modelEnd,
            int movePattern,
            float speedMin,
            float speedMax)
    {
        for(auto* drone : m_vecDrone)
        {
            if(!drone || drone->Alive())
            {
                continue;
            }

            T_KINETICS kt {};

            int modelIndex =
                RandomRange(modelBegin, modelEnd);

            drone->Model(EMODEL_DRONE[modelIndex]);

            auto found =
                m_texture.find(drone->Model());

            if(found != m_texture.end())
            {
                kt.box =
                {
                    static_cast<float>(found->second.width),
                    static_cast<float>(found->second.height)
                };
            }

            kt.alive = true;

            kt.pos =
            {
                RandomRange(-260.0F, 260.0F),
                RandomRange(550.0F, 750.0F)
            };

            kt.vlc =
            {
                0.0F,
                RandomRange(speedMin, speedMax)
            };

            drone->Init(movePattern, kt);
            break;
        }
    };

    if(PLAY_STATE::LOW == m_playState)
    {
        if(0.6F < m_timeDrone)
        {
            m_timeDrone -= 0.6F;
            spawnDrone(
                0,
                1,
                static_cast<int>(PLAY_STATE::LOW),
                -400.0F,
                -250.0F
            );
        }
    }
    else if(PLAY_STATE::MIDDLE == m_playState)
    {
        if(0.35F < m_timeDrone)
        {
            m_timeDrone -= 0.35F;
            spawnDrone(
                2,
                3,
                static_cast<int>(PLAY_STATE::MIDDLE),
                -400.0F,
                -350.0F
            );
        }
    }
    else if(PLAY_STATE::HI == m_playState)
    {
        if(0.1F < m_timeDrone)
        {
            m_timeDrone -= 0.15F;
            spawnDrone(
                2,
                3,
                static_cast<int>(PLAY_STATE::HI),
                -500.0F,
                -450.0F
            );
        }
    }
    else if(PLAY_STATE::BOSS == m_playState)
    {
        if(0.1F < m_timeDrone)
        {
            m_timeDrone -= 0.08F;
            spawnDrone(
                2,
                3,
                static_cast<int>(PLAY_STATE::HI),
                -500.0F,
                -450.0F
            );
        }
    }

    for(auto* drone : m_vecDrone)
    {
        if(!drone || !drone->Alive())
        {
            continue;
        }

        drone->Update(dt);

        if(drone->m_firedBullet)
        {
            drone->m_firedBullet = false;
            BulletFire(drone, false);
        }

        if(g2_GetScnH() / 1.5F <
           fabsf(drone->Position().y))
        {
            drone->Alive(false);
        }
    }

    return S_OK;
}

void ScenePlay::BulletFire(GameObject* obj, bool isPlayer)
{
    if(!obj)
    {
        return;
    }

    if(isPlayer)
    {
        BulletSoundPlay();

        for(auto* bullet : m_vecBulletPlayer)
        {
            if(!bullet || bullet->alive)
            {
                continue;
            }
            T_KINETICS kt {};
            bullet->m_model = EMODEL_BULLET[0];
            auto found = m_texture.find(bullet->m_model);
            if(found != m_texture.end())
            {
                kt.box =
                {
                    static_cast<float>(found->second.width),
                    static_cast<float>(found->second.height)
                };
            }
            kt.alive = true;
            kt.pos = obj->Position();
            kt.vlc.y = GameInfo::SPEED_BULLET *1.5F;
            bullet->Init(static_cast<int>(PLAY_STATE::LOW), kt, true);
            break;
        }

        return;
    }

    for(auto* bullet : m_vecBulletEnemy)
    {
        if(!bullet || bullet->alive)
        {
            continue;
        }

        T_KINETICS kt {};

        bullet->m_model = EMODEL_BULLET[2];

        auto found = m_texture.find(bullet->m_model);
        if(found != m_texture.end())
        {
            kt.box =
            {
                static_cast<float>(found->second.width),
                static_cast<float>(found->second.height)
            };
        }

        kt.alive = true;
        kt.pos = obj->Position();

        float vx = m_mainPlayer->Position().x - obj->Position().x;
        float vy = m_mainPlayer->Position().y - obj->Position().y;

        float length = sqrtf(vx * vx + vy * vy);

        if(length < 0.0001F)
        {
            bullet->m_model.clear();
            continue;
        }

        kt.vlc.x = vx * GameInfo::SPEED_BULLET / length;
        kt.vlc.y = vy * GameInfo::SPEED_BULLET / length;
        bullet->Init(static_cast<int>(PLAY_STATE::LOW), kt, false);

        break;
    }
}

void ScenePlay::BulletUpdate(float dt)
{
    auto* gameInfo = GameInfo::instance();

    auto playerBulletCollision =
        [&](GameBullet* bullet)
    {
        bool collision = false;

        if(m_droneBoss &&
           m_droneBoss->Alive())
        {
            auto* enemyKt =
                m_droneBoss->Kinetics();

            collision =
                IsCollision(enemyKt->pos.x, enemyKt->pos.y, enemyKt->box.x, enemyKt->box.y, enemyKt->scale,
                    bullet->pos.x, bullet->pos.y, bullet->box.x, bullet->box.y, bullet->scale);

            if(collision)
            {
                BoomSoundPlay();
                float hp = m_droneBoss->HP() - 5.0F;
                m_droneBoss->HP(hp);
            }
        }

        bool result = collision && gameInfo->m_enablePlay;
        if(result)
        {
            gameInfo->IncreaseScore(50);
        }

        if(!collision)
        {
            for(auto* missile : m_vecMissileEnemy)
            {
                if(!missile || !missile->alive)
                {
                    continue;
                }

                collision =
                    IsCollision(missile->pos.x, missile->pos.y, missile->box.x, missile->box.y, missile->scale,
                        bullet->pos.x, bullet->pos.y, bullet->box.x, bullet->box.y, bullet->scale);
                if(collision)
                {
                    missile->alive = false;
                    gameInfo->IncreaseScore(20);
                    break;
                }
            }
        }

        if(!collision)
        {
            for(auto* drone : m_vecDrone)
            {
                if(!drone || !drone->Alive())
                {
                    continue;
                }

                auto* enemyKt = drone->Kinetics();

                collision = IsCollision( enemyKt->pos.x, enemyKt->pos.y, enemyKt->box.x, enemyKt->box.y, enemyKt->scale,
                        bullet->pos.x, bullet->pos.y, bullet->box.x, bullet->box.y, bullet->scale);
                if(collision)
                {
                    BoomSoundPlay();
                    drone->Alive(false);
                    break;
                }
            }
        }

        result = collision && gameInfo->m_enablePlay;
        if(result)
        {
            gameInfo->IncreaseScore(250);
        }

        return result;
    };

    for(auto* bullet : m_vecBulletPlayer)
    {
        if(!bullet || !bullet->alive)
        {
            continue;
        }

        bullet->Update( dt, playerBulletCollision);

        if(g2_GetScnH()/2.0F < fabsf(bullet->pos.y) || g2_GetScnW()/2.0F < fabsf(bullet->pos.x))
        {
            bullet->alive = false;
        }
    }

    auto enemyBulletCollision =
        [&](GameBullet* bullet)
    {
        auto* playerKt =
            gameInfo->MainPlayer()->Kinetics();

        bool collision =
            IsCollision(
                playerKt->pos.x,
                playerKt->pos.y,
                playerKt->box.x,
                playerKt->box.y,
                playerKt->scale,
                bullet->pos.x,
                bullet->pos.y,
                bullet->box.x,
                bullet->box.y,
                bullet->scale
            );

        return
            collision &&
            gameInfo->m_enablePlay;
    };

    for(auto* bullet : m_vecBulletEnemy)
    {
        if(!bullet || !bullet->alive)
        {
            continue;
        }

        bullet->Update(
            dt,
            enemyBulletCollision
        );

        if(g2_GetScnH() / 2.0F <
               fabsf(bullet->pos.y) ||
           g2_GetScnW() / 2.0F <
               fabsf(bullet->pos.x))
        {
            bullet->alive = false;
        }
    }
}

void ScenePlay::MissileFire(GameObject* obj)
{
    if(!obj)
    {
        return;
    }

    constexpr int FIRE_MISSILE = 5;

    for(int i = 0; i < FIRE_MISSILE; ++i)
    {
        for(auto* missile : m_vecMissileEnemy)
        {
            if(!missile || missile->alive)
            {
                continue;
            }

            T_KINETICS kt {};

            int modelIndex = RandomRange(0, 2);

            missile->m_model = EMODEL_MISSILE[modelIndex];

            auto found = m_texture.find(missile->m_model);
            if(found != m_texture.end())
            {
                kt.box =
                {
                    static_cast<float>(found->second.width),
                    static_cast<float>(found->second.height)
                };
            }

            kt.dif = {1.0F, 1.0F, 1.0F, 1.0F};
            kt.alive = true;
            kt.pos = obj->Position();

            float acc_x = m_mainPlayer->Position().x - obj->Position().x;
            float acc_y = m_mainPlayer->Position().y - obj->Position().y;
            float len = sqrtf(acc_x * acc_x + acc_y * acc_y);

            if(0.0001F>len)
            {
                missile->m_model.clear();
                continue;
            }

            float acc_len = GameInfo::SPEED_MISSILE * 5.0F;

            kt.acc.x = acc_x * acc_len / len;
            kt.acc.y = acc_y * acc_len / len;

            float angle = RandomRange(0.0F, static_cast<float>(2.0 * M_PI) );

            kt.vlc.x = cosf(angle) * GameInfo::SPEED_MISSILE;
            kt.vlc.y = sinf(angle) * GameInfo::SPEED_MISSILE;

            missile->Init(static_cast<int>(PLAY_STATE::LOW), kt, false);
            break;
        }
    }
}

void ScenePlay::MissileUpdate(float dt)
{
    auto* gameInfo = GameInfo::instance();
    auto collision = [&](GameMissile* missile)
    {
        auto* playerKt = gameInfo->MainPlayer()->Kinetics();

        bool isCollision = IsCollision(playerKt->pos.x, playerKt->pos.y
                                , playerKt->box.x, playerKt->box.y, playerKt->scale
                                , missile->pos.x, missile->pos.y
                                , missile->box.x, missile->box.y, missile->scale);

        return isCollision && gameInfo->m_enablePlay;
    };

    for(auto* missile : m_vecMissileEnemy)
    {
        if(!missile || !missile->alive)
        {
            continue;
        }

        missile->Update(dt, collision);

        if(g2_GetScnH() / 2.0F < fabsf(missile->pos.y) || g2_GetScnW() / 2.0F < fabsf(missile->pos.x))
        {
            missile->alive = false;
        }
    }
}

void ScenePlay::DroneBossSetup()
{
    if(m_droneBoss)
    {
        return;
    }

    auto* boss = new(std::nothrow) EnemyBoss;
    if(!boss)
    {
        return;
    }

    T_KINETICS kt {};

    string modelName = EMODEL_BOSS[0];

    auto found = m_texture.find(modelName);

    if(found != m_texture.end())
    {
        kt.box =
        {
            static_cast<float>(found->second.width),
            static_cast<float>(found->second.height)
        };
    }

    kt.alive = true;
    kt.pos =
    {
        RandomRange(-260.0F, 260.0F),
        RandomRange(550.0F, 750.0F)
    };
    kt.vlc =
    {
        0.0F,
        RandomRange(-400.0F, -250.0F)
    };

    if(FAILED(boss->Init(static_cast<int>(PLAY_STATE::BOSS), kt)))
    {
        delete boss;
        return;
    }

    boss->Model(modelName);
    m_droneBoss = boss;
}

void ScenePlay::DroneBossUpdate(float dt)
{
    if(!m_droneBoss)
    {
        return;
    }

    if(GameInfo::instance()->m_enablePlay && m_droneBoss->m_firedMissile)
    {
        m_droneBoss->m_firedMissile = false;
        MissileFire(m_droneBoss);
    }

    m_droneBoss->Update(dt);
}

void ScenePlay::BulletSoundPlay()
{
    for(int sound : m_sndLaser)
    {
        if(sound < 0)
        {
            continue;
        }

        if(!g2_SoundIsPlaying(sound))
        {
            g2_SoundReset(sound);
            g2_SoundPlay(sound);
            break;
        }
    }
}

void ScenePlay::BoomSoundPlay()
{
    for(int sound : m_sndBoom)
    {
        if(sound < 0)
        {
            continue;
        }

        if(!g2_SoundIsPlaying(sound))
        {
            g2_SoundReset(sound);
            g2_SoundPlay(sound);
            break;
        }
    }
}
