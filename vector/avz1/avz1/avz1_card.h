/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-10-30 16:26:35
 * @Description: Card api
 */
#ifndef __AVZ1_CARD_H__
#define __AVZ1_CARD_H__

#include "avz1_time_operation.h"

namespace AvZ {

using CardName = ACardName;

// *** Not In Queue
// 选择一堆卡片
// ***注意：卡片名称与英文原版图鉴一致
// 使用此函数概率导致 PvZ 程序崩溃，尚未修复！！！
// *** 使用示例：
// SelectCards({
//     ICE_SHROOM,   // 寒冰菇
//     M_ICE_SHROOM, // 模仿寒冰菇
//     COFFEE_BEAN,  // 咖啡豆
//     DOOM_SHROOM,  // 毁灭菇
//     LILY_PAD,     // 荷叶
//     SQUASH,       // 倭瓜
//     CHERRY_BOMB,  // 樱桃炸弹
//     BLOVER,       // 三叶草
//     PUMPKIN,      // 南瓜头
//     PUFF_SHROOM,  // 小喷菇
// });
inline void SelectCards(const std::vector<int>& lst)
{
    ASelectCards(lst);
}

// *** Not In Queue
// 根据卡片名称得到卡片索引
// *** 注意：卡片索引从 0 开始
// *** 使用示例：
// GetCardIndex(ICE_SHROOM) ---- 得到寒冰菇的卡片索引
inline int GetCardIndex(APlantType plant_type)
{
    return AGetCardIndex(plant_type);
}

// *** Not In Queue
// 用卡函数
// *** 使用示例：
// CardNotInQueue(1, 2, 3)---------选取第1张卡片，放在2行,3列
// CardNotInQueue(1, {{2, 3}, {3, 4}})--------选取第1张卡片，优先放在2行,3列，其次放在3行,4列
inline void CardNotInQueue(int seed_index, int row, float col)
{
    ACard(seed_index, row, col);
}
inline void CardNotInQueue(int seed_index, const std::vector<Position>& lst)
{
    ACard(seed_index, lst);
}

// *** In Queue
// 用卡函数
// *** 注意：Card 将不再支持根据卡槽位置用卡的多张调用形式
// *** 使用示例：
// Card(1, 2, 3)---------选取第1张卡片，放在2行,3列
// Card(1, {{2, 3}, {3, 4}})--------选取第1张卡片，优先放在2行,3列，其次放在3行,4列
// 以下用卡片名称使用 Card,卡片名称为拼音首字母，具体参考图鉴
// Card({{CHERRY_BOMB, 2, 3}, {JALAPENO, 3, 4}})---------选取樱桃卡片，放在2行,3列，选取辣椒卡片，放在3行,4列
inline void Card(const std::vector<CardName>& lst)
{
    InsertOperation([=] {
        ACard(lst);
    });
}
inline void Card(int seed_index, int row, float col)
{
    InsertOperation([=] {
        ACard(seed_index, row, col);
    });
}
inline void Card(int seed_index, const std::vector<Position>& lst)
{
    InsertOperation([=] {
        ACard(seed_index, lst);
    });
}
inline void Card(APlantType plant_type, int row, float col)
{
    InsertOperation([=] {
        ACard(plant_type, row, col);
    });
}
inline void Card(APlantType plant_type, const std::vector<Position>& lst)
{
    InsertOperation([=] {
        ACard(plant_type, lst);
    });
}

} // namespace AvZ
#endif