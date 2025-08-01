/*
MIT License

Copyright (c) 2025 xiao_He

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

/*
version 0.2.0.2025_07_29
*/

#ifndef __MAKE_SKIP_H__
#define __MAKE_SKIP_H__

#include "stdlib.h"
#include <avz.h>
#include <vector>

using namespace AvZ;
AvZ::TickRunner __makeskip_tick_runner;

auto plant_array = [] { return GetMainObject()->plantArray(); };
auto zombie_array = [] { return GetMainObject()->zombieArray(); };
auto anim_array = [] { return GetPvzBase()->animationMain()->animationOffset()->animationArray(); };

// 动画类型
enum AnimType {
    PLANT_MAIN,         // 植物 本体
    PLANT_HEAD_1,       // 植物 头1
    PLANT_HEAD_2,       // 植物 头2
    PLANT_HEAD_3,       // 植物 头3
    PLANT_POTATO_LIGHT, // 植物 土豆雷闪灯
    PLANT_BLINK,        // 植物 眨眼
    PLANT_ZZZ,          // 植物 睡觉
    ZOMBIE_MAIN,        // 僵尸 本体
    ZOMBIE_BALL,        // 僵尸 冰火球
    ZOMBIE_FLAG,        // 僵尸 旗帜/植物僵尸头
    ZOMBIE_MOWER,       // 僵尸 被小推车碾压
};

// 复制模式
enum CopyMode {
    PP, // 植物 本体动画ID复制给 植物
    PZ, // 植物 本体动画ID复制给 僵尸
    ZP, // 僵尸 本体动画ID复制给 植物
    ZZ, // 僵尸 本体动画ID复制给 僵尸
};

// 咖啡豆的状况
enum CoffeeCondition {
    WAKENING,          // 唤醒蘑菇中
    DISAPPEARING,      // 消失中
    SUSPENDED,         // pss悬浮状态
    ERROR_COFFEE = -1, // 出错
};

// 植物防御域种类
enum PlantRectType {
    NORVAL_RECT,
    COB_RECT,
    TALLNUT_RECT,
    PUMPKIN_RECT,
};

// 在 row 行放置一只横坐标为 x、类型为 type 的僵尸，返回僵尸的编号
int PutZombie(int row, int x, ZombieType type) {
    Zombie* zombieArray = GetMainObject()->zombieArray();
    int index = GetMainObject()->zombieNext();
    int __row = row - 1;
    int __col = 9;
    __asm__ __volatile__(
        "pushal;"
        "movl %[__row], %%eax;"
        "pushl %[__col];"
        "pushl %[__type];"
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x768(%%ecx), %%ecx;"
        "movl 0x160(%%ecx), %%ecx;"
        "movl $0x42a0f0, %%edx;"
        "calll *%%edx;"
        "popal;"
        :
        : [__row] "g"(__row), [__col] "g"(__col), [__type] "g"(type)
        :);
    zombie_array()[index].abscissa() = x;
    return index;
}

// 在 row 行 col 列放置一个 type 类型的植物，返回植物的编号
int PutPlant(int row, int col, PlantType type) {
    Plant* plantArray = GetMainObject()->plantArray();
    int index = GetMainObject()->plantNext();
    int __row = row - 1;
    int __col = col - 1;

    int __imitatorType = -1;
    if (type >= M_PEASHOOTER) {
        __imitatorType = type - M_PEASHOOTER;
        type = IMITATOR;
    }
    __asm__ __volatile__(
        "pushal;"
        "pushl %[__imitatorType];"
        "pushl %[__type];"
        "movl %[__row], %%eax;"
        "pushl %[__col];"
        "movl 0x6a9ec0, %%ebp;"
        "movl 0x768(%%ebp), %%edi;"
        "pushl %%edi;"
        "movl $0x40d120, %%edx;"
        "calll *%%edx;"
        "popal;"
        :
        : [__imitatorType] "m"(__imitatorType), [__type] "m"(type), [__row] "m"(__row), [__col] "m"(__col)
        :);
    return index;
}

