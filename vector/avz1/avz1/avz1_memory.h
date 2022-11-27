/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-10-30 16:11:41
 * @Description: memory api
 */
#ifndef __AVZ1_MEMORY_H__
#define __AVZ1_MEMORY_H__

#include "avz1_pvzstruct.h"
#include "avz1_time_operation.h"

namespace AvZ {
// *** Not In Queue
// 游戏主要信息
inline MainObject* GetMainObject()
{
    return (MainObject*)AGetMainObject();
}

// *** Not In Queue
// 游戏基址
inline PvZ* GetPvzBase()
{
    return (PvZ*)AGetPvzBase();
}

// *** Not In Queue
// 返回鼠标所在行
inline int MouseRow()
{
    return AMouseRow();
}

// *** Not In Queue
// 返回鼠标所在列
inline float MouseCol()
{
    return AMouseCol();
}

// *** Not In Queue
// 获取指定类型植物的卡槽对象序列 植物类型与图鉴顺序相同，从0开始
// 返回的卡片对象序列范围：[0,9]
// GetSeedIndex(16)------------获得荷叶的卡槽对象序列
// GetSeedIndex(16, true)-------获得模仿者荷叶的卡槽对象序列
inline int GetSeedIndex(int type, bool imtator = false)
{
    return AGetSeedIndex(type, imtator);
}

// *** Not In Queue
// 得到指定位置和类型的植物对象序列
// 当参数type为默认值-1时该函数无视南瓜花盆荷叶
// *** 使用示例：
// GetPlantIndex(3, 4)------如果三行四列有除南瓜花盆荷叶之外的植物时，返回该植物的对象序列，否则返回-1
// GetPlantIndex(3, 4, 47)---如果三行四列有春哥，返回其对象序列，如果有其他植物，返回-2，否则返回-1
inline int GetPlantIndex(int row, int col, int type = -1)
{
    return AGetPlantIndex(row, col, type);
}

// *** Not In Queue
// 得到一组指定位置的植物的对象序列
// 参数1：填写一组指定位置
// 参数2：填写指定类型
// 参数3：得到对象序列，此函数按照位置的顺序填写对象序列
// *** 注意：如果没有植物填写-1，如果有植物但是不是指定类型，会填写-2
inline void GetPlantIndices(const std::vector<Grid>& lst_in_, int type, std::vector<int>& indexs_out_)
{
    AGetPlantIndices(lst_in_, type, indexs_out_);
}
inline std::vector<int> GetPlantIndices(const std::vector<Grid>& lst, int type)
{
    return AGetPlantIndices(lst, type);
}

// *** Not In Queue
// 检查僵尸是否存在
// *** 使用示例
// IsZombieExist()-------检查场上是否存在僵尸
// IsZombieExist(23)-------检查场上是否存在巨人僵尸
// IsZombieExist(-1,4)-----检查第四行是否有僵尸存在
// IsZombieExist(23,4)-----检查第四行是否有巨人僵尸存在
inline bool IsZombieExist(int type = -1, int row = -1)
{
    return AIsZombieExist(type, row);
}

struct WaveTime {
    int wave;
    int time;
};
// *** Not In Queue
// 设定特定波的波长
// *** 注意： wave 9 19 20 无法设定波长
// 波长的设定范围为 601 - 2510
// *** 使用示例：
// SetWavelength({{1, 601}, {4, 1000}}) ----- 将第一波的波长设置为 601，将第四波的波长设置为 1000
inline void SetWavelength(const std::vector<WaveTime>& lst)
{
    std::vector<ATime> timeVec;
    for (auto&& wave_time : lst) {
        timeVec.push_back(ATime(wave_time.wave, wave_time.time));
    }
    ASetWavelength(std::move(timeVec));
}
// 女仆秘籍
class MaidCheats : public AMaidCheats {
public:
    // *** In Queue
    // 召唤舞伴
    // 舞王不前进且每帧尝试召唤舞伴
    static void callPartner()
    {
        InsertOperation([=]() {
            AMaidCheats::CallPartner();
        });
    }

    // *** In Queue
    // 跳舞
    // 舞王不前进且不会召唤舞伴
    static void dancing()
    {
        InsertOperation([=]() {
            AMaidCheats::Dancing();
        });
    }

    // *** In Queue
    // 保持前进
    // 舞王一直前进
    static void move()
    {
        InsertOperation([=]() {
            AMaidCheats::Move();
        });
    }

