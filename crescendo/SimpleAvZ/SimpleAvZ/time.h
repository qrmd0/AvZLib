#pragma once

#include "global.h"
#include "libavz.h"

struct Time {
    enum class Type {
        ABS, // 绝对时间
        REL, // 相对时间
    };

    Type type;
    int time;
    bool fix_card_time_to_cob; // 是否将冰核樱辣窝修正至炮等效时机

    Time(int _time)
        : type(Type::ABS)
        , time(_time)
        , fix_card_time_to_cob(true)
    {
    }

    Time(Type _type, int _time, bool _fix_card_time_to_cob)
        : type(_type)
        , time(_time)
        , fix_card_time_to_cob(_fix_card_time_to_cob)
    {
    }
};

Time after(int t)
{
    return {Time::Type::REL, t, true};
}

const auto now = after(0);

Time exact(Time t)
{
    t.fix_card_time_to_cob = false;
    return t;
}

struct ShovelTime {
    enum class Type {
        NONE,  // 不指定
        KEEP,  // 指定铲除延迟
        UNTIL, // 指定铲除时机
    };

    Type type;
    int time;

    ShovelTime()
        : type(Type::NONE)
        , time(-1)
    {
    }

    ShovelTime(Type _type, int _time)
        : type(_type)
        , time(_time)
    {
    }
};

ShovelTime keep(int t)
{
    return ShovelTime(ShovelTime::Type::KEEP, t);
}

ShovelTime until(int t)
{
    return ShovelTime(ShovelTime::Type::UNTIL, t);
}

namespace _SimpleAvZInternal {

// 获得延迟时间, 并且更新[last_effect_time]
int get_delayed_time_and_update(int delay_time, const std::string& func_name)
{
    if (!global.is_last_effect_time_initialized()) {
        error(func_name + "-->after", "没有延迟的基准, 请先使用固定时间的用炮/用卡函数");
    }

    global.last_effect_time += delay_time;
    return global.last_effect_time;
}

// 获得生效时间, 并且更新[last_effect_time]
// 不适用于卡片, 卡片应用[get_card_effect_time]
int get_effect_time(Time time, const std::string& func_name)
{
    if (!global.is_last_effect_wave_initialized()) {
        error(func_name, "没有设置波数, 请用以下方式调用本函数:\nfor (auto w : waves(...)){\n    // 调用本函数\n}");
    }

    switch (time.type) {
    case Time::Type::ABS:
        global.last_effect_time = time.time;
        return time.time;
    case Time::Type::REL:
        return get_delayed_time_and_update(time.time, func_name);
    default:
        assert(false);
    }
}

// 在循环节内设定时间
void set_time_inside(int time, const std::string& func_name)
{
    if (!global.is_last_effect_wave_initialized()) {
        error(func_name, "没有设置波数, 请用以下方式调用本函数:\nfor (auto w : waves(...)){\n    // 调用本函数\n}");
    }
    AvZ::SetTime(time, global.last_effect_wave);
}

void get_effect_time_and_set_time(Time time, const std::string& func_name)
{
    set_time_inside(get_effect_time(time, func_name), func_name);
}

// 在循环节外设定时间
void set_time_outside(int time, int wave, const std::string& func_name)
{
    if (global.is_last_effect_wave_initialized()) {
        error(func_name, "若省略生效时间, 需在waves()循环节外使用");
    }

    AvZ::SetTime(time, wave);
}

} // namespace _SimpleAvZInternal
