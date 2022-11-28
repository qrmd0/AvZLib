/*
MIT License

Copyright (c) 2022 Reisen

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

// Not In Queue
// 将当前dance状态设为state（true = dance，false = 正常）
void SetDance(bool state) {
    if(state)
        __asm__ __volatile__(
            "pushal;"
            "movl 0x6a9ec0, %eax;"
            "movl 0x768(%eax), %ecx;"
            "pushl %ecx;"
            "movl $1, %ebx;"
            "movl $0x41afd0, %eax;"
            "calll *%eax;"
            "popal;"
        );
    else
        __asm__ __volatile__(
            "pushal;"
            "movl 0x6a9ec0, %eax;"
            "movl 0x768(%eax), %ecx;"
            "pushl %ecx;"
            "movl $0, %ebx;"
            "movl $0x41afd0, %eax;"
            "calll *%eax;"
            "popal;"
        );
}

enum class DanceCheatMode {
    FAST,
    SLOW,
    STOP
};

AvZ::TickRunner _reisen_dance_cheat_internal_runner;

// In Queue
// mode = DanceCheatMode::FAST: 加速模式
// mode = DanceCheatMode::SLOW: 减速模式
// mode = DanceCheatMode::STOP: 关闭dance秘籍，恢复正常运动模式
void DanceCheat(DanceCheatMode mode) {
    AvZ::InsertOperation([=](){
        auto& r = _reisen_dance_cheat_internal_runner;
        AvZ::InsertGuard _(false);
        if(r.getStatus() != AvZ::STOPPED)
            r.stop();
        if(mode != DanceCheatMode::STOP)
            r.pushFunc([=](){
                SetDance(mode == DanceCheatMode::SLOW);
            });
        else
            SetDance(false);
    });
}
#endif
