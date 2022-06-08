/*
 * @Author: crescendo
 * @Date: 2022-06-06 09:14:09
 * @Last Modified by: crescendo
 * @Last Modified time: 2022-06-06 15:00:04
 * __AVZ_VERSION__ == 220213
 * 智能发炮类
 */

/*
使用说明：
1. 请根据 https://tieba.baidu.com/p/7860494646 安装AvZ精确预测僵尸坐标插件，
确保AvZ目录inc文件夹下已有GetZombieAbscissas.h文件，且已替换pvzstruct.h
2. 将本文件置于AvZ目录inc文件夹下
3. 在Script()函数外添加 using namespace cresc;
*/

#pragma once
#include "libavz.h"
#include "GetZombieAbscissas.h"

#ifndef __ASC_VERSION__
#define __ASC_VERSION__ 220607
#endif

#ifndef ASC_DEBUG
// #define ASC_DEBUG
#endif

using namespace AvZ;

namespace cresc
{
    const std::map<ZombieType, std::vector<int>> dis_table_pe = {{GARGANTUAR, {125, 216, 132, 223, 132, 223}}, {GIGA_GARGANTUAR, {125, 216, 132, 223, 132, 223}}, {ZOMBONI, {111, 264, 115, 268, 114, 267}}, {BALLOON_ZOMBIE, {58, 172, 79, 193, 79, 193}}, {FOOTBALL_ZOMBIE, {57, 214, 65, 222, 64, 221}}, {CATAPULT_ZOMBIE, {111, 264, 115, 268, 114, 267}}, {DIGGER_ZOMBIE, {57, 185, 65, 193, 64, 192}}, {SNORKEL_ZOMBIE, {95, 181, 103, 189, 102, 188}}, {POGO_ZOMBIE, {10, 124, 77, 191, 79, 193}}, {ZOMBIE, {71, 185, 79, 193, 78, 192}}};

    const std::map<ZombieType, std::vector<int>> dis_table_de = {{GARGANTUAR, {118, 209, 132, 223, 132, 223}}, {GIGA_GARGANTUAR, {118, 209, 132, 223, 132, 223}}, {ZOMBONI, {106, 259, 115, 268, 113, 266}}, {BALLOON_ZOMBIE, {46, 160, 79, 193, 78, 192}}, {FOOTBALL_ZOMBIE, {50, 207, 65, 222, 61, 218}}, {CATAPULT_ZOMBIE, {106, 259, 115, 268, 113, 266}}, {DIGGER_ZOMBIE, {50, 178, 65, 193, 61, 189}}, {SNORKEL_ZOMBIE, {88, 174, 103, 189, 99, 185}}, {POGO_ZOMBIE, {-1, -1, 77, 191, 78, 192}}, {ZOMBIE, {64, 178, 79, 193, 75, 189}}};

    struct SmartHitInfo
    {
        ZombieType zombie_type;
        std::set<int> waves = {-1}; // -1代表current wave；空集代表不检测
        std::set<int> rows = {};
    };

    // 对炮操作目标进行分类
    // ALL - 全场
    // UPPER - 上半场
    // LOWER - 下半场
    enum PaoOperationType
    {
        ALL_FIELD = 0,
        UPPER,
        LOWER,
    };

    // 自定义PaoOperator子类，用于简化发炮
    class SmartPaoOperator : public PaoOperator
    {
    public:
        // ***In Queue
        // 发射并炸炮，五行场地炸2、4路，六行场地炸2、5路
        // ***使用示例：
        // p.pp(9);    // 双边并炸9列
        // p.pp(9, 8.5); // 一炮炸2路9列，一炮炸4/5路8.5列
        void pp(float col1 = 9, float col2 = -1)
        {
            InsertOperation([=]()
                            { 
                                InsertGuard _(false);
                                int scene = AvZ::GetMainObject()->scene();
                                int r2 = (scene == 2 || scene == 3) ? 5: 4;
                                if (col2 == -1)
                                {
                                    pao({{2, col1}, {r2, col1}});
                                }
                                else
                                {
                                    pao({{2, col1}, {r2, col2}});
                                } },
                            "SmartPaoOperator::pp");
        }

        // ***In Queue
        // 发射拦截炮，五行场地炸1、4路，六行场地炸1、5路
        // ***使用示例：
        // p.dd(7.4);    // 双边并炸7.4列
        // p.dd(7.4, 7.8); // 一炮炸1路7.4列，一炮炸4/5路7.8列
        void dd(float col1 = 7.4, float col2 = -1)
        {
            InsertOperation([=]()
                            { 
                                InsertGuard _(false);
                                int scene = AvZ::GetMainObject()->scene();
                                int r2 = (scene == 2 || scene == 3) ? 5: 4;
                                if (col2 == -1)
                                {
                                    pao({{1, col1}, {r2, col1}});
                                }
                                else
                                {
                                    pao({{1, col1}, {r2, col2}});
                                } },
                            "SmartPaoOperator::dd");
        }

