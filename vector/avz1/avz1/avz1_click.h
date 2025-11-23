/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-10-30 16:22:00
 * @Description: click api
 */
#ifndef __AVZ1_CLICK_H__
#define __AVZ1_CLICK_H__

#include "avz1_time_operation.h"

namespace AvZ {

// 将格子转换成坐标
inline void GridToCoordinate(int row, float col, int& x, int& y)
{
    AGridToCoordinate(row, col, x, y);
}
// *** Not In Queue
// 点击格子
// *** 使用示例：
// ClickGrid(3, 4)---- 点击格子(3, 4)
// ClickGrid(3, 4, 10)---- 向下偏移10像素点击格子(3, 4)
inline void ClickGrid(int row, float col, int offset = 0)
{
    AClickGrid(row, col, offset);
}

// *** Not In Queue
// 点击种子/卡片
// *** 使用示例：
// ClickSeed(1) ----- 点击第一个种子
inline void ClickSeed(int seed_index)
{
    AClickSeed(seed_index);
}

// *** Not In Queue
// 右键安全点击
inline void SafeClick()
{
    AAsm::ReleaseMouse();
}

// *** Not In Queue
// 鼠标左击
// LeftClick(400, 300)-----点击 PVZ 窗口中央
inline void LeftClick(int x, int y)
{
    ALeftClick(x, y);
}

inline void ShovelNotInQueue(int row, float col, bool pumpkin = false)
{
    AShovel(row, col, pumpkin);
}

// *** In Queue
// 铲除植物函数
inline void Shovel(int row, float col, bool pumpkin = false)
{
    InsertOperation([=]() {
        AShovel(row, col, pumpkin);
    });
}

// *** In Queue
// *** 使用示例：
// Shovel(4, 6)--------铲除4行6列的植物,如果植物有南瓜保护默认铲除被保护植物
// Shovel(4, 6, true)---铲除4行6列的植物,如果植物有南瓜保护铲除南瓜
// Shovel({{3, 6},{4, 6}})------铲除3行6列，4行6列的植物
inline void Shovel(const std::vector<AShovelPosition>& lst)
{
    InsertOperation([=]() {
        for (auto&& each : lst) {
            AShovel(each.row, each.col, each.targetType);
        }
    });
}

} // namespace AvZ
#endif