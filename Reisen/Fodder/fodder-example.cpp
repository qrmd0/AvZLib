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

#include "avz.h"
#include "fodder.h"

using namespace AvZ;

void Script() {
    // Fodder既可以定义在全局，也可以定义在局部。
    // 库提供了默认全局实例fodder，可以直接使用，无需定义。
    Fodder c({FLOWER_POT, GARLIC, PUFF_SHROOM, M_PUFF_SHROOM, BLOVER}); // 初始化要用的垫材
    fodder.setCards({FLOWER_POT, GARLIC, PUFF_SHROOM, M_PUFF_SHROOM, BLOVER}); // 另一种初始化方式

    SetTime(-599, 1);
    c(); // 在所有陆地行的9列（会自动检测场地）放垫材，延迟266cs铲除。
         // 花盆优先放在梯子最多的路，多路相同时放在小丑和橄榄最多的路。
    Delay(751);
    c({1, 2}, 1, 8, false); // 在1-8和2-8放垫材，秒铲。
                            // 由于使用了严格顺序，1-8的一定是花盆，2-8的一定是大蒜。
    c({3, 4, 5}, -1); // 不铲垫，不建议使用
    // c({1}); 由于此时所有垫材都在CD，运行时会报错。

    Fodder c_631({GARLIC, BLOVER}); // 可以定义多个Fodder实例，满足不同的垫材需求。
    SetTime(252, 2);
    c_631({2, 4}, 333 - 252);
    c({1, 5}); // 即使同时使用多个Fodder，也不会导致冲突。
               // 但如果交换这两行，c会直接用掉大蒜，c_631就会没得用报错了。

    // 注意[=]；换成[&]会导致游戏崩溃
    InsertTimeOperation(0, 10, [=](){
        c();
    });

    for(int t = 1000; t < 4300; t += 207) {
        SetTime(t);
        fodder({1}, 134);
    } // 自动垫巨人也很轻松！
}