        // ***Not In Queue
        // 精准之舞，默认359激活，466拦截
        // 可以指定要执行操作的具体半场
        // ***使用示例：
        // p.jw(1);     // w1进行精准之舞
        // p.jw(1, UPPER); // w1上半场进行精准之舞
        void jw(int wave, PaoOperationType type = ALL_FIELD)
        {
            SetTime(359 - 373, wave);
            InsertOperation([=]()
                            {
                                int scene = AvZ::GetMainObject()->scene();
                                int r2 = (scene == 2 || scene == 3) ? 5 : 4;
                                SetTime(359 - 373, wave);
                                if (type == ALL_FIELD)
                                {
                                    pao({{2, 9}, {r2, 9}});
                                    Delay(107);
                                    pao({{1, 7.8}, {r2, 7.8}});
                                }
                                else if (type == UPPER)
                                {
                                    pao(2, 9);
                                    Delay(107);
                                    pao(1, 7.8);
                                }
                                else if (type == LOWER)
                                {
                                    pao(r2, 9);
                                    Delay(107);
                                    pao(r2, 7.8);
                                } },
                            "SmartPaoOperator::jw");
        }

        // ***Not In Queue
        // 执行PDc时机的PD，默认225激活，延迟107拦截
        // 可以指定要执行操作的具体半场
        // ***使用示例：
        // p.pd(1);     // w1进行PD
        // p.pd(1, UPPER); // w1上半场进行PD
        void pd(int wave, PaoOperationType type = ALL_FIELD)
        {
            SetTime(225 - 373, wave);
            InsertOperation([=]()
                            {
                                int scene = AvZ::GetMainObject()->scene();
                                int r2 = (scene == 2 || scene == 3) ? 5 : 4;
                                SetTime(225 - 373, wave);
                                if (type == ALL_FIELD)
                                {
                                    pao({{2, 9}, {r2, 9}});
                                    Delay(107);
                                    pao({{1, 8.1875}, {r2, 8.1875}});
                                }
                                else if (type == UPPER)
                                {
                                    pao(2, 9);
                                    Delay(107);
                                    pao(1, 8.1875);
                                }
                                else if (type == LOWER)
                                {
                                    pao(r2, 9);
                                    Delay(107);
                                    pao(r2, 8.1875);
                                } },
                            "SmartPaoOperator::pd");
        }

        // ***Not In Queue
        // 执行连拦BD，默认240激活，延迟107拦截
        // 可以指定要执行操作的具体半场
        // ***使用示例：
        // p.bd(1);     // w1进行BD
        // p.bd(1, UPPER); // w1上半场进行BD
        void bd(int wave, PaoOperationType type = ALL_FIELD)
        {
            SetTime(240 - 373, wave);
            InsertOperation([=]()
                            {
                                int scene = AvZ::GetMainObject()->scene();
                                int r2 = (scene == 2 || scene == 3) ? 5 : 4;
                                SetTime(240 - 373, wave);
                                if (type == ALL_FIELD)
                                {
                                    pao({{1, 8.625}, {r2, 8.625}});
                                    Delay(107);
                                    pao({{1, 8.1625}, {r2, 8.1625}});
                                }
                                else if (type == UPPER)
                                {
                                    pao(2, 8.625);
                                    Delay(107);
                                    pao(1, 8.1875);
                                }
                                else if (type == LOWER)
                                {
                                    pao(r2, 8.625);
                                    Delay(107);
                                    pao(r2, 8.1875);
                                } },
                            "SmartPaoOperator::bd");
        }

        // 根据判定关系，查表得到判定距离
        std::pair<int, int> lookUpDist(ZombieType type, int diff, int scene)
        {
            // -1对应0, 0对应2, 1对应4
            diff = (diff + 1) * 2;
            if (diff < 0 || diff >= 6)
            {
                AvZ::ShowErrorNotInQueue("[ERROR] lookUpDist: diff=# is invalid", diff);
                return {-1, -1};
            }

            if (scene == 0 || scene == 1)
            {
                if (dis_table_de.find(type) != dis_table_de.end())
                    return {dis_table_de.at(type)[diff], dis_table_de.at(type)[diff + 1]};
                else
                    return {dis_table_de.at(ZOMBIE)[diff], dis_table_de.at(ZOMBIE)[diff + 1]};
            }
            else
            {
                if (dis_table_pe.find(type) != dis_table_pe.end())
                    return {dis_table_pe.at(type)[diff], dis_table_pe.at(type)[diff + 1]};
                else
                    return {dis_table_pe.at(ZOMBIE)[diff], dis_table_pe.at(ZOMBIE)[diff + 1]};
            }
        }

