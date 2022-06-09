/*
 * @Author: crescendo
 * @Date: 2022-05-29 15:08:18
 * @Last Modified by: crescendo
 * @Last Modified time: 2022-06-03 19:00:37
 * 使用 AvZ Testing 框架测试某种操作的砸率
 * 测试阵型：FE18守全 炮伤优化解 by Reisen
 * 参考视频：https://www.bilibili.com/video/BV1Hr4y1x7x4
 * 参考脚本：https://pastebin.com/aRjWMquS
 * 测试对象：测定该解法中w16~w19的片段，尤其关注w16/w17出生的巨人的砸率
 * 测试方法：一次选卡内重复该片段四次，分别对应w1~w4, w5~w8, w10~w13, w14~17
 *          每个片段的末尾秒杀所有僵尸
 *          秒杀不在测试范围内的w9, w18, w19, w20僵尸
 */

#include <bits/stdc++.h>
#include "avz.h"
#include "avz_testing.h"
#include "avz_logger.h"
#include "GetZombieAbscissas.h"

using namespace AvZ;
using namespace cresc;

/* 配置部分开始 */
#define TOTAL_TEST_ROUND 400
// 每一批次测试的选卡数
#define SET_ALL_AS_BIG_WAVE 0
// 0 - 将所有波次转为普通波；1 - 将所有波次转为旗帜波
#define SKIP_TICK 1
// 0 - 慢放，用于检查情况；1 - 开启跳帧测试
#define SET_WAVELENGTH 0
// 锁定每一波的波长，支持范围[601, 2500]，超过范围代表不锁定
#define EPOCH 10
// 定期输出当前统计信息的频率（指定输出总次数）
std::vector<Grid> test_plant_list =
    {{1, 7}, {2, 7}, {5, 7}, {6, 7}};
// 要统计砸率的植物列表
std::vector<int> hammer_stats(21, 0);
std::vector<int> ice_time_list = {-1};
// 要测试的用冰时机，0代表不用冰
std::vector<int> zombie_type_list = {POLE_VAULTING_ZOMBIE, DANCING_ZOMBIE, ZOMBONI, POGO_ZOMBIE, JACK_IN_THE_BOX_ZOMBIE, LADDER_ZOMBIE, CATAPULT_ZOMBIE, DOLPHIN_RIDER_ZOMBIE, GARGANTUAR, GIGA_GARGANTUAR};
// 指定要测试的僵尸组合（极限出怪）
/* 配置部分结束 */

Logger Log;
ZombieData zd;
int completed_round = 0;
int total_zombie_num = 0;

using namespace std;

class ZombieIterator : public iterator<forward_iterator_tag, SafePtr<Zombie>>
{
private:
    SafePtr<Zombie> cur;

    ZombieIterator(SafePtr<Zombie> cur_) : cur(cur_) {}

    void forward()
    {
        ++cur;
        while (cur != GetMainObject()->zombieArray() + GetMainObject()->zombieTotal() &&
               (cur->isDead() || cur->isDisappeared()))
            ++cur;
    }

public:
    ZombieIterator()
    {
        cur = GetMainObject()->zombieArray();
        if (cur->isDead() || cur->isDisappeared())
            forward();
    }

    SafePtr<Zombie> operator*() const
    {
        return cur;
    }

    SafePtr<Zombie> operator++()
    {
        forward();
        return cur;
    }

    SafePtr<Zombie> operator++(int)
    {
        auto tmp = cur;
        forward();
        return tmp;
    }

    bool operator==(ZombieIterator &rhs)
    {
        return cur.toUnsafe() == rhs.cur.toUnsafe();
    }

    bool operator!=(ZombieIterator &rhs)
    {
        return !(*this == rhs);
    }

    static ZombieIterator begin()
    {
        return ZombieIterator();
    }

    static ZombieIterator end()
    {
        return ZombieIterator(GetMainObject()->zombieArray() + GetMainObject()->zombieTotal());
    }
};

#define cob pao_operator.pao

template <class... Ts>
void Card(const vector<PlantType> &plant_types, Ts... args)
{
    for (auto p : plant_types)
        Card(p, args...);
}

