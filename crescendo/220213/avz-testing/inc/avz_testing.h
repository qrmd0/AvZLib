/*
 * @Author: crescendo
 * @Date: 2022-05-28 08:45:31
 * @Last Modified by: crescendo
 * @Last Modified time: 2022-06-04 19:57:50
 *  __AVZ_VERSION__ == 220213
 */

/*
使用说明：
1. 将本文件置于AvZ目录inc文件夹下
2. 在Script()函数外添加 using namespace cresc;
*/

#pragma once
#include "libavz.h"

#ifndef __AZT_VERSION__
#define __AZT_VERSION__ 220605
#endif

namespace cresc
{
    struct ZombieRowInfo
    {
        ZombieType zombie_type;
        std::set<int> rows;
    };

    struct ZombieAZT : public Zombie
    {
        int &layer()
        {
            return (int &)((uint8_t *)this)[0x20];
        }
        int &animationIndex()
        {
            return (int &)((uint8_t *)this)[0x118];
        }
    };

    std::pair<int, int> getExplodeDefenseRange(PlantType type)
    {
        switch (type)
        {
        case TALL_NUT:
            return {-50, 30};
        case PUMPKIN:
            return {-60, 40};
        case COB_CANNON:
            return {-60, 80};
        default:
            return {-50, 10};
        }
    }

    std::pair<int, int> getHitDefenseRange(PlantType type)
    {
        switch (type)
        {
        case TALL_NUT:
            return {30, 60};
        case PUMPKIN:
            return {20, 80};
        case COB_CANNON:
            return {20, 120};
        default:
            return {30, 50};
        }
    }

    bool judgeExplode(SafePtr<Zombie> z, SafePtr<Plant> p)
    {
        int x = z->abscissa(), y = z->ordinate();
        int y_dis = 0;
        if (y < p->yi() - 60)
            y_dis = (p->yi() - 60) - y;
        else if (y > p->yi() + 20)
            y_dis = y - (p->yi() + 20);
        if (y_dis > 90)
            return false;
        int x_dis = sqrt(90 * 90 - y_dis * y_dis);
        auto def = getExplodeDefenseRange(PlantType(p->type()));
        return p->xi() + def.first - x_dis <= x && x <= p->xi() + def.second + x_dis;
    }

    bool judgeHammer(SafePtr<Zombie> z, SafePtr<Plant> p)
    {
        if (z->row() != p->row())
            return false;
        std::pair<int, int> z_atk = {z->abscissa() - 30, z->abscissa() - 30 + 89};
        auto p_def = getHitDefenseRange(PlantType(p->type()));
        p_def.first += p->xi();
        p_def.second += p->xi();
        bool result = std::max(z_atk.first, p_def.first) <= std::min(z_atk.second, p_def.second);
        // if (result)
        // {
        //     AvZ::ShowErrorNotInQueue("z_x=#, p_x=#, z_atk=#~#, pdef=#~#", z->abscissa(), p->xi(), z_atk.first, z_atk.second, p_def.first, p_def.second);
        // }

        return result;
    }

    void killZombie(SafePtr<Zombie> zombie)
    {
        zombie->state() = 3;
    }

    // 僵尸数据记录
    class ZombieData : public AvZ::TickRunner
    {
    private:
        int callback_num = 0;
        void run(std::function<bool(SafePtr<Zombie>)> condition, std::function<void(SafePtr<Zombie>)> callback)
        {
            SafePtr<Zombie> zombie = AvZ::GetMainObject()->zombieArray();
            int zombies_count_max = AvZ::GetMainObject()->zombieTotal();
            for (int i = 0; i < zombies_count_max; ++i, ++zombie)
            {
                if (zombie->isDead() || zombie->isDisappeared())
                    continue;
                if (condition(zombie))
                {
                    callback(zombie);
                }
            }
        }

    public:
        // ***In Queue
        void start(std::function<bool(SafePtr<Zombie>)> condition, std::function<void(SafePtr<Zombie>)> callback)
        {
            AvZ::InsertOperation([=]()
                                 { pushFunc([=]()
                                            { run(condition, callback); }); },
                                 "ZombieData::start");
        }
        int getCallbackNum()
        {
            return callback_num;
        }
        int incCallbackNum(int diff = 1)
        {
            callback_num += diff;
        }
    };