        // 检查某个僵尸是否在合理的行数
        bool isValidRow(int row, ZombieType type, int cob_row, int scene)
        {
            auto valid_rows = getValidRows(type, cob_row, scene);
            if (valid_rows.empty())
                return false;
            return std::find(valid_rows.begin(), valid_rows.end(), row) != valid_rows.end();
        }

        // 获得有必要考虑的合理的僵尸行数
        std::vector<int> getValidRows(ZombieType type, int cob_row, int scene)
        {
            // 排除不合理的僵尸类型
            std::set<ZombieType> exclude_list = {BACKUP_DANCER, DUCKY_TUBE_ZOMBIE, ZOMBIE_BOBSLED_TEAM, ZOMBIE_YETI, BUNGEE_ZOMBIE, DR_ZOMBOSS};
            if (exclude_list.find(type) != exclude_list.end())
                return {};

            // 根据场景推测该僵尸可能出现的行数
            std::vector<int> rows;
            if (scene == 0 || scene == 1)
            {
                if (type == DANCING_ZOMBIE)
                    rows = {2, 3, 4};
                else if (type == SNORKEL_ZOMBIE || type == DOLPHIN_RIDER_ZOMBIE)
                    rows = {};
                else if (type == ZOMBONI && scene == 1)
                    rows = {};
                else
                    rows = {1, 2, 3, 4, 5};
            }
            else if (scene == 2 || scene == 3)
            {
                if (type == SNORKEL_ZOMBIE || type == DOLPHIN_RIDER_ZOMBIE)
                    rows = {3, 4};
                else if (type == BALLOON_ZOMBIE)
                    rows = {1, 2, 3, 4, 5, 6};
                else
                    rows = {1, 2, 5, 6}; // GetZombieAbscissas暂时无法处理鸭子入水，故暂不考虑
            }
            else
            {
                if (type == DANCING_ZOMBIE || type == DIGGER_ZOMBIE)
                    rows = {};
                else
                    rows = {1, 2, 3, 4, 5};
            }

            // 根据欲发炮行数，排除不在炮范围内的行数
            for (auto it = rows.begin(); it != rows.end();)
            {
                if (abs(cob_row - *it) > 1)
                {
                    it = rows.erase(it);
                }
                else
                {
                    it++;
                }
            }
            return rows;
        }

