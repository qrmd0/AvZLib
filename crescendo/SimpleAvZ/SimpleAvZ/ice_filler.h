#pragma once

#include "card.h"
#include "global.h"
#include "libavz.h"
#include "time.h"

namespace _SimpleAvZInternal {

void set_ice_internal(const std::vector<AvZ::Grid>& ice_positions)
{
    int max_row = _SimpleAvZInternal::is_backyard() ? 6 : 5;

    for (const auto& pos : ice_positions) {
        auto row = pos.row;
        auto col = pos.col;
        if (row < 1 || row > max_row) {
            _SimpleAvZInternal::error("SetIce", "Ice row should be within 1~#: #", max_row, row);
        }
        if (col < 1 || col > 9) {
            _SimpleAvZInternal::error("SetIce", "Ice col should be within 1~9: #", col);
        }
    }

    if (!_SimpleAvZInternal::global.is_ice_positions_initialized) {
        _SimpleAvZInternal::global.is_ice_positions_initialized = true;
        AvZ::ice_filler.start(ice_positions);
    } else {
        AvZ::ice_filler.resetFillList(ice_positions);
    }
}

} // namespace _SimpleAvZInternal

// Set where to store ices in daytime.
// If effect time is not specified, it defaults to game start (wave 1, -599cs).
// *** Usage:
// SetIce({{1, 1}, {1, 2}})----- Store and use ices at 1-1, 1-2 (use 1-2 first), effective since game start [EXT]
// SetIce(400, {...})----------- Effective since 400cs
void SetIce(Time time, const std::vector<AvZ::Grid>& ice_positions)
{
    _SimpleAvZInternal::get_effect_time_and_set_time(time, "SetIce");
    _SimpleAvZInternal::set_ice_internal(ice_positions);
}

void SetIce(const std::vector<AvZ::Grid>& ice_positions)
{
    _SimpleAvZInternal::set_time_outside(-599, 1, "SetIce");
    _SimpleAvZInternal::set_ice_internal(ice_positions);
}

// Use ice in daytime. Effect time is auto-corrected.
// If effect time is not specified, it defaults to 601cs of the current wave (perfect ice for next wave).
// *** Usage:
// I()--------------- Use ice, taking effect at 601cs (perfect ice)
// I(after(210))----- Use ice, takeing effect after 210cs (ice3), recommended to be used after activation cobs
// I(359)------------ Use ice, taking effect at 359cs
void I(Time time = Time(601))
{
    if (_SimpleAvZInternal::is_night_time()) {
        _SimpleAvZInternal::error("I", "Cannot use daytime version of I() at nighttime\nPlease provide ice position");
    }

    auto effect_time = _SimpleAvZInternal::get_card_effect_time(time, {ICE_SHROOM, COFFEE_BEAN}, "I");
    _SimpleAvZInternal::set_time_inside(effect_time - 299, "I");
    AvZ::ice_filler.coffee();
    AvZ::SetPlantActiveTime(ICE_SHROOM, 298);
}