void Card(vector<PlantType> plant_types, vector<Position> args)
{
    for (auto p : plant_types)
        Card(p, args);
}

void I()
{
    Delay(-299);
    ice_filler.coffee();
    Delay(299);
    SetPlantActiveTime(ICE_SHROOM, -1);
}

vector<int> _fodder_cards;
void SetFodderCards(const vector<PlantType> &fodder_cards)
{
    InsertOperation([=]()
                    {
        _fodder_cards.resize(fodder_cards.size());
        transform(fodder_cards.begin(), fodder_cards.end(), _fodder_cards.begin(), [](auto p){
            bool is_imitator = (p >= M_PEASHOOTER);
            return GetSeedIndex(is_imitator ? p - M_PEASHOOTER : p, is_imitator);
        }); });
}

void Fodder(PlantType type, int row, int col, int removal_delay = 266)
{
    Card(type, row, col);
    Delay(removal_delay);
    Shovel(row, col);
    Delay(-removal_delay);
}

void C(vector<int> rows = {}, int removal_delay = 266, int col = 9, bool smart = true)
{
    if (rows.empty())
    {
        int scene = GetMainObject()->scene();
        rows = (scene == 2 || scene == 3) ? vector<int>{1, 2, 5, 6} : vector<int>{1, 2, 3, 4, 5};
    }
    InsertOperation([=]()
                    {
        map<int, pair<int, int>> rows_;
        for(int r : rows)
            rows_[r] = {0, 0};
        for(auto z : ZombieIterator())
            if(rows_.count(z->row() + 1))
                if(z->type() == LADDER_ZOMBIE)
                    rows_[z->row() + 1].first++;
                else if(z->type() == JACK_IN_THE_BOX_ZOMBIE || z->type() == FOOTBALL_ZOMBIE)
                    rows_[z->row() + 1].second++;
        vector<pair<pair<int, int>, int>> rows__;
        for(auto t : rows_)
            rows__.push_back({t.second, t.first});
        if(smart)
            sort(rows__.begin(), rows__.end(), greater<>());
        auto sa = GetMainObject()->seedArray();
        auto cur = _fodder_cards.begin();
        for(auto t : rows__) {
            int r = t.second;
            while(cur != _fodder_cards.end() && !sa[*cur].isUsable())
                cur++;
            if(cur == _fodder_cards.end())
                ShowErrorNotInQueue("无可用垫材");
            else
                SetNowTime(), Fodder(PlantType(sa[*cur].type()), r, col, removal_delay);
        } });
}

namespace SmartEvasion
{
    class ZombieAdd : public Zombie
    {
    public:
        int &wave()
        {
            return (int &)((uint8_t *)this)[0x6C];
        }
    };

    bool judge(Zombie *z, const vector<int> &evade_waves)
    {
        return std::find(evade_waves.begin(), evade_waves.end(),
                         ((ZombieAdd *)z)->wave() + 1) != evade_waves.end();
    }

    float calculate_pos(int row, const vector<int> &evade_giga_waves,
                        const vector<int> &evade_normal_waves)
    {
        int scene = GetMainObject()->scene();
        auto dis = (scene == 2 || scene == 3) ? vector<int>{119, 126, 126} : vector<int>{112, 126, 126};
        int pos = 720;
        for (auto z : ZombieIterator())
            if (row - 1 <= z->row() + 1 && z->row() + 1 <= row + 1)
                if ((z->type() == GIGA_GARGANTUAR && judge(z, evade_giga_waves)) ||
                    (z->type() == GARGANTUAR && judge(z, evade_normal_waves)))
                {
                    int id = z.toUnsafe() - GetMainObject()->zombieArray().toUnsafe();
                    pos = min(pos, int(z->abscissa()) - dis[z->row() - row + 2]);
                }
        return (pos - 1) / 80.0;
    }

