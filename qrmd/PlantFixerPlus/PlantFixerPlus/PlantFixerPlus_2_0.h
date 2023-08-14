/*
 * @Author: qrmd
 * @Date: 2022-08-13 13:20:20
 * @LastEditors: qrmd
 * @LastEditTime: 2023-08-14 20:31:23
 * @Description:PlantFixer的增强版本，支持紫卡修补、咖啡豆修补等功能
 * 来自AvZLib公开插件仓库：
 * 主库：https://github.com/qrmd0/AvZLib
 * 镜像库1：https://gitee.com/qrmd/AvZLib
 * 镜像库2：https://gitlab.com/avzlib/AvZLib
 * Copyright (c) 2022 by qrmd.
 */
#ifndef __PLANTFIXERPLUS_2_0__
#define __PLANTFIXERPLUS_2_0__

#include "avz.h"

ALogger<AMsgBox> _msgboxLogger;

class APlantFixerPlus : public ATickRunner {
private:
    // 待修补的植物种类
    int _type;
    // 待修补植物的格子位置列表
    std::vector<AGrid> _grids;
    // 修补血量阈值
    double _fixHpThreshold;
    // 种子序号列表
    std::vector<int> _seedIndexList = {-1, -1, -1};
    // 是否在战斗开始时检查是否携带待修补植物的种子
    bool _isCheckCards = true;
    // 是否同时使用模仿种子执行修补
    bool _isUseImitatorSeed = true;
    // 是否不打断手动操作
    bool _isNotInterrupt = true;
    // 是否不修补上面有植物的睡莲叶或花盆
    bool _isNotFixLilyPadOrFlowerPotWithPlant = true;
    // 修补阳光阈值
    int _fixSunThreshold = 0;
    // 执行修补判断与操作的时间间隔（厘秒）
    int _runInterval = 1;
    // 是否完成参数初始化
    bool _isParamsInitialized = false;
    // 定时执行的修补判断与操作
    void _Run();

    // 返回[type]植物是否为升级植物
    inline bool _GetIsUpdateType(const int& type);

    // 返回[type]种类植物的种子序号，如果未找到则返回 -1
    inline int _GetSeedIndex(const int& type);

    // 返回[type]种类植物的模仿种类，[type]为一个属于[0, 47]的整数
    inline int _GetImitatorType(const int& type);

    // 如果[type]植物为升级植物，返回其前置植物种类，否则返回[type]
    inline int _GetPrePlantType(const int& type);

    // 返回[plant_type]的[seed_index]号种子是否可用，升级植物的阳光花费考虑生存模式的递增
    bool _GetIsSeedUsable(const int& plant_type, const int& seed_index);

    // 设置待修补植物的格子位置列表为场上的此类植物的格子位置列表
    void _AutoSetGrids();

    // 返回位于[row]行[col]列的玉米加农炮距离下一次可用的时间（厘秒）
    int _GetCannonRecoverTime(const int& row, const int& col);

    // 铲除[row]行[col]列的[type]植物
    void _ShovelThePlant(const int& row, const int& col, const int& type);

    // 参数合法性检查与初始化
    void _InitializeParams(const int& type, const std::vector<AGrid>& grids, const float& fixThreshold, const bool& isUseImitatorSeed);

public:
    // 启用自动修补
    // ------------参数------------
    // [type]为植物种类，填入一个属于[0, 47]的整数或APlantType（模仿植物除外）
    // [grids]为待修补的格子位置列表，不填或为空时自动获取场上的[type]植物的格子位置列表
    // [fixThreshold]为修补血量阈值，植物血量不高于此值时触发修补操作，小于 1 的值被视为最大血量的比例，不填时为 0.9999
    // [isUseImitatorSeed]为是否同时使用模仿种子修补，不填时为 是
    // ------------示例------------
    // Start(APUMPKIN, {}, 1200, false) // 修补全场的南瓜头，血量阈值为1200，不使用模仿种子
    // Start(AGLOOM_SHROOM) // 修补全场的忧郁菇，血量阈值为299，同时使用模仿种子
    // Start(ACOB_CANNON, {{1, 1}, {2, 1}}, 0.3)------修补位于{1，1}，{2，1}的玉米加农炮，血量阈值为90，同时使用模仿种子
    void Start(const int& type, const std::vector<AGrid>& grids = {}, const float& fixThreshold = 0.9999, const bool& isUseImitatorSeed = true);