    // 小丑数据记录
    class JackData : public ZombieData
    {
    private:
        std::vector<std::pair<AvZ::Grid, int>> jack_stats;
        void incJackNum(const AvZ::Grid &plant_position)
        {
            for (auto it = jack_stats.begin(); it != jack_stats.end(); it++)
            {
                if ((*it).first == plant_position)
                {
                    (*it).second++;
                    return;
                }
            }
        }
        void run(const std::vector<AvZ::Grid> &plant_list)
        {
            SafePtr<Plant> plant = AvZ::GetMainObject()->plantArray();
            std::vector<SafePtr<Plant>> final_plant_list;
            for (const auto &p : plant_list)
            {
                int idx = AvZ::GetPlantIndex(p.row, p.col);
                if (idx >= 0)
                {
                    final_plant_list.push_back(plant + idx);
                }
            }
            SafePtr<Zombie> zombie = AvZ::GetMainObject()->zombieArray();
            int zombies_count_max = AvZ::GetMainObject()->zombieTotal();
            for (int i = 0; i < zombies_count_max; ++i, ++zombie)
            {
                if (zombie->isDead() || zombie->isDisappeared())
                    continue;
                if ((zombie->type() == JACK_IN_THE_BOX_ZOMBIE && zombie->state() == 16))
                {
                    for (auto p : final_plant_list)
                    {
                        if (judgeExplode(zombie, p))
                        {
                            incJackNum({p->row() + 1, p->col() + 1});
                        }
                    }
                    killZombie(zombie);
                }
            }
        }

    public:
        // ***In Queue
        void start(const std::vector<AvZ::Grid> &plant_list)
        {
            for (auto p : plant_list)
            {
                jack_stats.push_back({p, 0});
            }
            AvZ::InsertOperation([=]()
                                 { pushFunc([=]()
                                            { run(plant_list); }); },
                                 "JackData::start");
        }

        std::string stats()
        {
            std::stringstream ss;
            ss << "[小丑统计信息]\n";
            bool empty = true;
            bool first = true;
            for (const auto &s : jack_stats)
            {
                if (s.second == 0)
                    continue;
                if (first)
                    first = false;
                else
                    ss << "\n";
                ss << s.first.row << "-" << s.first.col << ": " << s.second;
                if (empty)
                    empty = false;
            }
            if (empty)
            {
                ss << "未出现爆炸\n";
            }
            return ss.str();
        }
    };

    struct PlantAttackInfo
    {
        int shoot_countdown;
        std::set<int> trigger_time = {1};
    };

    struct PlantModifyInfo
    {
        PlantType type;
        AvZ::Grid plant_position;
        int modify_delay;
    };

    struct PlantAZT : public Plant
    {
        int &attackCountdown()
        {
            return (int &)((uint8_t *)this)[0x58];
        }
    };

    // 将植物设为永动攻击状态
    class AlwaysAttack : public AvZ::TickRunner
    {
    private:
        // 满足修改条件后，延迟若干cs后再修改
        static const int MODIFY_DELAY = 5;

        // 由于技术原因，暂时不支持以下植物：仙人掌、裂荚、三线、玉米、香蒲、地刺
        const std::map<PlantType, PlantAttackInfo> PLANT_ATK_INFO = {{PEASHOOTER, {35}}, {SNOW_PEA, {35}}, {REPEATER, {26, {26, 1}}}, {PUFF_SHROOM, {29}}, {SEA_SHROOM, {29}}, {FUME_SHROOM, {50}}, {SCAREDY_SHROOM, {25}}, {STARFRUIT, {40}}, {CABBAGE_PULT, {26}}, {MELON_PULT, {36}}, {WINTER_MELON, {36}}, {GATLING_PEA, {100}}, {GLOOM_SHROOM, {200}}};

        SafePtr<PlantAZT> getValidPlant(const AvZ::Grid &p, int type = -1)
        {
            auto idx = AvZ::GetPlantIndex(p.row, p.col);
            if (idx < 0)
                return nullptr;
            SafePtr<PlantAZT> plant = (PlantAZT *)AvZ::GetMainObject()->plantArray();
            plant += idx;
            bool valid_plant = true;
            if (type != -1)
            {
                if (plant->type() != type)
                    valid_plant = false;
                if (type == CACTUS)
                {
                    if (!(plant->state() >= 30 && plant->state() <= 33))
                        valid_plant = false;
                }
                else if (type == SCAREDY_SHROOM)
                {
                    if (plant->state() != 1)
                        valid_plant = false;
                }
                else
                {
                    if (plant->state() != 0)
                        valid_plant = false;
                }
            }
            return valid_plant ? plant : nullptr;
        }

