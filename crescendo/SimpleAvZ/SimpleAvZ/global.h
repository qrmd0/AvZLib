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
    return scene == 1 || scene == 3 || scene == 5 || scene == 6 || scene == 8 || scene == 11;
}

bool is_visually_six_rows()
{
    auto scene = AvZ::GetMainObject()->scene();
    return scene == 2 || scene == 3 || scene == 10 || scene == 11;
}

bool is_actually_six_rows()
{
    auto scene = AvZ::GetMainObject()->scene();
    return scene == 2 || scene == 3 || scene == 8 || scene == 10 || scene == 11;
}

int get_max_spawn_row()
{
    return is_actually_six_rows() ? 6 : 5;
}

bool has_water_rows()
{
    auto scene = AvZ::GetMainObject()->scene();
    return scene == 2 || scene == 3 || scene == 8;
}

bool no_dancing_in_side_rows()
{
    auto scene = AvZ::GetMainObject()->scene();
    return scene == 0 || scene == 1 || scene == 6 || scene == 7 || scene == 9 || scene == 10 || scene == 11;
}

bool no_zomboni()
{
    auto scene = AvZ::GetMainObject()->scene();
    return scene == 1 || scene == 11;
}

bool is_roof()
{
    auto scene = AvZ::GetMainObject()->scene();
    return scene == 4 || scene == 5;
}

} // namespace _SimpleAvZInternal
