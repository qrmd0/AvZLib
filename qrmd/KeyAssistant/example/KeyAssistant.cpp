/*
 * @Author: qrmd
 * @Date: 2022-06-06 15:44:53
 * @LastEditors: qrmd
 * @LastEditTime: 2022-07-06 12:53:11
 * @Description:KeyAssistant按键辅助AvZ插件使用示例
 * 来自AsmVsZombies的公开插件仓库：
 * 主库：https://github.com/qrmd0/AvZLib
 * 镜像库1：https://gitee.com/qrmd/AvZLib
 * 镜像库2：https://gitlab.com/avzlib/AvZLib
 * Copyright (c) 2022 by qrmd, All Rights Reserved.
 */

#include "KeyAssistant.h"
#include "avz.h"

using namespace AvZ;

void Script()
{
    // 重复注入
    OpenMultipleEffective(-1, AvZ::MAIN_UI_OR_FIGHT_UI);
    // 取消报错
    SetErrorMode(NONE);
    // 默认关闭自动收集
    item_collector.stop();
    // 启用KeyAssistant
    key_assistant.enable();
}