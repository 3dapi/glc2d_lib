#pragma once
#ifndef _GameObject_H_
#define _GameObject_H_

#include "AppCommon.h"

using namespace std;

struct T_KINETICS
{
    VEC2    pos   { 0.0F, 0.0F };
    VEC2    vlc   { 0.0F, 0.0F };
    VEC2    acc   { 0.0F, 0.0F };
    float   scale { 1.0F };
    float   rot   { 0.0F };
    DCOL    dif   { 1.0F, 0.0F, 1.0F, 1.0F };
    VEC2    box   { 0.0F, 0.0F };
    bool        alive { false };
};

inline bool IsCollision(
    float x0, float y0, float w0, float h0, float s0,
    float x1, float y1, float w1, float h1, float s1)
{
    float p0_w = w0 * s0;
    float p0_h = h0 * s0;
    float p0_x = x0 - p0_w * 0.5F;
    float p0_y = y0 - p0_h * 0.5F;

    float p1_w = w1 * s1;
    float p1_h = h1 * s1;
    float p1_x = x1 - p1_w * 0.5F;
    float p1_y = y1 - p1_h * 0.5F;

    bool ret0 = p0_x        <= p1_x + p1_w;
    bool ret1 = p0_x + p0_w >= p1_x;
    bool ret2 = p0_y        <= p1_y + p1_h;
    bool ret3 = p0_y + p0_h >= p1_y;

    return ret0 && ret1 && ret2 && ret3;
}

class GameInfo;

class GameObject
{
    friend GameInfo;

protected:
    EAPP_CHAR_STATE m_state { EAPP_CHAR_STATE::ESTATE_CHAR_MOVE };
    T_KINETICS      m_kt {};
    float           m_spdForce {};
    float           m_hp { 100 };
    float           m_damage { 50 };
    string          m_model;

public:
    virtual ~GameObject();

    virtual void State(EAPP_CHAR_STATE v);
    virtual EAPP_CHAR_STATE State() const;

    virtual void HP(float v);
    virtual float HP() const;
    virtual void Damage(float v);
    virtual float Damage() const;

    virtual const T_KINETICS* Kinetics() const { return &m_kt; }
    virtual void Position(const VEC2& v);
    virtual VEC2 Position() const;
    virtual void Velocity(const VEC2& v);
    virtual VEC2 Velocity() const;
    virtual void Acceleration(const VEC2& v);
    virtual VEC2 Acceleration() const;
    virtual VEC2 Direction() const;
    virtual float Speed() const;
    virtual void Scale(float v);
    virtual float Scale() const;
    virtual void Rot(float v);
    virtual float Rot() const;
    virtual void Diffuse(const DCOL& v);
    virtual DCOL Diffuse() const;
    virtual void Box(const VEC2& v);
    virtual VEC2 Box() const;
    virtual void Alive(bool v);
    virtual bool Alive() const;

    virtual void MoveForceSpeed(float v);
    virtual float MoveForceSpeed() const;

    virtual void Model(const string& v) { m_model = v; }
    virtual string Model() const { return m_model; }

    virtual void Move(float dt);
    virtual void MoveLeft(float dt);
    virtual void MoveRight(float dt);
    virtual void MoveUp(float dt);
    virtual void MoveDown(float dt);
};

class GamePlayer : public GameObject
{
public:
    GamePlayer();

    int Init(const string& model = "");
    int Update(float dt);

    void State(EAPP_CHAR_STATE v) override;
    EAPP_CHAR_STATE State() const override;

    void PositionScreenBoundary(const ::SIZE& screenSize);
};

class GameBullet : public T_KINETICS
{
public:
    int    m_movePattern {};
    bool   m_isPlayer { false };
    string m_model;

public:
    int Init(int movePattern, const T_KINETICS& kt, bool isPlayer);
    int Update(float dt, const function<bool(GameBullet* obj)>& funcCollision);
};

class GameMissile : public T_KINETICS
{
public:
    int    m_movePattern {};
    bool   m_isPlayer { false };
    string m_model;

public:
    int Init(int movePattern, const T_KINETICS& kt, bool isPlayer);
    int Update(float dt, const function<bool(GameMissile* obj)>& funcCollision);
};

class EnemyDrone : public GameObject
{
public:
    int   m_movePattern {};
    int   m_bullet { 1 };
    bool  m_firedBullet {};
    float m_timeStore {};
    float m_timeFire {};
    float m_flipLen {};
    float m_flipAngle {};

public:
    EnemyDrone();

    int Init(int movePattern, const T_KINETICS& kt);
    int Update(float dt);

    void Move(float dt) override;
    void FireBullet();
};

struct TMOVE_PATTERN
{
    VEC2 dir;
    VEC2 dest;
};

struct TMOVE_PATTERN_CIRCLE
{
    float   phase {};
    float   angle {};
    VEC2    center {};
    float   len {};
    float   speed {};
};

class EnemyBoss : public GameObject
{
public:
    int   m_movePattern {};
    int   m_missile {};
    bool  m_firedMissile {};
    float m_timeStore {};
    float m_timeFire {};

    TMOVE_PATTERN_CIRCLE m_mov_pattern {};

public:
    EnemyBoss();

    int Init(int movePattern, const T_KINETICS& kt);
    int Update(float dt);

    void MovePatternCircle(float dt);
    void FireBosMissle();
};

#endif
