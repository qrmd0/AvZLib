#pragma once

#include "error.h"
#include "global.h"
#include "libavz.h"
#include "time.h"
#include "util.h"

class CobOperator : public AvZ::PaoOperator {
public:
    // Create CobOperator and specify cobs with tail at which cols to use.
    // *** Usage:
    // CobOperator c1(1)--------- Only use cobs with tail at col 1
    // CobOperator c45(4, 5)----- Only use cobs with tail at col 4 or 5
    template <typename... Args>
    CobOperator(Args... args)
        : AvZ::PaoOperator()
    {
        for (int col : {args...}) {
            validate_cob_col(col, "CobOperator constructor");
            if (cob_cols.count(col)) {
                _SimpleAvZInternal::error("CobOperator constructor", "Duplicate cob tail cols: #", col);
            }
            cob_cols.insert(col);
        }
    }

    CobOperator()
        : AvZ::PaoOperator()
    {
    }

    // Fire two cobs.
    // If rows are omitted, they default to row 2 & 5 for PE/FE and row 2 & 4 for other scenes.
    // If col is omitted, it defaults to 9.
    // *** Usage:
    // c.PP(318)----------------- Fire (2,9) and (5,9), taking effect at 318cs
    // c.PP(318, 8)-------------- Fire (2,8) and (5,8)
    // c.PP(318, {8, 9})--------- Fire (2,8) and (5,9)
    // c.PP(318, {2, 6}, 9)------ Fire (2,9) and (6,9)
    // c.PP(after(110), ...)----- Take effect after 110cs
    void PP(Time time, const std::array<int, 2>& rows, float col)
    {
        cob_internal(time, {{rows[0], col}, {rows[1], col}}, "PP");
    }

    void PP(Time time, const std::array<float, 2>& cols)
    {
        auto row2 = _SimpleAvZInternal::is_backyard() ? 5 : 4;
        cob_internal(time, {{2, cols[0]}, {row2, cols[1]}}, "PP");
    }

    void PP(Time time, float col)
    {
        PP(time, {col, col});
    }

    void PP(Time time)
    {
        PP(time, {9, 9});
    }

    // Fire two interception cobs.
    // If rows are omitted, they default to row 1 & 5 for PE/FE and row 1 & 4 for other scenes.
    // If col is omitted, it defaults to 9.
    // *** Usage:
    // c.DD(318)----------------- Fire (1,9) and (5,9), taking effect at 318cs
    // c.DD(318, 8)-------------- Fire (1,8) and (5,8)
    // c.DD(318, {8, 9})--------- Fire (1,8) and (5,9)
    // c.DD(318, {2, 5}, 3.5)---- Fire (2,3.5) and (5,3.5)
    // c.DD(after(110), ...)----- Take effect after 110cs
    void DD(Time time, const std::array<int, 2>& rows, float col)
    {
        std::vector<AvZ::Position> positions;
        for (const auto& row : rows)
            positions.push_back({row, col});
        cob_internal(time, positions, "DD");
    }

    void DD(Time time, const std::array<float, 2>& cols)
    {
        auto row2 = _SimpleAvZInternal::is_backyard() ? 5 : 4;
        cob_internal(time, {{1, cols[0]}, {row2, cols[1]}}, "DD");
    }

    void DD(Time time, float col)
    {
        DD(time, {col, col});
    }

    void DD(Time time)
    {
        DD(time, {9, 9});
    }

    // Fire one cob.
    // You may specify which cob to use.
    // *** Usage:
    // c.P(318, 2, 9)----------------- Fire (2,9), taking effect at 318cs
    // c.P(318, 1, 1, 2, 8)----------- Use cob at 1-1 to fire (2,8)
    // c.P(after(110), ...)----------- Take effect after 110cs
    void P(Time time, int row, float col)
    {
        cob_internal(time, {{row, col}}, "P");
    }

    void P(Time time, int cob_row, int cob_col, int row, float col)
    {
        cob_internal(time, {{row, col}}, "P", {cob_row, cob_col});
    }

    // Fire one separation cob.
    // *** Usage:
    // c.B(304, 5, 8.225)----- Fire (5, 8.225), taking effect at 304cs
    void B(Time time, int row, float col)
    {
        cob_internal(time, {{row, col}}, "B");
    }

    // Fire one interception cob.
    // *** Usage:
    // c.D(395, 1, 9)----- Fire (1,9), taking effect at 395cs
    void D(Time time, int row, float col)
    {
        cob_internal(time, {{row, col}}, "D");
    }

