/*
MIT License

Copyright (c) 2022-2023 Reisen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef _REISEN_DANCE_CHEAT_DANCE_CHEAT_H
#define _REISEN_DANCE_CHEAT_DANCE_CHEAT_H
#include "avz.h"

#if __AVZ_VERSION__ <= 221001
#define SELECT_BY_AVZ_VERSION(a, b) a
#else
#define SELECT_BY_AVZ_VERSION(a, b) b
#endif

// 将当前 dance 状态设为 state（true = dance，false = 正常）
void SetDance(bool state) {
    if(SELECT_BY_AVZ_VERSION(AvZ::ReadMemory<int>(0x6a9ec0, 0x82c, 0xf4),
        AGetPvzBase()->MPtr<APvzStruct>(0x82c)->MRef<int>(0xf4)) < 500) {
        SELECT_BY_AVZ_VERSION(AvZ::ShowErrorNotInQueue, AGetInternalLogger()->Error)("智慧树高度不足 500 英尺，无法使用 dance 指令");
        return;
    }
    asm volatile(
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%ecx;"
        "pushl %%ecx;"
        "movl %[state], %%ebx;"
        "movl $0x41afd0, %%eax;"
        "calll *%%eax;"
        :
        : [state] "rm"(unsigned(state))
        : "eax", "ebx", "ecx", "edx"
    );
}

enum class DanceCheatMode {
    FAST,
    SLOW,
    STOP
} _dance_cheat_state = DanceCheatMode::STOP;

SELECT_BY_AVZ_VERSION(AvZ::TickRunner, ATickRunner) _dance_cheat_runner;

class : SELECT_BY_AVZ_VERSION(AvZ::GlobalVar, AStateHook) {
    void SELECT_BY_AVZ_VERSION(enterFight, _EnterFight)() override {
        _dance_cheat_runner.SELECT_BY_AVZ_VERSION(pushFunc, Start)([]{
            if(_dance_cheat_state == DanceCheatMode::STOP)
                return;
            SetDance(_dance_cheat_state == DanceCheatMode::SLOW);
        });
    }
} _dance_cheat_runner_starter;

// mode = DanceCheatMode::FAST: 加速模式
// mode = DanceCheatMode::SLOW: 减速模式
// mode = DanceCheatMode::STOP: 关闭dance秘籍，恢复正常运动模式
void SELECT_BY_AVZ_VERSION(DanceCheatNotInQueue, DanceCheat)(DanceCheatMode mode) {
    if(SELECT_BY_AVZ_VERSION(AvZ::ReadMemory<int>(0x6a9ec0, 0x82c, 0xf4),
        AGetPvzBase()->MPtr<APvzStruct>(0x82c)->MRef<int>(0xf4)) < 500) {
        SELECT_BY_AVZ_VERSION(AvZ::ShowErrorNotInQueue, AGetInternalLogger()->Error)("智慧树高度不足 500 英尺，无法使用 dance 指令");
        return;
    }
    _dance_cheat_state = mode;
    if(mode == DanceCheatMode::STOP)
        SetDance(false);
}

#if __AVZ_VERSION__ <= 221001
void DanceCheat(DanceCheatMode mode) {
    AvZ::InsertOperation([=]{ DanceCheatNotInQueue(mode); });
}
#else
[[nodiscard("ARelOp 需要绑定到时间才会执行")]]
ARelOp DanceCheatR(DanceCheatMode mode) {
    return ARelOp([=]{ DanceCheat(mode); });
}
#endif

#undef SELECT_BY_AVZ_VERSION
#endif