// 获取编号为 index 的对象的类型为 type 的动画ID
int& _GetAnimationId(AnimType type, int index) {
    switch (type) {
    case PLANT_MAIN:
        return plant_array()[index].mRef<int>(0x94);
    case ZOMBIE_MAIN:
        return zombie_array()[index].mRef<int>(0x118);

    case PLANT_HEAD_1:
        return plant_array()[index].mRef<int>(0x98);
    case PLANT_HEAD_2:
        return plant_array()[index].mRef<int>(0x9C);
    case PLANT_HEAD_3:
        return plant_array()[index].mRef<int>(0xA0);
    case PLANT_BLINK:
        return plant_array()[index].mRef<int>(0xA4);
    case PLANT_POTATO_LIGHT:
        return plant_array()[index].mRef<int>(0xA8);
    case PLANT_ZZZ:
        return plant_array()[index].mRef<int>(0xAC);
    case ZOMBIE_BALL:
        return zombie_array()[index].mRef<int>(0x140);
    case ZOMBIE_FLAG:
        return zombie_array()[index].mRef<int>(0x144);
    case ZOMBIE_MOWER:
        return zombie_array()[index].mRef<int>(0x150);
    }
}

// 获取编号为 index 的对象的类型为 type 的动画编号
short int& _GetAnimationIndex(AnimType type, int index) {
    return *(short*)&_GetAnimationId(type, index);
}

// 获取编号为 index 的咖啡豆的情况。
CoffeeCondition _GetCoffeeCondition(int index) {
    if (plant_array()[index].type() != COFFEE_BEAN) {
        return ERROR_COFFEE;
    } else if (plant_array()[index].state() == 0 && plant_array()[index].explodeCountdown() > 0) {
        return WAKENING;
    } else if (plant_array()[index].state() == 2) {
        return DISAPPEARING;
    } else if (plant_array()[index].state() == 0 && plant_array()[index].explodeCountdown() == 0) {
        return SUSPENDED;
    } else {
        return ERROR_COFFEE;
    }
}

// 播放编号为 index 的植物的闲置动画。
void _PlantPlayIdleAnim(int index, float rate) {
    __asm__ __volatile__(
        "pushal;"
        "pushl %[rate];"
        "movl 0x6a9ec0, %%edi;"
        "movl 0x768(%%edi), %%edi;"
        "movl 0xac(%%edi), %%edi;"
        "imull $0x14c, %[index], %%edx;"
        "addl %%edx, %%edi;"
        "movl $0x468280, %%ebx;"
        "calll *%%ebx;"
        "popal;"
        :
        : [index] "m"(index), [rate] "m"(rate)
        :);
}

// 进入关卡后立刻生效一次的类
class Once {
public:
    // 销毁编号为 index 的对象的类型为 type 的动画
    static void DeleteAnimation(AnimType type, int index) {
        int anim_id = _GetAnimationId(type, index);
        __asm__ __volatile__(
            "pushal;"
            "movl 0x6a9ec0, %%eax;"
            "movl %[anim_id], %%edx;"
            "movl $0x453CF0, %%ebx;"
            "calll *%%ebx;"
            "popal;"
            :
            : [anim_id] "m"(anim_id)
            :);
    }

    // 将编号为 index_1 的对象的类型为 type_1 的动画ID, 复制给编号为 index_2 的对象的类型为 type_2 的动画ID
    static void CopyAnimationId(AnimType type_1, int index_1, AnimType type_2, int index_2) {
        DeleteAnimation(type_2, index_2);
        _GetAnimationId(type_2, index_2) = _GetAnimationId(type_1, index_1);
    }

    // 以 mode 模式将编号为 index_1 的对象的本体动画ID复制给编号为 index_2 的对象
    static void CopyAnimationId(CopyMode mode, int index_1, int index_2) {
        switch (mode) {
        case PP:
            CopyAnimationId(PLANT_MAIN, index_1, PLANT_MAIN, index_2);
            break;
        case PZ:
            CopyAnimationId(PLANT_MAIN, index_1, ZOMBIE_MAIN, index_2);
            break;
        case ZP:
            CopyAnimationId(ZOMBIE_MAIN, index_1, PLANT_MAIN, index_2);
            break;
        case ZZ:
            CopyAnimationId(ZOMBIE_MAIN, index_1, ZOMBIE_MAIN, index_2);
            break;
        }
    }

