#pragma once

#include "error.h"
#include "global.h"
#include "libavz.h"
#include "time.h"
#include "util.h"

namespace _SimpleAvZInternal {

PlantType non_imitater(const PlantType& plant_type)
{
    return (plant_type > IMITATOR) ? (PlantType)(plant_type - IMITATOR - 1) : plant_type;
}

// 是否为一次性植物(白天咖啡豆+冰/核的组合也视为一次性)
bool is_instant(std::vector<PlantType> plant_types)
{
    for (auto& plant_type : plant_types)
        plant_type = non_imitater(plant_type);

    for (const auto& plant_type : plant_types) {
        if (is_night_time() && contains({ICE_SHROOM, DOOM_SHROOM}, plant_type))
            return true;
        if (!is_night_time() && contains({ICE_SHROOM, DOOM_SHROOM}, plant_type) && contains(plant_types, COFFEE_BEAN))
            return true;
        if (contains({CHERRY_BOMB, JALAPENO, SQUASH}, plant_type))
            return true;
    }
    return false;
}

int get_card_effect_time(Time time, const std::vector<PlantType>& plant_types, const std::string& func_name)
{
    auto effect_time = get_effect_time(time, func_name);
    if (time.fix_card_time_to_cob && is_instant(plant_types))
        return effect_time + 1;
    else
        return effect_time;
}

int get_prep_time(const PlantType& plant_type, const std::vector<PlantType>& all_plants)
{
    int prep_time = 0;
    auto non_im_plant_type = non_imitater(plant_type);

    if (contains({LILY_PAD, FLOWER_POT}, non_im_plant_type)) {
        if (all_plants.empty() || (all_plants.size() == 1 && all_plants.at(0) == plant_type))
            return 0;
        else
            return (plant_type > IMITATOR) ? -2000 : -1000; // 之后特殊处理
    }

    if (contains({CHERRY_BOMB, JALAPENO}, non_im_plant_type)) {
        prep_time = 100;
    } else if (non_im_plant_type == SQUASH) {
        prep_time = 182;
    } else if (contains(all_plants, COFFEE_BEAN)) {
        prep_time = 299;
    } else if (contains(all_plants, M_COFFEE_BEAN)) {
        prep_time = 299 + 320;
    } else if (contains({ICE_SHROOM, DOOM_SHROOM}, non_im_plant_type) && is_night_time()) {
        prep_time = 100;
    } else {
        return 0;
    }

    if (plant_type > IMITATOR)
        prep_time += 320;

    return prep_time;
}

int get_prep_time(const PlantType& plant_type)
{
    return get_prep_time(plant_type, {{plant_type}});
}

std::vector<int> get_prep_times(const std::vector<PlantType>& plant_types)
{
    std::vector<int> prep_times;
    for (const auto& plant_type : plant_types)
        prep_times.push_back(get_prep_time(plant_type, plant_types));

    // 特殊处理容器(荷叶, 花盆)
    for (auto& prep_time : prep_times) {
        if (prep_time < 0) {
            if (prep_time == -1000)
                prep_time = *std::max_element(prep_times.begin(), prep_times.end());
            else if (prep_time == -2000)
                prep_time = *std::max_element(prep_times.begin(), prep_times.end()) + 320;
            prep_time = (prep_time < 0) ? 0 : prep_time;
        }
    }

    return prep_times;
}

std::vector<bool> get_set_active_time_flags(const std::vector<PlantType>& plant_types)
{
    std::vector<bool> set_active_time_flags;

    for (const auto& plant_type : plant_types) {
        bool flag = false;
        bool is_imitater = plant_type > IMITATOR;
        auto non_im_plant_type = non_imitater(plant_type);

        if (contains({CHERRY_BOMB, JALAPENO}, non_im_plant_type)) {
            flag = is_imitater;
        } else if (contains({ICE_SHROOM, DOOM_SHROOM}, non_im_plant_type)) {
            if (is_night_time())
                flag = is_imitater;
            else if (contains(plant_types, COFFEE_BEAN) || contains(plant_types, M_COFFEE_BEAN))
                flag = true;
        }

        set_active_time_flags.push_back(flag);
    }

    return set_active_time_flags;
}

bool get_set_active_time_flag(const PlantType& plant_type)
{
    return get_set_active_time_flags({{plant_type}}).front();
}

// 如果存在目标植物, 则铲之
// 除此之外, 若仅有容器, 则也铲之, 否则不铲
void shovel_with_container(int time, PlantType target, int row, int col, const std::string& func_name)
{
    target = non_imitater(target);
    set_time_inside(time, func_name);
    AvZ::InsertOperation([=]() {
        bool shovel_container = true;
        int container_num = 0;
        for (auto& p : AvZ::alive_plant_filter) {
            if (p.row() + 1 == row) {
                if (p.col() + 1 == col || (p.col() == col && p.type() == COB_CANNON)) {
                    if (p.type() == target)
                        AvZ::ShovelNotInQueue(row, col, target == PUMPKIN);
                    else if (p.type() == LILY_PAD || p.type() == FLOWER_POT)
                        container_num++;
                    else
                        shovel_container = false;
                }
            }
        }
        if (shovel_container)
            for (int i = 0; i < container_num; i++)
                AvZ::ShovelNotInQueue(row, col);
    },
        func_name + "-->shovel_with_container");
}

std::set<PlantType> get_invalid_plants(int row)
{
    switch (AvZ::GetMainObject()->scene()) {
    case 0:
        return {GRAVE_BUSTER, LILY_PAD, TANGLE_KELP, SEA_SHROOM, CATTAIL};
    case 1:
        return {LILY_PAD, TANGLE_KELP, SEA_SHROOM, COFFEE_BEAN, CATTAIL};
    case 2:
        if (row == 3 || row == 4) {
            return {POTATO_MINE, GRAVE_BUSTER, SPIKEWEED, FLOWER_POT, SPIKEROCK};
        } else {
            return {GRAVE_BUSTER, LILY_PAD, TANGLE_KELP, SEA_SHROOM, CATTAIL};
        }
    case 3:
        if (row == 3 || row == 4) {
            return {POTATO_MINE, GRAVE_BUSTER, SPIKEWEED, FLOWER_POT, COFFEE_BEAN, SPIKEROCK};
        } else {
            return {GRAVE_BUSTER, LILY_PAD, TANGLE_KELP, SEA_SHROOM, COFFEE_BEAN, CATTAIL};
        }
    case 4:
        return {GRAVE_BUSTER, LILY_PAD, TANGLE_KELP, SPIKEWEED, SEA_SHROOM, CATTAIL, SPIKEROCK};
    default:
        return {GRAVE_BUSTER, LILY_PAD, TANGLE_KELP, SPIKEWEED, SEA_SHROOM, COFFEE_BEAN, CATTAIL, SPIKEROCK};
    }
}

void validate_card_position(const PlantType& plant_type, int row, int col, const std::string& func_name)
{
    int max_row = is_backyard() ? 6 : 5;
    if (row < 1 || row > max_row) {
        error(func_name, "Card row should be within 1~#: #", max_row, row);
    }
    if (col < 1 || col > 9) {
        error(func_name, "Card col should be within 1~9: #", col);
    }
    if (get_invalid_plants(row).count(non_imitater(plant_type))) {
        error(func_name, "Cannot use this card in the current scene\nCard: #\nCard row: #", non_imitater(plant_type), row);
    }
}

void validate_shovel_position(int row, int col, const std::string& func_name)
{
    int max_row = _SimpleAvZInternal::is_backyard() ? 6 : 5;
    if (row < 1 || row > max_row) {
        _SimpleAvZInternal::error(func_name, "Shovel row should be within 1~#: #", max_row, row);
    }
    if (col < 1 || col > 9) {
        _SimpleAvZInternal::error(func_name, "Shovel col should be within 1~9: #", col);
    }
}

} // namespace _SimpleAvZInternal

