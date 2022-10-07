/*
 * @Author: qrmd
 * @Date: 2022-08-05 17:20:33
 * @LastEditors: qrmd
 * @LastEditTime: 2022-10-07 20:41:40
 * @Description:SelectCards的增强版本，能瞬间完成选卡并自动填充剩余卡槽
 *          1、将本文件粘贴到AsmVsZombies/inc；
 *          2、在要应用此库的脚本开头添加 #include "SelectCardsPlus.h" ；
 *          3、在README与本库的代码注释中查看函数与变量的用法。
 * 来自AvZLib公开插件仓库：
 * 主库：https://github.com/qrmd0/AvZLib
 * 镜像库1：https://gitee.com/qrmd/AvZLib
 * 镜像库2：https://gitlab.com/avzlib/AvZLib
 * Copyright (c) 2022 by qrmd, All Rights Reserved.
 */

#ifndef __SelectCardsPlus
#define __SelectCardsPlus

#include "avz.h"

using namespace AvZ;

namespace _qrmd {

// 植物卡列表
std::vector<int> _card_list = {};

TickRunner cards_selector;

// 记录游戏时钟
int game_clock_0 = 2147483647;

// 无间隔时间地选择多张植物卡
void _SelectCardsFast();

// *** Not In Queue
// 快速选择植物卡并开始游戏，自动填充多余的卡槽；
// [list]的元素可以是植物的英文名、拼音简称或种类序号；
// [is_accelerate]为true时加快选卡速度，否则不加快
// 示例：// 选择植物卡{咖啡豆、睡莲叶、毁灭菇、模仿毁灭菇、寒冰菇、樱桃炸弹、南瓜头}，若卡槽未被填满，则自动选择植物将其填满
// SelectCardsPlus({COFFEE_BEAN, HY_16, HMG_15, M_DOOM_SHROOM, 14, 2, PUMPKIN});
void SelectCardsPlus(std::vector<int> list, bool is_accelerate = true);
void SelectCardsPlus();

}; // namespace _qrmd

void _qrmd::_SelectCardsFast()
{

    if (GetMainObject()->globalClock() - game_clock_0 >= 338) {
        WriteMemory<bool>(false, 0x6A9EAB);
        game_clock_0 = 2147483647;
        {
            InsertGuard ig(false);
            cards_selector.stop();
        }
    }

    if (GetMainObject()->text()->disappearCountdown() || GetMainObject()->selectCardUi_m()->orizontalScreenOffset() != 4250)
        return;

    int it;
    for (int index = 0; index < _card_list.size(); ++index) {
        it = _card_list[index] > IMITATOR ? IMITATOR : _card_list[index];
        if (_card_list[index] > IMITATOR) {
            if (GetPvzBase()->selectCardUi_p()->cardMoveState(it) == 4)
                Asm::chooseImitatorCard(_card_list[index] - IMITATOR - 1);

        } else {
            if (GetPvzBase()->selectCardUi_p()->cardMoveState(it) == 3)
                Asm::chooseCard(_card_list[index]);
        }
    }

    bool is_finish_select_cards;
    is_finish_select_cards = true;
    for (int index = 0; index < _card_list.size(); ++index) {
        it = _card_list[index] > IMITATOR ? IMITATOR : _card_list[index];
        if (GetPvzBase()->selectCardUi_p()->cardMoveState(it) != 1) {
            is_finish_select_cards = false;
            break;
        }
    }
    if (is_finish_select_cards) {
        _card_list.clear();
        Asm::rock();
        game_clock_0 = ReadMemory<int>(0x6A9EC0, 0x768, 0x556C);
    }
}

void _qrmd::SelectCardsPlus(std::vector<int> list, bool is_accelerate)
{

    bool is_imitator_selected = false;
    if (list.size() > 6 + ReadMemory<int>(0x6A9EC0, 0x82C, 0x214)) {
        ShowErrorNotInQueue("SelectCardsPlus:\n    您选择的植物卡的数量超过了卡槽的数量");
        return;
    }
    _card_list.clear();
    for (const auto& each : list) {
        if (each > 87 || each < 0) {
            ShowErrorNotInQueue("SelectCardsPlus:\n    原版游戏不存在种类序号为#的植物卡", each);
            return;
        }

        if (std::find(_card_list.begin(), _card_list.end(), each) != _card_list.end()) {
            ShowErrorNotInQueue("SelectCardsPlus:\n    您重复选择了种类序号为#的植物卡", each);
            return;
        } else {
            _card_list.emplace_back(each);
        }

        if (each > IMITATOR) {
            if (is_imitator_selected) {
                ShowErrorNotInQueue("SelectCardsPlus:\n    您重复选择了模仿者植物卡");
                return;
            }
            is_imitator_selected = true;
        }
    }
    for (int type = 0; type <= COB_CANNON; ++type) {
        if (_card_list.size() == 6 + ReadMemory<int>(0x6A9EC0, 0x82C, 0x214))
            break;
        if (std::find(_card_list.begin(), _card_list.end(), type) != _card_list.end())
            continue;
        _card_list.emplace_back(type);
    }
    int card_fly_time = ReadMemory<int>(0x48606A);
    // 设置植物卡被点选后飞行时间为0
    WriteMemory<uint8_t>(0, 0x48606A);
    InsertTimeOperation(-599, 1, [=]() {
        WriteMemory<uint8_t>(card_fly_time, 0x48606A);
    });
    if (is_accelerate) {
        int original_tick_ms = ReadMemory<int>(0x6A9EC0, 0x454);
        SetGameSpeed(10);
        // 启用游戏内置20倍速
        WriteMemory<bool>(true, 0x6A9EAB);
        InsertTimeOperation(-599, 1, [=]() {
            WriteMemory<int>(original_tick_ms, 0x6A9EC0, 0x454);
        });
    }

    if (cards_selector.getStatus() != RUNNING)
        cards_selector.pushFunc(_SelectCardsFast, false);
}
void _qrmd::SelectCardsPlus()
{
    SelectCardsPlus({}, true);
}
using _qrmd::SelectCardsPlus;

#endif