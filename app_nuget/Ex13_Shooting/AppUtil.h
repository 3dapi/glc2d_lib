#pragma once
#ifndef _AppUtil_H_
#define _AppUtil_H_

#include "AppCommon.h"

inline bool PointInRect(
    int x,
    int y,
    int left,
    int top,
    int right,
    int bottom)
{
    return left <= x && x <= right &&
           top <= y && y <= bottom;
}

inline VEC2 GameCoordToScreen(const VEC2& p)
{
    return
    {
        g2_GetScnW() * 0.5F + p.x,
        g2_GetScnH() * 0.5F - p.y
    };
}

#endif
