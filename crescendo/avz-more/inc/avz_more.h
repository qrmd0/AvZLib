/*
 * @Author: crescendo
 * @Date: 2021-10-12 13:19:26
 * @Last Modified by: crescendo
 * @Last Modified time: 2022-06-06 11:52:18
 *  __AVZ_VERSION__ == 220213
 * 各种AvZ附加功能
 */

/*
使用说明：
1. 将本文件置于AvZ目录inc文件夹下
2. 在Script()函数外添加 using namespace cresc;
*/

#pragma once
#include "libavz.h"

#ifndef __AZM_VERSION__
#define __AZM_VERSION__ 220822
#endif

using namespace AvZ;

namespace cresc {

/** ===自定义类部分===**/

// 自定义僵尸子类
struct ZombieAZM : public Zombie {
    int &layer() { return (int &)((uint8_t *)this)[0x20]; }
    bool &isHypnotic() { return (bool &)((uint8_t *)this)[0xb8]; }
    bool &notDying() { return (bool &)((uint8_t *)this)[0xBA]; }  //若为濒死状态，则为false
    uint16_t &animationCode() { return (uint16_t &)((uint8_t *)this)[0x118]; }
    int &attackWidth() { return (int &)((uint8_t *)this)[0xA4]; }
};

// 获得动画对象
Animation *GetAnimation(SafePtr<ZombieAZM> z) {
    return GetPvzBase()
               ->animationMain()
               ->animationOffset()
               ->animationArray() +
           z->animationCode();
}

Animation *GetAnimation(SafePtr<Zombie> z) {
    return GetAnimation((ZombieAZM *)z);
}

// 获得动画对象
Animation *GetAnimation(SafePtr<Plant> p) {
    return GetPvzBase()
               ->animationMain()
               ->animationOffset()
               ->animationArray() +
           p->animationCode();
}

// 自定义场物类
struct Place {
   private:
    void operator=(Place &&_) {}
    Place(Place &&_) {}

   public:
    Place() {}
    uint8_t data[0xec];

    // 行数
    int &row() { return (int &)((uint8_t *)this)[0x14]; }

    // 列数
    int &col() { return (int &)((uint8_t *)this)[0x10]; }

    // 类型;
    // 1 - 墓碑；2 - 弹坑
    int &type() { return (int &)((uint8_t *)this)[0x8]; }

    // 计时或血量
    int &countdown() { return (int &)((uint8_t *)this)[0x18]; }

    // 是否消失
    bool &isDisappeared() { return (bool &)((uint8_t *)this)[0x20]; }
};

// 自定义MainObject子类
struct MainObjectAZM : public MainObject {
    // 最多时场地物品数量
    int &placeTotal() { return (int &)((uint8_t *)this)[0x120]; }

    // 场地物品内存数组
    SafePtr<Place> placeArray() { return *(Place **)((uint8_t *)this + 0x11c); }

    // 阳光
    int &sun() {
        return (int &)((uint8_t *)this)[0x5560];
    }

    // 冰道坐标数组（最大值为800）
    uint32_t *IceTrailAbscissaList() {
        return (uint32_t *)((uint8_t *)this + 0x60c);
    }

