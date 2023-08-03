/*
 * @Author: qrmd
 * @Date: 2023-08-03 23:07:41
 * @LastEditors: qrmd
 * @LastEditTime: 2023-08-03 23:46:16
 * @Description:
 */
#ifndef __FIGHTING__
#define __FIGHTING__

#include "avz.h"
#include "global.h"

// 计算指定波次僵尸在指定行的血量之和，用于智能激活或抗压
// ------------参数------------
// wave 波次
int GetZombiesHp(int wave, int row)
{
    int hp = 0;
    AAliveFilter<AZombie> zombieFilter;
    for (auto&& zombie : zombieFilter) {
        if (zombie.AtWave() == wave - 1 && zombie.Row() == row - 1 && zombie.Type() != ABACKUP_DANCER) {
            hp += zombie.Hp() + zombie.OneHp() + zombie.TwoHp() / 5;
        }
    }
    return hp;
}

// 不断开炮直到前场不存在本波僵尸，用于收尾
// ------------参数------------
// wave 要收尾的波次
// time 第一次开炮的本波时刻
// delay 开炮间隔时间
// ------------示例------------
// // PE经典八炮的收尾操作
// for (auto wave : {9, 19, 20}) {
//     PPEndingTheWave(wave, 869 - 200 + 869 - 373, 869);
// }
void PPEndingTheWave(int wave, int time, int delay)
{
    AConnect(ATime(wave, time), [=] {
        bool isCleared = true;
        AAliveFilter<AZombie> zombieAtFrontFromThisWave([=](AZombie* zmb) {
            return zmb->AtWave() == wave - 1 && zmb->Abscissa() > 320;
        });
        for (auto&& zmb : zombieAtFrontFromThisWave) {
            (void)zmb;
            isCleared = false;
            break;
        }
        if (isCleared) {
            return;
        } else {
            aCobManager.Fire({{2, 8.8}, {5, 8.8}});
            return PPEndingTheWave(wave, time + delay, delay);
        }
    });
}

#endif