#pragma once

#include "error.h"

namespace _SimpleAvZInternal {

class Global : AvZ::GlobalVar {
private:
    const int TIME_WAVE_INIT_VAL = -0xFFFF;

public:
    int last_effect_time = TIME_WAVE_INIT_VAL;
    int last_effect_wave = TIME_WAVE_INIT_VAL;
    bool is_ice_positions_initialized = false;

    void virtual beforeScript() override
    {
        last_effect_time = TIME_WAVE_INIT_VAL;
        last_effect_wave = TIME_WAVE_INIT_VAL;
        is_ice_positions_initialized = false;
    }

    void reset_last_effect_time()
    {
        last_effect_time = TIME_WAVE_INIT_VAL;
    }

    void reset_last_effect_wave()
    {
        last_effect_wave = TIME_WAVE_INIT_VAL;
    }

    bool is_last_effect_time_initialized()
    {
        return last_effect_time != TIME_WAVE_INIT_VAL;
    }

    bool is_last_effect_wave_initialized()
    {
        return last_effect_wave != TIME_WAVE_INIT_VAL;
    }
};

Global global;

// scene 相关
bool is_night_time()
{
    auto scene = AvZ::GetMainObject()->scene();
    return (scene > 4) || (scene % 2 == 1);
}

bool is_frontyard()
{
    auto scene = AvZ::GetMainObject()->scene();
    return (scene == 0) || (scene == 1);
}

bool is_backyard()
{
    auto scene = AvZ::GetMainObject()->scene();
    return (scene == 2) || (scene == 3);
}

bool is_roof()
{
    auto scene = AvZ::GetMainObject()->scene();
    return scene >= 4;
}

} // namespace _SimpleAvZInternal
