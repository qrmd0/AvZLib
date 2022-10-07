/*
 * @Author: qrmd
 * @Date: 2022-06-09 00:03:58
 * @LastEditors: qrmd
 * @LastEditTime: 2022-10-07 20:35:28
 * @Description:KeyAssistant按键辅助AvZ插件
 * 来自AsmVsZombies的公开插件仓库：
 * 主库：https://github.com/qrmd0/AvZLib
 * 镜像库1：https://gitee.com/qrmd/AvZLib
 * 镜像库2：https://gitlab.com/avzlib/AvZLib
 * Copyright (c) 2022 by qrmd, All Rights Reserved.
 */

#ifndef __KeyAssistant
#define __KeyAssistant

#include "avz.h"
using namespace AvZ;
#include <vector>

class KeyAssistant {
private:
    KeyConnector key_connect_plus;
    const std::vector<float> game_speeds = {0.1, 0.2, 0.5, 1.0, 2.0, 5.0, 10.0};
    // 舞王僵尸状态
    bool is_maid_activated = false;

public:
    // *** In Queue
    // 启用
    void enable()

    {

        InsertOperation([=]() {
            ShowErrorNotInQueue("按键辅助已启用，快捷键说明：\n数字键：在光标处种植对应卡槽位置的植物\nC：铲除光标处的植物\nR：开关高级暂停\nA：减慢游戏运行速度\nD：加快游戏运行速度\nS：恢复游戏运行速度\nX：开关女仆秘籍\nZ：开关自动收集掉落物\nP：停用按键辅助，恢复游戏运行速度并停用女仆秘籍");
            key_connect_plus.clear();
            // 按键种植
            key_connect_plus.add('1', [=]() { CardNotInQueue(1, MouseRow(), MouseCol()); });
            key_connect_plus.add('2', [=]() { CardNotInQueue(2, MouseRow(), MouseCol()); });
            key_connect_plus.add('3', [=]() { CardNotInQueue(3, MouseRow(), MouseCol()); });
            key_connect_plus.add('4', [=]() { CardNotInQueue(4, MouseRow(), MouseCol()); });
            key_connect_plus.add('5', [=]() { CardNotInQueue(5, MouseRow(), MouseCol()); });
            key_connect_plus.add('6', [=]() { CardNotInQueue(6, MouseRow(), MouseCol()); });
            key_connect_plus.add('7', [=]() { CardNotInQueue(7, MouseRow(), MouseCol()); });
            key_connect_plus.add('8', [=]() { CardNotInQueue(8, MouseRow(), MouseCol()); });
            key_connect_plus.add('9', [=]() { CardNotInQueue(9, MouseRow(), MouseCol()); });
            key_connect_plus.add('0', [=]() { CardNotInQueue(10, MouseRow(), MouseCol()); });

            // 按键铲除（优先南瓜头）
            key_connect_plus.add('C', [=]() { ShovelNotInQueue(MouseRow(), MouseCol(), true); });

            // 按键发炮
            key_connect_plus.add('F', [=]() {
                pao_operator.autoGetPaoList();
                pao_operator.pao(MouseRow(), MouseCol());
            });

            // 按键记录游戏进度
            key_connect_plus.add('T', [=]() {
                Asm::saveData();
            });

            // 按键回退游戏进度
            key_connect_plus.add('Y', [=]() {
                Asm::loadData();
            });

            // 按键高级暂停
            SetAdvancedPauseKey('R');

            // 按键控制游戏运行速度
            key_connect_plus.add('A', [=]() {
                int game_speed_gear = 7;
                float game_speed = 10.0 / GetPvzBase()->tickMs();
                for (int i = 0; i < game_speeds.size(); ++i) {
                    if (game_speed <= game_speeds[i]) {
                        game_speed_gear = i;
                        break;
                    }
                }
                if (game_speed_gear > 0) {
                    SetGameSpeed(game_speeds[game_speed_gear - 1]);
                }
            });
            key_connect_plus.add('D', [=]() {
                int game_speed_gear = 0;
                float game_speed = 10.0 / GetPvzBase()->tickMs();
                for (int i = game_speeds.size() - 1; i >= 0; --i) {
                    if (game_speed >= game_speeds[i]) {
                        game_speed_gear = i;
                        break;
                    }
                }
                if (game_speed_gear < game_speeds.size() - 1) {
                    SetGameSpeed(game_speeds[game_speed_gear + 1]);
                }
            });
            key_connect_plus.add('S', [=]() {
                SetGameSpeed(1.0);
            });

            // 按键控制女仆秘籍
            key_connect_plus.add('X', [=]() {
                if (is_maid_activated) {
                    MaidCheats::dancing();
                } else {
                    MaidCheats::stop();
                }
                is_maid_activated = !is_maid_activated;
            });
            // 按键控制自动收集掉落物
            key_connect_plus.add('Z', [=]() {
                if (item_collector.getStatus() == RUNNING) {
                    item_collector.stop();
                } else if (item_collector.getStatus() == STOPPED) {
                    item_collector.start();
                }
            });
            // 按键停用按键辅助
            key_connect_plus.add('P', [=]() {
                disable();
            });
        });
    }
    // *** In Queue
    // 停用
    void disable()
    {
        InsertOperation([=]() {
            key_connect_plus.clear();
            SetGameSpeed(1.0);
            MaidCheats::stop();
        });
        ShowErrorNotInQueue("按键辅助已停用，将恢复游戏运行速度并停用女仆秘籍");
    }
} key_assistant;

#endif