    // Exclude certain cobs from being used.
    // *** Usage:
    // c.ExcludeCob(3, 5)--------- Do not use cob at 3-5, effective since game start [EXT]
    // c.ExcludeCob(400, ...)----- Effective since 400cs
    void ExcludeCob(Time time, int row, int col)
    {
        _SimpleAvZInternal::get_effect_time_and_set_time(time, "ExcludeCob");
        exclude_cob_internal(row, col);
    }

    void ExcludeCob(int row, int col)
    {
        _SimpleAvZInternal::set_time_outside(-599, 1, "ExcludeCob");
        exclude_cob_internal(row, col);
    }

    // Reset to use all cobs.
    // *** Usage:
    // c.ResetCob(400)----- Reset to use all cobs, effective since 400cs
    void ResetCob(Time time)
    {
        _SimpleAvZInternal::get_effect_time_and_set_time(time, "ResetCob");
        autoGetPaoList();
    }

    void virtual beforeScript() override
    {
        initialState();
        sequential_mode = TIME;
        next_pao = 0;

        if (cob_cols.empty()) {
            AvZ::SetTime(-599, 1);
            autoGetPaoList();
        } else {
            AvZ::InsertTimeOperation(
                -599, 1, [=]() {
                    std::vector<AvZ::Grid> valid_cobs;
                    for (auto& p : AvZ::alive_plant_filter) {
                        if (p.type() == COB_CANNON && cob_cols.count(p.col() + 1))
                            valid_cobs.push_back({p.row() + 1, p.col() + 1});
                    }

                    std::sort(valid_cobs.begin(), valid_cobs.end()); // 按炮位置排序

                    AvZ::InsertGuard _(false);
                    resetPaoList(valid_cobs);
                },
                "CobOperator::beforeScript");
        }
    }

private:
    std::set<int> cob_cols;

    void validate_cob_row(int row, const std::string& func_name)
    {
        int max_row = _SimpleAvZInternal::is_backyard() ? 6 : 5;
        if (row < 1 || row > max_row) {
            _SimpleAvZInternal::error(func_name, "Cob row should be within 1~#: #", max_row, row);
        }
    }

    void validate_cob_col(int col, const std::string& func_name)
    {
        if (col < 1 || col > 8) {
            _SimpleAvZInternal::error(func_name, "Cob col should be within 1~8: #", col);
        }
    }

    // 主发炮函数
    void cob_internal(Time time, const std::vector<AvZ::Position>& positions, const std::string& func_name,
        const AvZ::Grid& specified_cob = {-1, -1})
    {
        int max_row = _SimpleAvZInternal::is_backyard() ? 6 : 5;

        for (const auto& pos : positions) {
            if (pos.row < 1 || pos.row > max_row) {
                _SimpleAvZInternal::error(func_name, "Cob hit row should be within 1~#: #", max_row, pos.row);
            }
            if (pos.col < 0 || pos.col > 10) {
                _SimpleAvZInternal::error(func_name, "Cob hit col should be within 0.0~10.0: #", pos.col);
            }
        }

        bool specify_cob = false;
        if (specified_cob.row != -1 || specified_cob.col != -1) {
            specify_cob = true;
            validate_cob_row(specified_cob.row, func_name);
            validate_cob_col(specified_cob.col, func_name);
        }

        auto effect_time = _SimpleAvZInternal::get_effect_time(time, func_name);

        for (const auto& pos : positions) {
            auto row = pos.row;
            auto col = pos.col;
            if (_SimpleAvZInternal::is_roof()) {
                _SimpleAvZInternal::set_time_inside(effect_time - 387, func_name);
                if (specify_cob)
                    PaoOperator::rawRoofPao(specified_cob.row, specified_cob.col, row, col);
                else
                    PaoOperator::roofPao(row, col);
            } else if (_SimpleAvZInternal::is_backyard() && AvZ::RangeIn(row, {3, 4})) {
                _SimpleAvZInternal::set_time_inside(effect_time - 378, func_name);
                if (specify_cob)
                    PaoOperator::rawPao(specified_cob.row, specified_cob.col, row, col);
                else
                    PaoOperator::pao(row, col);
            } else {
                _SimpleAvZInternal::set_time_inside(effect_time - 373, func_name);
                if (specify_cob)
                    PaoOperator::rawPao(specified_cob.row, specified_cob.col, row, col);
                else
                    PaoOperator::pao(row, col);
            }
        }
    }

