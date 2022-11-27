/*
 * @Author: error: git config user.name & please set dead value or install git
 * @Date: 2022-06-13 19:25:37
 * @LastEditors: error: git config user.name & please set dead value or install git
 * @LastEditTime: 2022-11-21 22:12:22
 * @Description:
 * 使用方法：1、前往https://gitee.com/vector-wlc/AsmVsZombies，根据教程下载并安装好AsmVsZombies
 *          2、前往游戏存档文件夹C:/ProgramData/PopCap Games/PlantsVsZombies/userdata，备份原游戏存档，然后用脚本配套的存档文件替换同名文件
 *          3、在Visul Studio Code中打开本脚本，右键点击编辑区空白处，在弹出菜单中选择“AvZ:Run Script”
 * 来自AvZScript公开脚本仓库：
 * 主库：https://github.com/qrmd0/AvZScript
 * 镜像库：https://gitee.com/qrmd/AvZScript
 * Copyright (c) 2022 by error: git config user.name & please set dead value or install git, All Rights Reserved.
 */
#include "GetZombieAbscissas\GetZombieAbscissas.h"
#include "avz.h"
#include "avz_logger.h"
using namespace AvZ;

cresc::Logger Log("D:/Games/PlantsVsZombie/AvZLog");

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
        int time = 1000;
        std::vector<float> abs = GetZombieAbscissas(index_zombie, time);
        for (int t = 0; t <= time; ++t) {
            InsertTimeOperation(t, 1, [=]() {
                auto zombies = GetMainObject()->zombieArray();
                float real_abs = zombies[index_zombie].abscissa();
                ShowErrorNotInQueue("#号僵尸#厘秒的预测坐标为#，实际坐标为#，两者差为#", index_zombie, t, abs[t], real_abs, real_abs - abs[t]);
            });
        }
        ShowErrorNotInQueue("僵尸相对速度为：#", zombies[index_zombie].speed());
    });
}