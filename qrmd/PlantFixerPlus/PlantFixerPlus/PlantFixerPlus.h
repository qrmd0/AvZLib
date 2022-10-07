/*
 * @Author: qrmd
 * @Date: 2022-08-13 13:20:20
 * @LastEditors: qrmd
 * @LastEditTime: 2022-10-07 20:37:29
 * @Description:PlantFixer的增强版本，支持紫卡修补、咖啡豆修补等功能
 * 使用方法：1、将本文件粘贴到AsmVsZombies/inc；
 *          2、在要应用此库的脚本开头添加 #include "PlantFixerPlus.h" ；
 *          3、在README与本库的代码注释中查看函数与变量的用法。
 * 来自AvZLib公开插件仓库：
 * 主库：https://github.com/qrmd0/AvZLib
 * 镜像库1：https://gitee.com/qrmd/AvZLib
 * 镜像库2：https://gitlab.com/avzlib/AvZLib
 * Copyright (c) 2022 by qrmd, All Rights Reserved.
 */
#ifndef __PlantFixerPlus
#define __PlantFixerPlus
#include "avz.h"

using namespace AvZ;

class PlantFixerPlus : public TickRunner {
private:
    // 待修补的植物种类
    int _type;
    // 待修补植物的格子位置列表
    std::vector<Grid> _grids;
    // 修补血量阈值
    double _fix_hp_threshold;
    // 种子序号列表
    std::vector<int> _seed_index_list = {-1, -1, -1};
    // 是否在战斗开始时检查是否携带待修补植物的种子
    bool _is_check_cards = true;
    // 是否同时使用模仿种子执行修补
    bool _is_use_imitator_seed = true;
    // 是否不打断手动操作
    bool _is_not_interrupt = true;
    // 是否不修补上面有植物的睡莲叶或花盆
    bool _is_not_fix_lily_pad_or_flower_pot_with_plant = true;
    // 修补阳光阈值
    int _fix_sun_threshold = 0;
    // 执行修补判断与操作的时间间隔（厘秒）
    int _run_interval = 1;

    // 定时执行的修补判断与操作
    void _Run();

    // 返回[type]植物是否为升级植物
    bool _GetIsUpdateType(int type);

    // 返回[type]种类植物的种子序号，如果未找到则返回 -1
    int _GetSeedIndex(int type);

    // 返回[type]种类植物的模仿种类，[type]为一个属于[0, 47]的整数
    int _GetImitatorType(int type);

    // 如果[type]植物为升级植物，返回其前置植物种类，否则返回[type]
    int _GetPrePlantType(int type);

    // 返回[plant_type]的[seed_index]号种子是否可用，升级植物的阳光花费考虑生存模式的递增
    bool _GetIsSeedUsable(int plant_type, int seed_index);

    // 设置待修补植物的格子位置列表为场上的此类植物的格子位置列表
    void _AutoSetGrids();

    // 返回位于[row]行[col]列的玉米加农炮距离下一次可用的时间（厘秒）
    int _GetCannonRecoverTime(int row, int col);

    // 铲除[row]行[col]列的[type]植物
    void _ShovelThePlant(int row, int col, int type);

public:
    // *** In Queue
    // 启用自动修补
    // [type]为植物种类，填入一个属于[0, 47]的整数
    // [grids]为待修补的格子位置列表，不填或为空时自动获取场上的[type]植物的格子位置列表
    // [fix_threshold]为修补血量阈值，植物血量不高于此值时触发修补操作，小于 1 的值被视为最大血量的比例，不填时为 0.9999
    // [is_use_imitator_seed]为是否同时使用模仿种子修补，不填时为 是
    // *** 使用示例：
    // start(PUMPKIN, {}, 1200, false) // 修补全场的南瓜头，血量阈值为1200，不使用模仿种子
    // start(GLOOM_SHROOM) // 修补全场的忧郁菇，血量阈值为299，同时使用模仿种子
    // start(COB_CANNON, {{1, 1}, {2, 1}}, 0.3)------修补位于{1，1}，{2，1}的玉米加农炮，血量阈值为90，同时使用模仿种子
    void start(int type, std::vector<Grid> grids = {}, float fix_threshold = 0.9999, bool is_use_imitator_seed = true);

    // *** Not In Queue
    // 设置是否在战斗开始时检查是否携带待修补植物的种子，默认为 是
    void setIsCheckCards(bool is_check_cards);