    void smart_evasion(int row, const vector<int> &evade_giga_waves,
                       const vector<int> &evade_normal_waves)
    {
        InsertOperation([=]()
                        {
        int scene = GetMainObject()->scene();
        auto dis = (scene == 2 || scene == 3) ? vector<int>{119, 126, 126} : vector<int>{112, 126, 126};
        // TODO
        int pos = 720;
        for (auto z : ZombieIterator())
            if (row - 1 <= z->row() + 1 && z->row() + 1 <= row + 1)
                if ((z->type() == GIGA_GARGANTUAR && judge(z, evade_giga_waves)) ||
                    (z->type() == GARGANTUAR && judge(z, evade_normal_waves)))
                {
                    int id = z.toUnsafe() - GetMainObject()->zombieArray().toUnsafe();
                    auto t = GetZombieAbscissas(id, 374);
                    int x = (t.empty() ? z->abscissa() : t[373]);
                    pos = min(pos, x - dis[z->row() - row + 2]);
                }
        InsertGuard _(false);
        cob(row, (pos - 1) / 80.0); });
    }

    template <class... Ts>
    void smart_evasion(vector<int> rows, const vector<int> &evade_giga_waves,
                       const vector<int> &evade_normal_waves)
    {
        for (int r : rows)
            smart_evasion(r, evade_giga_waves, evade_normal_waves);
    }
};

using SmartEvasion::smart_evasion;

void printLog()
{
    std::stringstream ss;
    ss << "总共测试了" << total_zombie_num << "只红眼。\n砸炮数： " << zd.getCallbackNum() << "\n";
    bool first = true;
    for (int i = 1; i < hammer_stats.size(); i++)
    {
        if (hammer_stats[i] != 0)
        {
            if (first)
                first = false;
            else
                ss << "\n";
            ss << "w" << i << ": " << hammer_stats[i];
        }
    }
    Log.log(ss.str());
}