// Shovel plants.
// You may provide a single position, multiple rows with the same column, or a certain plant type to be shoveled.
// *** Usage:
// RM(400, SUNFLOWER)----------- Shovel all sunflowers at 400cs
// RM(400, PUMPKIN, 1, 1)------- Shovel pumpkin at 1-1 (if there is no pumpkin, do nothing)
// RM(400, 1, 1)---------------- Shovel 1-1 (non-pumpkin plants first)
// RM(400, {1, 2, 5, 6}, 9)----- Shovel 1-9, 2-9, 5-9, 6-9 (non-pumpkin plants first)
// RM(after(751), ...)---------- Same usage, taking effect after 751cs
void RM(Time time, PlantType target)
{
    target = _SimpleAvZInternal::non_imitater(target);
    if (target == GRAVE_BUSTER) {
        _SimpleAvZInternal::error("RM", "Cannot shovel grave buster");
    }

    _SimpleAvZInternal::get_effect_time_and_set_time(time, "RM");
    AvZ::InsertOperation([=]() {
        for (auto& p : AvZ::alive_plant_filter) {
            if (p.type() == target) {
                AvZ::ShovelNotInQueue(p.row() + 1, p.col() + 1, target == PUMPKIN);
            }
        }
    },
        "RM");
}

void RM(Time time, PlantType target, int row, int col)
{

    target = _SimpleAvZInternal::non_imitater(target);
    if (target == GRAVE_BUSTER) {
        _SimpleAvZInternal::error("RM", "Cannot shovel grave buster");
    }
    _SimpleAvZInternal::validate_shovel_position(row, col, "RM");

    _SimpleAvZInternal::get_effect_time_and_set_time(time, "RM");
    AvZ::InsertOperation([=]() {
        bool found = false;

        for (auto& p : AvZ::alive_plant_filter) {
            if (p.type() == target && p.row() + 1 == row) {
                if (p.col() + 1 == col)
                    found = true;
                else if (p.type() == COB_CANNON && p.col() + 2 == col)
                    found = true;
            }
        }

        if (found) {
            AvZ::ShovelNotInQueue(row, col, target == PUMPKIN);
        }
    },
        "RM");
}

