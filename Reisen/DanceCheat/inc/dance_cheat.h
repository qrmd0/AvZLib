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

#ifndef REISEN_DANCE_CHEAT_DANCE_CHEAT_H
#define REISEN_DANCE_CHEAT_DANCE_CHEAT_H

#include "avz.h"

using namespace AvZ;

// Not In Queue
// 将当前dance状态设为state（true = dance，false = 正常）
void setDance(bool state) {
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

namespace _dance_cheat_internal {
    TickRunner runner;

    class Manager : GlobalVar {
    public:
        void beforeScript() {
            runner.pushFunc([](){
                setDance(false);
            });
            InsertTimeOperation(-599, 1, [](){
                runner.pause();
            });
        }
    } _;
};

namespace DanceCheat {
    // In Queue
    void start() {
        _dance_cheat_internal::runner.goOn();
    }

    // In Queue
    void stop() {
        _dance_cheat_internal::runner.pause();
    }
};
#endif