    void exclude_cob_internal(int row, int col)
    {
        validate_cob_row(row, "ExcludeCob");
        validate_cob_col(col, "ExcludeCob");

        // 保留原定炮序
        AvZ::InsertOperation([=]() {
            auto prev_index_vec = pao_index_vec;
            auto excluded_index = AvZ::GetPlantIndex(row, col, COB_CANNON);
            std::vector<AvZ::Grid> valid_cobs;

            auto plant_array = AvZ::GetMainObject()->plantArray();
            for (const auto& index : pao_index_vec) {
                if (index != excluded_index)
                    valid_cobs.push_back({plant_array[index].row() + 1,
                        plant_array[index].col() + 1});
            }

            AvZ::InsertGuard _(false);
            resetPaoList(valid_cobs);
        },
            "CobOperator::ExcludeCob");
    }
};

CobOperator cob_operator;

// Fire two cobs.
// If rows are omitted, they default to row 2 & 5 for PE/FE and row 2 & 4 for other scenes.
// If col is omitted, it defaults to 9.
// *** Usage:
// PP(318)----------------- Fire (2,9) and (5,9), taking effect at 318cs
// PP(318, 8)-------------- Fire (2,8) and (5,8)
// PP(318, {8, 9})--------- Fire (2,8) and (5,9)
// PP(318, {2, 6}, 9)------ Fire (2,9) and (6,9)
// PP(after(110), ...)----- Take effect after 110cs
void PP(Time time, const std::array<int, 2>& rows, float col)
{
    cob_operator.PP(time, rows, col);
}

void PP(Time time, const std::array<float, 2>& cols)
{
    cob_operator.PP(time, cols);
}

void PP(Time time, float col)
{
    cob_operator.PP(time, col);
}

void PP(Time time)
{
    cob_operator.PP(time);
}

// Fire two interception cobs.
// If rows are omitted, they default to row 1 & 5 for PE/FE and row 1 & 4 for other scenes.
// If col is omitted, it defaults to 9.
// *** Usage:
// DD(318)----------------- Fire (1,9) and (5,9), taking effect at 318cs
// DD(318, 8)-------------- Fire (1,8) and (5,8)
// DD(318, {8, 9})--------- Fire (1,8) and (5,9)
// DD(318, {2, 5}, 3.5)---- Fire (2,3.5) and (5,3.5)
// DD(after(110), ...)----- Take effect after 110cs
void DD(Time time, const std::array<int, 2>& rows, float col)
{
    cob_operator.DD(time, rows, col);
}

void DD(Time time, const std::array<float, 2>& cols)
{
    cob_operator.DD(time, cols);
}

void DD(Time time, float col)
{
    cob_operator.DD(time, col);
}

void DD(Time time)
{
    cob_operator.DD(time);
}

// Fire one cob.
// You may specify which cob to use.
// *** Usage:
// P(318, 2, 9)----------------- Fire (2,9), taking effect at 318cs
// P(318, 1, 1, 2, 8)----------- Use cob at 1-1 to fire (2,8)
// P(after(110), ...)----------- Take effect after 110cs
void P(Time time, int row, float col)
{
    cob_operator.P(time, row, col);
}

// Fire one separation cob.
// *** Usage:
// B(304, 5, 8.225)----- Fire (5, 8.225), taking effect at 304cs
void B(Time time, int row, float col)
{
    cob_operator.B(time, row, col);
}

// Fire one interception cob.
// *** Usage:
// D(395, 1, 9)----- Fire (1,9), taking effect at 395cs
void D(Time time, int row, float col)
{
    cob_operator.D(time, row, col);
}

// Exclude certain cobs from being used.
// *** Usage:
// ExcludeCob(3, 5)--------- Do not use cob at 3-5, effective since game start [EXT]
// ExcludeCob(400, ...)----- Effective since 400cs
void ExcludeCob(Time time, int row, int col)
{
    cob_operator.ExcludeCob(time, row, col);
}

void ExcludeCob(int row, int col)
{
    cob_operator.ExcludeCob(row, col);
}

// Reset to use all cobs.
// *** Usage:
// ResetCob(400)----- Reset to use all cobs, effective since 400cs
void ResetCob(Time time)
{
    cob_operator.ResetCob(time);
}