#include "GetZombieAbscissas.h"
#include "avz.h"
using namespace AvZ;

void Script()
{
    SetErrorMode(CONSOLE);
    SetGameSpeed(10);
    OpenMultipleEffective('Q', AvZ::MAIN_UI_OR_FIGHT_UI);
    SetZombies({ZOMBIE, CONEHEAD_ZOMBIE, BUCKETHEAD_ZOMBIE, SCREEN_DOOR_ZOMBIE});
    SelectCards({BLOVER, CHERRY_BOMB, FUME_SHROOM, GARLIC, SUNFLOWER, SUN_SHROOM, SCAREDY_SHROOM, FLOWER_POT, PUFF_SHROOM, M_PUFF_SHROOM});
    InsertTimeOperation(0, 1, [=]() {
        SetGameSpeed(1.0);
        int index_zombie = -1;
        auto zombies = GetMainObject()->zombieArray();
        for (int index = 0; index < GetMainObject()->zombieTotal(); ++index) {
            if (!zombies[index].isDisappeared() && !zombies[index].isDead() && zombies[index].type() == ZOMBIE) {
                index_zombie = index;
                break;
            }
        }
        int time = 3000;
        std::vector<float> abs = GetZombieAbscissas(index_zombie, time);
        for (int t = 0; t <= time; ++t) {
            InsertTimeOperation(t, 1, [=]() {
                auto zombies = GetMainObject()->zombieArray();
                float real_abs = zombies[index_zombie].abscissa();
                ShowErrorNotInQueue("#号僵尸#厘秒的预测坐标为#，实际坐标为#，两者差为#", index_zombie, t, abs[t], real_abs, real_abs - abs[t]);
            });
        }
    });
}