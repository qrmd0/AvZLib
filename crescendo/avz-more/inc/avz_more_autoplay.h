#pragma once
#include "avz_more.h"
#include "libavz.h"

#ifndef __AZM_ATP_VERSION__
#define __AZM_ATP_VERSION__ 220822
#endif

using namespace AvZ;
using namespace cresc;

namespace cresc {

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
        if (!plant->isCrushed() && !plant->isDisappeared() &&
            plant->type() == COB_CANNON) {
            int recoverTime = -1;
            auto animation_memory = GetPvzBase()
                                        ->animationMain()
                                        ->animationOffset()
                                        ->animationArray() +
                                    plant->animationCode();
            switch (plant->state()) {
                case 35:
                    recoverTime = 125 + plant->stateCountdown();
                    break;
                case 36:
                    recoverTime =
                        int(125 * (1 - animation_memory->circulationRate()) +
                            0.5) +
                        1;
                    break;
                case 37:
                    recoverTime = 0;
                    break;
                case 38:
                    recoverTime =
                        3125 +
                        int(350 * (1 - animation_memory->circulationRate()) +
                            0.5);
                    break;
            }
            if (recoverTime <= time) {
                counter++;
            }
        }
    }
    return counter;
}

// 返回真实倒计时（若为w9/w19，考虑大波倒计时接管的额外部分）
int RealCountdown() {
    if (RangeIn(GetMainObject()->wave(), {9, 19})) {
        if (GetMainObject()->refreshCountdown() > 5) {
            return GetMainObject()->refreshCountdown() - 5 + 750;
        } else {
            return GetMainObject()->hugeWaveCountdown();
        }
    }
    return GetMainObject()->refreshCountdown();
}

// 某格子植物受锤最短用时（不受锤返回0）
int GridHitCD(int row, int col, PlantType plant_type = PEASHOOTER) {
    int hitCD = 0;
    auto zombie = GetMainObject()->zombieArray();
    int zombies_count_max = GetMainObject()->zombieTotal();
    for (int i = 0; i < zombies_count_max; ++i, ++zombie) {
        if (!zombie->isDisappeared() && !zombie->isDead() && RangeIn(zombie->type(), {GARGANTUAR, GIGA_GARGANTUAR}) && zombie->state() == 70 && JudgeHit(zombie, row, col, plant_type)) {
            float animation_progress = GetAnimation(zombie)->circulationRate();
            if (animation_progress >= 0.648) continue;
            int remain_time = int((zombie->slowCountdown() > 0 ? 414 : 207) * (0.648 - animation_progress)) + 1 + zombie->freezeCountdown();
            if (remain_time > hitCD) hitCD = remain_time;
        }
    }
    return hitCD;
}

// 某格子植物受炸最短用时（不被炸返回0）
int GridJackCD(int row, int col, PlantType plant_type = PEASHOOTER) {
    int jackCD = 0;
    auto zombie = GetMainObject()->zombieArray();
    int zombies_count_max = GetMainObject()->zombieTotal();
    for (int i = 0; i < zombies_count_max; ++i, ++zombie) {
        if (!zombie->isDisappeared() && !zombie->isDead() && zombie->type() == JACK_IN_THE_BOX_ZOMBIE &&
            zombie->state() == 16 && JudgeExplode(zombie, row, col, plant_type)) {
            if (zombie->stateCountdown() + 1 > jackCD) jackCD = zombie->stateCountdown() + 1;
        }
    }
    return jackCD;
}

// 描述一组僵尸的描述信息
struct ZombieInfo {
    int count = 0;
    int x_min = 999;
    int x_max = -999;
    int hp_min = 999;
    int hp_max = -999;
    int hp_total = 0;
};

// 获得能攻击到某格植物的僵尸的描述信息
ZombieInfo GridZomInfo(int row, int col, int zombie_type, PlantType plant_type, bool ignore_iced, bool ignore_slow, int min_hp) {
    ZombieInfo info;
    auto zombie = GetMainObject()->zombieArray();
    int zombies_count_max = GetMainObject()->zombieTotal();
    for (int i = 0; i < zombies_count_max; ++i, ++zombie) {
        auto z = (ZombieAZM *)zombie;
        if (!zombie->isDisappeared() && !zombie->isDead() && zombie->type() == zombie_type && JudgeHit(zombie, row, col, plant_type)) {
            if (!RangeIn(zombie_type, {ZOMBONI}) && !z->notDying()) continue;  // 只有濒死冰车依旧有威胁
            if (ignore_iced && zombie->freezeCountdown() > 0) continue;
            if (ignore_slow && zombie->slowCountdown() > 0) continue;
            int hp = zombie->hp() + zombie->oneHp() + zombie->twoHp();
            if (hp < min_hp) continue;
            info.count++;
            int x = int(zombie->abscissa());
            if (x < info.x_min) info.x_min = x;
            if (x > info.x_max) info.x_max = x;
            if (hp < info.hp_min) info.hp_min = hp;
            if (hp > info.hp_max) info.hp_max = hp;
            info.hp_total += hp;
        }
    }
    return info;
}

