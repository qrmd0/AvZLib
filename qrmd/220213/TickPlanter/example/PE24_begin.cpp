/*
 * @字符编码: UTF-8
 * @作者: qrmd
 * @时间: 2022-06-04 22:07:38
 * @描述: PE二十四炮挂机开局0轮
 * @AvZ版本：220213
 */
#include "avz.h"
#include "tick_planter.h"
using namespace AvZ;
using namespace std;
struct ZombieAdd : public Zombie {
    // 僵尸动画编号
    uint16_t& animationCode()
    {
        return (uint16_t&)((uint8_t*)this)[0x118];
    }
};
struct AnimationAdd : public Animation {
    // 动画起始帧
    int& beginFrame()
    {
        return (int&)((uint8_t*)this)[0x18];
    }
};
PlantFixer tall_nut_fixer;
PlantFixer garlic_fixer;
TickRunner press_reductioner;
TickRunner cob_planter;
// 返回[wave]波首个出现的僵尸所在的行数，若场上不存在[wave]波刷出的僵尸，返回-1
int GetZombieRefreshRow(int wave);
// 种植多个玉米加农炮，种植时铲除占位的植物
void ShovelAndPlantCobCannons(std::vector<Grid> pao_list, int sun_down_bound, int time_interval = 1);
// 发炮降低高坚果压力，触发条件为当高坚果前方一格存在的僵尸数量不小于3或其总血量大于1370(一只铁桶僵尸)
void PressReductionCob();
// 预测[index_zombie]号僵尸在[time_length]厘秒内的横坐标
vector<float> GetZombieAbsccisas(int index_zombie, int time_length);
//在[wave]波发炮射击2行和5行消灭大部分僵尸，只留下最慢的领带僵尸。适用于六行场地
void SeparateLastZombie(int wave);
void Script()
{
    // 游戏程序的运行速度倍率
    SetGameSpeed(2);
    // 开启日志窗口
    // SetErrorMode(CONSOLE);
    SelectCards({SUNFLOWER, M_SUNFLOWER, TWIN_SUNFLOWER, POTATO_MINE, GARLIC, TALL_NUT, LILY_PAD, KERNEL_PULT, COB_CANNON, SNOW_PEA});
    SetTime(-1799, 1);
    tick_planter.start();
    press_reductioner.pushFunc(PressReductionCob);
    tick_planter.clear();
    // wave 1 智能土豆地雷，陆路后场的向日葵
    InsertTimeOperation(-1799, 1, [=]() {
        tick_planter.delayPlantNotInQuene(SUNFLOWER, {{1, 1}, {2, 1}, {1, 2}, {2, 2}});
        tick_planter.delayPlantNotInQuene(M_SUNFLOWER, {{5, 1}, {6, 1}, {5, 2}, {6, 2}});
    });
    InsertTimeOperation(0, 1, [=]() {
        tick_planter.waitPlantNotInQuene(POTATO_MINE, {{GetZombieRefreshRow(1), 5}});
    });
    // wave 2 智能土豆地雷
    InsertTimeOperation(0, 2, [=]() {
        tick_planter.waitPlantNotInQuene(POTATO_MINE, {{GetZombieRefreshRow(2), 4}});
        tick_planter.delayPlantNotInQuene(SUNFLOWER, {{1, 3}, {2, 3}});
        tick_planter.delayPlantNotInQuene(M_SUNFLOWER, {{5, 3}, {6, 3}});
    });
    // wave 3, 4 智能大蒜、土豆地雷、寒冰射手、高坚果
    InsertTimeOperation(0, 3, [=]() {
        int zombie_row = GetZombieRefreshRow(3);
        int garlic_row = zombie_row <= 2 ? 1 : 6;
        int snow_pea_row = zombie_row <= 2 ? 2 : 5;
        tick_planter.waitPlantNotInQuene(GARLIC, {{garlic_row, 9}});
        InsertTimeOperation(500, 3, [=]() {
            tick_planter.waitPlantNotInQuene(TALL_NUT, {{snow_pea_row, 7}});
        });
        InsertTimeOperation(900, 3, [=]() {
            tick_planter.waitPlantNotInQuene(SNOW_PEA, {{snow_pea_row, 6}});
        });
        InsertTimeOperation(0, 4, [=]() {
            tick_planter.waitPlantNotInQuene(GARLIC, {{garlic_row == 6 ? 1 : 6, 9}});
            tick_planter.waitPlantNotInQuene(TALL_NUT, {{snow_pea_row == 5 ? 2 : 5, 7}});
            tick_planter.waitPlantNotInQuene(SNOW_PEA, {{snow_pea_row == 5 ? 2 : 5, 6}});
        });
    });
    // wave 4 边路向日葵、睡莲叶
    InsertTimeOperation(1, 4, [=]() {
        tick_planter.delayPlantNotInQuene(SUNFLOWER, {{1, 4}, {1, 5}, {1, 6}, {1, 7}, {1, 8}, {2, 4}, {2, 5}, {4, 3}});
        tick_planter.delayPlantNotInQuene(M_SUNFLOWER, {{6, 4}, {6, 5}, {6, 6}, {6, 7}, {6, 8}, {5, 4}, {5, 5}, {4, 4}});
        tick_planter.delayPlantNotInQuene(LILY_PAD, {{4, 8}, {3, 7}, {4, 7}, {3, 6}, {4, 6}, {5, 6}, {3, 5}, {4, 5}, {4, 3}, {4, 4}, {3, 1}, {3, 2}, {4, 1}, {4, 2}, {3, 3}, {3, 4}});
    });
    // wave 5 水路大蒜、南瓜头、寒冰射手、向日葵
    InsertTimeOperation(1, 5, [=]() {
        tick_planter.waitPlantNotInQuene(LILY_PAD, {{4, 9}});
        tick_planter.waitPlantNotInQuene(GARLIC, {{4, 9}});
        tick_planter.waitPlantNotInQuene(LILY_PAD, {{3, 9}});
        tick_planter.waitPlantNotInQuene(TALL_NUT, {{3, 9}});
        tick_planter.waitPlantNotInQuene(LILY_PAD, {{3, 8}});
        tick_planter.waitPlantNotInQuene(SNOW_PEA, {{3, 8}});
        tick_planter.delayPlantNotInQuene(SUNFLOWER, {{3, 7}, {3, 6}, {3, 5}, {3, 4}, {3, 3}});
        tick_planter.delayPlantNotInQuene(M_SUNFLOWER, {{4, 8}, {4, 7}, {4, 6}, {4, 5}});
    });
    // wave 6 开启高坚果和大蒜修补
    InsertTimeOperation(0, 6, [=]() {
        SetTime(0, 6);
        tall_nut_fixer.start(TALL_NUT, {{2, 7}, {3, 9}, {5, 7}}, 2000);
        garlic_fixer.start(GARLIC, {{1, 9}, {4, 9}, {6, 9}}, 80);
    });
    // wave 7 玉米加农炮、双子向日葵
    InsertTimeOperation(0, 7, [=]() {
        tick_planter.delayPlantNotInQuene(TWIN_SUNFLOWER, {{1, 7}, {3, 7}, {4, 7}, {4, 8}, {6, 7}});
        tick_planter.delayPlantNotInQuene(COB_CANNON, {{3, 1}, {4, 1}, {3, 3}, {4, 3}});
        cob_planter.pushFunc([]() {
            ShovelAndPlantCobCannons({{3, 1}, {4, 1}, {3, 3}, {4, 3}, {1, 1}, {2, 1}, {5, 1}, {6, 1}}, 1400);
        });
    });
    // wave 19 拖收尾
    InsertTimeOperation(0, 19, [=]() {
        SetTime(0, 19);
        Shovel({{2, 6}, {3, 8}, {5, 6}});
        tick_planter.delayPlantNotInQuene(SUNFLOWER, {{2, 6}, {3, 8}});
        tick_planter.delayPlantNotInQuene(M_SUNFLOWER, {{5, 6}});
        tick_planter.delayPlantNotInQuene(TWIN_SUNFLOWER, {{3, 8}});
        press_reductioner.stop();
        pao_operator.autoGetPaoList();
        SeparateLastZombie(19);
        SetTime(4500 - 200 - 373, 19);
        pao_operator.autoGetPaoList();
        pao_operator.pao({{2, 8.8}, {5, 8.8}});
    });
    // wave 20 炮消珊瑚、拖收尾
    SetTime(223 - 373, 20);
    pao_operator.autoGetPaoList();
    pao_operator.pao(4, 7.5875);
    SetTime(0, 20);
    pao_operator.autoGetPaoList();
    SeparateLastZombie(20);
    SetTime(5500 - 200 - 373, 20);
    pao_operator.autoGetPaoList();
    pao_operator.pao({{2, 8.8}, {5, 8.8}});
}
int GetZombieRefreshRow(int wave)
{
    auto zombies = GetMainObject()->zombieArray();
    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index) {
        if (!zombies[index].isDisappeared() && !zombies[index].isDead() && zombies[index].existTime() == NowTime(wave)) {
            return zombies[index].row() + 1;
        }
    }
    return -1;
}
void ShovelAndPlantCobCannons(std::vector<Grid> pao_list, int sun_down_bound, int time_interval)
{
    if (GetMainObject()->gameClock() % time_interval != 0)
        return;
    auto seeds = GetMainObject()->seedArray();
    for (Grid each : pao_list) {
        int present_sun = GetMainObject()->sun();
        int index_cob_cannon_seed = GetSeedIndex(COB_CANNON);
        bool is_usable_cob_cannon_seed = seeds[index_cob_cannon_seed].isUsable();
        int remaining_cd_cob_cannon = 5000 - seeds[index_cob_cannon_seed].cd();
        int index_kernel_pult_seed = GetSeedIndex(KERNEL_PULT);
        bool is_usable_kernel_pult_seed = seeds[index_kernel_pult_seed].isUsable();
        if (GetPlantIndex(each.row, each.col, COB_CANNON) < 0) {
            if (GetPlantIndex(each.row, each.col, KERNEL_PULT) < 0) {
                if (present_sun >= sun_down_bound && (remaining_cd_cob_cannon < 750 || is_usable_cob_cannon_seed) && is_usable_kernel_pult_seed) {
                    if (GetPlantIndex(each.row, each.col) >= 0) {
                        ShovelNotInQueue(each.row, each.col);
                    }
                    CardNotInQueue(index_kernel_pult_seed + 1, each.row, each.col);
                }
            } else {
                if (is_usable_kernel_pult_seed) {
                    if (GetPlantIndex(each.row, each.col + 1) >= 0) {
                        ShovelNotInQueue(each.row, each.col + 1);
                    }
                    CardNotInQueue(index_kernel_pult_seed + 1, each.row, each.col + 1);
                    CardNotInQueue(index_cob_cannon_seed + 1, each.row, each.col);
                }
            }
            break;
        }
    }
}
void PressReductionCob()
{
    if (GetMainObject()->gameClock() % 377 != 0)
        return;
    auto zombies = GetMainObject()->zombieArray();
    int zombies_num_in_the_row_2 = 0;
    int zombies_num_in_the_row_3 = 0;
    int zombies_num_in_the_row_5 = 0;
    int zombies_hp_in_the_row_2 = 0;
    int zombies_hp_in_the_row_3 = 0;
    int zombies_hp_in_the_row_5 = 0;
    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index) {
        if (!zombies[index].isDisappeared() && !zombies[index].isDead() && zombies[index].row() + 1 == 2 && zombies[index].abscissa() <= 8 * 80.0) {
            zombies_num_in_the_row_2++;
            zombies_hp_in_the_row_2 += zombies[index].hp() + zombies[index].oneHp() + zombies[index].twoHp() / 5;
        }
    }
    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index) {
        if (!zombies[index].isDisappeared() && !zombies[index].isDead() && zombies[index].row() + 1 == 3 && zombies[index].abscissa() <= 765) {
            zombies_num_in_the_row_3++;
            zombies_hp_in_the_row_3 += zombies[index].hp() + zombies[index].oneHp() + zombies[index].twoHp() / 5;
        }
    }
    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index) {
        if (!zombies[index].isDisappeared() && !zombies[index].isDead() && zombies[index].row() + 1 == 5 && zombies[index].abscissa() <= 8 * 80.0) {
            zombies_num_in_the_row_5++;
            zombies_hp_in_the_row_5 += zombies[index].hp() + zombies[index].oneHp() + zombies[index].twoHp() / 5;
        }
    }
    int index_ready_pao = -1;
    auto plants = GetMainObject()->plantArray();
    for (int index = 0; index < GetMainObject()->plantTotal(); ++index) {
        if (plants[index].type() == COB_CANNON && plants[index].state() == 37) {
            index_ready_pao = index;
            break;
        }
    }
    if (index_ready_pao >= 0) {
        if (zombies_num_in_the_row_2 >= 3 || zombies_hp_in_the_row_2 > 1370 || zombies_num_in_the_row_3 >= 3 || zombies_hp_in_the_row_3 > 1370) {
            SafeClick();
            Asm::shootPao(80 * 8, 55 + 85 * 2, index_ready_pao);
            SafeClick();
        } else if (zombies_num_in_the_row_5 >= 3 || zombies_hp_in_the_row_5 > 1370) {
            SafeClick();
            Asm::shootPao(80 * 7, 55 + 85 * 5, index_ready_pao);
            SafeClick();
        }
    }
}
vector<float> GetZombieAbsccisas(int index_zombie, int time_length)
{
    vector<float> zombie_walk_a = {-9.8, -8.4, -7, -5.6, -4.1, -2.7, -1.3, 0, 1.4, 2.8, 4.2, 5.7, 7.1, 7.9, 8.8, 9.7, 10.5, 10.6, 10.8, 10.9, 11, 11, 11, 11, 11, 13.4, 15.8, 18.1, 20.5, 22.8, 25.2, 27.6, 29.9, 31.1, 32.3, 33.5, 34.6, 35.9, 37, 38.2, 39.4, 39.5, 39.6, 39.7, 39.8, 39.9, 40};
    vector<float> zombie_walk_b = {-9.8, -8.5, -7.3, -6, -4.7, -3.4, -2.1, -0.9, 0.3, 1.6, 2.8, 4.1, 5.4, 6.7, 8, 9.2, 10.5, 10.6, 10.7, 10.7, 10.8, 10.8, 10.9, 11, 12.8, 14.5, 16.3, 18.1, 19.9, 21.6, 23.4, 25.2, 27, 28.8, 30.5, 32.3, 34, 35.9, 37.6, 39.4, 39.5, 39.5, 39.6, 39.8, 39.9, 39.9, 40};
    vector<float> zombie_dance = {-9.8, -9.4, -8.9, -8.4, -7.9, -7.5, -7, -6.5, -6.1, -5.6, -5.1, -4.7, -4.2, -3.7, -3.3, -2.8, -2.3, -1.8, -1.4, -0.9, -0.4, 0, 0.3, 0.8, 1.3, 1.8, 2.2, 2.6, 3.1, 3.6, 4.1, 4.6, 5, 5.5, 6, 6.5, 6.9, 7.3, 7.8, 8.3, 8.8, 9.3, 9.7, 10.2, 10.7, 11.1, 11.6, 12.1, 12.6, 13};
    SafePtr<ZombieAdd> zombies = (ZombieAdd*)(GetMainObject()->zombieArray());
    SafePtr<AnimationAdd> animations = (AnimationAdd*)(GetPvzBase()->animationMain()->animationOffset()->animationArray());
    int index_animation = zombies[index_zombie].animationCode();
    int begin_frame = animations[index_animation].beginFrame();
    vector<float> zombie_walk;
    switch (begin_frame) {
    case 44:
        zombie_walk.assign(zombie_walk_a.begin(), zombie_walk_a.end());
        break;
    case 91:
        zombie_walk.assign(zombie_walk_b.begin(), zombie_walk_b.end());
        break;
    case 454:
        zombie_walk.assign(zombie_dance.begin(), zombie_dance.end());
        break;
    default:
        break;
    }
    float total_movement = zombie_walk.back() - zombie_walk.front();
    int frame_number = zombie_walk.size();
    float zombie_speed = zombies[index_zombie].speed();
    float frame_speed = zombie_speed * 47 * frame_number * 0.01 / total_movement;
    float circulation_rate = animations[index_animation].circulationRate();
    float circulation_rate_change = zombie_speed * 47 * 0.01 / total_movement;
    float zombie_abscissa = zombies[index_zombie].abscissa();
    vector<float> result;
    result.emplace_back(zombie_abscissa);
    int index_present_frame = 0;
    float present_movement = 0;
    for (int time = 0; time < time_length; ++time) {
        index_present_frame = (int)(circulation_rate * (frame_number - 1) + 1);
        present_movement = frame_speed * (zombie_walk[index_present_frame] - zombie_walk[index_present_frame - 1]);
        zombie_abscissa -= present_movement;
        result.emplace_back(zombie_abscissa);
        circulation_rate += circulation_rate_change;
        circulation_rate = circulation_rate > 1 ? circulation_rate - 1 : circulation_rate;
    }
    return result;
}
void SeparateLastZombie(int wave)
{
    InsertTimeOperation(0, wave, [=]() {
        struct ZombieEnterMovement {
            int index;
            int row;
            vector<float> absccisas;
            int enter_time;
        };
        vector<ZombieEnterMovement> zombie_enter_movements;
        auto zombies = GetMainObject()->zombieArray();
        ZombieEnterMovement temp = {};
        for (int index = 0; index < GetMainObject()->zombieTotal(); ++index) {
            if (!zombies[index].isDisappeared() && !zombies[index].isDead() && zombies[index].existTime() == NowTime(wave) && (zombies[index].type() == ZOMBIE || zombies[index].type() == CONEHEAD_ZOMBIE || zombies[index].type() == BUCKETHEAD_ZOMBIE || zombies[index].type() == SCREEN_DOOR_ZOMBIE)) {
                temp.index = index;
                temp.row = zombies[index].row() + 1;
                temp.absccisas = GetZombieAbsccisas(index, 820);
                for (int t = 0; t <= 820; ++t) {
                    if (temp.absccisas[t] < 765) {
                        temp.enter_time = t;
                        break;
                    }
                }
                zombie_enter_movements.emplace_back(temp);
            }
        }
        int max_enter_time = zombie_enter_movements[0].enter_time;
        for (int i = 0; i < zombie_enter_movements.size(); ++i) {
            if (zombie_enter_movements[i].enter_time > max_enter_time) {
                max_enter_time = zombie_enter_movements[i].enter_time;
            }
        }
        if (max_enter_time > 373) {
            SetTime(max_enter_time - 1 - 373, wave);
            pao_operator.pao({{2, 8.8}, {5, 8.8}});
        } else {
            //灰烬的作用范围是圆，所以对本行、上行、和下行的能打击到的僵尸的横坐标范围不同，因此选择最右僵尸时，应根据其(横坐标+落点行炮距-本行炮距)判断
            const int cannon_left_length_of_zombie_at_up_row = 71;
            const int cannon_left_length_of_zombie_at_the_row = 79;
            const int cannon_left_length_of_zombie_at_down_row = 78;
            float max_absccisa = 0;
            float cannon_left_length_of_zombie_offset = 0;
            float cannon_drop_absccisa = 0;
            for (int i = 0; i < zombie_enter_movements.size(); ++i) {
                if (zombie_enter_movements[i].row == 1 || zombie_enter_movements[i].row == 4) {
                    cannon_left_length_of_zombie_offset = cannon_left_length_of_zombie_at_the_row - cannon_left_length_of_zombie_at_up_row;
                } else if (zombie_enter_movements[i].row == 2 || zombie_enter_movements[i].row == 5) {
                    cannon_left_length_of_zombie_offset = 0;
                } else if (zombie_enter_movements[i].row == 3 || zombie_enter_movements[i].row == 6) {
                    cannon_left_length_of_zombie_offset = cannon_left_length_of_zombie_at_the_row - cannon_left_length_of_zombie_at_down_row;
                }
                if (zombie_enter_movements[i].absccisas[373] + cannon_left_length_of_zombie_offset > max_absccisa) {
                    max_absccisa = zombie_enter_movements[i].absccisas[373] + cannon_left_length_of_zombie_offset;
                    cannon_drop_absccisa = max_absccisa - cannon_left_length_of_zombie_at_the_row + 7 - 1;
                }
            }
            SetTime(0, wave);
            pao_operator.pao({{2, cannon_drop_absccisa / 80}, {5, cannon_drop_absccisa / 80}});
        }
    });
}