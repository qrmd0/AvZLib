#include "GetZombieabscissas.h"
#include "avz.h"
using namespace AvZ;

//在[wave]波向前场发炮伤害大部分僵尸，留下入场最慢的领带僵尸（普通僵尸、旗帜僵尸、路障僵尸、铁桶僵尸、铁门僵尸），适用于六行场地
void SeparateLastZombie(int wave);
void Script()
{
    SetErrorMode(CONSOLE);
    OpenMultipleEffective('Q', AvZ::MAIN_UI_OR_FIGHT_UI);
    SetZombies({ZOMBIE, CONEHEAD_ZOMBIE, BUCKETHEAD_ZOMBIE, SCREEN_DOOR_ZOMBIE});
    SelectCards({BLOVER, CHERRY_BOMB, FUME_SHROOM, GARLIC, SUNFLOWER, SUN_SHROOM, SCAREDY_SHROOM, FLOWER_POT, PUFF_SHROOM, M_PUFF_SHROOM});
    pao_operator.resetPaoList({{1, 5}, {6, 5}, {2, 5}, {5, 5}, {3, 1}, {4, 1}, {3, 3}, {4, 3}, {3, 5}, {4, 5}, {3, 7}, {4, 7}});
    for (int wave = 1; wave <= 20; ++wave) {
        SeparateLastZombie(wave);
    }
    SetTime(486 + 601, 9);
    pao_operator.pao({{2, 8.8}, {5, 8.8}});
    SetTime(486 + 601, 19);
    pao_operator.pao({{2, 8.8}, {5, 8.8}});
    SetTime(223 - 373, 20);
    pao_operator.pao(4, 7.5875);
    SetTime(486 + 601, 20);
    pao_operator.pao({{2, 8.8}, {5, 8.8}});
}
void SeparateLastZombie(int wave)
{
    InsertTimeOperation(0, wave, [=]() {
        struct ZombieEnterMovement {
            int index;
            int row;
            std::vector<float> absccisas;
            int enter_time;
        };
        std::vector<ZombieEnterMovement> zombie_enter_movements;
        auto zombies = GetMainObject()->zombieArray();
        ZombieEnterMovement temp = {};
        for (int index = 0; index < GetMainObject()->zombieTotal(); ++index) {
            if (!zombies[index].isDisappeared() && !zombies[index].isDead() && zombies[index].existTime() <= NowTime(wave) && (zombies[index].type() == ZOMBIE || zombies[index].type() == CONEHEAD_ZOMBIE || zombies[index].type() == BUCKETHEAD_ZOMBIE || zombies[index].type() == SCREEN_DOOR_ZOMBIE)) {
                temp.index = index;
                temp.row = zombies[index].row() + 1;
                temp.absccisas = GetZombieAbscissas(index, 820);
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
        // ShowErrorNotInQueue("第#波领带最晚入场时间#", wave, max_enter_time);
        if (max_enter_time > 373) {
            SetTime(max_enter_time - 1 - 373, wave);
            pao_operator.pao({{{2, 8.8}, {5, 8.8}}});
        } else {
            //灰烬的作用范围是圆，所以对本行、上行、和下行的能打击到的僵尸的横坐标范围不同，因此根据(横坐标+落点行炮距-本行炮距)确定最右僵尸
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