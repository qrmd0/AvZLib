/*
 * @Author: crescendo
 * @Date: 2021-10-12 13:19:26
 * @Last Modified by: crescendo
 * @Last Modified time: 2022-06-06 11:52:18
 *  __AVZ_VERSION__ == 220213
 * 各种AvZ附加功能
 * https://github.com/qrmd0/AvZLib/tree/main/crescendo/avz-more
 */

/*
使用说明：
1. 将本文件置于AvZ目录inc文件夹下
2. 在Script()函数外添加 using namespace cresc;
*/

#pragma once
#include "libavz.h"

#ifndef __AZM_VERSION__
#define __AZM_VERSION__ 220721
#endif

using namespace AvZ;

namespace cresc {
// ***Not In Queue
// 检测僵尸是否存在
bool CheckZombie(int type = -1, int row = -1, int left_lim = -1, int right_lim = -1) {
    auto zombie = GetMainObject()->zombieArray();
    int zombies_count_max = GetMainObject()->zombieTotal();
    for (int i = 0; i < zombies_count_max; ++i, ++zombie) {
        if (!zombie->isDisappeared() && !zombie->isDead())
            if ((left_lim == -1 || int(zombie->abscissa()) >= left_lim) && (right_lim == -1 || int(zombie->abscissa()) <= right_lim)) {
                if (type < 0 && row < 0)
                    return true;
                else if (type >= 0 && row >= 0) {
                    if (zombie->row() == row - 1 && zombie->type() == type)
                        return true;
                } else if (type < 0 && row >= 0) {
                    if (zombie->row() == row - 1)
                        return true;
                } else  // if (type >= 0 && row < 0)
                {
                    if (zombie->type() == type)
                        return true;
                }
            }
    }
    return false;
}

// ***Not In Queue
// 数僵尸数量
int CountZombie(int type = -1, int row = -1, int left_lim = -1, int right_lim = -1, const std::set<int> &states = {}) {
    int sum = 0;
    auto zombie = GetMainObject()->zombieArray();
    int zombies_count_max = GetMainObject()->zombieTotal();
    for (int i = 0; i < zombies_count_max; ++i, ++zombie) {
        if (!zombie->isDisappeared() && !zombie->isDead())
            if ((left_lim == -1 || int(zombie->abscissa()) >= left_lim) && (right_lim == -1 || int(zombie->abscissa()) <= right_lim) && (states.empty() || states.find(zombie->state()) != states.end())) {
                if (type < 0 && row < 0)
                    sum++;
                else if (type >= 0 && row >= 0) {
                    if (zombie->row() == row - 1 && zombie->type() == type)
                        sum++;
                } else if (type < 0 && row >= 0) {
                    if (zombie->row() == row - 1)
                        sum++;
                } else  // if (type >= 0 && row < 0)
                {
                    if (zombie->type() == type)
                        sum++;
                }
            }
    }
    return sum;
}

// ***In Queue
// 根据僵尸是否存在判断用卡
// 默认检测是否有僵尸在所用卡同一行
// ***使用示例：
// SetTime(330, 1);
// SmartCard(CHERRY_BOMB, 2, 9, {ZOMBONI}); //如果场上存在冰车，则种植樱桃于2-9
// SmartCard(SPIKEWEED, 2, 9, {ZOMBONI}, {2}); //如果有冰车在第2行，则种植地刺于2-9
// SmartCard(CHERRY_BOMB, 2, 9, {GARGANTUAR, GIGA_GARGANTUAR}, {1, 2}, {-1, 800}, {70});
// 如果有白眼或红眼在第1或2行，且横坐标整数部分小于等于800，且状态为70，则种植樱桃于2-9
void SmartCard(PlantType plant_type, int row, float col, const std::set<int> &_zombie_type, const std::set<int> &_check_rows = {}, const std::pair<int, int> &_limits = {-1, -1}, const std::set<int> &_states = {}) {
    InsertOperation(
        [=]() {
            std::set<int> zombie_type = _zombie_type;
            if (zombie_type.empty())
                zombie_type.insert(-1);
            std::set<int> check_rows = _check_rows;
            if (check_rows.empty())
                check_rows.insert(-1);
            std::pair<int, int> limits = _limits;
            if (limits.first > limits.second)
                limits = {-1, -1};
            for (auto z : zombie_type) {
                for (auto r : check_rows) {
                    if (CountZombie(z, r, limits.first, limits.second, _states) == 0)
                        continue;
                    InsertGuard ig(false);
                    Card(plant_type, row, col);
                    return;
                }
            }
        },
        "SmartCard");
}

// ***In Queue
// 等卡片冷却完毕后使用，与PaoOperator::recoverPao类似
// ***使用示例：
// SetTime(300, 9);
// RecoverCard(CHERRY_BOMB, 2, 3); //等樱桃卡片CD恢复后，选取樱桃卡片，放在2行,3列
void RecoverCard(PlantType plant_type, int row, float col) {
    InsertOperation([=]() {
                        int idx = GetCardIndex(plant_type);
                        if (idx >= 0 && idx < 10)
                        {
                            auto seeds = GetMainObject()->seedArray();
                            if (seeds[idx].isUsable())
                            {
                                InsertGuard ig(false);
                                Card(plant_type, row, col);
                            }
                            else
                            {
                                SetNowTime();
                                Delay(seeds[idx].initialCd() - seeds[idx].cd() + 1);
                                Card(plant_type, row, col);
                            }
                        } },
                    "RecoverCard");
}

// ***In Queue
// 等待上下三行内小丑炸完后用卡（默认卡片需要100cs生效）
// ***使用示例：
// SetTime(300, 9);
// SafeCard(CHERRY_BOMB, 2, 3); //等待上下三行小丑爆炸完毕后，选取樱桃卡片，放在2行,3列
void SafeCard(PlantType plant_type, int row, float col) {
    InsertOperation([=]() {
                        auto zombie = GetMainObject()->zombieArray();
                        int zombie_count_max = GetMainObject()->zombieTotal();
                        int countdown = -1;
                        for (int i = 0; i < zombie_count_max; ++i, ++zombie)
                        {
                            if (zombie->type() == JACK_IN_THE_BOX_ZOMBIE &&
                                !zombie->isDisappeared() &&
                                !zombie->isDead() &&
                                RangeIn(zombie->row() + 1, {row - 1, row, row + 1}) && zombie->state() == 16)
                            {
                                int temp = zombie->stateCountdown();
                                if (temp <= 100 && temp > countdown)
                                {
                                    countdown = temp;
                                }
                            }
                        }
                        if (countdown == -1)
                        {
                            InsertGuard ig(false);
                            Card(plant_type, row, col);
                        }
                        else
                        {
                            SetNowTime();
                            Delay(countdown + 1);
                            SafeCard(plant_type, row, col);
                        } },
                    "SafeCard");
}

// ***In Queue
// RecoverCard和SafeCard的结合
// ***使用示例：
// SetTime(300, 9);
// RecoverSafeCard(CHERRY_BOMB, 2, 3); //等待樱桃卡片CD恢复后，等待上下三行小丑爆炸完毕后，选取樱桃卡片，放在2行,3列
void RecoverSafeCard(PlantType plant_type, int row, float col) {
    InsertOperation([=]() {
                        int idx = GetCardIndex(plant_type);
                        if (idx >= 0 && idx < 10)
                        {
                            auto seeds = GetMainObject()->seedArray();
                            if (seeds[idx].isUsable())
                            {
                                SafeCard(plant_type, row, col);
                            }
                            else
                            {
                                Delay(seeds[idx].initialCd() - seeds[idx].cd() + 1);
                                SafeCard(plant_type, row, col);
                            }
                        } },
                    "RecoverSafeCard");
}

// 自定义IceFiller子类
// 用于白昼简化点冰
class IceFillerAZM : public IceFiller {
   public:
    // ***Not In Queue
    // 白昼便捷点冰
    // ***使用示例：
    // i.ice();  // 冰于当前波次602生效
    // i.ice(1500, 3);   // 冰于第三波1500生效
    void ice(int time = 601, int wave = -1) {
        if (wave == -1) {
            SetTime(time - 298);
        } else {
            SetTime(time - 298, wave);
        }
        coffee();
        SetPlantActiveTime(ICE_SHROOM, 298);
    }
};

// ***Not In Queue
// 便捷原版冰放置
// ***使用示例：
// nice(1, 2);  // 于1-2放置原版冰，于当前波次602生效
// nice(1, 2, leng+10, wave);   // 于1-2放置原版冰，于第wave波leng+11生效（假设leng为波长，则为ice3冰时机）
void nice(int row, float col, int time = 601, int wave = -1) {
    if (wave == -1) {
        SetTime(time - 100 + 1);
    } else {
        SetTime(time - 100 + 1, wave);
    }
    Card(ICE_SHROOM, row, col);
}

// ***Not In Queue
// 便捷复制冰放置
// ***使用示例：
// mice(1, 2);  // 于1-2放置复制冰，于当前波次602生效
// mice(1, 2, leng+10, wave);   // 于1-2放置复制冰，于第wave波leng+11生效（假设leng为波长，则为ice3冰时机）
void mice(int row, float col, int time = 601, int wave = -1) {
    if (wave == -1) {
        SetTime(time - 420);
    } else {
        SetTime(time - 420, wave);
    }
    Card(M_ICE_SHROOM, row, col);
    SetPlantActiveTime(ICE_SHROOM, 420);
}

// 自定义僵尸子类
struct ZombieAZM : public Zombie {
    int &layer() {
        return (int &)((uint8_t *)this)[0x20];
    }
    bool &isHypnotic() {
        return (bool &)((uint8_t *)this)[0xb8];
    }
};

// 自定义场物类
struct Place {
   private:
    void operator=(Place &&_) {}
    Place(Place &&_) {}