// 最大血量
int GridZomHP(int row, int col, int zombie_type = GIGA_GARGANTUAR, PlantType plant_type = PEASHOOTER, bool ignore_iced = false, bool ignore_slow = false, int min_hp = 0) {
    auto &&info = GridZomInfo(row, col, zombie_type, plant_type, ignore_iced, ignore_slow, min_hp);
    if (info.count == 0) return 0;
    return info.hp_max;
}

// 格子锁
class GridLocker : public TickRunner {
   private:
    struct GLock {
        int status = 0;  // 0 - pending; 1 - effective; 2 - releasing; 3 - released
        std::vector<Grid> lst;
        std::function<bool(std::vector<Grid>)> release_cond;
        int release_delay;
        TimeWave effective_tw;
        TimeWave release_tw = {-1, -1};
    };
    int now_wave = 0;
    std::vector<GLock> lock_list;
    std::map<Grid, int> blocked_grid_list;

    void run() {
        for (auto &lock : lock_list) {
            if (lock.status == 0) {
                if (NowTime(lock.effective_tw.wave) >= lock.effective_tw.time) {
                    for (const auto &g : lock.lst) {
                        if (blocked_grid_list.count(g) == 0)
                            blocked_grid_list[g] = 1;
                        else
                            ++blocked_grid_list[g];
                    }
                    lock.status = 1;
                }
            } else if (lock.status == 1) {
                if (lock.release_cond(lock.lst)) {
                    lock.status = 2;
                    lock.release_tw = {NowTime(GetRunningWave()) + lock.release_delay, GetRunningWave()};
                }
            } else if (lock.status == 2) {
                if (NowTime(lock.release_tw.wave) >= lock.release_tw.time) {
                    lock.status = 3;
                    for (const auto &g : lock.lst) {
                        if (blocked_grid_list.count(g) > 0) {
                            blocked_grid_list[g]--;
                            if (blocked_grid_list[g] == 0) blocked_grid_list.erase(g);
                        }
                    }
                }
            }
        }
        auto iter = lock_list.begin();
        while (iter != lock_list.end()) {
            if ((*iter).status == 3) {
                iter = lock_list.erase(iter);
            } else {
                iter++;
            }
        }
        // if (GetRunningWave() != now_wave) {
        //     now_wave = GetRunningWave();
        //     log();
        // }
    }

   public:
    void start() {
        InsertOperation([=]() { pushFunc([=]() { run(); }); },
                        "GridLocker::start");
    }

    // 即将在某处种植某植物，加锁
    void addPlantLock(int time, int wave, PlantType plant_type, const std::vector<Grid> &lst) {
        InsertOperation([=]() {
            std::function<bool(std::vector<Grid>)> release_cond = [=](std::vector<Grid> lst) -> bool {
                for (auto l : lst)
                    if (GetPlantIndex(l.row, l.col, plant_type) >= 0) return true;
                return false;
            };
            lock_list.push_back({0,
                                 lst,
                                 release_cond,
                                 100,
                                 {time, wave}});
        },
                        "addPlantLock");
    }

    std::vector<Grid> getBlockedList() {
        std::vector<Grid> blocked_list;
        for (auto it : blocked_grid_list) {
            blocked_list.push_back(it.first);
        }
        return blocked_list;
    }

    bool gridBlocked(Grid g) {
        return blocked_grid_list.count(g) > 0;
    }

    void log() {
        std::stringstream ss;
        for (auto it : blocked_grid_list) ss << "{" << it.first.row << "," << it.first.col << "},";
        ShowErrorNotInQueue("blocked_grid_list: N=#, #", blocked_grid_list.size(), ss.str());
    }
};

// 用卡单位
class CardUnit : public GlobalVar, TickRunner {
   public:
    std::vector<TimeWave> card_ddl;
    std::vector<SafePtr<CardUnit>> friend_units;
    int trigger_count = 0;
    int trigger_max = 1;

    // 触发条件
    virtual bool triggered() {
        if (trigger_count > trigger_max) return false;
        if (card_ddl.empty()) {
            ++trigger_count;
            return true;
        }
        for (auto &&ddl : card_ddl) {
            if (Past(ddl) > 0) {
                ++trigger_count;
                return true;
            }
        }
        return false;
    }
    virtual void callback();  // 触发后动作
    virtual void run() {
        if (triggered()) {
            callback();
        }
    }

    virtual void enterFight() override {
        InsertGuard ig(false);
        pushFunc([=]() { run(); });
        pause();
    }
    virtual void start() {
        InsertOperation([=]() {
            SetNowTime();
            goOn();
        },
                        "CardUnit::start");
    }
};

}  // namespace cresc