        void run(const std::vector<AvZ::Grid> &plant_position)
        {
            for (auto t = triggered_plant_list.begin(); t != triggered_plant_list.end();)
            {
                if (t->modify_delay == 0)
                {
                    auto plant = getValidPlant(t->plant_position, t->type);
                    if (plant == nullptr)
                        continue;
                    auto it = PLANT_ATK_INFO.find(t->type);
                    if (it != PLANT_ATK_INFO.end() && plant->shootCountdown() <= 1)
                    {
                        plant->shootCountdown() = std::max(it->second.shoot_countdown - MODIFY_DELAY, 0);
                    }
                    triggered_plant_list.erase(t);
                }
                else
                {
                    t->modify_delay--;
                    t++;
                }
            }
            for (const auto &p : plant_position)
            {
                auto plant = getValidPlant(p);
                if (plant == nullptr)
                    continue;
                auto it = PLANT_ATK_INFO.find((PlantType)plant->type());
                if (it != PLANT_ATK_INFO.end())
                {
                    auto trigger_time = it->second.trigger_time;
                    if (trigger_time.find(plant->attackCountdown()) != trigger_time.end())
                    {
                        triggered_plant_list.push_back({(PlantType)plant->type(), p, MODIFY_DELAY});
                    }
                }
            }
        }
        std::vector<PlantModifyInfo> triggered_plant_list;

    public:
        void start(const std::vector<AvZ::Grid> &plant_list)
        {
            if (!plant_list.empty())
            {
                AvZ::InsertOperation([=]()
                                     { pushFunc([=]()
                                                { run(plant_list); }); },
                                     "AlwaysAttack::start");
            }
        }
    };

    // 暂停出怪
    void
    zombieSpawnPause(bool f = true)
    {
        if (f)
        {
            AvZ::WriteMemory<uint8_t>(0xeb, 0x004265dc);
        }
        else
        {
            AvZ::WriteMemory<uint8_t>(0x74, 0x004265dc);
        }
    }

    // AvZ::IsZombieExist有bug，重新实现
    bool checkZombieExist(int type = -1, int row = -1)
    {
        SafePtr<Zombie> zombie = AvZ::GetMainObject()->zombieArray();
        int zombies_count_max = AvZ::GetMainObject()->zombieTotal();
        for (int i = 0; i < zombies_count_max; ++i, ++zombie)
        {
            if (!zombie->isDead() && !zombie->isDisappeared())
                if (type < 0 && row < 0)
                {
                    return true;
                }
                else if (type >= 0 && row >= 0)
                {
                    if (zombie->row() == row - 1 && zombie->type() == type)
                        return true;
                }
                else if (type < 0 && row >= 0)
                {
                    if (zombie->row() == row - 1)
                        return true;
                }
                else // if (type >= 0 && row < 0)
                {
                    if (zombie->type() == type)
                        return true;
                }
        }
        return false;
    }

    // 锁定刷新工具，精准控制刷新波长
    class LockWave : public AvZ::TickRunner
    {
    private:
        void run(const std::vector<ZombieType> &zombie_type_list)
        {
            bool zombie_found = false;
            if (zombie_type_list.empty())
            {
                zombie_found = checkZombieExist();
            }
            else
            {
                for (auto z : zombie_type_list)
                {
                    if (checkZombieExist(z))
                    {
                        zombie_found = true;
                        break;
                    }
                }
            }
            int refreshed_time = AvZ::NowTime(AvZ::GetRunningWave());
            if (zombie_found && refreshed_time >= 400)
            {
                zombieSpawnPause(true);
            }
            else
            {
                zombieSpawnPause(false);
            }
        }

    public:
        void start(const std::vector<ZombieType> &zombie_type_list = {})
        {
            AvZ::InsertOperation([=]()
                                 { pushFunc([=]()
                                            { run(zombie_type_list); }); },
                                 "LockWave::start");
        }
    };

