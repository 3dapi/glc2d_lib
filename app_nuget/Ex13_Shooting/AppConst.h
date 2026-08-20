#pragma once
#ifndef __APPCONST_H__
#define __APPCONST_H__

#include <string>
#include <vector>

using std::string;
using std::vector;

enum EAPP_SCENE
{
    EAPP_SCENE_NONE  = 0,
    EAPP_SCENE_BEGIN = 1,
    EAPP_SCENE_LOBBY,
    EAPP_SCENE_PLAY,
    EAPP_SCENE_END,
    EAPP_SCENE_COUNT,
};

enum class EAPP_CHAR_STATE : int
{
    ESTATE_CHAR_NONE = 0,
    ESTATE_CHAR_MOVE,
    ESTATE_CHAR_DYING,
    ESTATE_CHAR_DEATH,
};

inline static const vector<string> EMODEL_SHIP
{
    "space_shooter/Ship_2_C_Small",
    "space_shooter/Ship_2_D_Small",
    "space_shooter/Ship_2_C_Medium",
    "space_shooter/Ship_2_D_Medium",
};

inline static const vector<string> EMODEL_DRONE
{
    "space_shooter/Pickup_1_A_Small",
    "space_shooter/Pickup_1_B_Small",
    "space_shooter/Pickup_1_C_Small",
    "space_shooter/Pickup_1_D_Small",
    "space_shooter/Pickup_1_E_Small",

    "space_shooter/Pickup_2_A_Small",
    "space_shooter/Pickup_2_B_Small",
    "space_shooter/Pickup_2_C_Small",
    "space_shooter/Pickup_2_D_Small",
    "space_shooter/Pickup_2_E_Small",
};

inline static const vector<string> EMODEL_MISSILE
{
    "space_shooter/Missile_A_Small",
    "space_shooter/Missile_B_Small",
    "space_shooter/Missile_C_Small",
    "space_shooter/Missile_D_Small",
    "space_shooter/Missile_E_Small",
};

inline static const vector<string> EMODEL_BULLET
{
    "bullet-red",
    "bullet-green",
    "bullet-yellow",
    "bullet-cyan",
};

inline static const vector<string> EMODEL_BOSS
{
    "space_shooter/Boss_1_A_Small",
    "space_shooter/Boss_1_B_Small",
};

inline static const vector<string> EMODEL_BG
{
    "Background Grid",
};

#endif // __APPCONST_H__
