/*
 * @字符编码: UTF-8
 * @作者: qrmd
 * @时间: 2022-06-09 15:44:08
 * @描述: 按键辅助
 * @AvZ版本：220213
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