void Script()
{
    // 跳帧测试
    auto condition = [=]()
    {
        return completed_round < TOTAL_TEST_ROUND;
    };
    auto callback = [=]()
    {
        OpenMultipleEffective('W', -1);
        SetErrorMode(POP_WINDOW);
        SetGameSpeed(0.2);

        // 关闭修改功能
        forbidItemDrop(false);
        forbidEnterHome(false);
        plantInvincible(false);
        freePlanting(false);
        cobCannonNoCD(false);

        // 显示统计结果
        AvZ::ShowErrorNotInQueue("测试完毕，总共测试了#只红眼", total_zombie_num);
        printLog();
    };
    if (SKIP_TICK == 1)
        SkipTick(condition, callback);

    OpenMultipleEffective('Q', AvZ::MAIN_UI_OR_FIGHT_UI);
    SetErrorMode(NONE);
    SetGameSpeed(10);

    // 设置出怪与选卡
    SetZombies(zombie_type_list);
    SelectCards({ICE_SHROOM, M_ICE_SHROOM, DOOM_SHROOM, CHERRY_BOMB, JALAPENO, LILY_PAD, FUME_SHROOM, SCAREDY_SHROOM, FLOWER_POT, SUNFLOWER});
    SetFodderCards({FLOWER_POT, SCAREDY_SHROOM, SUNFLOWER, FUME_SHROOM});

    // 开启修改功能
    forbidItemDrop();  // 僵尸不掉钱
    forbidEnterHome(); // 僵尸不进家
    plantInvincible(); // 植物无敌
    freePlanting();    // 自由种植
    cobCannonNoCD();   // 玉米炮无冷却

    // 统一波次
    if (SET_ALL_AS_BIG_WAVE == 1)
        unifyAllWaves(BIG_WAVE);
    else
        unifyAllWaves(NORMAL_WAVE);

    // 秒杀特定波僵尸
    killAllZombie({10}, {BUNGEE_ZOMBIE});
    killAllZombie({4, 8, 13, 17}, {}, 400);
    killAllZombie({9, 18, 19});

    // 连续PDc操作
    SetTime(-599, 1);
    pao_operator.resetPaoList({{1, 1}, {2, 1}, {5, 1}, {6, 1}, {1, 3}, {2, 3}, {5, 3}, {6, 3}});
    for (auto w : {1, 5, 10, 14})
    {
        if (SET_WAVELENGTH >= 601 && SET_WAVELENGTH <= 2500 && !RangeIn(w, {9, 19, 20}))
        {
            SetWavelength({{w, SET_WAVELENGTH}});
        }
        // w6: PADDcccc (359)
        SetTime(360 - 100, w);
        Card(CHERRY_BOMB, 5, 9);
        SetTime(359 - 373);
        cob(2, 9);
        Delay(107);
        smart_evasion({1, 5}, {w}, {});
        SetTime(513);
        C({}, 134);
        SetWavelength({{w, 601}});

        // w17: IPP-cccc-B-PPDD 1551
        SetTime(601 + 58, w);
        Ice3(-1);
        Delay(-420);
        Card(M_ICE_SHROOM, 2, 2);
        SetTime(433 - 373, w + 1);
        smart_evasion({1, 5}, {w}, {});
        SetTime(807);
        C({1, 5, 6}, 266);
        Delay(267);
        C({1}, 413 - 267);
        SetTime(1122 - 378);
        cob(3, 9);
        SetTime(1351 - 373);
        cob({{1, 8.4875}, {5, 8.575}});
        Delay(217);
        smart_evasion({1, 5}, {w + 1}, {});
        SetWavelength({{w + 1, 1551}});

        // w18: ccccIPP-cccc-B-PPDD 1607
        SetTime(12 - 100, w + 2);
        Card(ICE_SHROOM, 2, 2);
        Ice3(-1);
        SetTime(807 + 751 - 1551);
        C({1, 5, 6}, 1);
        SetTime(1407 - 413 - 212 - 1);
        C({2}, 1);
        SetTime(433 - 373);
        smart_evasion({1, 5}, {w + 1}, {});
        SetTime(931);
        C({1, 5, 6});
        SetTime(1173 - 378);
        cob(3, 9);
        SetTime(1407 - 373);
        cob({{1, 8.3375}, {5, 9}});
        Delay(217);
        smart_evasion({1, 5}, {w + 2}, {});
        SetWavelength({{w + 2, 1607}});

        // w19 : ccccPPDDNa' SetTime(195, 19);
        SetTime(195, w + 3);
        C({}, 134);
        SetTime(375 - 373);
        cob({{2, 8.75}, {5, 8.75}});
    }

    // 记录砸率数据
    auto zd_cond = [=](SafePtr<Zombie> z) -> bool
    {
        if (z->isDead() || z->isDisappeared())
            return false;
        if (z->type() != GIGA_GARGANTUAR || !z->isHammering())
            return false;
        bool can_hit = false;
        auto plant_array = GetMainObject()->plantArray();
        for (auto p : test_plant_list)
        {
            int idx = GetPlantIndex(p.row, p.col);
            if (idx >= 0 && judgeHammer(z, plant_array + idx))
            {
                can_hit = true;
                break;
            }
        }
        if (!can_hit)
            return false;
        auto animation = getAnimationFromZombie(z);
        return (animation != nullptr && animation->circulationRate() > 0.644);
    };
    auto zd_callback = [=](SafePtr<Zombie> z)
    {
        int zombie_spawn_wave = z->standState() + 1;
        // Log.log("#", zombie_spawn_wave);
        if (zombie_spawn_wave >= 0 && zombie_spawn_wave < 21)
            hammer_stats[zombie_spawn_wave]++;
        killZombie(z);
        zd.incCallbackNum();
    };
    SetTime(-599, 1);
    zd.start(zd_cond, zd_callback); // 开始记录指定植物的砸率数据

    // 更新已经完成的选卡数
    killAllZombie({20}, {}, 300);
    InsertTimeOperation(
        300, 20, [=]()
        {
        total_zombie_num += countZombie({GIGA_GARGANTUAR}, {3, 4, 7, 8, 9, 12, 13, 16, 17, 18, 19, 20});
        completed_round++;
        if (EPOCH != 0 && (TOTAL_TEST_ROUND / EPOCH != 0) && completed_round % (TOTAL_TEST_ROUND / EPOCH) == 0)
        {
            printLog();
        } },
        "w20-stats");
}

/*
MIT License

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