    // 植物无敌
    void plantInvincible(bool f = true)
    {
        if (f)
        {
            AvZ::WriteMemory(std::array<uint8_t, 3>{0x46, 0x40, 0x00}, 0x0052fcf1);
            AvZ::WriteMemory<uint8_t>(0xeb, 0x0041cc2f);
            AvZ::WriteMemory<uint8_t>(0xeb, 0x005276ea);
            AvZ::WriteMemory(std::array<uint8_t, 3>{0x90, 0x90, 0x90}, 0x0046cfeb);
            AvZ::WriteMemory(std::array<uint8_t, 3>{0x90, 0x90, 0x90}, 0x0046d7a6);
            AvZ::WriteMemory<uint8_t>(0xeb, 0x0052e93b);
            AvZ::WriteMemory<uint8_t>(0x70, 0x0045ee0a);
            AvZ::WriteMemory<uint8_t>(0x00, 0x0045ec66);
            AvZ::WriteMemory(std::array<uint8_t, 3>{0xc2, 0x04, 0x00}, 0x00462b80);
        }
        else
        {
            AvZ::WriteMemory(std::array<uint8_t, 3>{0x46, 0x40, 0xfc}, 0x0052fcf1);
            AvZ::WriteMemory<uint8_t>(0x74, 0x0041cc2f);
            AvZ::WriteMemory<uint8_t>(0x75, 0x005276ea);
            AvZ::WriteMemory(std::array<uint8_t, 3>{0x29, 0x50, 0x40}, 0x0046cfeb);
            AvZ::WriteMemory(std::array<uint8_t, 3>{0x29, 0x4e, 0x40}, 0x0046d7a6);
            AvZ::WriteMemory<uint8_t>(0x74, 0x0052e93b);
            AvZ::WriteMemory<uint8_t>(0x75, 0x0045ee0a);
            AvZ::WriteMemory<uint8_t>(0xce, 0x0045ec66);
            AvZ::WriteMemory(std::array<uint8_t, 3>{0x53, 0x55, 0x8b}, 0x00462b80);
        }
    }

    // 僵尸进家后不食脑
    void forbidEnterHome(bool f = true)
    {
        if (f)
        {
            AvZ::WriteMemory(std::array<uint8_t, 2>{0x90, 0x90}, 0x52b308);
        }
        else
        {
            AvZ::WriteMemory(std::array<uint8_t, 2>{0x74, 0x07}, 0x52b308);
        }
    }

    // 僵尸不掉钱
    void forbidItemDrop(bool f = true)
    {
        if (f)
        {
            AvZ::WriteMemory(std::array<uint8_t, 2>{0x90, 0xe9}, 0x0041d025);
        }
        else
        {
            AvZ::WriteMemory(std::array<uint8_t, 2>{0x0f, 0x8f}, 0x0041d025);
        }
    }

    // 自由种植（无视阳光+取消冷却+紫卡直种+随意放置+蘑菇免唤醒）
    void freePlanting(bool f = true)
    {
        if (f)
        {
            // 无视阳光
            AvZ::WriteMemory<uint8_t>(0x70, 0x0041ba72);
            AvZ::WriteMemory<uint8_t>(0x3b, 0x0041ba74);
            AvZ::WriteMemory<uint8_t>(0x91, 0x0041bac0);
            AvZ::WriteMemory<uint8_t>(0x80, 0x00427a92);
            AvZ::WriteMemory<uint8_t>(0x80, 0x00427dfd);
            AvZ::WriteMemory<uint8_t>(0xeb, 0x0042487f);

            // 取消冷却
            AvZ::WriteMemory<uint8_t>(0x70, 0x00487296);
            AvZ::WriteMemory<uint8_t>(0xeb, 0x00488250);

            // 紫卡直种
            AvZ::WriteMemory(std::array<uint8_t, 3>{0xb0, 0x01, 0xc3}, 0x0041d7d0);
            AvZ::WriteMemory<uint8_t>(0xeb, 0x0040e477);

            // 随意放置
            AvZ::WriteMemory<uint8_t>(0x81, 0x0040fe30);
            AvZ::WriteMemory<uint8_t>(0xeb, 0x00438e40);
            AvZ::WriteMemory<uint8_t>(0x8d, 0x0042a2d9);

            // 蘑菇免唤醒
            AvZ::WriteMemory<uint8_t>(0xeb, 0x45de8e);
        }
        else
        {
            // 取消无视阳光
            AvZ::WriteMemory<uint8_t>(0x7f, 0x0041ba72);
            AvZ::WriteMemory<uint8_t>(0x2b, 0x0041ba74);
            AvZ::WriteMemory<uint8_t>(0x9e, 0x0041bac0);
            AvZ::WriteMemory<uint8_t>(0x8f, 0x00427a92);
            AvZ::WriteMemory<uint8_t>(0x8f, 0x00427dfd);
            AvZ::WriteMemory<uint8_t>(0x74, 0x0042487f);

            // 恢复冷却
            AvZ::WriteMemory<uint8_t>(0x7e, 0x00487296);
            AvZ::WriteMemory<uint8_t>(0x75, 0x00488250);

            // 取消紫卡直种
            AvZ::WriteMemory(std::array<uint8_t, 3>{0x51, 0x83, 0xf8}, 0x0041d7d0);
            AvZ::WriteMemory<uint8_t>(0x74, 0x0040e477);

            // 取消随意放置
            AvZ::WriteMemory<uint8_t>(0x84, 0x0040fe30);
            AvZ::WriteMemory<uint8_t>(0x74, 0x00438e40);
            AvZ::WriteMemory<uint8_t>(0x84, 0x0042a2d9);

            // 取消蘑菇免唤醒
            AvZ::WriteMemory<uint8_t>(0x74, 0x45de8e);
        }
    }