    // 以 mode 方式将编号为 index_1 的对象的本体动画ID复制给编号为 indexs 的对象
    static void CopyAnimationId(CopyMode mode, int index_1, std::vector<int> indexes) {
        for (int idx : indexes) {
            CopyAnimationId(mode, index_1, idx);
        }
    }

    // 将编号为 index_1 的对象的类型为 type_1 的动画编号, 复制给编号为 index_2 的对象的类型为 type_2 的动画编号
    static void CopyAnimationIndex(AnimType type_1, int index_1, AnimType type_2, int index_2) {
        DeleteAnimation(type_2, index_2);
        _GetAnimationIndex(type_2, index_2) = _GetAnimationIndex(type_1, index_1);
    }

    // 以 mode 模式将编号为 index_1 的对象的本体动画编号复制给编号为 index_2 的对象
    static void CopyAnimationIndex(CopyMode mode, int index_1, int index_2) {
        switch (mode) {
        case PP:
            CopyAnimationIndex(PLANT_MAIN, index_1, PLANT_MAIN, index_2);
            break;
        case PZ:
            CopyAnimationIndex(PLANT_MAIN, index_1, ZOMBIE_MAIN, index_2);
            break;
        case ZP:
            CopyAnimationIndex(ZOMBIE_MAIN, index_1, PLANT_MAIN, index_2);
            break;
        case ZZ:
            CopyAnimationIndex(ZOMBIE_MAIN, index_1, ZOMBIE_MAIN, index_2);
            break;
        }
    }

    // 以 mode 方式将编号为 index_1 的对象的本体动画编号复制给编号为 indexs 的对象们
    static void CopyAnimationIndex(CopyMode mode, int index_1, std::vector<int> indexes) {
        for (int idx : indexes) {
            CopyAnimationIndex(mode, index_1, idx);
        }
    }

    // 将编号为 index 的对象的类型 type 的动画的动画速度设为0、动画进度设为 rate（默认保持不变）。
    // 存模仿者时，如果要消除生效时间波动，可将rate设为0.998064。
    static void LockAnimation(AnimType type, int index, float rate = -1) {
        short int anim_index = _GetAnimationIndex(type, index);
        anim_array()[anim_index].mRef<float>(0x8) = 0;
        if (rate != -1)
            anim_array()[anim_index].circulationRate() = rate;
    }

    // 将编号为 index 的植物设置为pss状态。（不保证对所有植物有效）
    static void MakePss(int index) {
        int x = rand() % 800;
        int y = 80 + rand() % 520;
        __asm__ __volatile__( // 调用点炮函数，设置发射倒计时为206。
            "pushal;"
            "movl 0x6a9ec0, %%eax;"
            "movl 0x768(%%eax), %%edi;"
            "movl 0xac(%%edi), %%eax;"
            "movl $0x14c, %%ecx;"
            "imull %[index], %%ecx;"
            "addl %%ecx, %%eax;"
            "pushl %[y];"
            "pushl %[x];"
            "movl $0x466d50, %%edx;"
            "calll *%%edx;"
            "popal;"
            :
            : [x] "m"(x), [y] "m"(y), [index] "m"(index)
            :);

        plant_array()[index].shootCountdown() = 2; // 发射倒计时改为2，接下来会立刻发射。
        WriteMemory<uint8_t>(0xe9, 0x004672b0);
        WriteMemory<uint32_t>(0x00000294, 0x004672b1);
        WriteMemory<uint16_t>(0xc483, 0x00467549);
        WriteMemory<uint8_t>(0x14, 0x0046754b); // 修改发射子弹函数，跳过子弹生成和初始化部分。
        __asm__ __volatile__(
            "pushal;"
            "movl 0x6a9ec0, %%eax;"
            "movl 0x768(%%eax), %%edi;"
            "movl 0xac(%%edi), %%eax;"
            "movl $0x14c, %%ecx;"
            "imull %[index], %%ecx;"
            "addl %%ecx, %%eax;"
            "movl $0x464820, %%ebx;"
            "calll *%%ebx;"
            "popal;"
            :
            : [index] "m"(index)
            :); // 调用发射子弹函数。
        WriteMemory<uint8_t>(0xe8, 0x004672b0);
        WriteMemory<uint32_t>(0xfffa636b, 0x004672b1);
        WriteMemory<uint16_t>(0x58d9, 0x00467549);
        WriteMemory<uint8_t>(0x48, 0x0046754b); // 还原发射子弹函数。
    }

