/*
 * @Author: qrmd
 * @Date: 2022-07-05 22:31:59
 * @LastEditors: qrmd
 * @LastEditTime: 2022-07-08 17:28:05
 * @Description:SetZombiesPlus的示例脚本
 * 来自AvZLib公开插件仓库：
 * 主库：https://github.com/qrmd0/AvZLib
 * 镜像库1：https://gitee.com/qrmd/AvZLib
 * 镜像库2：https://gitlab.com/avzlib/AvZLib
 * Copyright (c) 2022 by qrmd, All Rights Reserved.
 */
#include "SetZombiesPlus.h"
#include "avz.h"

using namespace AvZ;

void Script()
{
    // 以自然（游戏原生）方式设置出怪为{普通僵尸、路障僵尸、撑杆僵尸、舞王僵尸、海豚僵尸、小丑僵尸、气球僵尸、矿工僵尸、跳跳僵尸、红眼巨人僵尸}
    SetZombiesPlus({PJ_0, LZ_2, CG_3, WW_8, HT_14, XC_15, QQ_16, KG_17, TT_18, HY_32}, INTERNAL);
    // 显示为了满足限定条件刷新出怪列表的调试信息
    SetIsShowInfo(true);
    // 限定第9波不刷出红眼巨人僵尸、第10波刷出至少10只普通僵尸以及变速的波数不小于15
    SetZombiesLimits({{9, GIGA_GARGANTUAR, "==", 0}, {10, ZOMBIE, ">=", 10}, {TRANS_WAVE, GIGA_GARGANTUAR, ">=", 15}});
    // 设置第18波的出怪为：小鬼僵尸20只、蹦极僵尸14只和巨人僵尸16只
    SetWaveZombieList(18, {{IMP, 20}, {BUNGEE_ZOMBIE, 14}, {GARGANTUAR, 16}});
}