    // 玉米炮无冷却
    void cobCannonNoCD(bool f = true)
    {
        if (f)
        {
            AvZ::WriteMemory<uint8_t>(0x80, 0x0046103b);
        }
        else
        {
            AvZ::WriteMemory<uint8_t>(0x85, 0x0046103b);
        }
    }

    // ***Not In Queue
    // 移动单波僵尸
    void moveZombieOne(ZombieType zombie_type, const std::set<int> &rows, int height, int baseY)
    {
        SafePtr<ZombieAZT> zombie = (ZombieAZT *)AvZ::GetMainObject()->zombieArray();
        int zombies_count_max = AvZ::GetMainObject()->zombieTotal();
        auto it = rows.begin();
        std::advance(it, rand() % (rows.size()));
        for (int i = 0; i < zombies_count_max; ++i, ++zombie)
        {
            if (zombie->isDead() || zombie->isDisappeared())
                continue;

            // 只移动本波僵尸
            if (zombie->existTime() > 100)
                continue;

            // 检测类型
            if (zombie->type() != zombie_type)
                continue;

            int row = zombie->row();
            int new_row = *it;
            int diff = new_row - row;
            if (diff != 0)
            {
                zombie->row() = new_row;
                zombie->ordinate() = baseY + new_row * height;
                zombie->layer() += 10000 * diff;
                it++;
                if (it == rows.end())
                {
                    it = rows.begin();
                }
            }
        }
    }

    // 将僵尸移动至特定行
    // ***使用示例：
    // moveZombieRow({{JACK_IN_THE_BOX_ZOMBIE, {2, 5}}, {LADDER_ZOMBIE, {1, 6}}});
    void moveZombieRow(const std::vector<ZombieRowInfo> &zombie_row_info)
    {
        for (const auto &zri : zombie_row_info)
        {
            auto zombie_type = zri.zombie_type;
            auto rows = zri.rows;
            if (zombie_type < 0 || zombie_type > 32)
                continue;
            std::set<int> rows_converted; // 0~5
            for (auto r : rows)
            {
                if (r >= 1 && r <= 6)
                {
                    rows_converted.emplace(r - 1);
                }
            }
            if (rows_converted.empty())
                continue;
            int scene = AvZ::GetMainObject()->scene();
            int height = scene == 0 || scene == 1 ? 100 : 85;
            int baseY = scene == 0 || scene == 1 || scene == 2 || scene == 3 ? 50 : 40;
            for (auto w = 1; w <= 20; w++)
            {
                AvZ::InsertTimeOperation(
                    1, w, [=]()
                    { moveZombieOne(zombie_type, rows_converted, height, baseY); },
                    "moveZombieOne");
            }
        }
    }

    // 将僵尸移动至特定行
    // ***使用示例：
    // moveZombieRow(JACK_IN_THE_BOX_ZOMBIE, {2, 5});
    void moveZombieRow(ZombieType zombie_type, const std::set<int> &rows)
    {
        moveZombieRow({{zombie_type, rows}});
    }

    void killAllZombieByType(const std::set<ZombieType> &type_list)
    {
        SafePtr<Zombie> zombie = AvZ::GetMainObject()->zombieArray();
        int zombies_count_max = AvZ::GetMainObject()->zombieTotal();
        for (int i = 0; i < zombies_count_max; ++i, ++zombie)
        {
            if (zombie->isDead() || zombie->isDisappeared())
                continue;
            if (type_list.empty() || type_list.find((ZombieType)zombie->type()) != type_list.end())
                zombie->state() = 3;
        }
    }

