#pragma once

#include "libavz.h"
#include "time.h"

// 舞王/伴舞原地不动.
// 若不指定生效时间, 默认在 wave 1, -599cs 生效.
// *** 使用用例:
// MaidOn()---------原地不动, 游戏开始时起效[外]
// MaidOn(400)------400cs起效
void MaidOn()
{
    _SimpleAvZInternal::set_time_outside(-599, 1, "MaidOn");
    AvZ::MaidCheats::dancing();
}
void MaidOn(Time time)
{
    _SimpleAvZInternal::set_effect_time_and_update(time, "MaidOn");
    AvZ::MaidCheats::dancing();
}

// 舞王/伴舞一直前进.
// 若不指定生效时间, 默认在 wave 1, -599cs 生效.
// *** 使用用例:
// MaidMove()---------一直前进, 游戏开始时起效[外]
// MaidMove(400)------400cs起效
void MaidMove()
{
    _SimpleAvZInternal::set_time_outside(-599, 1, "MaidMove");
    AvZ::MaidCheats::move();
}
void MaidMove(Time time)
{
    _SimpleAvZInternal::set_effect_time_and_update(time, "MaidMove");
    AvZ::MaidCheats::move();
}

// 舞王每帧尝试召唤舞伴, 舞王/伴舞不前进.
// 若不指定生效时间, 默认在 wave 1, -599cs 生效.
// *** 使用用例:
// MaidCall()---------每帧尝试召唤, 游戏开始时起效[外]
// MaidCall(400)------400cs起效
void MaidCall()
{
    _SimpleAvZInternal::set_time_outside(-599, 1, "MaidCall");
    AvZ::MaidCheats::callPartner();
}
void MaidCall(Time time)
{
    _SimpleAvZInternal::set_effect_time_and_update(time, "MaidCall");
    AvZ::MaidCheats::callPartner();
}

// 关闭女仆秘籍, 恢复游戏原样.
// 若不指定生效时间, 默认在 wave 1, -599cs 生效.
// *** 使用用例:
// MaidStop()---------关闭女仆秘籍, 游戏开始时起效[外]
// MaidStop(400)------400cs起效
void MaidStop()
{
    _SimpleAvZInternal::set_time_outside(-599, 1, "MaidStop");
    AvZ::MaidCheats::stop();
}
void MaidStop(Time time)
{
    _SimpleAvZInternal::set_effect_time_and_update(time, "MaidStop");
    AvZ::MaidCheats::stop();
}
