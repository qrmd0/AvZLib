/*
 * @Author: crescendo
 * @Date: 2022-05-29 15:08:18
 * @Last Modified by: crescendo
 * @Last Modified time: 2022-05-29 21:48:01
 * 使用 AvZ Testing 框架测试PE水路七八九列曾放羊炸率
 * 锁定小丑只出现在岸路，可配合Rnd锁定小丑早爆，增加测试效率
 * w20僵尸刷出后秒杀，不纳入测试范围（为统计简便）
 */

#include "avz.h"
#include "avz_testing.h"
#include "avz_logger.h"

using namespace AvZ;
using namespace AZT;

/* 配置部分开始 */
#define TOTAL_TEST_ROUND 52
// 每一批次测试的选卡数
#define SET_ALL_AS_BIG_WAVE 0
// 0 - 将所有波次转为普通波；1 - 将所有波次转为旗帜波
#define SKIP_TICK 1
// 0 - 慢放，用于检查情况；1 - 开启跳帧测试
#define LOCK_WAVE 0
// 0 - 不锁定刷新；1 - 锁定刷新，直至当前波次小丑全部消灭
#define BATCH_SIZE 1
// 批次数，对应不同的用冰时机
#define EPOCH 10
// 定期输出当前统计信息的频率（指定输出总次数）
std::vector<Grid> test_plant_list = {{3, 7}, {4, 7}, {3, 8}, {4, 8}, {3, 9}, {4, 9}};
// 数据列表
std::vector<Grid> always_attack_list = {{3, 7}, {4, 7}, {3, 8}, {4, 8}, {3, 9}, {4, 9}};
// 设为永动攻击的植物列表
std::set<int> jack_rows = {2, 5};
// 小丑行数
std::vector<int> ice_time_list = {-1};
// 要测试的用冰时机，0代表不用冰
/* 配置部分结束 */

AZL::Logger Log;
AlwaysAttack aa;
LockWave lw;
std::vector<JackData> jd_list(BATCH_SIZE, JackData());
int completed_round = 0;
int current_jackdata = 0;
int total_jack_num = 0;

void Script()
{
    // 跳帧测试
    auto condition = [=]()
    {
        return current_jackdata < BATCH_SIZE && ice_time_list.size() == BATCH_SIZE;
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

        // 显示统计结果
        if (ice_time_list.size() != BATCH_SIZE)
        {
            ShowErrorNotInQueue("测试程序未能执行。原因：BATCH_SIZE设置有误。");
        }
        else
        {
            // 显示统计结果
            for (int i = 0; i < BATCH_SIZE; i++)
            {
                AvZ::ShowErrorNotInQueue("测试完毕，总共测试了#只小丑", total_jack_num);
                Log.log("总共测试了#只小丑。\n#", total_jack_num, jd_list[i].stats());
            }
        }
    };
    if (SKIP_TICK == 1)
        SkipTick(condition, callback);

    OpenMultipleEffective('Q', AvZ::MAIN_UI_OR_FIGHT_UI);
    SetErrorMode(NONE);
    SetGameSpeed(10);

    // 设置出怪与选卡
    SetZombies({JACK_IN_THE_BOX_ZOMBIE});
    SelectCards({ICE_SHROOM, M_ICE_SHROOM, 40, 41, 42, 43, 44, 45, 46, 47});

    // 开启修改功能
    forbidItemDrop();  // 僵尸不掉钱
    forbidEnterHome(); // 僵尸不进家
    plantInvincible(); // 植物无敌
    freePlanting();    // 自由种植

    // 将小丑僵尸平均分配至指定路
    moveZombieRow(JACK_IN_THE_BOX_ZOMBIE, jack_rows);

    // 统一波次
    if (SET_ALL_AS_BIG_WAVE == 1)
        unifyAllWaves(BIG_WAVE);
    else
        unifyAllWaves(NORMAL_WAVE);

    // 秒杀旗帜波小偷
    killAllZombie({10}, {BUNGEE_ZOMBIE});
    killAllZombie({20});

    SetTime(-599, 1);
    jd_list[current_jackdata].start(test_plant_list); // 开始记录指定植物的炸率数据
    aa.start(always_attack_list);                     // 使指定植物永动
    if (LOCK_WAVE == 1)
    {
        lw.start({JACK_IN_THE_BOX_ZOMBIE});
    }

    // 自由种植功能里已开启蘑菇免唤醒
    for (int w = 1; w <= 20; w++)
    {
        if (ice_time_list[current_jackdata] > 0)
        {
            SetTime(ice_time_list[current_jackdata] - 100, w);
            Card(ICE_SHROOM, 1, 1);
        }
    }

    // 更新已经完成的选卡数
    InsertTimeOperation(
        0, 20, [=]()
        {  
            // 统计本次选卡的小丑总数（排除w20）
            total_jack_num += countZombie({JACK_IN_THE_BOX_ZOMBIE}, {20});
            completed_round++;

            // 更新已完成选卡数和已完成批次数
            if (completed_round >= TOTAL_TEST_ROUND)
            {
                completed_round = 0;
                current_jackdata++;
            }

            // 保存测试途中信息，防止意外中止
            if (current_jackdata < BATCH_SIZE && EPOCH > 0 && completed_round % (TOTAL_TEST_ROUND / EPOCH) == 0)
            {
                Log.log("总共测试了#只小丑。\n#", total_jack_num, jd_list[current_jackdata].stats());
            } },
        "w20-stats");
}