    // 冰道倒计时数组
    uint32_t *IceTrailCountdownList() {
        return (uint32_t *)((uint8_t *)this + 0x624);
    }
};

// 获得MainObjectAZM对象
MainObjectAZM *MyMainObject() {
    return (MainObjectAZM *)GetMainObject();
}

// 完成关卡数
int GetCurrentLevel() {
    return ReadMemory<int>(0x6A9EC0, 0x768, 0x160, 0x6c);
}

// 当前阳光数
int GetSun() {
    return MyMainObject()->sun();
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
// nice(1, 2, leng+10, wave);   //
// 于1-2放置原版冰，于第wave波leng+11生效（假设leng为波长，则为ice3冰时机）
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
// mice(1, 2, leng+10, wave);   //
// 于1-2放置复制冰，于第wave波leng+11生效（假设leng为波长，则为ice3冰时机）
void mice(int row, float col, int time = 601, int wave = -1) {
    if (wave == -1) {
        SetTime(time - 420);
    } else {
        SetTime(time - 420, wave);
    }
    Card(M_ICE_SHROOM, row, col);
    SetPlantActiveTime(ICE_SHROOM, 420);
}

/** ===判定部分===**/

// 获得row行col列无偏移植物的内存坐标
std::pair<int, int> GetPlantCoord(int row, int col) {
    auto scene = GetMainObject()->scene();
    if (scene == 0 || scene == 1)
        return {40 + (col - 1) * 80, 80 + (row - 1) * 100};
    if (scene == 2 || scene == 3)
        return {40 + (col - 1) * 80, 80 + (row - 1) * 85};
    return {40 + (col - 1) * 80, 70 + (row - 1) * 85 + (col <= 5 ? (6 - col) * 20 : 0)};
}

// 获得某特定类型植物的防御域
// 使用现行标准，内存坐标 → 90,0)基准的调试模式
// https://wiki.crescb.com/timespace/ts/
std::pair<int, int> GetDefenseRange(PlantType type) {
    switch (type) {
        case TALL_NUT:
            return {30, 70};
        case PUMPKIN:
            return {20, 80};
        case COB_CANNON:
            return {20, 120};
        default:
            return {30, 50};
    }
}

// 获得某特定类型植物对小丑爆炸的防御域
std::pair<int, int> GetExplodeDefenseRange(PlantType type) {
    switch (type) {
        case TALL_NUT:
            return {10, 90};
        case PUMPKIN:
            return {0, 100};
        case COB_CANNON:
            return {0, 100};
        default:
            return {10, 70};
    }
}

// 判断两个区间是否重叠
bool IntervalIntersectInterval(int x1, int w1, int x2, int w2) {
    return ((x1 <= x2 + w2) && (x2 <= x1 + w1));
}

// 判断某僵尸攻击域和某植物防御域是否重叠
bool JudgeHit(SafePtr<Zombie> zombie, int plant_row, int plant_col, PlantType plant_type = PEASHOOTER) {
    auto z = (ZombieAZM *)zombie;
    auto plant_coord = GetPlantCoord(plant_row, plant_col);
    auto def = GetDefenseRange(plant_type);

    return zombie->row() + 1 == plant_row && IntervalIntersectInterval(int(zombie->abscissa()) + zombie->attackAbscissa(), z->attackWidth(), plant_coord.first + def.first, def.second - def.first);
}

// 判断小丑爆炸范围和某植物防御域是否重叠
bool JudgeExplode(SafePtr<Zombie> zombie, int plant_row, int plant_col, PlantType plant_type = PEASHOOTER) {
    int x = zombie->abscissa() + 60, y = zombie->ordinate() + 60;  // 小丑爆心偏移
    auto plant_coord = GetPlantCoord(plant_row, plant_col);
    int plant_x = plant_coord.first, plant_y = plant_coord.second;
    int y_dis = 0;
    if (y < plant_y)
        y_dis = plant_y - y;
    else if (y > plant_y + 80)
        y_dis = y - (plant_y + 80);
    if (y_dis > 90)
        return false;
    int x_dis = sqrt(90 * 90 - y_dis * y_dis);
    auto def = GetExplodeDefenseRange(plant_type);
    return plant_x + def.first - x_dis <= x && x <= plant_x + def.second + x_dis;
}

/** ===读僵尸内存部分===**/

// ***Not In Queue
// 检测僵尸是否存在
// type: 类型；row: 行；left_lim: 坐标左限；right_lim: 坐标右限；wave: 来自波次；hp_min: 最低血量
bool CheckZombie(int type = -1, int row = -1, int left_lim = -1,
                 int right_lim = -1, int wave = -1, int hp_min = -1) {
    auto zombie = GetMainObject()->zombieArray();
    int zombies_count_max = GetMainObject()->zombieTotal();
    for (int i = 0; i < zombies_count_max; ++i, ++zombie) {
        if (!zombie->isDisappeared() && !zombie->isDead())
            if ((left_lim == -1 || int(zombie->abscissa()) >= left_lim) &&
                (right_lim == -1 || int(zombie->abscissa()) <= right_lim) &&
                (wave == -1 || zombie->standState() + 1 == wave) &&
                (hp_min == -1 || zombie->hp() + zombie->oneHp() >= hp_min)) {
                if (type < 0 && row < 0)
                    return true;
                else if (type >= 0 && row >= 0) {
                    if (zombie->row() == row - 1 && zombie->type() == type)
                        return true;
                } else if (type < 0 && row >= 0) {
                    if (zombie->row() == row - 1) return true;
                } else  // if (type >= 0 && row < 0)
                {
                    if (zombie->type() == type) return true;
                }
            }
    }
    return false;
}

// ***Not In Queue
// 数僵尸数量
int CountZombie(int type = -1, int row = -1, int left_lim = -1,
                int right_lim = -1, const std::set<int> &states = {}, int from_wave = -1) {
    int sum = 0;
    auto zombie = GetMainObject()->zombieArray();
    int zombies_count_max = GetMainObject()->zombieTotal();
    for (int i = 0; i < zombies_count_max; ++i, ++zombie) {
        if (!zombie->isDisappeared() && !zombie->isDead())
            if ((left_lim == -1 || int(zombie->abscissa()) >= left_lim) &&
                (right_lim == -1 || int(zombie->abscissa()) <= right_lim) &&
                (states.empty() ||
                 states.find(zombie->state()) != states.end()) &&
                (from_wave == -1 || zombie->standState() + 1 == from_wave)) {
                if (type < 0 && row < 0)
                    sum++;
                else if (type >= 0 && row >= 0) {
                    if (zombie->row() == row - 1 && zombie->type() == type)
                        sum++;
                } else if (type < 0 && row >= 0) {
                    if (zombie->row() == row - 1) sum++;
                } else  // if (type >= 0 && row < 0)
                {
                    if (zombie->type() == type) sum++;
                }
            }
    }
    return sum;
}

/** ===卡片部分===**/

// 某卡片是否可用
bool IsUsable(PlantType type) {
    auto idx = GetCardIndex(type);
    if (idx < 0 || idx > 9) {
        return false;
    }
    auto seeds = GetMainObject()->seedArray();
    return seeds[idx].isUsable();
}

// 某格子的核坑倒计时
int DoomCD(int row, int col) {
    int doom_cd = 0;
    SafePtr<Place> place = ((MainObjectAZM *)GetMainObject())->placeArray();
    for (int i = 0; i < ((MainObjectAZM *)GetMainObject())->placeTotal();
         ++i, ++place) {
        if (!place->isDisappeared() && place->row() == row - 1 &&
            place->col() == col - 1 && place->type() == 2) {
            doom_cd = place->countdown();
            break;
        }
    }
    return doom_cd;
}

// 某行的冰道倒计时
int IceTrailCD(int row) {
    if (row < 1 || row > 6) return -1;
    return MyMainObject()->IceTrailCountdownList()[row - 1];
}

// 某格子的冰道倒计时
int IceTrailCD(int row, int col) {
    if (row < 1 || row > 6) return -1;
    if (col < 1 || col > 9) return -1;
    int ice_trail_abs = MyMainObject()->IceTrailAbscissaList()[row - 1];
    int no_cover_limit = 80 * col + 28 + (col == 9 ? 3 : 0);  // 9列格子被冰道覆盖有特判
    if (ice_trail_abs >= no_cover_limit)
        return 0;
    else
        return IceTrailCD(row);
}

// 某各自的核坑倒计时和冰道倒计时的较大值
int GridCD(int row, int col) {
    return std::max(DoomCD(row, col), IceTrailCD(row, col));
}

// 返回某卡能否成功用在某处
// 主要用于防止Card函数一帧内调用多次Card导致不明原因崩溃
bool Plantable(PlantType type, int row, int col) {
    return Asm::getPlantRejectType(type, row - 1, col - 1) == Asm::NIL;
}

// 返回某卡的冷却倒计时
int CardCD(PlantType type) {
    auto idx = GetCardIndex(type);
    if (idx < 0 || idx > 9) {
        return -1;
    }
    auto seeds = GetMainObject()->seedArray();
    if (seeds[idx].isUsable()) {
        return 0;
    }
    return seeds[idx].initialCd() - seeds[idx].cd() + 1;
}

// 如果放置成功或下一帧尝试，返回true；否则返回false
bool CardNIQ(PlantType plant_type, int row, int col, bool force = false) {
    bool is_usable = IsUsable(plant_type), plantable = Plantable(plant_type, row, col);
    if (!is_usable) return false;
    if (plantable) {
        CardNotInQueue(GetCardIndex(plant_type) + 1, row, col);
        return true;
    }

    // 如果想要强制种植，尝试铲除该位置的植物后重试
    if (force) {
        bool shovel_flag = false;
        if (!RangeIn(plant_type, {LILY_PAD, FLOWER_POT, PUMPKIN})) {
            if (GetPlantIndex(row, col) >= 0 && !(GetPlantIndex(row, col, CATTAIL) >= 0 && GetPlantIndex(row, col, PUMPKIN) < 0)) {
                shovel_flag = true;
            }
        } else if (GetPlantIndex(row, col, plant_type) >= 0) {
            shovel_flag = true;
        }
        if (shovel_flag) {
            SetNowTime();
            Shovel(row, col);
            Delay(1);
            InsertOperation([=]() {
                CardNIQ(plant_type, row, col, false);
            },
                            "cardNIQ");
            return true;
        }
    }
    return false;
}

void CardNIQ(PlantType plant_type, const std::vector<Grid> &lst, bool force = false) {
    for (const auto &l : lst) {
        if (!IsUsable(plant_type)) break;
        if (CardNIQ(plant_type, l.row, l.col, force)) return;
    }
}

// ***In Queue
// 根据僵尸是否存在判断用卡
// ***使用示例：
// SetTime(330, 1);
// SmartCard(SPIKEWEED, 2, 9, {ZOMBONI}); //如果有冰车在第2行，则种植地刺于2-9
// SmartCard(CHERRY_BOMB, 2, 9, {ZOMBONI}, {1, 2});
// //如果有冰车在第1或2行，则种植樱桃于2-9 SmartCard(CHERRY_BOMB, 2, 9,
// {GARGANTUAR, GIGA_GARGANTUAR}, {1, 2}, {-1, 800}, {70});
// 如果有白眼或红眼在第1或2行，且横坐标整数部分小于等于800，且状态为70，则种植樱桃于2-9
void SmartCard(PlantType plant_type, int row, int col,
               const std::set<int> &_zombie_type,
               const std::set<int> &_check_rows = {},
               const std::pair<int, int> &_limits = {-1, -1},
               const std::set<int> &_states = {}) {
    InsertOperation(
        [=]() {
            if (GetCardIndex(plant_type) < 0 || GetCardIndex(plant_type) > 9) return;
            std::set<int> zombie_type = _zombie_type;
            if (zombie_type.empty()) zombie_type.insert(-1);
            std::set<int> check_rows = _check_rows;
            if (check_rows.empty()) check_rows.insert(-1);
            std::pair<int, int> limits = _limits;
            if (limits.first > limits.second) limits = {-1, -1};
            for (auto z : zombie_type) {
                for (auto r : check_rows) {
                    if (CountZombie(z, r, limits.first, limits.second,
                                    _states) == 0)
                        continue;
                    CardNIQ(plant_type, row, col);
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
// RecoverCard(CHERRY_BOMB, 2, 3);
// //等樱桃卡片CD恢复后，选取樱桃卡片，放在2行,3列
void RecoverCard(PlantType plant_type, int row, int col, bool force = true) {
    InsertOperation(
        [=]() {
            if (GetCardIndex(plant_type) < 0 || GetCardIndex(plant_type) > 9) return;
            if (IsUsable(plant_type)) {
                CardNIQ(plant_type, row, col, force);
            } else {
                SetNowTime();
                Delay(CardCD(plant_type));
                InsertOperation([=]() {
                    CardNIQ(plant_type, row, col, force);
                },
                                "RecoverCard_inner");
            }
        },
        "RecoverCard");
}

void RecoverCard(PlantType plant_type, const std::vector<Grid> &lst, bool force = true) {
    if (lst.empty()) return;

    // lst若长度为1，转发
    if (lst.size() == 1) {
        RecoverCard(plant_type, lst.at(0).row, lst.at(0).col, force);
        return;
    }

    InsertOperation(
        [=]() {
            if (GetCardIndex(plant_type) < 0 || GetCardIndex(plant_type) > 9) return;
            if (IsUsable(plant_type)) {
                CardNIQ(plant_type, lst, force);
            } else {
                SetNowTime();
                Delay(CardCD(plant_type));
                InsertOperation([=]() {
                    CardNIQ(plant_type, lst, force);
                },
                                "RecoverCard_inner");
            }
        },
        "RecoverCard");
}

// ***In Queue
// 等待上下三行内小丑炸完后用卡（默认卡片需要100cs生效）
// ***使用示例：
// SetTime(300, 9);
// SafeCard(CHERRY_BOMB, 2, 3);
// //等待上下三行小丑爆炸完毕后，选取樱桃卡片，放在2行,3列
void SafeCard(PlantType plant_type, int row, int col, bool force = true, int need_time = 100) {
    InsertOperation(
        [=]() {
            if (GetCardIndex(plant_type) < 0 || GetCardIndex(plant_type) > 9) return;
            if (!IsUsable(plant_type)) return;
            auto zombie = GetMainObject()->zombieArray();
            int zombie_count_max = GetMainObject()->zombieTotal();
            int countdown = -1;
            for (int i = 0; i < zombie_count_max; ++i, ++zombie) {
                if (zombie->type() == JACK_IN_THE_BOX_ZOMBIE &&
                    !zombie->isDisappeared() && !zombie->isDead() && zombie->state() == 16 && JudgeExplode(zombie, row, col, plant_type)) {
                    int temp = zombie->stateCountdown();
                    if (temp <= need_time && temp > countdown) {
                        countdown = temp;
                    }
                }
            }
            if (countdown == -1) {
                CardNIQ(plant_type, row, col, force);
            } else {
                SetNowTime();
                Delay(countdown + 1);
                SafeCard(plant_type, row, col, force, need_time);
            }
        },
        "SafeCard");
}

void SafeCard(PlantType plant_type, const std::vector<Grid> &lst, bool force = true, int need_time = 100) {
    if (lst.empty()) return;

    // lst若长度为1，转发
    if (lst.size() == 1) {
        SafeCard(plant_type, lst.at(0).row, lst.at(0).col, force, need_time);
        return;
    }

    InsertOperation(
        [=]() {
            if (GetCardIndex(plant_type) < 0 || GetCardIndex(plant_type) > 9) return;
            if (!IsUsable(plant_type)) return;
            auto zombie = GetMainObject()->zombieArray();
            int zombie_count_max = GetMainObject()->zombieTotal();
            std::vector<int> countdown_list(lst.size(), 0);
            for (int i = 0; i < zombie_count_max; ++i, ++zombie) {
                if (zombie->type() == JACK_IN_THE_BOX_ZOMBIE &&
                    !zombie->isDisappeared() && !zombie->isDead() &&
                    zombie->state() == 16) {
                    for (auto j = 0; j < lst.size(); j++) {
                        if (JudgeExplode(zombie, lst[j].row, lst[j].col, plant_type)) {
                            int temp = zombie->stateCountdown();
                            if (temp <= need_time && temp > countdown_list[j]) {
                                countdown_list[j] = temp;
                            }
                        }
                    }
                }
            }
            int min_wait = 999;
            for (auto j = 0; j < lst.size(); j++) {
                if (countdown_list[j] == 0) {
                    if (CardNIQ(plant_type, lst[j].row, lst[j].col, force)) return;
                } else if (countdown_list[j] < min_wait)
                    min_wait = countdown_list[j];
            }
            if (min_wait < 999)
                min_wait += 1;
            else
                min_wait = 5;  // 如果没有小丑爆炸却用卡失败，5cs后重试
            SetNowTime();
            Delay(min_wait);
            SafeCard(plant_type, lst, force, need_time);
        },
        "SafeCard");
}

// 使用多张卡片的SafeCard
void SafeCard(const std::vector<PlantType> &lst, int row, int col, bool force = true, int need_time = 100) {
    if (lst.empty()) return;

    // lst若长度为1，转发
    if (lst.size() == 1) {
        SafeCard(lst.at(0), row, col, force, need_time);
        return;
    }

    InsertOperation(
        [=]() {
            for (auto &&plant_type : lst) {
                if (GetCardIndex(plant_type) < 0 || GetCardIndex(plant_type) > 9) return;
                if (!IsUsable(plant_type)) return;
            }
            auto zombie = GetMainObject()->zombieArray();
            int zombie_count_max = GetMainObject()->zombieTotal();
            int countdown = -1;
            for (int i = 0; i < zombie_count_max; ++i, ++zombie) {
                if (zombie->type() == JACK_IN_THE_BOX_ZOMBIE &&
                    !zombie->isDisappeared() && !zombie->isDead() && zombie->state() == 16 && JudgeExplode(zombie, row, col, lst.at(0))) {
                    int temp = zombie->stateCountdown();
                    if (temp <= need_time && temp > countdown) {
                        countdown = temp;
                    }
                }
            }
            if (countdown == -1) {
                for (auto l : lst)
                    CardNIQ(l, row, col, force);
            } else {
                SetNowTime();
                Delay(countdown + 1);
                SafeCard(lst, row, col, force, need_time);
            }
        },
        "SafeCard");
}

// ***In Queue
// RecoverCard和SafeCard的结合
// ***使用示例：
// SetTime(300, 9);
// RecoverSafeCard(CHERRY_BOMB, 2, 3);
// //等待樱桃卡片CD恢复后，等待上下三行小丑爆炸完毕后，选取樱桃卡片，放在2行,3列
void RecoverSafeCard(PlantType plant_type, int row, float col, bool force = true, int need_time = 100) {
    InsertOperation(
        [=]() {
            if (GetCardIndex(plant_type) < 0 || GetCardIndex(plant_type) > 9) return;
            SetNowTime();
            if (IsUsable(plant_type))
                SafeCard(plant_type, row, col, force, need_time);
            else {
                Delay(CardCD(plant_type));
                SafeCard(plant_type, row, col, force, need_time);
            }
        },
        "RecoverSafeCard");
}

void RecoverSafeCard(PlantType plant_type, const std::vector<Grid> &lst, bool force = true, int need_time = 100) {
    InsertOperation(
        [=]() {
            if (GetCardIndex(plant_type) < 0 || GetCardIndex(plant_type) > 9) return;
            SetNowTime();
            if (IsUsable(plant_type))
                SafeCard(plant_type, lst, force, need_time);
            else {
                Delay(CardCD(plant_type));
                SafeCard(plant_type, lst, force, need_time);
            }
        },
        "RecoverSafeCard");
}

// ***In Queue
// 智能垫垫材
// 函数默认若垫材卡中含有花盆，则优先级最高（刚种植100cs内免疫啃食），其余垫材卡按参数传入的顺序
// 每条路的威胁程度计算方法为梯子数与小丑数相加
// ***使用示例：
// SetTime(824, 1);
// SmartStall({1, 2, 3, 4, 5}, {PUFF_SHROOM, SUN_SHROOM, FLOWER_POT,
// SCAREDY_SHROOM, SUNFLOWER}); //
// 于第一波824放置垫材，垫1、2、3、4、5路，使用小喷菇、阳光菇、花盆、胆小菇、向日葵这五张垫材
void SmartStall(int time, int wave, const std::vector<int> &r = {1, 2, 5, 6},
                const std::vector<PlantType> &t = {FLOWER_POT, SCAREDY_SHROOM,
                                                   PUFF_SHROOM, SUN_SHROOM}) {
    InsertOperation([=]() {
        std::vector<PlantType> types = t;
        if (std::find(types.begin(), types.end(), FLOWER_POT) != types.end()) {
            types.erase(std::find(types.begin(), types.end(), FLOWER_POT));
            types.insert(types.begin(), FLOWER_POT);
        }
        std::vector<int> rows = r;
        std::vector<int> count;
        for (int r : rows) {
            count.push_back(CountZombie(LADDER_ZOMBIE, r) +
                            CountZombie(JACK_IN_THE_BOX_ZOMBIE, r));
        }
        std::vector<int> card_row;
        std::vector<int> card_type;
        while (!rows.empty() && !types.empty()) {
            int max = count.front();
            int num = 0;
            for (int i = 1; i < rows.size(); i++) {
                if (count[i] > max) {
                    max = count[i];
                    num = i;
                }
            }
            InsertGuard ig(false);
            Card(types[0], rows[num], 9);
            rows.erase(rows.begin() + num);
            types.erase(types.begin());
            count.erase(count.begin() + num);
        }
    });
}

/** ===其它功能===**/

// ***Not In Queue
// 平均分配指定种类僵尸所在行数
// 默认只平均分配红眼
// *** 注意：只能读取DE、NE、PE、FE、RE五大场景，无法辨认的场景默认是ME
// *** 使用示例
// AverageSpawn(); //不填参数，平均分配每波红眼
// AverageSpawn({POGO_ZOMBIE, ZOMBONI}); //平均分配每波跳跳和冰车
void AverageSpawn(const std::vector<ZombieType> &lst = {GIGA_GARGANTUAR},
                  bool onlyHugeWave = false, int start_row = 0) {
    int scene = GetMainObject()->scene();
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
        if (onlyHugeWave && (wave != 9 && wave != 19)) continue;
        InsertTimeOperation(
            0, wave,
            [=]() {
                SafePtr<ZombieAZM> zombie =
                    (ZombieAZM *)GetMainObject()->zombieArray();
                int zombies_count_max = GetMainObject()->zombieTotal();
                int spawnRow[33];
                for (int i = 0; i < 33; ++i) {
                    if (std::find(zombieRow.begin(), zombieRow.end(),
                                  start_row - 1) == zombieRow.end()) {
                        spawnRow[i] = rand() % (zombieRow.size());
                    } else {
                        spawnRow[i] =
                            std::find(zombieRow.begin(), zombieRow.end(),
                                      start_row - 1) -
                            zombieRow.begin();
                    }
                }
                for (int i = 0; i < zombies_count_max; ++i, ++zombie) {
                    if (zombie->isDead() || zombie->isDisappeared()) continue;

                    // 只移动本波僵尸
                    if (zombie->existTime() > 100) continue;

                    // 检测类型
                    int type = zombie->type();
                    if (std::find(lst.begin(), lst.end(), type) == lst.end())
                        continue;

                    // 不移动水路僵尸
                    int row = zombie->row();
                    if ((scene == 2 || scene == 3) && (row == 2 || row == 3)) {
                        continue;
                    }

                    // 不移动前院场地的舞王
                    if ((scene == 0 || scene == 1) &&
                        (type == DANCING_ZOMBIE)) {
                        continue;
                    }

                    if (type >= 0 && type < 33 && spawnRow[type] >= 0 &&
                        spawnRow[type] < zombieRow.size()) {
                        int diff = zombieRow[spawnRow[type]] - row;
                        zombie->row() = zombieRow[spawnRow[type]];
                        zombie->ordinate() =
                            baseY + zombieRow[spawnRow[type]] * height;
                        zombie->layer() += 10000 * diff;
                        spawnRow[type]++;
                        if (spawnRow[type] >= zombieRow.size()) {
                            spawnRow[type] -= zombieRow.size();
                        }
                    }
                }
            },
            "AverageSpawn");
    }
}

// ***Not In Queue
// 设置弹坑CD
// 注意：该位置必须先有一个弹坑
// ***使用示例：
// SetDoomPlace(2, 4, 3000); // 将2-4位置的弹坑CD设置为3000cs
void SetDoomPlace(int row, int col, int cd) {
    SafePtr<Place> place = ((MainObjectAZM *)GetMainObject())->placeArray();
    for (int i = 0; i < ((MainObjectAZM *)GetMainObject())->placeTotal();
         ++i, ++place) {
        if (!place->isDisappeared() && place->row() == row - 1 &&
            place->col() == col - 1 && place->type() == 2) {
            place->countdown() = cd;
        }
    }
}

// *** Not In Queue
// 得到指定位置和类型的植物对象序列
// 当参数type为默认值-1时该函数无视南瓜、花盆、荷叶、咖啡豆
// 注意：玉米炮两格都算
// *** 使用示例：
// CheckPlant(3, 4)------如果三行四列有除南瓜花盆荷叶之外的植物时，返回该植物的对象序列，否则返回-1
// CheckPlant(3, 4, 47)---如果三行四列有春哥，返回其对象序列，如果有其他植物，返回-2，否则返回-1
int CheckPlant(int row, int col, int type = -1,
               const std::set<int> &ignore_types = {16, 30, 33, 35}) {
    auto plant = GetMainObject()->plantArray();
    int plants_count_max = GetMainObject()->plantCountMax();
    int plant_type;
    std::set<Grid> valid_grids = {row, col};
    if (type == COB_CANNON) valid_grids.insert({row, col - 1});
    for (int i = 0; i < plants_count_max; ++i, ++plant) {
        if ((!plant->isDisappeared()) && (!plant->isCrushed()) &&
            valid_grids.count({plant->row() + 1, plant->col() + 1}) > 0) {
            plant_type = plant->type();
            if (type == -1) {
                //如果植物存在	且不在排除类型中
                if (ignore_types.count(plant_type) == 0)
                    return i;  //返回植物的对象序列
            } else {
                if (plant_type == type) {
                    return i;
                } else if (ignore_types.count(plant_type) == 0 &&
                           ignore_types.count(type) == 0) {
                    return -2;
                }
            }
        }
        return -1;  //没有符合要求的植物返回-1
    }
}

// 某两波（包含）之间所有波的波长总和
// 如果输入不符合预期，或有波次尚未刷出，返回-1
int WaveLength(int start_wave, int end_wave) {
    if (start_wave > end_wave) return -1;
    for (int i = end_wave + 1; i >= start_wave; --i)
        if (NowTime(i) < -999) return -1;
    return NowTime(end_wave + 1) - NowTime(start_wave);
}

// 返回某波的波长
int WaveLength(int wave) {
    return WaveLength(wave, wave);
}

// 是否出现以下出怪之一
bool TypeOr(const std::vector<int> &zombie_types) {
    auto zombie_type_list = GetZombieTypeList();
    for (int z : zombie_types)
        if (z >= 0 && z <= 32 && zombie_type_list[z]) return true;
    return false;
}

// 是否包含以下所有出怪
bool TypeAnd(const std::vector<int> &zombie_types) {
    auto zombie_type_list = GetZombieTypeList();
    for (int z : zombie_types)
        if (z >= 0 && z <= 32 && !zombie_type_list[z]) return false;
    return true;
}

// 包含以下出怪的个数
int TypeCount(const std::vector<int> &zombie_types) {
    int count = 0;
    auto zombie_type_list = GetZombieTypeList();
    for (int z : zombie_types)
        if (z >= 0 && z <= 32 && zombie_type_list[z]) ++count;
    return count;
}

// 检查当前时间是否超过某个时间点
// -1：未知；0：未超过；1：超过
int Past(TimeWave time_wave) {
    int relative_time = NowTime(time_wave.wave);
    if (relative_time < -999) return -1;
    return relative_time >= time_wave.time ? 1 : 0;
}

void Append(std::vector<Grid> &dest, const std::vector<Grid> &src) {
    for (auto &&s : src) dest.emplace_back(s);
}

}  // namespace cresc