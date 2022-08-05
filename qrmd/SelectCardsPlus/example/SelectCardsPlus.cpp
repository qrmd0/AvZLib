/*
 * @Author: qrmd
 * @Date: 2022-04-24 19:52:51
 * @LastEditors: qrmd
 * @LastEditTime: 2022-08-05 23:29:47
 * @Description:SelectCardsPlus的示例脚本
 * 来自AvZLib公开插件仓库：
 * 主库：https://github.com/qrmd0/AvZLib
 * 镜像库1：https://gitee.com/qrmd/AvZLib
 * 镜像库2：https://gitlab.com/avzlib/AvZLib
 * Copyright (c) 2022 by qrmd, All Rights Reserved.
 */
#include "SelectCardsPlus.h"
#include "avz.h"

using namespace AvZ;

void Script()
{
    // 选择植物卡{咖啡豆、睡莲叶、毁灭菇、模仿毁灭菇、寒冰菇、樱桃炸弹、南瓜头}，若卡槽未被填满，则自动选择植物将其填满
    SelectCardsPlus({COFFEE_BEAN, HY_16, HMG_15, M_DOOM_SHROOM, 14, 2, PUMPKIN});

    SetTime(-599, 1);
    pao_operator.autoGetPaoList();
}