        // 智能求炮落点列数
        float getSmartHitNotInQueue(int cob_row, float default_col, const std::vector<SmartHitInfo> zombie_hit_info, bool prefer_left = true, int calc_delay = 373)
        {
#ifdef ASC_DEBUG
            AvZ::ShowErrorNotInQueue("getSmartHitNotInQueue");
#endif
            // 提取出type
            std::vector<ZombieType> zombie_types;
            for (auto z : zombie_hit_info)
                zombie_types.push_back(z.zombie_type);

            // 根据每个类型的僵尸的坐标极值，求落点范围
            std::map<ZombieType, std::pair<int, int>> zombie_hit_range;
            auto zombie = AvZ::GetMainObject()->zombieArray();
            int zombie_count_max = AvZ::GetMainObject()->zombieTotal();
            for (int i = 0; i < zombie_count_max; i++, zombie++)
            {
                // 检查合法性
                if (zombie->isDisappeared() || zombie->isDead())
                    continue;
                if (std::find(zombie_types.begin(), zombie_types.end(), (ZombieType)zombie->type()) == zombie_types.end())
                    continue;
                int idx = -1;
                for (int temp = 0; temp < zombie_hit_info.size(); temp++)
                    if (zombie_hit_info[temp].zombie_type == zombie->type())
                    {
                        idx = temp;
                        break;
                    }
                if (idx == -1)
                    continue;
                auto curr_hit_info = zombie_hit_info[idx];
                if (curr_hit_info.waves.empty())
                {
                    if (zombie->standState() + 1 != AvZ::GetRunningWave())
                        continue;
                }
                else if (curr_hit_info.waves.find(-1) == curr_hit_info.waves.end())
                {
                    if (curr_hit_info.waves.find(zombie->standState() + 1) == curr_hit_info.waves.end())
                        continue;
                }

                // 检查行数
                int zombie_row = zombie->row() + 1;
                int scene = AvZ::GetMainObject()->scene();
                if (!isValidRow(zombie_row, (ZombieType)zombie->type(), cob_row, scene))
                    continue;

                // 查询炮距
                std::pair<int, int> hit_dist = lookUpDist((ZombieType)zombie->type(), zombie_row - cob_row, scene);
                if (hit_dist.first < 0 || hit_dist.second < 0)
                    continue;

                // 预测坐标
                auto abscissas = GetZombieAbscissas(i, calc_delay + 1);
                if (abscissas.empty())
                    continue;
                int expected_absc = abscissas[calc_delay];

                // 求当前僵尸对应的落点范围
                std::pair<int, int> curr_hit_range = {expected_absc - hit_dist.first, expected_absc + hit_dist.second};

                // 如果没有记录过该僵尸的落点范围，记录之
                if (zombie_hit_range.find((ZombieType)zombie->type()) == zombie_hit_range.end())
                {
                    zombie_hit_range[(ZombieType)zombie->type()] = curr_hit_range;
                }
                else
                {
                    // 将原范围缩小至新范围，检查新范围是否合法
                    auto old_hit_range = zombie_hit_range[(ZombieType)zombie->type()];
                    std::pair<int, int> new_hit_range = {std::max(old_hit_range.first, curr_hit_range.first), std::min(old_hit_range.second, curr_hit_range.second)};
                    if (new_hit_range.first <= new_hit_range.second)
                    {
                        zombie_hit_range[(ZombieType)zombie->type()] = new_hit_range;
                    }
                }
            }

            // 根据僵尸优先级顺序，确定落点范围
            const int MIN = 0, MAX = 737;
            int lo = MIN, hi = MAX;
            for (auto z : zombie_types)
            {
                auto hit_it = zombie_hit_range.find(z);
                if (hit_it != zombie_hit_range.end())
                {
                    auto hit_val = hit_it->second;
                    int new_lo = hit_val.first;
                    int new_hi = hit_val.second;

                    // 仅当新范围合法时，才进行收缩
                    if (std::max(lo, new_lo) <= std::min(hi, new_hi))
                    {
                        lo = std::max(lo, new_lo);
                        hi = std::min(hi, new_hi);
                    }
#ifdef ASC_DEBUG
                    AvZ::ShowErrorNotInQueue("new range - # : (#, #)", z, new_lo, new_hi);
#endif
                }
            }

            int result = prefer_left ? lo : hi;
            if (result == MIN || result == MAX)
                return default_col;
            else
                return (result + 7) / 80.0;
        }

        // ***In Queue
        // （完整函数）精确预测僵尸位置，智能确定炮落点
        // ***使用示例：
        // SetTime(800 - 373, 2);
        // p.smartHitFull(2, 7.5, {{GARGANTUAR, {1}}, {LADDER_ZOMBIE, {-1}, {5, 6}}}, false, 300);
        // 炮炸2路，默认炸7.5列，根据巨人与扶梯的实际位置推测修改落点
        // 要求全收第一波的巨人，并在此基础上全收当前波次（-1代表当前波）的5、6路的扶梯
        // 存在多个可行的炮落点时，倾向于使用靠右的落点（false的含义）；僵尸坐标按未来300cs折算
        // 因为预测坐标函数无法预知垫材逻辑，若在途中放垫，可用此时机进行修正
        // p.smartHitFull(5, 2.5, {{IMP, {}}}, false);
        // 炮炸5路，默认2.5列，要求全收任意波次的小鬼，落点尽可能取右
        // {IMP, {}}中第二个参数填{}代表任意波次 
        void
        smartHitFull(int row, float default_col, const std::vector<SmartHitInfo> zombie_hit_info, bool prefer_left = true, int calc_delay = 373)
        {
            InsertOperation([=]()
                            {
                                InsertGuard _(false);
                                float col = getSmartHitNotInQueue(row, default_col, zombie_hit_info, prefer_left, calc_delay);
                                pao(row, col); },
                            "smartHit");
        }

        // ***In Queue
        // （简易函数）精确预测僵尸位置，智能确定炮落点
        // ***使用示例：
        // SetTime(800 - 373, 2);
        // p.smartHitFull(2, 7.5, {GARGANTUAR, LADDER});
        // 炮炸2路，默认炸7.5列，根据巨人与扶梯的实际位置推测修改落点
        // 要求全收当前波次的巨人与扶梯
        void smartHit(int row, float default_col, const std::vector<ZombieType> &zombie_types, bool prefer_left = true)
        {
            std::vector<SmartHitInfo> zombie_hit_info;
            for (auto z : zombie_types)
                zombie_hit_info.push_back({z});
            smartHitFull(row, default_col, zombie_hit_info, prefer_left, 373);
        }
    };

} // namespace cresc