    // *** In Queue
    // 设置是否不打断手动操作，默认为 是
    void setIsNotInterrupt(bool is_not_interrupt);

    // *** In Queue
    // 设置是否不修补上面有植物的睡莲叶或花盆
    void setIsNotFixLilyPadOrFlowerPot(bool is_not_fix_lily_pad_or_flower_pot_with_plant);

    // *** In Queue
    // 设置修补阳光阈值，阳光数量小于此值时不修补，默认为 0
    void setFixSunThreshold(unsigned int fix_sun_threshold);

    // *** In Queue
    // 设置执行修补判断的时间间隔（厘秒），默认为 1
    void setRunInterval(unsigned int run_interval);

    // *** In Queue
    // 重置植物修补位置
    void resetFixList(std::vector<Grid> grids);

    // *** In Queue
    // 重置修补血量阈值
    void resetFixHpThreshold(unsigned int fix_hp_threshold);

} plant_fixer_plus;

void PlantFixerPlus::_Run()
{
    // 定时启用
    if (GetMainObject()->globalClock() % _run_interval != 0)
        return;

    // 不打断手动操作
    if (_is_not_interrupt && GetMainObject()->mouseAttribution()->type() != 0)
        return;

    // 阳光数量小于阈值时不修补
    if (GetMainObject()->sun() < _fix_sun_threshold)
        return;

    // 升级植物不可用时不修补
    if (_GetIsUpdateType(_type) && !_GetIsSeedUsable(_type, _seed_index_list[2]))
        return;

    // 确定待修补植物血量最低的格子位置列表
    std::vector<Grid> min_hp_grids;
    int min_hp = 2147483647;
    std::vector<int> grids_hp;
    int plant_index;
    auto plants = GetMainObject()->plantArray();
    for (Grid each : _grids) {
        plant_index = GetPlantIndex(each.row, each.col, _type);
        grids_hp.emplace_back(plant_index >= 0 ? plants[plant_index].hp() : 0);
    }
    min_hp = *std::min_element(grids_hp.begin(), grids_hp.end());
    for (int i = 0; i < grids_hp.size(); ++i) {
        if (grids_hp[i] == min_hp)
            min_hp_grids.emplace_back(_grids[i]);
    }

    for (Grid each_min_hp_grid : min_hp_grids) {
        // 睡莲叶或花盆上有植物时不对其修补
        if (_is_not_fix_lily_pad_or_flower_pot_with_plant && (_type == LILY_PAD || _type == FLOWER_POT)) {
            if (GetPlantIndex(each_min_hp_grid.row, each_min_hp_grid.col) >= 0 || GetPlantIndex(each_min_hp_grid.row, each_min_hp_grid.col, PUMPKIN) >= 0)
                return;
        }
        // 玉米加农炮即将可用或正在发射且炮弹未出膛时不对其修补
        if (_type == COB_CANNON && GetPlantIndex(each_min_hp_grid.row, each_min_hp_grid.col, COB_CANNON) >= 0) {
            int cannon_recover_time = _GetCannonRecoverTime(each_min_hp_grid.row, each_min_hp_grid.col);
            if (cannon_recover_time <= 625 + 751 + _run_interval || cannon_recover_time >= 3475 - (205 + 1))
                return;
        }
        // 修补判断
        std::vector<Grid> fix_grids = {{each_min_hp_grid.row, each_min_hp_grid.col}};
        if (_type == COB_CANNON) {
            Grid temp = {each_min_hp_grid.row, each_min_hp_grid.col + 1};
            fix_grids.emplace_back(temp);
        }
        auto seeds = GetMainObject()->seedArray();
        int seed_type;
        int plant_type;
        if (min_hp <= _fix_hp_threshold) {
            for (Grid each_grid : fix_grids) {
                for (int each_seed_index : _seed_index_list) {
                    if (each_seed_index >= 0) {
                        seed_type = seeds[each_seed_index].type();
                        plant_type = seed_type == IMITATOR ? _GetPrePlantType(_type) : seed_type;
                        if (_GetIsSeedUsable(plant_type, each_seed_index)) {
                            _ShovelThePlant(each_grid.row, each_grid.col, _type);
                            if (Asm::getPlantRejectType(plant_type, each_grid.row - 1, each_grid.col - 1) == Asm::NIL) {
                                CardNotInQueue(each_seed_index + 1, each_grid.row, each_grid.col);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}
bool PlantFixerPlus::_GetIsUpdateType(int type)
{
    return type >= GATLING_PEA && type <= COB_CANNON;
}
int PlantFixerPlus::_GetSeedIndex(int type)
{
    return type > IMITATOR ? GetSeedIndex(type - (IMITATOR + 1), true) : GetSeedIndex(type, false);
}
int PlantFixerPlus::_GetImitatorType(int type)
{
    return type + (IMITATOR + 1);
}
int PlantFixerPlus::_GetPrePlantType(int type)
{
    std::map<int, int> pre_plant_map = {{GATLING_PEA, REPEATER}, {TWIN_SUNFLOWER, SUNFLOWER}, {GLOOM_SHROOM, FUME_SHROOM}, {CATTAIL, LILY_PAD}, {WINTER_MELON, MELON_PULT}, {GOLD_MAGNET, MAGNET_SHROOM}, {SPIKEROCK, SPIKEWEED}, {COB_CANNON, KERNEL_PULT}};
    return _GetIsUpdateType(type) ? pre_plant_map[type] : type;
}
bool PlantFixerPlus::_GetIsSeedUsable(int plant_type, int seed_index)
{
    auto seeds = GetMainObject()->seedArray();
    bool is_usable = seeds[seed_index].isUsable();
    int sun_cost = *(int*)(0x69F2C0 + plant_type * 0x24);
    int plant_count = 0;
    if (plant_type >= GATLING_PEA && plant_type <= COB_CANNON) {
        for (auto&& plant : alive_plant_filter) {
            if (plant->type() == plant_type)
                plant_count += 1;
        }
        sun_cost += plant_count * 50;
    }
    return is_usable && sun_cost <= GetMainObject()->sun();
}

void PlantFixerPlus::_AutoSetGrids()
{
    std::vector<Grid> vec;
    Grid temp;
    for (auto&& plant : alive_plant_filter) {
        if (plant->type() == _type) {
            temp = {plant->row(), plant->col()};
            vec.emplace_back(temp);
        }
    }
    _grids = vec;
}

int PlantFixerPlus::_GetCannonRecoverTime(int row, int col)
{
    int cannon_index = GetPlantIndex(row, col, COB_CANNON);
    if (!(cannon_index >= 0))
        return -1;
    auto cannon = GetMainObject()->plantArray() + cannon_index;
    auto cannon_animation = GetPvzBase()->animationMain()->animationOffset()->animationArray() + cannon->animationCode();
    switch (cannon->state()) {
    case 35:
        return 125 + cannon->stateCountdown();
    case 36:
        return int(125 * (1 - cannon_animation->circulationRate()) + 0.5) + 1;
    case 37:
        return 0;
    case 38:
        return 3125 + int(350 * (1 - cannon_animation->circulationRate()) + 0.5);
    default:
        return -1;
    }
}

void PlantFixerPlus::_ShovelThePlant(int row, int col, int type)
{
    if (type == COFFEE_BEAN)
        return;
    if (GetPlantIndex(row, col, type) >= 0) {
        if (type == PUMPKIN)
            ShovelNotInQueue(row, col, true);
        else
            ShovelNotInQueue(row, col, false);
    }
}
void PlantFixerPlus::start(int type, std::vector<Grid> grids, float fix_threshold, bool is_use_imitator_seed)
{
    // 参数合法性检查
    // 植物种类合法性检查
    if (!(type >= PEASHOOTER && type <= COB_CANNON)) {
        ShowErrorNotInQueue("PlantFixerPlus:\n  非法的植物种类 # ，请填入一个属于[0, 47]的整数", type);
        return;
    }
    // 格子位置行数合法性检查
    for (Grid each : grids) {
        // 泳池或雾夜为六行场地
        if (GetMainObject()->scene() == 2 || GetMainObject()->scene() == 3) {
            if (!(each.row >= 1 && each.row <= 6)) {
                ShowErrorNotInQueue("PlantFixerPlus:\n  非法的格子位置行数 # ，请填入一个属于[1, 6]的整数", each.row);
                return;
            }
        } else {
            if (!(each.row >= 1 && each.row <= 5)) {
                ShowErrorNotInQueue("PlantFixerPlus:\n  非法的格子位置行数 # ，请填入一个属于[1, 5]的整数", each.row);
                return;
            }
        }
    }
    // 格子位置列数合法性检查
    if (type == COB_CANNON) {
        for (Grid each : grids) {
            if (!(each.col >= 1 && each.col <= 8)) {
                ShowErrorNotInQueue("PlantFixerPlus:\n  非法的格子位置列数 # ，请填入一个属于[1, 8]的整数，注意玉米加农炮的位置判定为后轮（远离眼睛的一侧）所在的格子", each.row);
                return;
            }
        }
    } else {
        for (Grid each : grids) {
            if (!(each.col >= 1 && each.col <= 9)) {
                ShowErrorNotInQueue("PlantFixerPlus:\n  非法的格子位置列数 # ，请填入一个属于[1, 9]的整数", each.row);
                return;
            }
        }
    }
    InsertOperation([=]() {
        _type = type;
        // 获取种子序号数组
        // 种子序号或其前置植物种子的序号
        _seed_index_list[0] = _GetSeedIndex(_GetPrePlantType(_type));
        // 模仿种子序号或其前置植物模仿种子的序号
        _seed_index_list[1] = _is_use_imitator_seed ? _GetSeedIndex(_GetImitatorType(_GetPrePlantType(_type))) : -1;
        // 升级植物的种子序号
        _seed_index_list[2] = _GetIsUpdateType(type) ? _GetSeedIndex(_type) : -1;
        // 卡槽可用性检查
        if (_is_check_cards && (_seed_index_list[0] == -1 && _seed_index_list[1] == -1 && _seed_index_list[2] == -1)) {
            ShowErrorNotInQueue("PlantFixerPlus:\n    由于您未携带种类代号为 # 的植物的相关种子，无法执行修补操作。\n    您可以在void Scrpit()中执行 setIsCheckCards(false) 方法以关闭此提示", _type);
            return;
        }
        _grids = grids;
        if (_grids.empty())
            _AutoSetGrids();

        if (fix_threshold > 1) {
            _fix_hp_threshold = int(fix_threshold);
        } else {
            const std::vector<int> max_hps = {300, 300, 300, 4000, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 8000, 300, 300, 300, 300, 300, 300, 4000, 300, 300, 300, 300, 300, 400, 300, 300, 300, 300, 300, 300, 300, 300, 300, 450, 300};
            _fix_hp_threshold = int(max_hps[_type] * fix_threshold);
        }
        _is_use_imitator_seed = is_use_imitator_seed;
        pushFunc([=]() { _Run(); });
    },
        "PlantFixerPlus started");
}
void PlantFixerPlus::setIsCheckCards(bool is_check_cards)
{
    _is_check_cards = is_check_cards;
}
void PlantFixerPlus::setIsNotInterrupt(bool is_not_interrupt)
{
    InsertOperation([=]() { _is_not_interrupt = is_not_interrupt; }, "setIsNotInterrupt:" + std::to_string(is_not_interrupt));
}
void PlantFixerPlus::setIsNotFixLilyPadOrFlowerPot(bool is_not_fix_lily_pad_or_flower_pot_with_plant)
{
    InsertOperation([=]() { _is_not_fix_lily_pad_or_flower_pot_with_plant = is_not_fix_lily_pad_or_flower_pot_with_plant; }, "setIsNotFixLilyPadOrFlowerPot:" + std::to_string(is_not_fix_lily_pad_or_flower_pot_with_plant));
}
void PlantFixerPlus::setFixSunThreshold(unsigned int fix_sun_threshold)
{
    InsertOperation([=]() { _fix_sun_threshold = fix_sun_threshold; }, "setFixSunThreshold:" + std::to_string(fix_sun_threshold));
}
void PlantFixerPlus::setRunInterval(unsigned int run_interval)
{
    InsertOperation([=]() { _run_interval = run_interval; }, "setRunInterval:" + std::to_string(run_interval));
}
void PlantFixerPlus::resetFixList(std::vector<Grid> grids)
{
    InsertOperation([=]() { _grids = grids; }, "resetFixList");
}
void PlantFixerPlus::resetFixHpThreshold(unsigned int fix_hp_threshold)
{
    InsertOperation([=]() { _fix_hp_threshold = fix_hp_threshold; }, "resetFixHp:" + std::to_string(fix_hp_threshold));
}

#endif