    // 将编号为 indexes 的植物们设置为pss状态。（不保证对所有植物有效）
    static void MakePss(std::vector<int> indexes) {
        for (int idx : indexes) {
            MakePss(idx);
        }
    }

    // 将编号为 index 的咖啡豆设置为pss悬浮状态。
    static void MakePssCoffee(int index) {
        if (index >= GetMainObject()->plantTotal() || plant_array()[index].isDisappeared()) {
            ShowErrorNotInQueue("编号为#的植物不存在。", index);
            return;
        }
        if (plant_array()[index].type() != COFFEE_BEAN) {
            ShowErrorNotInQueue("编号为#的植物不是咖啡豆或模仿者未完成变身。", index);
            return;
        }
        switch (_GetCoffeeCondition(index)) {
        case WAKENING:
            MakePss(index);
            break;
        case DISAPPEARING:
            plant_array()[index].state() = 0;
            _PlantPlayIdleAnim(index, 12.0);
            MakePss(index);
            break;
        case SUSPENDED:
            break;
        }
    }

    // 将编号为 indexes 的咖啡豆设置为pss状态。
    static void MakePssCoffee(std::vector<int> indexes) {
        for (int idx : indexes) {
            MakePssCoffee(idx);
        }
    }

    // 将编号为 index 的窝瓜设置为pss状态，坐标设置为 x 。
    static void MakePssSquash(int index, int x) {
        if (index >= GetMainObject()->plantTotal() || plant_array()[index].isDisappeared()) {
            ShowErrorNotInQueue("编号为#的植物不存在。", index);
            return;
        }
        if (plant_array()[index].type() != SQUASH) {
            ShowErrorNotInQueue("编号为#的植物不是窝瓜或模仿者未完成变身。", index);
            return;
        }
        MakePss(index);
        plant_array()[index].abscissa() = x;
    }

    // 将编号为 indexes 的窝瓜设置为pss状态，坐标设置为 x 。
    static void MakePssSquash(std::vector<int> indexes, int x) {
        for (int idx : indexes) {
            MakePssSquash(idx, x);
        }
    }

    // 将编号为 index 的窝瓜设置为pss状态，防御域右边界与 col 列(炮前轮)的 plant_rect_type 种类植物重合。
    static void MakePssSquash(int index, PlantRectType plant_rect_type, int col) {
        int offset;
        switch (plant_rect_type) {
        case NORVAL_RECT:
            offset = 0;
            break;
        case COB_RECT:
            offset = -10;
            break;
        case TALLNUT_RECT:
            offset = 20;
            break;
        case PUMPKIN_RECT:
            offset = 30;
            break;
        }
        int x = (col - 1) * 80 + 40 + offset;
        MakePssSquash(index, x);
    }

    // 将编号为 indexes 的窝瓜设置为pss状态，防御域右边界与 col 列(炮前轮)的 plant_rect_type 种类植物重合。
    static void MakePssSquash(std::vector<int> indexes, PlantRectType plant_rect_type, int col) {
        for (int idx : indexes) {
            MakePssSquash(idx, plant_rect_type, col);
        }
    }
};

// 进入关卡后持续生效的类
class Keep {
private:
    static void __MakePssCoffee() {
        for (auto&& plant : AvZ::alive_plant_filter) {
            if (plant.type() == COFFEE_BEAN) {
                Once::MakePssCoffee(int(plant.mRef<short int>(0x148))); // 所以其实这个函数本身应该也是Once属性的……
            }
        }
    }

public:
    // 持续性地将咖啡豆设置为pss状态
    static void MakePssCoffee() {
        __makeskip_tick_runner.pushFunc(__MakePssCoffee);
    }
};

#endif