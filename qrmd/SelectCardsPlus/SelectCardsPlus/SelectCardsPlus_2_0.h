/*
 * @Author: qrmd
 * @Date: 2022-08-05 17:20:33
 * @LastEditors: qrmd
 * @LastEditTime: 2023-08-14 11:18:53
 * @Description:SelectCards的增强版本，能瞬间完成选卡并自动填充剩余卡槽
 *          1、将本文件粘贴到AsmVsZombies/inc；
 *          2、在要应用此库的脚本开头添加 #include "SelectCardsPlus.h" ；
 *          3、在README与本库的代码注释中查看函数与变量的用法。
 * 来自AvZLib公开插件仓库：
 * 主库：https://github.com/qrmd0/AvZLib
 * 镜像库1：https://gitee.com/qrmd/AvZLib
 * 镜像库2：https://gitlab.com/avzlib/AvZLib
 * Copyright (c) 2022 by qrmd, AAll Rights Reserved.
 */

#ifndef __SELECTCARDSPLUS_2_0__
#define __SELECTCARDSPLUS_2_0__

#include "avz.h"

namespace _qrmd {
ALogger<AMsgBox> _msgBoxLogger;
ALogger<AConsole> _csLog;
// 设置20倍速
void Set20xSpeed(bool isEnable = true)
{
    AMRef<bool>(0x6A9EAB) = isEnable;
}

const int _defaulCcardFlyTime = 17;
// 设置植物卡被点选后飞入卡槽内的耗时
void SetCardFlyTime(int time = 0)
{
    AMRef<uint8_t>(0x48606A) = time;
}

// 返回游戏帧时长
int GetTickMs()
{
    return AMPtr<APvzStruct>(0x6A9EC0)
        ->MRef<int>(0x454);
}

// 设置游戏帧时长
void SetTickMs(int time)
{
    AMPtr<APvzStruct>(0x6A9EC0)->MRef<int>(0x454) = time;
}

// 返回购买的卡槽数量
int GetPurchasedCardSlotsNum()
{
    return AMPtr<APvzStruct>(0x6A9EC0)
        ->MPtr<APvzStruct>(0x82C)
        ->MRef<int>(0x214);
}

int _tickMs = 10;
// 还原内存
void restoreMemory()
{
    SetCardFlyTime(_defaulCcardFlyTime);
    SetTickMs(_tickMs);
}
class Debugger : public AStateHook {
protected:
    virtual void _EnterFight() override
    {
        restoreMemory();
    }
    virtual void _BeforeScript() override
    {
        _tickMs = GetTickMs();
    }
};
Debugger debug;

// 记录游戏时钟
int gameClock = 2147483647;
// 点击Rock之后的取消20倍速的延迟时间
const int _disable20xDelay = 338;
ATickRunner cardSelector;
std::vector<int> _cardsList = {};
// 无间隔选卡，结合全局TickRunner使用
void _SelectCardsFast()
{
    if (AGetMainObject()->GlobalClock() - gameClock >= _disable20xDelay) {
        restoreMemory();
        Set20xSpeed(false);
        gameClock = 2147483647;
        cardSelector.Stop();
    }
    if (AGetMainObject()->Words()->DisappearCountdown() || AGetMainObject()->SelectCardUi_m()->OrizontalScreenOffset() != 4250)
        return;

    int it;
    for (int index = 0; index < _cardsList.size(); ++index) {

        it = _cardsList[index] > AIMITATOR ? AIMITATOR : _cardsList[index];
        if (_cardsList[index] > AIMITATOR) {
            if (AGetPvzBase()->SelectCardUi_p()->CardMoveState(it) == 4)
                AAsm::ChooseImitatorCard(_cardsList[index] - AM_PEASHOOTER);
        } else {
            if (AGetPvzBase()->SelectCardUi_p()->CardMoveState(it) == 3)
                AAsm::ChooseCard(_cardsList[index]);
        }
    }

    bool isNeedRock;
    isNeedRock = true;
    for (int index = 0; index < _cardsList.size(); ++index) {
        it = _cardsList[index] > AIMITATOR ? AIMITATOR : _cardsList[index];
        if (AGetPvzBase()->SelectCardUi_p()->CardMoveState(it) != 1) {
            isNeedRock = false;
            break;
        }
    }
    if (isNeedRock) {
        _cardsList.clear();
        gameClock = AGetMainObject()->GlobalClock();
        AAsm::Rock();
        isNeedRock = false;
    }
}
// 快速选择植物卡并开始游戏，自动填充多余的卡槽；
// ------------参数------------
// list 要选择的种子列表，不足卡槽数量时自动选择种子填充；
// isAccelerate  为true时加快选卡速度，否则不加快
// ------------示例------------
// 快速选择植物卡{咖啡豆、睡莲叶、毁灭菇、模仿毁灭菇、寒冰菇、樱桃炸弹、南瓜头}，若卡槽未被填满，则自动选择植物将其填满
// ASelectCardsPlus({ACOFFEE_BEAN, AHY_16, AHMG_15, AM_DOOM_SHROOM, AHBG_14, ACHERRY_BOMB, APUMPKIN});
void ASelectCardsPlus(const std::vector<int>& list, bool isAccelerate = true)
{

    int cardSlotsNum = 6 + GetPurchasedCardSlotsNum();

    if (list.size() > cardSlotsNum) {
        _msgBoxLogger.Error("SelectCardsPlus:\n    您选择的植物卡的数量超过了卡槽的数量");
        restoreMemory();
        return;
    }

    bool isImitatorSelected = false;
    for (auto each : list) {
        if (each > 87 || each < 0) {
            restoreMemory();
            _msgBoxLogger.Error("SelectCardsPlus:\n    原版游戏不存在种类序号为#的植物卡", each);
            return;
        }
        if (std::find(_cardsList.begin(), _cardsList.end(), each) != _cardsList.end()) {
            restoreMemory();
            _msgBoxLogger.Error("SelectCardsPlus:\n    您重复选择了种类序号为#的植物卡", each);
            return;
        } else {
            _cardsList.emplace_back(each);
        }
        if (each > int(AIMITATOR)) {
            if (isImitatorSelected) {
                restoreMemory();
                _msgBoxLogger.Error("SelectCardsPlus:\n    您重复选择了模仿者植物卡");
                return;
            }
            isImitatorSelected = true;
        }
    }
    for (int type = 0; type <= ACOB_CANNON; ++type) {
        if (_cardsList.size() == cardSlotsNum)
            break;
        if (std::find(_cardsList.begin(), _cardsList.end(), type) != _cardsList.end())
            continue;
        _cardsList.emplace_back(type);
    }

    if (isAccelerate) {
        Set20xSpeed(true);
        SetTickMs(1);
    }
    SetCardFlyTime(0);
    cardSelector.Start(_SelectCardsFast, true);
}
void ASelectCardsPlus()
{
    ASelectCardsPlus({}, true);
}
}; // namespace _qrmd

using _qrmd::ASelectCardsPlus;

#endif