void RM(Time time, const std::vector<int>& rows, int col)
{
    for (const auto& row : rows) {
        _SimpleAvZInternal::validate_shovel_position(row, col, "RM");
    }

    _SimpleAvZInternal::get_effect_time_and_set_time(time, "RM");
    for (const auto& row : rows) {
        AvZ::Shovel(row, col);
    }
}

void RM(Time time, int row, int col)
{
    RM(time, {{row}}, col);
}

// Use ice in nighttime. Effect time is auto-corrected.
// If effect time is not specified, it defaults to 601cs of the current wave (perfect ice for next wave).
// *** Usage:
// I(1, 2)------------------ Place ice at 1-2, taking effect at 601cs (perfect ice)
// I(after(210), 1, 2)------ Use ice, takeing effect after 210cs (ice3), recommended to be used after activation cobs
// I(359, 1, 2)------------- Use ice, taking effect at 359cs
void I(Time time, int row, int col)
{
    if (!_SimpleAvZInternal::is_night_time()) {
        _SimpleAvZInternal::error("I", "Cannot use nighttime version of I() at daytime\nThere is no need to provide ice position");
    }
    _SimpleAvZInternal::validate_card_position(ICE_SHROOM, row, col, "I");

    auto effect_time = _SimpleAvZInternal::get_card_effect_time(time, {ICE_SHROOM}, "I");
    _SimpleAvZInternal::set_time_inside(effect_time - 100, "I");
    AvZ::Card(ICE_SHROOM, row, col);
}

void I(int row, int col)
{
    I(601, row, col);
}

// Use imitater ice in nighttime. Effect time is auto-corrected.
// If effect time is not specified, it defaults to 601cs of the current wave (perfect ice for next wave).
// *** Usage:
// M_I(1, 2)------------------ Place imitater ice at 1-2, taking effect at 601cs (perfect ice)
// M_I(after(210), 1, 2)------ Use imitater ice, takeing effect after 210cs (ice3), recommended to be used after activation cobs
// M_I(359, 1, 2)------------- Use imitater ice, taking effect at 359cs
void M_I(Time time, int row, int col)
{
    if (!_SimpleAvZInternal::is_night_time()) {
        _SimpleAvZInternal::error("M_I", "M_I is for nighttime only");
    }
    _SimpleAvZInternal::validate_card_position(M_ICE_SHROOM, row, col, "M_I");

    auto effect_time = _SimpleAvZInternal::get_card_effect_time(time, {M_ICE_SHROOM}, "M_I");
    _SimpleAvZInternal::set_time_inside(effect_time - 420, "M_I");
    AvZ::Card(M_ICE_SHROOM, row, col);
    AvZ::SetPlantActiveTime(ICE_SHROOM, 419);
}

