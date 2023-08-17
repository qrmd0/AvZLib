/*
 * @Author: qrmd
 * @Date: 2023-08-03 23:07:41
 * @LastEditors: qrmd
 * @LastEditTime: 2023-08-17 15:52:49
 * @Description:
 */
#ifndef __FIGHTING__
#define __FIGHTING__

#include "avz.h"
#include "global.h"
#include "paint.h"

// 计算指定波次僵尸在指定行的血量之和，用于智能激活或抗压
// ------------参数------------
// wave 波次
// row 行数
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

// 不断开炮直到前场不存在本波僵尸（伴舞除外），用于收尾
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
            return zmb->AtWave() == wave - 1 && zmb->Abscissa() > 320 && zmb->Type() != ABACKUP_DANCER;
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

// 自动种植三叶草吹走漏掉的气球僵尸
// ------------参数------------
// row 种植三叶草的格子行数
// col 种植三叶草的格子列数
// threshold 存在横坐标小于多少的气球僵尸时种植三叶草
void AAutoBlover(int row, int col, float threshold = 250)
{
    if (AGetSeedIndex(ABLOVER) < 0)
        return;
    AConnect([] { return true; }, [=] {
        for (auto&& zmb : aAliveZombieFilter) {
            if (zmb.Type() == ABALLOON_ZOMBIE && zmb.Abscissa() <= threshold) {
                for (auto&& sd : aAliveSeedFilter) {
                    if (sd.Type() == ABLOVER && sd.IsUsable() && AGetIsPlantAllowable(ABLOVER, row, col)) {
                        ACard(ABLOVER, row, col);
                        return;
                    }
                }
            }
        } });
}

ALogger<AConsole> _qmConsoleLogger;
bool _qmIsSkipping = false;
// 智能跳帧，按键启停，检测到指定位置的指定植物不血量全满时自动停止，用于挂机冲关
// ------------参数------------
// startKey 启动键的虚拟按键码，详见：https://learn.microsoft.com/zh-cn/windows/win32/inputdev/virtual-key-codes
// stopKey 停止键的虚拟按键码
// grids 要检测的格子列表，玉米加农炮为后轮格子
// types 要检测的植物种类，默认为{玉米加农炮、忧郁蘑菇、冰瓜、双子向日葵}
void ASmartSkipTick(const int& startKey, const int& stopKey, const std::vector<AGrid>& grids, const std::vector<APlantType>& types = {ACOB_CANNON, AGLOOM_SHROOM, AWINTER_MELON, ATWIN_SUNFLOWER})
{
    _qmConsoleLogger.Info("智能跳帧已就绪，按 # 开始，按 # 或阵型受损时停止", AGetValue(_keyNames, startKey), AGetValue(_keyNames, stopKey));
    auto GetIsPlantsFullHp = [=]() -> bool {
        for (auto each : grids) {
            for (auto type : types) {
                if (AGetPlantIndex(each.row, each.col, type) == -1)
                    return false;
                for (auto&& plt : aAlivePlantFilter) {
                    if (plt.Row() + 1 == each.row && plt.Col() + 1 == each.col && plt.Type() == type && plt.Hp() < plt.HpMax())
                        return false;
                }
            }
        }
        return true;
    };
    auto condition = [=]() -> bool {
        return GetIsPlantsFullHp() && !AGetIsKeysDown(stopKey, false);
    };
    auto callback = [=]() mutable {
        // ALeftClick(740, 13);
        _qmIsSkipping = false;
        HWND _pvzHwnd = AGetPvzBase()->MRef<HWND>(0x350);
        ShowWindow(_pvzHwnd, SW_RESTORE);
        SetForegroundWindow(_pvzHwnd);
        ALogger<AMsgBox> _msgLogger;
        _msgLogger.Info("您按下了停止键或阵型受损，停止跳帧");
    };
    AConnect(startKey, [=]() mutable {
        _qmIsSkipping = true;
        _qmConsoleLogger.Info("跳帧运行中，按 # 或阵型受损时停止", AGetValue(_keyNames, stopKey));
        ASkipTick(condition, callback);
    });
    if (_qmIsSkipping)
        ASkipTick(condition, callback);
}

// 在控制台中显示本轮出怪逐波统计表
void AShowZombiesList()
{
    const std::string zmbName[33] = {"普僵", "旗帜", "路障", "撑杆", "铁桶", "读报", "铁门", "橄榄", "舞王", "伴舞", "鸭子", "潜水", "冰车", "雪橇", "海豚", "小丑", "气球", "矿工", "跳跳", "雪人", "蹦极", "扶梯", "投篮", "白眼", "小鬼", "僵博", "豌豆", "坚果", "辣椒", "机枪", "窝瓜", "高坚", "红眼"};
    auto zmbList = AGetMainObject()->ZombieList();
    // 各波次各种僵尸数量，第0波表示总计
    int zmbNumAtWave[21][33] = {{0}};
    for (int wave = 1; wave <= 20; ++wave) {
        for (int i = (wave - 1) * 50; i < (wave - 1) * 50 + 50; ++i) {
            if (zmbList[i] == 0xffffffff) {
                break;
            } else {
                zmbNumAtWave[wave][zmbList[i]] += 1;
                zmbNumAtWave[0][zmbList[i]] += 1;
            }
        }
    }
    int flagNumNow = (AMPtr<APvzStruct>(0x6A9EC0)->MPtr<APvzStruct>(0x768)->MPtr<APvzStruct>(0x160)->MRef<int>(0x6C)) * 2;
    std::string text = "本轮已通过旗帜数为：" + std::to_string(flagNumNow) + "，出怪情况：\n\n";
    text += "波次 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 合计\n";
    text += "--------------------------------------------------------------------\n";
    for (int type = 0; type < 33; ++type) {
        if (zmbNumAtWave[0][type] == 0)
            continue;
        text += zmbName[type] + " ";
        for (int wave = 1; wave <= 20; ++wave) {
            std::string _temp = std::to_string(zmbNumAtWave[wave][type]);
            std::string _temp2 = std::string(2 - _temp.length(), '0') + _temp;
            text += _temp2 + " ";
        }
        std::string _temp = std::to_string(zmbNumAtWave[0][type]);
        std::string _temp2 = std::string(3 - _temp.length(), '0') + _temp;
        text += _temp2 + "\n";
    }
    _qmConsoleLogger.Info(text);
}
#endif