   public:
    Place() {}
    uint8_t data[0xec];

    // 行数
    int &row() {
        return (int &)((uint8_t *)this)[0x14];
    }

    // 列数
    int &col() {
        return (int &)((uint8_t *)this)[0x10];
    }

    // 类型
    int &type() {
        return (int &)((uint8_t *)this)[0x8];
    }

    // 计时或血量
    int &countdown() {
        return (int &)((uint8_t *)this)[0x18];
    }

    // 是否消失
    bool &isDisappeared() {
        return (bool &)((uint8_t *)this)[0x20];
    }
};

// 自定义MainObject子类
struct MainObjectAZM : public MainObject {
    // 最多时场地物品数量
    int &placeTotal() {
        return (int &)((uint8_t *)this)[0x120];
    }

    // 场地物品内存数组
    SafePtr<Place> placeArray() {
        return *(Place **)((uint8_t *)this + 0x11c);
    }

    // 完成关卡数
    int levelCompleted() {
        return ReadMemory<int>(0x6A9EC0, 0x768, 0x160, 0x6c);
    }
};

// ***Not In Queue
// 平均分配指定种类僵尸所在行数
// 默认只平均分配红眼
// *** 注意：只能读取DE、NE、PE、FE、RE五大场景，无法辨认的场景默认是ME
// *** 使用示例
// AverageSpawn(); //不填参数，平均分配每波红眼
// AverageSpawn({POGO_ZOMBIE, ZOMBONI}); //平均分配每波跳跳和冰车
void AverageSpawn(const std::vector<ZombieType> &lst = {GIGA_GARGANTUAR}, bool onlyHugeWave = false) {
    int scene = AvZ::GetMainObject()->scene();
    std::vector<int> zombieRow = {0, 1, 2, 3, 4};
    int height = 85;
    if (scene == 2 || scene == 3) {
        zombieRow = {0, 1, 4, 5};
    }
    if (scene == 0 || scene == 1) {
        height = 100;
    }
    int baseY = 40;
    if (scene == 0 || scene == 1 || scene == 2 || scene == 3) {
        baseY = 50;
    }

    for (int wave = 1; wave <= 20; ++wave) {
        if (onlyHugeWave && (wave != 9 && wave != 19))
            continue;
        InsertTimeOperation(
            0, wave, [=]() {
                                SafePtr<ZombieAZM> zombie = (ZombieAZM *)GetMainObject()->zombieArray();
                                int zombies_count_max = GetMainObject()->zombieTotal();
                                int spawnRow[33];
                                for (int i = 0; i < 33; ++i)
                                {
                                    spawnRow[i] = rand() % (zombieRow.size());
                                }
                                for (int i = 0; i < zombies_count_max; ++i, ++zombie)
                                {
                                    if (zombie->isDead() || zombie->isDisappeared())
                                        continue;

                                    // 只移动本波僵尸
                                    if (zombie->existTime() > 100)
                                        continue;

                                    // 检测类型
                                    int type = zombie->type();
                                    if (std::find(lst.begin(), lst.end(), type) == lst.end())
                                        continue;

                                    // 不移动水路僵尸
                                    int row = zombie->row();
                                    if ((scene == 2 || scene == 3) && (row == 2 || row == 3))
                                    {
                                        continue;
                                    }

                                    // 不移动前院场地的舞王
                                    if ((scene == 0 || scene == 1) && (type == DANCING_ZOMBIE))
                                    {
                                        continue;
                                    }

                                    if (type >= 0 && type < 33 && spawnRow[type] >= 0 && spawnRow[type] < zombieRow.size())
                                    {
                                        int diff = zombieRow[spawnRow[type]] - row;
                                        zombie->row() = zombieRow[spawnRow[type]];
                                        zombie->ordinate() = baseY + zombieRow[spawnRow[type]] * height;
                                        zombie->layer() += 10000 * diff;
                                        spawnRow[type]++;
                                        if (spawnRow[type] >= zombieRow.size())
                                        {
                                            spawnRow[type] -= zombieRow.size();
                                        }
                                    }
                                } },
            "AverageSpawn");
    }
}

// ***Not In Queue
// 无视阳光
void IgnoreSun(bool f = true) {
    if (f) {
        WriteMemory<byte>(0x70, 0x0041ba72);
        WriteMemory<byte>(0x3b, 0x0041ba74);
        WriteMemory<byte>(0x91, 0x0041bac0);
        WriteMemory<byte>(0x80, 0x00427a92);
        WriteMemory<byte>(0x80, 0x00427dfd);
        WriteMemory<byte>(0xeb, 0x0042487f);
    } else {
        WriteMemory<byte>(0x7f, 0x0041ba72);
        WriteMemory<byte>(0x2b, 0x0041ba74);
        WriteMemory<byte>(0x9e, 0x0041bac0);
        WriteMemory<byte>(0x8f, 0x00427a92);
        WriteMemory<byte>(0x8f, 0x00427dfd);
        WriteMemory<byte>(0x74, 0x0042487f);
    }
}

// ***Not In Queue
// 清除浓雾
void NoFog(bool f = true) {
    if (f) {
        WriteMemory<unsigned short>(0xd231, 0x0041a68d);
    } else {
        WriteMemory<unsigned short>(0xf23b, 0x0041a68d);
    }
}

// ***In Queue
// 读取僵尸图层并显示，主要供调试用
void ReadLayer() {
    InsertOperation([=]() {
                            SafePtr<ZombieAZM> zombie = (ZombieAZM *)GetMainObject()->zombieArray();
                            int zombies_count_max = GetMainObject()->zombieTotal();
                            for (int i = 0; i < zombies_count_max; ++i, ++zombie)
                            {
                                if (zombie->isDead() || zombie->isDisappeared())
                                {
                                    continue;
                                }
                                ShowErrorNotInQueue("类型# 行数# 图层#", zombie->type(), zombie->row(), zombie->layer());
                            } });
}

// ***Not In Queue
// 计算可供使用的炮数
// ***使用示例：
// int c = CountReadyPao(); // 计算当前可供使用的炮数
// int c2 = CountReadyPao(400); // 计算400cs后可供使用的炮数
int CountReadyPao(int time = 0) {
    auto plant = GetMainObject()->plantArray();
    int plants_count_max = GetMainObject()->plantTotal();
    int counter = 0;
    for (int i = 0; i < plants_count_max; ++i, ++plant) {
        if (!plant->isCrushed() && !plant->isDisappeared() && plant->type() == COB_CANNON) {
            int recoverTime = -1;
            auto animation_memory = AvZ::GetPvzBase()->animationMain()->animationOffset()->animationArray() + plant->animationCode();
            switch (plant->state()) {
                case 35:
                    recoverTime = 125 + plant->stateCountdown();
                    break;
                case 36:
                    recoverTime = int(125 * (1 - animation_memory->circulationRate()) + 0.5) + 1;
                    break;
                case 37:
                    recoverTime = 0;
                    break;
                case 38:
                    recoverTime = 3125 + int(350 * (1 - animation_memory->circulationRate()) + 0.5);
                    break;
            }
            if (recoverTime <= time) {
                counter++;
            }
        }
    }
    // AvZ::ShowErrorNotInQueue("已装填炮数：#", counter);
    return counter;
}

// ***Not In Queue
// 读取当前所有场物并显示，主要供调试用
void ShowPlace() {
    SafePtr<Place> place = ((MainObjectAZM *)GetMainObject())->placeArray();
    for (int i = 0; i < ((MainObjectAZM *)GetMainObject())->placeTotal(); ++i, ++place) {
        if (!place->isDisappeared())
            ShowErrorNotInQueue("#, #, #, #", place->row(), place->col(), place->type(), place->countdown());
    }
}

// ***Not In Queue
// 设置弹坑CD
// 注意：该位置必须先有一个弹坑
// ***使用示例：
// SetDoomPlace(2, 4, 3000); // 将2-4位置的弹坑CD设置为3000cs
void SetDoomPlace(int row, int col, int cd) {
    SafePtr<Place> place = ((MainObjectAZM *)GetMainObject())->placeArray();
    for (int i = 0; i < ((MainObjectAZM *)GetMainObject())->placeTotal(); ++i, ++place) {
        if (!place->isDisappeared() && place->row() == row - 1 && place->col() == col - 1) {
            place->countdown() = cd;
        }
    }
}

// ***Not In Queue
// 返回当前已完成的选卡数
int GetCurrentLevel() {
    return ((MainObjectAZM *)GetMainObject())->levelCompleted();
}

// ***In Queue
// 智能垫垫材
// 函数默认若垫材卡中含有花盆，则优先级最高（刚种植100cs内免疫啃食），其余垫材卡按参数传入的顺序
// 每条路的威胁程度计算方法为梯子数与小丑数相加
// ***使用示例：
// SetTime(824, 1);
// SmartStall({1, 2, 3, 4, 5}, {PUFF_SHROOM, SUN_SHROOM, FLOWER_POT, SCAREDY_SHROOM, SUNFLOWER}); // 于第一波824放置垫材，垫1、2、3、4、5路，使用小喷菇、阳光菇、花盆、胆小菇、向日葵这五张垫材
void SmartStall(const std::vector<int> &r = {1, 2, 5, 6}, const std::vector<PlantType> &t = {FLOWER_POT, SCAREDY_SHROOM, PUFF_SHROOM, SUN_SHROOM}) {
    InsertOperation([=]() {
                            std::vector<PlantType> types = t;
                            if (std::find(types.begin(), types.end(), FLOWER_POT) != types.end())
                            {
                                types.erase(std::find(types.begin(), types.end(), FLOWER_POT));
                                types.insert(types.begin(), FLOWER_POT);
                            }
                            std::vector<int> rows = r;
                            std::vector<int> count;
                            for (int r : rows)
                            {
                                count.push_back(CountZombie(LADDER_ZOMBIE, r) + CountZombie(JACK_IN_THE_BOX_ZOMBIE, r));
                            }
                            std::vector<int> card_row;
                            std::vector<int> card_type;
                            while (!rows.empty() && !types.empty())
                            {
                                int max = count.front();
                                int num = 0;
                                for (int i = 1; i < rows.size(); i++)
                                {
                                    if (count[i] > max)
                                    {
                                        max = count[i];
                                        num = i;
                                    }
                                }
                                InsertGuard ig(false);
                                Card(types[0], rows[num], 9);
                                rows.erase(rows.begin() + num);
                                types.erase(types.begin());
                                count.erase(count.begin() + num);
                            } });
}

}  // namespace cresc