void M_I(int row, int col)
{
    M_I(601, row, col);
}

// Use cards. You may provide a single position, multiple rows with the same column, or multiple cards at the same position.
// *** Usage:
// C(359, CHERRY, 2, 9)------------------- Place cherry at 2-9, taking effect at 359cs
// C(400, {PUFF, SUN}, {1, 2}, 9)--------- Place puff-shroom at 1-9 and sun-shroom at 2-9
// C(359, {LILY, DOOM, COFFEE}, 3, 9)----- Place lily pad, doom, and coffee bean at 3-9
//
// *** Alternative forms of card time:
// C(after(110), ...)----------- Same usage, taking effect after 110cs
// C(exact(800), ...)----------- Use card-based time instead of cob-based time, taking effect at 800cs
// C(after(exact(..)), ...)----- Combination of the above two
//
// *** Specify shovel time:
// C(400, keep(266), ...)------- Shovel 266cs after effect time
// C(400, until(1036), ...)----- Shovel at 1036cs
// Note: containers (lily pad, flower pot) will be shoveled as well.
void C(Time time, ShovelTime shovel_time, const std::vector<PlantType>& plant_types, const std::vector<int>& rows, int col)
{
    if (plant_types.empty()) {
        _SimpleAvZInternal::error("C", "Must provide the card to use");
    }
    if (rows.empty()) {
        _SimpleAvZInternal::error("C", "Must provide which row to use the card");
    }
    if (plant_types.size() != rows.size()) {
        _SimpleAvZInternal::error("C", "Number of cards and number of rows must be the same\nNumber of cards: #\nNumber of rows: #", plant_types.size(), rows.size());
    }
    for (int i = 0; i < plant_types.size(); i++) {
        auto plant_type = plant_types.at(i);
        auto row = rows.at(i);
        _SimpleAvZInternal::validate_card_position(plant_type, row, col, "C");
    }

    auto effect_time = _SimpleAvZInternal::get_card_effect_time(time, plant_types, "C");

    for (int i = 0; i < plant_types.size(); i++) {
        auto plant_type = plant_types.at(i);
        auto row = rows.at(i);

        auto prep_time = _SimpleAvZInternal::get_prep_time(plant_type);

        _SimpleAvZInternal::set_time_inside(effect_time - prep_time, "C");
        AvZ::Card(plant_type, row, col);
        if (_SimpleAvZInternal::get_set_active_time_flag(plant_type))
            AvZ::SetPlantActiveTime(_SimpleAvZInternal::non_imitater(plant_type), prep_time - 1);

        if (shovel_time.type != ShovelTime::Type::NONE) {
            if (shovel_time.type == ShovelTime::Type::KEEP)
                _SimpleAvZInternal::shovel_with_container(effect_time + shovel_time.time, plant_type, row, col, "C");
            else if (shovel_time.type == ShovelTime::Type::UNTIL)
                _SimpleAvZInternal::shovel_with_container(shovel_time.time, plant_type, row, col, "C");
        }
    }
}

void C(Time time, const std::vector<PlantType>& plant_types, const std::vector<int>& rows, int col)
{
    C(time, ShovelTime(), plant_types, rows, col);
}

void C(Time time, ShovelTime shovel_time, const std::vector<PlantType>& plant_types, int row, int col)
{
    if (plant_types.empty()) {
        _SimpleAvZInternal::error("C", "Must provide the card to use");
    }
    for (const auto& plant_type : plant_types) {
        _SimpleAvZInternal::validate_card_position(plant_type, row, col, "C");
    }

    auto effect_time = _SimpleAvZInternal::get_card_effect_time(time, plant_types, "C");
    auto prep_times = _SimpleAvZInternal::get_prep_times(plant_types);
    auto set_active_time_flags = _SimpleAvZInternal::get_set_active_time_flags(plant_types);

    for (int i = 0; i < plant_types.size(); i++) {
        auto plant_type = plant_types.at(i);
        auto prep_time = prep_times.at(i);
        auto set_active_time_flag = set_active_time_flags.at(i);

        _SimpleAvZInternal::set_time_inside(effect_time - prep_time, "C");
        AvZ::Card(plant_type, row, col);
        if (set_active_time_flag)
            AvZ::SetPlantActiveTime(_SimpleAvZInternal::non_imitater(plant_type), prep_time - 1);

        if (shovel_time.type != ShovelTime::Type::NONE) {
            if (shovel_time.type == ShovelTime::Type::KEEP)
                _SimpleAvZInternal::shovel_with_container(effect_time + shovel_time.time, plant_type, row, col, "C");
            else if (shovel_time.type == ShovelTime::Type::UNTIL)
                _SimpleAvZInternal::shovel_with_container(shovel_time.time, plant_type, row, col, "C");
        }
    }
}

