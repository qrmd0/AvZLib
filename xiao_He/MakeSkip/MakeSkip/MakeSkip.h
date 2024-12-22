/*
MIT License

Copyright (c) 2022-2024 xiao_He

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
version 0.1.0
2024.12.14
*/

#ifndef __MAKE_SKIP_H__
#define __MAKE_SKIP_H__

#include <avz.h>
#include <vector>

using namespace AvZ;

// 动画类型
enum AnimType
{
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
enum CopyMode
{
    PP, // 植物 本体动画ID复制给 植物
    PZ, // 植物 本体动画ID复制给 僵尸
    ZP, // 僵尸 本体动画ID复制给 植物
    ZZ, // 僵尸 本体动画ID复制给 僵尸
};

// 获取编号为 index 的对象的类型为 type 的动画ID
inline int &_GetAnimationId(AnimType type, int index)
{
    auto plant_array = GetMainObject()->plantArray();
    auto zombie_array = GetMainObject()->zombieArray();
    switch (type)
    {
    case PLANT_MAIN:
        return plant_array[index].mRef<int>(0x94);
    case ZOMBIE_MAIN:
        return zombie_array[index].mRef<int>(0x118);

    case PLANT_HEAD_1:
        return plant_array[index].mRef<int>(0x98);
    case PLANT_HEAD_2:
        return plant_array[index].mRef<int>(0x9C);
    case PLANT_HEAD_3:
        return plant_array[index].mRef<int>(0xA0);
    case PLANT_BLINK:
        return plant_array[index].mRef<int>(0xA4);
    case PLANT_POTATO_LIGHT:
        return plant_array[index].mRef<int>(0xA8);
    case PLANT_ZZZ:
        return plant_array[index].mRef<int>(0xAC);
    case ZOMBIE_BALL:
        return zombie_array[index].mRef<int>(0x140);
    case ZOMBIE_FLAG:
        return zombie_array[index].mRef<int>(0x144);
    case ZOMBIE_MOWER:
        return zombie_array[index].mRef<int>(0x150);
    }
}

// 销毁编号为 index 的对象的类型为 type 的动画
inline void DeleteAnimation(AnimType type, int index)
{
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

// 将编号为 index_1 的对象的类型为 type_1 的动画编号, 复制给编号为 index_2 的对象的类型为 type_2 的动画编号
inline void CopyAnimationId(AnimType type_1, int index_1, AnimType type_2, int index_2)
{
    DeleteAnimation(type_2, index_2);
    _GetAnimationId(type_2, index_2) = _GetAnimationId(type_1, index_1);
}

// 以 mode 模式将编号为 index_1 的对象的本体动画编号复制给编号为 index_2 的对象
inline void CopyAnimationId(CopyMode mode, int index_1, int index_2)
{
    switch (mode)
    {
    case PP:
    {
        CopyAnimationId(PLANT_MAIN, index_1, PLANT_MAIN, index_2);
        break;
    }
    case PZ:
    {
        CopyAnimationId(PLANT_MAIN, index_1, ZOMBIE_MAIN, index_2);
        break;
    }
    case ZP:
    {
        CopyAnimationId(ZOMBIE_MAIN, index_1, PLANT_MAIN, index_2);
        break;
    }
    case ZZ:
    {
        CopyAnimationId(ZOMBIE_MAIN, index_1, ZOMBIE_MAIN, index_2);
        break;
    }
    }
}

// 以 mode 方式将编号为 index_1 的对象的本体动画编号复制给编号为 indexs 的对象们
inline void CopyAnimationId(CopyMode mode, int index_1, std::vector<int> indexes)
{
    for (int idx : indexes)
    {
        CopyAnimationId(mode, index_1, idx);
    }
}

#endif