    // 秒杀全部僵尸
    // ***使用示例：
    // killAllZombie({10, 20}); // w10、w20僵尸刷出后立刻秒杀
    // killAllZombie({1}, {}, 401); // w1僵尸刷出401cs后秒杀
    // killAllZombie({10, 20}, {BUNGEE_ZOMBIE}); // w10、w20蹦极僵尸刷出后立刻秒杀
    void killAllZombie(const std::set<int> &wave, std::set<ZombieType> type_list = {}, int time = 1)
    {
        if (time < 0)
        {
            time = 1;
        }
        for (auto w : wave)
        {
            if (w >= 1 && w <= 20)
            {
                AvZ::InsertTimeOperation(
                    time, w, [=]()
                    { killAllZombieByType(type_list); },
                    "killAllZombie");
            }
        }
    }

    enum UnifyMode
    {
        BIG_WAVE = 0,
        NORMAL_WAVE,
    };

    // 将所有波次统一为非旗帜波或旗帜波
    // ***使用示例
    // unifyAllWaves(BIG_WAVE); // 将所有波次转换为旗帜波
    void unifyAllWaves(UnifyMode mode)
    {
        bool f = (mode == BIG_WAVE);
        int x_offset = f ? 40 : -40;
        std::set<ZombieType> exclude_zombie_list = {
            FLAG_ZOMBIE,
            POLE_VAULTING_ZOMBIE,
            ZOMBONI,
            CATAPULT_ZOMBIE,
            GARGANTUAR,
            GIGA_GARGANTUAR,
            BUNGEE_ZOMBIE};

        for (int w = 1; w <= 20; w++)
        {
            if (f && (w == 10 || w == 20))
                continue;
            if (!f && !(w == 10 || w == 20))
                continue;
            AvZ::InsertTimeOperation(
                1, w, [=]()
                {
                SafePtr<Zombie> zombie = AvZ::GetMainObject()->zombieArray();
                int zombies_count_max = AvZ::GetMainObject()->zombieTotal();
                for (int i = 0; i < zombies_count_max; ++i, ++zombie)
                {
                    if (zombie->isDead() || zombie->isDisappeared())
                        continue;
                    if (zombie->existTime() > 100)
                        continue;
                    if (exclude_zombie_list.find((ZombieType)zombie->type()) != exclude_zombie_list.end())
                        continue;
                    zombie->abscissa() += x_offset;
                } },
                "unifyAllWaves");
        }
    }

    // 根据出怪列表记录僵尸总数，可指定要记录的僵尸类别与要忽略的波次
    // ***使用示例：
    // sum += countZombie({JACK_IN_THE_BOX_ZOMBIE}, {10, 20}); // 统计非旗帜波的小丑总数
    int countZombie(std::set<ZombieType> type_list = {}, std::set<int> ignore_wave = {})
    {
        int count = 0;
        uint32_t *current_zombie = AvZ::GetMainObject()->zombieList();
        for (int wave = 1; wave <= 20; wave++)
        {
            if (!ignore_wave.empty() && ignore_wave.find(wave) != ignore_wave.end())
            {
                current_zombie += 50;
                continue;
            }
            for (int j = 0; j < 50; j++)
            {
                if (type_list.empty() || type_list.find((ZombieType)*current_zombie) != type_list.end())
                {
                    count++;
                }
                current_zombie++;
            }
        }
        return count;
    }

    struct AnimationMainAZT : public AnimationMain
    {
    public:
        int &animationTotal()
        {
            return (int &)((uint8_t *)this)[0x4];
        }
    };

    struct AnimationAZT : public Animation
    {
        // 动画循环率
        int &animationObjectIndex()
        {
            return (int &)((uint8_t *)this)[0x9C];
        }
    };

    AnimationAZT *getAnimationFromZombie(SafePtr<Zombie> zombie)
    {
        ZombieAZT *zombieazt_ptr = (ZombieAZT *)zombie;
        int animation_index = zombieazt_ptr->animationIndex();
        AnimationAZT *animation = (AnimationAZT *)AvZ::GetPvzBase()->animationMain()->animationOffset()->animationArray();
        AnimationMainAZT *animation_main = (AnimationMainAZT *)AvZ::GetPvzBase()->animationMain();
        int animation_count_max = animation_main->animationTotal();
        for (int i = 0; i < animation_count_max; i++, animation++)
        {
            if (animation->animationObjectIndex() == animation_index)
                return animation;
        }
        return nullptr;
    }

} // namespace cresc

/*
getDefenseRange and judgeExplode are taken from 炸率测试-v1.0rc2.cpp with the
following License.

MIT License

Copyright (c) 2022 Reisen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/