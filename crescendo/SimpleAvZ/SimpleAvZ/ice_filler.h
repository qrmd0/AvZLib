#pragma once

#include "card.h"
#include "global.h"
#include "libavz.h"
#include "time.h"

namespace _SimpleAvZInternal {

void set_ice_internal(const std::vector<AvZ::Grid>& ice_positions)
{
    int max_row = _SimpleAvZInternal::is_visually_six_rows() ? 6 : 5;

    for (const auto& pos : ice_positions) {
        auto row = pos.row;
        auto col = pos.col;
        if (row < 1 || row > max_row) {
            _SimpleAvZInternal::error("SetIce", "存冰行数应在1~#内\n存冰行数: #", max_row, row);
        }
        if (col < 1 || col > 9) {
            _SimpleAvZInternal::error("SetIce", "存冰列数应在1~9内\n存冰列数: #", col);
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

// 设置存冰位置.
// 若不指定生效时间, 默认在 wave 1, -599cs 生效.
// *** 使用示例:
// SetIce({{1, 1}, {1, 2}})-----在1-1, 1-2存冰(优先使用1-2)[外]
// SetIce(400, {...})-----------400cs生效
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

// 白昼点冰. 自带生效时机修正.
// 若不指定生效时间, 默认在本波 601cs 生效.
// *** 使用示例:
// I()---------------点冰, 601cs生效(完美预判冰)
// I(after(210))-----延迟210cs生效(ice3), 推荐在激活炮后使用
// I(359)------------359cs生效
void I(Time time = Time(601))
{
    if (_SimpleAvZInternal::is_night_time()) {
        _SimpleAvZInternal::error("I", "不可在夜间使用白昼版本的I()\n请提供用冰位置");
    }

    auto effect_time = _SimpleAvZInternal::get_card_effect_time(time, {ICE_SHROOM, COFFEE_BEAN}, "I");
    _SimpleAvZInternal::set_time_inside(effect_time - 299, "I");
    AvZ::ice_filler.coffee();
    AvZ::SetPlantActiveTime(ICE_SHROOM, 298);
}