    // *** In Queue
    // 停止女仆秘籍
    // 恢复游戏原样
    static void stop()
    {
        InsertOperation([=]() {
            AMaidCheats::Stop();
        });
    }
};

// *** Not In Queue
// 设置出怪 此函数不管填不填蹦极都会在 wave 10 20 刷蹦极！！！！！！！！！！！！
// 参数命名规则：与英文原版图鉴名称一致
// *** 使用示例：
// SetZombies({
//     POLE_VAULTING_ZOMBIE,   // 撑杆
//     BUCKETHEAD_ZOMBIE,      // 铁桶
//     ZOMBONI,                // 冰车
//     JACK_IN_THE_BOX_ZOMBIE, // 小丑
//     BALLOON_ZOMBIE,         // 气球
//     LADDER_ZOMBIE,          // 梯子
//     CATAPULT_ZOMBIE,        // 投篮
//     GARGANTUAR,             // 巨人
//     GIGA_GARGANTUAR,        // 红眼巨人
//     POGO_ZOMBIE,            // 跳跳
// });
// 设置出怪类型为：撑杆 铁桶 冰车 小丑 气球 扶梯 投篮 白眼 红眼 跳跳
//
// SetZombies({
//     BUCKETHEAD_ZOMBIE,
//     ZOMBONI,
//     ZOMBONI,
// });
// 设置出怪类型为：铁桶 冰车 并且两种僵尸的比例为 1：2
inline void SetZombies(const std::vector<int>& zombie_type)
{
    ASetZombies(zombie_type);
}

// *** Not In Queue
// 参数命名规则：与英文原版图鉴名称一致
// *** 使用示例：
// SetWaveZombies(1, {
//     POLE_VAULTING_ZOMBIE,   // 撑杆
//     BUCKETHEAD_ZOMBIE,      // 铁桶
//     ZOMBONI,                // 冰车
//     JACK_IN_THE_BOX_ZOMBIE, // 小丑
//     BALLOON_ZOMBIE,         // 气球
//     LADDER_ZOMBIE,          // 梯子
//     CATAPULT_ZOMBIE,        // 投篮
//     GARGANTUAR,             // 巨人
//     GIGA_GARGANTUAR,        // 红眼巨人
//     POGO_ZOMBIE,            // 跳跳
// });
// 设置第一波出怪类型为：撑杆 铁桶 冰车 小丑 气球 扶梯 投篮 白眼 红眼 跳跳
//
// SetWaveZombies(1, {
//     BUCKETHEAD_ZOMBIE,
//     ZOMBONI,
//     ZOMBONI,
// });
// 设置第一波出怪类型为：铁桶 冰车 并且两种僵尸的比例为 1：2
inline void SetWaveZombies(int wave, const std::vector<int>& zombie_type)
{
    ASetWaveZombies(wave, zombie_type);
}

// *** Not In Queue
// *** 使用示例：
// 检查当前出怪中是否有红眼：
// auto zombie_type_list = GetZombieTypeList();
// bool is_has_hong_yan = zombie_type_list[HY_32]
// if(is_has_hong_yan){
//      // 如果有红眼干啥
// }
inline uint8_t* GetZombieTypeList()
{
    return (uint8_t*)AGetZombieTypeList();
}

// *** In Queue
// 卡片精准生效
// *** 使用示例：
// SetPlantActiveTime(ICE_SHROOM, 298) --------- 修正寒冰菇生效时间点至当前时刻的 298cs 后
inline void SetPlantActiveTime(APlantType plant_type, int delay_time)
{
    InsertOperation([=] {
        ASetPlantActiveTime(plant_type, delay_time);
    });
}

// *** In Queue
// 卡片精准生效
// *** 使用示例：
// SetPlantActiveTime(ICE_SHROOM, 298) --------- 修正寒冰菇生效时间点至当前时刻的 298cs 后
inline void Ice3(int delay_time)
{
    InsertOperation([=] {
        ASetPlantActiveTime(ICE_SHROOM, delay_time);
    });
}

// *** Not In Queue
// 设置游戏倍速
// *** 注意：倍速设置的范围为 [0.05, 10]
// *** 使用示例
// SetGameSpeed(5) ---- 将游戏速度设置为 5 倍速
// SetGameSpeed(0.1) --- 将游戏速度设置为 0.1 倍速
inline void SetGameSpeed(float x)
{
    ASetGameSpeed(x);
}
} // namespace AvZ
#endif