void C(Time time, const std::vector<PlantType>& plant_types, int row, int col)
{
    C(time, ShovelTime(), plant_types, row, col);
}

void C(Time time, ShovelTime shovel_time, PlantType plant_type, int row, int col)
{
    C(time, shovel_time, {{plant_type}}, {{row}}, col);
}

void C(Time time, PlantType plant_type, int row, int col)
{
    C(time, ShovelTime(), {{plant_type}}, {{row}}, col);
}

// Decide whether or not to use cards depending on zombies.
// You may either check if a certain type of zombie exists, or check if certain types of zombie's x is smaller than some threshold.
// *** Usage:
// C_IF(exist(ZOMBONI), 400, SPIKEWEED, 1, 9)------- If there is zomboni in row 1, place spikeweed at 1-9 at 400cs
// C_IF(pos({GARG, GIGA}, 680), 400, POT, 1, 8)----- If there is garg or giga with int(x)≤680, place flower pot at 1-8 at 400cs
void C_IF(const std::function<bool(int)>& condition, Time time, ShovelTime shovel_time, const PlantType& plant_type, int row, int col)
{
    _SimpleAvZInternal::validate_card_position(plant_type, row, col, "C");

    auto effect_time = _SimpleAvZInternal::get_card_effect_time(time, {{plant_type}}, "C_IF");
    auto prep_time = _SimpleAvZInternal::get_prep_time(plant_type);
    auto set_active_time_flag = _SimpleAvZInternal::get_set_active_time_flag(plant_type);

    _SimpleAvZInternal::set_time_inside(effect_time - prep_time, "C_IF");
    AvZ::InsertOperation([=]() {
        if (condition(row)) {
            AvZ::SetNowTime();
            AvZ::Card(plant_type, row, col);
            if (set_active_time_flag)
                AvZ::SetPlantActiveTime(_SimpleAvZInternal::non_imitater(plant_type), prep_time - 1);
            if (shovel_time.type != ShovelTime::Type::NONE) {
                if (shovel_time.type == ShovelTime::Type::KEEP)
                    _SimpleAvZInternal::shovel_with_container(effect_time + shovel_time.time, plant_type, row, col, "C_IF");
                else if (shovel_time.type == ShovelTime::Type::UNTIL)
                    _SimpleAvZInternal::shovel_with_container(shovel_time.time, plant_type, row, col, "C_IF");
            }
        }
    },
        "C_IF");
}

void C_IF(const std::function<bool(int)>& condition, Time time, const PlantType& plant_type, int row, int col)
{
    C_IF(condition, time, ShovelTime(), plant_type, row, col);
}

std::function<bool(int)> exist(const std::vector<ZombieType>& zombie_types)
{
    return [=](int row) {
        for (auto& z : AvZ::alive_zombie_filter) {
            if (z.row() + 1 == row && _SimpleAvZInternal::contains(zombie_types, (ZombieType)z.type()))
                return true;
        }
        return false;
    };
}

std::function<bool(int)> exist(const ZombieType& zombie_type)
{
    return exist({{zombie_type}});
}

std::function<bool(int)> pos(const std::vector<ZombieType>& zombie_types, int x)
{
    return [=](int row) {
        for (auto& z : AvZ::alive_zombie_filter) {
            if (z.row() + 1 == row && _SimpleAvZInternal::contains(zombie_types, (ZombieType)z.type()) && static_cast<int>(z.abscissa()) <= x)
                return true;
        }
        return false;
    };
}

std::function<bool(int)> pos(const ZombieType& zombie_type, int x)
{
    return pos({{zombie_type}}, x);
}