    // 设置是否在战斗开始时检查是否携带待修补植物的种子，默认为 是
    void SetIsCheckCards(bool is_check_cards);

    // 设置是否不打断手动操作，默认为 是
    void SetIsNotInterrupt(bool is_not_interrupt);

    // 设置是否不修补上面有植物的睡莲叶或花盆
    void SetIsNotFixLilyPadOrFlowerPot(bool is_not_fix_lily_pad_or_flower_pot_with_plant);

    // 设置修补阳光阈值，阳光数量小于此值时不修补，默认为 0
    void SetFixSunThreshold(unsigned int fix_sun_threshold);

    // 设置执行修补判断的时间间隔（厘秒），默认为 1
    void SetRunInterval(unsigned int run_interval);

    // 重置植物修补位置
    void ResetFixList(std::vector<AGrid> grids);

    // 重置修补血量阈值
    void ResetFixHpThreshold(unsigned int fix_hp_threshold);

} aPlantFixerPlus;

void APlantFixerPlus::_Run()
{
    // 定时启用
    if (AGetMainObject()->GameClock() % _runInterval != 0)
        return;

    // 不打断手动操作
    if (_isNotInterrupt && AGetMainObject()->MouseAttribution()->Type() != 0)
        return;

    // 阳光数量小于阈值时不修补
    if (AGetMainObject()->Sun() < _fixSunThreshold)
        return;

    // 升级植物不可用时不修补
    if (_GetIsUpdateType(_type) && !_GetIsSeedUsable(_type, _seedIndexList[2]))
        return;

    // 确定待修补植物血量最低的格子位置列表
    std::vector<AGrid> minHpGrids;
    int minHp = 2147483647;
    std::vector<int> gridsHp;
    int plantIndex;
    auto plants = AGetMainObject()->PlantArray();
    for (auto each : _grids) {
        plantIndex = AGetPlantIndex(each.row, each.col, _type);
        gridsHp.emplace_back(plantIndex >= 0 ? plants[plantIndex].Hp() : 0);
    }
    minHp = *std::min_element(gridsHp.begin(), gridsHp.end());
    for (int i = 0; i < gridsHp.size(); ++i) {
        if (gridsHp[i] == minHp)
            minHpGrids.emplace_back(_grids[i]);
    }

    for (auto eachMinHpGrid : minHpGrids) {
        // 睡莲叶或花盆上有植物时不对其修补
        if (_isNotFixLilyPadOrFlowerPotWithPlant && (_type == ALILY_PAD || _type == AFLOWER_POT)) {
            if (AGetPlantIndex(eachMinHpGrid.row, eachMinHpGrid.col) >= 0 || AGetPlantIndex(eachMinHpGrid.row, eachMinHpGrid.col, APUMPKIN) >= 0)
                return;
        }
        // 玉米加农炮即将可用或正在发射且炮弹未出膛时不对其修补
        if (_type == ACOB_CANNON && AGetPlantIndex(eachMinHpGrid.row, eachMinHpGrid.col, ACOB_CANNON) >= 0) {
            int cannonRecoverTime = _GetCannonRecoverTime(eachMinHpGrid.row, eachMinHpGrid.col);
            if (cannonRecoverTime <= 625 + 751 + _runInterval || cannonRecoverTime >= 3475 - (205 + 1))
                return;
        }
        // 修补判断
        std::vector<AGrid> fixGrids = {{eachMinHpGrid.row, eachMinHpGrid.col}};
        if (_type == ACOB_CANNON) {
            AGrid temp = {eachMinHpGrid.row, eachMinHpGrid.col + 1};
            fixGrids.emplace_back(temp);
        }
        auto seeds = AGetMainObject()->SeedArray();
        int seedType;
        int plantType;
        if (minHp <= _fixHpThreshold) {
            for (auto eachGrid : fixGrids) {
                for (int eachSeedIndex : _seedIndexList) {
                    if (eachSeedIndex >= 0) {
                        seedType = seeds[eachSeedIndex].Type();
                        plantType = seedType == AIMITATOR ? _GetPrePlantType(_type) : seedType;
                        if (_GetIsSeedUsable(plantType, eachSeedIndex)) {
                            _ShovelThePlant(eachGrid.row, eachGrid.col, _type);
                            if (AAsm::GetPlantRejectType(plantType, eachGrid.row - 1, eachGrid.col - 1) == AAsm::NIL) {
                                ACard(eachSeedIndex + 1, eachGrid.row, eachGrid.col);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}
inline bool APlantFixerPlus::_GetIsUpdateType(const int& type)
{
    return type >= AGATLING_PEA && type <= ACOB_CANNON;
}
inline int APlantFixerPlus::_GetSeedIndex(const int& type)
{
    return type > AIMITATOR ? AGetSeedIndex(type - (AIMITATOR + 1), true) : AGetSeedIndex(type, false);
}
inline int APlantFixerPlus::_GetImitatorType(const int& type)
{
    return type + (AIMITATOR + 1);
}
inline int APlantFixerPlus::_GetPrePlantType(const int& type)
{
    std::map<int, int> pre_plant_map = {{AGATLING_PEA, AREPEATER}, {ATWIN_SUNFLOWER, ASUNFLOWER}, {AGLOOM_SHROOM, AFUME_SHROOM}, {ACATTAIL, ALILY_PAD}, {AWINTER_MELON, AMELON_PULT}, {AGOLD_MAGNET, AMAGNET_SHROOM}, {ASPIKEROCK, ASPIKEWEED}, {ACOB_CANNON, AKERNEL_PULT}};
    return _GetIsUpdateType(type) ? pre_plant_map[type] : type;
}
bool APlantFixerPlus::_GetIsSeedUsable(const int& plantType, const int& seed_index)
{
    auto seeds = AGetMainObject()->SeedArray();
    bool isUsable = seeds[seed_index].IsUsable();
    int sunCost = AMRef<int>(0x69F2C0 + plantType * 0x24);
    int plantCount = 0;
    // 无尽模式紫卡涨价
    if (plantType >= AGATLING_PEA && plantType <= ACOB_CANNON) {
        for (auto&& plt : aAlivePlantFilter) {
            if (plt.Type() == plantType)
                plantCount += 1;
        }
        sunCost += plantCount * 50;
    }
    return isUsable && sunCost <= AGetMainObject()->Sun();
}

void APlantFixerPlus::_AutoSetGrids()
{
    std::vector<AGrid> vec;
    AGrid temp;
    for (auto&& plant : aAlivePlantFilter) {
        if (plant.Type() == _type) {
            temp = {plant.Row(), plant.Col()};
            vec.emplace_back(temp);
        }
    }
    _grids = vec;
}

int APlantFixerPlus::_GetCannonRecoverTime(const int& row, const int& col)
{
    int cannonIndex = AGetPlantIndex(row, col, ACOB_CANNON);
    if (!(cannonIndex >= 0))
        return -1;
    auto cannon = AGetMainObject()->PlantArray() + cannonIndex;
    auto cannonAnimation = AGetPvzBase()->AnimationMain()->AnimationOffset()->AnimationArray() + cannon->AnimationCode();
    switch (cannon->State()) {
    case 35:
        return 125 + cannon->StateCountdown();
    case 36:
        return int(125 * (1 - cannonAnimation->CirculationRate()) + 0.5) + 1;
    case 37:
        return 0;
    case 38:
        return 3125 + int(350 * (1 - cannonAnimation->CirculationRate()) + 0.5);
    default:
        return -1;
    }
}

void APlantFixerPlus::_ShovelThePlant(const int& row, const int& col, const int& type)
{
    if (type == ACOFFEE_BEAN)
        return;
    if (AGetPlantIndex(row, col, type) >= 0) {
        if (type == APUMPKIN)
            AShovel(row, col, true);
        else
            AShovel(row, col, false);
    }
}
void APlantFixerPlus::_InitializeParams(const int& type, const std::vector<AGrid>& grids, const float& fixThreshold, const bool& isUseImitatorSeed)
{
    // 植物种类合法性检查
    if (!(type >= APEASHOOTER && type <= ACOB_CANNON)) {
        _msgboxLogger.Error("APlantFixerPlus:\n  非法的植物种类 # ，请填入一个属于[0, 47]的整数", type);
        return;
    }
    // 格子位置行数合法性检查
    for (auto each : grids) {
        // 泳池或雾夜为六行场地
        if (AGetMainObject()->Scene() == 2 || AGetMainObject()->Scene() == 3) {
            if (!(each.row >= 1 && each.row <= 6)) {
                _msgboxLogger.Error("APlantFixerPlus:\n  非法的格子位置行数 # ，请填入一个属于[1, 6]的整数", each.row);
                return;
            }
        } else {
            if (!(each.row >= 1 && each.row <= 5)) {
                _msgboxLogger.Error("APlantFixerPlus:\n  非法的格子位置行数 # ，请填入一个属于[1, 5]的整数", each.row);
                return;
            }
        }
    }
    // 格子位置列数合法性检查
    if (type == ACOB_CANNON) {
        for (auto each : grids) {
            if (!(each.col >= 1 && each.col <= 8)) {
                _msgboxLogger.Error("APlantFixerPlus:\n  非法的格子位置列数 # ，请填入一个属于[1, 8]的整数，注意玉米加农炮的位置判定为后轮（远离眼睛的一侧）所在的格子", each.row);
                return;
            }
        }
    } else {
        for (auto each : grids) {
            if (!(each.col >= 1 && each.col <= 9)) {
                _msgboxLogger.Error("APlantFixerPlus:\n  非法的格子位置列数 # ，请填入一个属于[1, 9]的整数", each.row);
                return;
            }
        }
    }

    _type = type;
    // 获取种子序号数组
    // 种子序号或其前置植物种子的序号
    _seedIndexList[0] = _GetSeedIndex(_GetPrePlantType(_type));
    // 模仿种子序号或其前置植物模仿种子的序号
    _seedIndexList[1] = _isUseImitatorSeed ? _GetSeedIndex(_GetImitatorType(_GetPrePlantType(_type))) : -1;
    // 升级植物的种子序号
    _seedIndexList[2] = _GetIsUpdateType(type) ? _GetSeedIndex(_type) : -1;
    // 卡槽可用性检查
    if (_isCheckCards && (_seedIndexList[0] == -1 && _seedIndexList[1] == -1 && _seedIndexList[2] == -1)) {
        _msgboxLogger.Error("APlantFixerPlus:\n    由于您未携带种类代号为 # 的植物的相关种子，无法执行修补操作。\n    您可以在void AScrpit()中执行本操作类的 SetIsCheckCards(false) 方法以关闭此提示", _type);
        return;
    }
    _grids = grids;
    if (_grids.empty())
        _AutoSetGrids();

    if (fixThreshold > 1) {
        _fixHpThreshold = int(fixThreshold);
    } else {
        const std::vector<int> max_hps = {300, 300, 300, 4000, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 8000, 300, 300, 300, 300, 300, 300, 4000, 300, 300, 300, 300, 300, 400, 300, 300, 300, 300, 300, 300, 300, 300, 300, 450, 300};
        _fixHpThreshold = int(max_hps[_type] * fixThreshold);
    }
    _isUseImitatorSeed = isUseImitatorSeed;
    _isParamsInitialized = true;
}

void APlantFixerPlus::Start(const int& type, const std::vector<AGrid>& grids, const float& fixThreshold, const bool& isUseImitatorSeed)
{
    if (!_isParamsInitialized)
        _InitializeParams(type, grids, fixThreshold, isUseImitatorSeed);
    if (!_isParamsInitialized)
        return;
    ATickRunner::Start([this]() { _Run(); }, false);
}
void APlantFixerPlus::SetIsCheckCards(bool isCheckCards)
{
    _isCheckCards = isCheckCards;
}
void APlantFixerPlus::SetIsNotInterrupt(bool isNotInterrupt)
{
    _isNotInterrupt = isNotInterrupt;
}
void APlantFixerPlus::SetIsNotFixLilyPadOrFlowerPot(bool isNotFixLilyPadOrFlowerPotWithPlant)
{
    _isNotFixLilyPadOrFlowerPotWithPlant = isNotFixLilyPadOrFlowerPotWithPlant;
}
void APlantFixerPlus::SetFixSunThreshold(unsigned int fixSunThreshold)
{
    _fixSunThreshold = fixSunThreshold;
}
void APlantFixerPlus::SetRunInterval(unsigned int runInterval)
{
    _runInterval = runInterval;
}
void APlantFixerPlus::ResetFixList(std::vector<AGrid> grids)
{
    _grids = grids;
}
void APlantFixerPlus::ResetFixHpThreshold(unsigned int fixHpThreshold)
{
    _fixHpThreshold = fixHpThreshold;
}

#endif
