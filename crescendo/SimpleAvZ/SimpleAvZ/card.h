#pragma once

#include "error.h"
#include "global.h"
#include "libavz.h"
#include "time.h"
#include "util.h"

namespace _SimpleAvZInternal {

PlantType non_imitater(const PlantType& plant_type)
{
    return (plant_type > IMITATOR) ? static_cast<PlantType>(plant_type - IMITATOR - 1) : plant_type;
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
void shovel_with_container(int time, PlantType target, int row, int col, const std::string& func_name,
    bool usable_outside_loop = false)
{
    bool is_imitater = target > IMITATOR;
    target = non_imitater(target);
    if (usable_outside_loop) {
        AvZ::SetTime(time, AvZ::GetRunningWave());
    } else {
        set_time_inside(time, func_name);
    }
    AvZ::InsertOperation([=]() {
        bool shovel_container = true;
        int container_num = 0;
        for (auto& p : AvZ::alive_plant_filter) {
            if (p.row() + 1 == row) {
                if (p.col() + 1 == col || (p.col() == col && p.type() == COB_CANNON)) {
                    if ((p.type() == target) || (p.type() == IMITATOR && is_imitater))
                        AvZ::ShovelNotInQueue(row, static_cast<float>(col), target == PUMPKIN);
                    else if (p.type() == LILY_PAD || p.type() == FLOWER_POT)
                        container_num++;
                    else
                        shovel_container = false;
                }
            }
        }
        if (shovel_container)
            for (int i = 0; i < container_num; i++)
                AvZ::ShovelNotInQueue(row, static_cast<float>(col));
    },
        func_name + "-->shovel_with_container");
}

std::set<PlantType> get_invalid_plants(int row, const std::string& func_name)
{
    switch (AvZ::GetMainObject()->scene()) {
    case 0:
    case 7:  // 禅境花园
    case 9:  // 智慧树
    case 10: // D6E
        return {GRAVE_BUSTER, LILY_PAD, TANGLE_KELP, SEA_SHROOM, CATTAIL};
    case 1:
    case 11: // N6E
        return {LILY_PAD, TANGLE_KELP, SEA_SHROOM, COFFEE_BEAN, CATTAIL};
    case 6: // 蘑菇园
        return {GRAVE_BUSTER, LILY_PAD, TANGLE_KELP, SEA_SHROOM, COFFEE_BEAN, CATTAIL};
    case 2:
        if (row == 3 || row == 4) {
            return {POTATO_MINE, GRAVE_BUSTER, SPIKEWEED, FLOWER_POT, SPIKEROCK};
        } else {
            return {GRAVE_BUSTER, LILY_PAD, TANGLE_KELP, SEA_SHROOM, CATTAIL};
        }
    case 3:
    case 8: // 水族馆
        if (row == 3 || row == 4) {
            return {POTATO_MINE, GRAVE_BUSTER, SPIKEWEED, FLOWER_POT, COFFEE_BEAN, SPIKEROCK};
        } else {
            return {GRAVE_BUSTER, LILY_PAD, TANGLE_KELP, SEA_SHROOM, COFFEE_BEAN, CATTAIL};
        }
    case 4:
        return {GRAVE_BUSTER, LILY_PAD, TANGLE_KELP, SPIKEWEED, SEA_SHROOM, CATTAIL, SPIKEROCK};
    case 5:
        return {GRAVE_BUSTER, LILY_PAD, TANGLE_KELP, SPIKEWEED, SEA_SHROOM, COFFEE_BEAN, CATTAIL, SPIKEROCK};
    default:
        error(func_name, "不支持的场景: #", AvZ::GetMainObject()->scene());
        return {};
    }
}

void validate_card_position(const PlantType& plant_type, int row, int col, const std::string& func_name)
{
    int max_row = is_visually_six_rows() ? 6 : 5;
    if (row < 1 || row > max_row) {
        error(func_name, "用卡行应在1~#内\n用卡行: #", max_row, row);
    }
    if (col < 1 || col > 9) {
        error(func_name, "用卡列应在1~9内\n用卡列: #", col);
    }
    if (get_invalid_plants(row, func_name).count(non_imitater(plant_type))) {
        error(func_name, "当前场景不可在此行使用此卡片\n卡片: #\n用卡行: #", non_imitater(plant_type), row);
    }
}

void validate_shovel_position(int row, int col, const std::string& func_name)
{
    int max_row = is_visually_six_rows() ? 6 : 5;
    if (row < 1 || row > max_row) {
        error(func_name, "铲除行应在1~#内\n铲除行: #", max_row, row);
    }
    if (col < 1 || col > 9) {
        error(func_name, "铲除列应在1~9内\n铲除列: #", col);
    }
}

} // namespace _SimpleAvZInternal

// 铲除植物. 可提供单一坐标, 多行同列. 也可指定要铲除的植物种类.
// *** 使用示例:
// RM(400, SUNFLOWER)-----------于400cs铲除场地上所有小向
// RM(400, PUMPKIN, 1, 1)-------铲除1-1南瓜（没有则不铲）
// RM(400, 1, 1)----------------铲除1-1, 优先铲除非南瓜
// RM(400, {1, 2, 5, 6}, 9)-----铲除1,2,5,6路9列, 优先铲除非南瓜
// RM(after(751), ...)----------用法同上, 延迟751cs生效
void RM(Time time, PlantType target)
{
    target = _SimpleAvZInternal::non_imitater(target);
    if (target == GRAVE_BUSTER) {
        _SimpleAvZInternal::error("RM", "墓碑吞噬者无法铲除");
    }

    _SimpleAvZInternal::get_effect_time_and_set_time(time, "RM");
    AvZ::InsertOperation([=]() {
        for (auto& p : AvZ::alive_plant_filter) {
            if (p.type() == target) {
                AvZ::ShovelNotInQueue(p.row() + 1, static_cast<float>(p.col() + 1), target == PUMPKIN);
            }
        }
    },
        "RM");
}

void RM(Time time, PlantType target, int row, int col)
{

    target = _SimpleAvZInternal::non_imitater(target);
    if (target == GRAVE_BUSTER) {
        _SimpleAvZInternal::error("RM", "墓碑吞噬者无法铲除");
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
            AvZ::ShovelNotInQueue(row, static_cast<float>(col), target == PUMPKIN);
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
        AvZ::Shovel(row, static_cast<float>(col));
    }
}

void RM(Time time, int row, int col)
{
    RM(time, {{row}}, col);
}

// 夜间用原版冰. 自带生效时机修正.
// 若不指定生效时间, 默认在本波 601cs 生效.
// *** 使用示例:
// I(1, 2)------------------于1-2放置原版冰, 601cs生效(完美预判冰)
// I(after(210), 1, 2)------延迟210cs生效(ice3), 推荐在激活炮后使用
// I(359, 1, 2)-------------359cs生效
void I(Time time, int row, int col)
{
    if (!_SimpleAvZInternal::is_night_time()) {
        _SimpleAvZInternal::error("I", "不可在白昼使用夜间版本的I()\n无需提供用冰位置");
    }
    _SimpleAvZInternal::validate_card_position(ICE_SHROOM, row, col, "I");

    auto effect_time = _SimpleAvZInternal::get_card_effect_time(time, {ICE_SHROOM}, "I");
    _SimpleAvZInternal::set_time_inside(effect_time - 100, "I");
    AvZ::Card(ICE_SHROOM, row, static_cast<float>(col));
}

void I(int row, int col)
{
    I(601, row, col);
}

// 夜间用复制冰. 自带生效时机修正.
// 若不指定生效时间, 默认在本波 601cs 生效.
// *** 使用示例:
// M_I(1, 2)------------------于1-2放置复制冰, 601cs生效(完美预判冰)
// M_I(after(210), 1, 2)------延迟210cs生效(ice3), 推荐在激活炮后使用
// M_I(359, 1, 2)-------------359cs生效
void M_I(Time time, int row, int col)
{
    if (!_SimpleAvZInternal::is_night_time()) {
        _SimpleAvZInternal::error("M_I", "M_I为夜间复制冰函数, 不可在白昼使用");
    }
    _SimpleAvZInternal::validate_card_position(M_ICE_SHROOM, row, col, "M_I");

    auto effect_time = _SimpleAvZInternal::get_card_effect_time(time, {M_ICE_SHROOM}, "M_I");
    _SimpleAvZInternal::set_time_inside(effect_time - 420, "M_I");
    AvZ::Card(M_ICE_SHROOM, row, static_cast<float>(col));
    AvZ::SetPlantActiveTime(ICE_SHROOM, 419);
}

void M_I(int row, int col)
{
    M_I(601, row, col);
}

// 用卡. 可提供单一坐标, 多行同列, 或多卡同坐标.
// *** 使用示例:
// C(359, CHERRY, 2, 9)-------------------于2-9放置樱桃, 359cs生效
// C(400, {PUFF, SUN}, {1, 2}, 9)---------于1-9放置小喷, 2-9放置阳光菇
// C(359, {LILY, DOOM, COFFEE}, 3, 9)-----于3-9放置荷叶, 核武, 咖啡豆
//
// *** 生效时间的变种:
// C(after(110), ...)-----------用法同上, 延迟110cs生效
// C(exact(800), ...)-----------不使用炮等效时间, 800cs生效
// C(after(exact(..)), ...)-----以上两者的结合
//
// *** 指定铲除时机:
// C(400, keep(266), ...)-------放置后266cs铲
// C(400, until(1036), ...)-----1036cs铲
// 注意: 容器会被一并铲除.
void C(Time time, ShovelTime shovel_time, const std::vector<PlantType>& plant_types, const std::vector<int>& rows, int col)
{
    if (plant_types.empty()) {
        _SimpleAvZInternal::error("C", "要用的卡片不可为空");
    }
    if (rows.empty()) {
        _SimpleAvZInternal::error("C", "用卡行数不可为空");
    }
    if (plant_types.size() != rows.size()) {
        _SimpleAvZInternal::error("C", "卡片数与行数不一致\n卡片数: #\n行数: #", plant_types.size(), rows.size());
    }
    for (size_t i = 0; i < plant_types.size(); i++) {
        auto plant_type = plant_types.at(i);
        auto row = rows.at(i);
        _SimpleAvZInternal::validate_card_position(plant_type, row, col, "C");
    }

    auto effect_time = _SimpleAvZInternal::get_card_effect_time(time, plant_types, "C");

    for (size_t i = 0; i < plant_types.size(); i++) {
        auto plant_type = plant_types.at(i);
        auto row = rows.at(i);

        auto prep_time = _SimpleAvZInternal::get_prep_time(plant_type);

        _SimpleAvZInternal::set_time_inside(effect_time - prep_time, "C");
        AvZ::Card(plant_type, row, static_cast<float>(col));
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
        _SimpleAvZInternal::error("C", "要用的卡片不可为空");
    }
    for (const auto& plant_type : plant_types) {
        _SimpleAvZInternal::validate_card_position(plant_type, row, col, "C");
    }

    auto effect_time = _SimpleAvZInternal::get_card_effect_time(time, plant_types, "C");
    auto prep_times = _SimpleAvZInternal::get_prep_times(plant_types);
    auto set_active_time_flags = _SimpleAvZInternal::get_set_active_time_flags(plant_types);

    for (size_t i = 0; i < plant_types.size(); i++) {
        auto plant_type = plant_types.at(i);
        auto prep_time = prep_times.at(i);
        auto set_active_time_flag = set_active_time_flags.at(i);

        _SimpleAvZInternal::set_time_inside(effect_time - prep_time, "C");
        AvZ::Card(plant_type, row, static_cast<float>(col));
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

// 智能用卡. 根据本行僵尸情况决定是否用卡.
// 提供"僵尸是否存在", 以及"僵尸x是否小于某值"两种判断方式.
// *** 使用示例:
// C_IF(exist(ZOMBONI), 400, SPIKEWEED, 1, 9)-------若本行存在冰车, 于400cs在1-9放置地刺
// C_IF(pos({GARG, GIGA}, 680), 400, POT, 1, 8)-----若本行存在int(x)≤680的白眼或红眼, 于400cs在1-8放置花盆
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
            AvZ::Card(plant_type, row, static_cast<float>(col));
            if (set_active_time_flag)
                AvZ::SetPlantActiveTime(_SimpleAvZInternal::non_imitater(plant_type), prep_time - 1);
            if (shovel_time.type != ShovelTime::Type::NONE) {
                if (shovel_time.type == ShovelTime::Type::KEEP)
                    _SimpleAvZInternal::shovel_with_container(effect_time + shovel_time.time, plant_type, row, col, "C_IF", true);
                else if (shovel_time.type == ShovelTime::Type::UNTIL)
                    _SimpleAvZInternal::shovel_with_container(shovel_time.time, plant_type, row, col, "C_IF", true);
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
            if (z.row() + 1 == row && _SimpleAvZInternal::contains(zombie_types, static_cast<ZombieType>(z.type())))
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
            if (z.row() + 1 == row && _SimpleAvZInternal::contains(zombie_types, static_cast<ZombieType>(z.type())) && static_cast<int>(z.abscissa()) <= x)
                return true;
        }
        return false;
    };
}

std::function<bool(int)> pos(const ZombieType& zombie_type, int x)
{
    return pos({{zombie_type}}, x);
}