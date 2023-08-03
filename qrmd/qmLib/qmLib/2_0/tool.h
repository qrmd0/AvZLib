#ifndef __TOOL__
#define __TOOL__

#include "avz.h"

// 移除场上所有的植物
void ARemoveAllPlants()
{
    for (int row = 1; row <= 6; ++row) {
        for (int col = 1; col <= 9; ++col) {
            for (int i = 0; i < 4; ++i) {
                ARemovePlant(row, col);
            }
        }
    }
}

// 移除场上所有的僵尸
void ARemoveAllZombies()
{
    AAliveFilter<AZombie> zombieFilter;
    for (auto&& zombie : zombieFilter) {
        AAsm::RemoveZombie(&zombie);
    }
}

// 移除场上所有的场地物品（钉耙除外）
// 警告：实现方式为将其移动至255行，待优化
void ARemoveAllPlaceItems()
{

    AAliveFilter<APlaceItem> aAlivePlaceItemFilter;
    for (auto&& placeItem : aAlivePlaceItemFilter) {
        placeItem.Row() = 255;
    }
}

// 清空场地
void AClearGround()
{
    ARemoveAllPlants();
    ARemoveAllZombies();
    ARemoveAllPlaceItems();
}

// 在指定位置生成一株植物，返回其内存对象
// ------------参数------------
// type 要生成的植物种类
// row 生成的格子行数
// col 生成的格子列数
// ------------示例------------
// // 在2行1列处生成向日葵
// APutPlant(ASUNFLOWER, 2, 1);
// // 在1行9列和5行8列处生成模仿小喷菇
// APutPlant(AM_PUFF_SHROOM, {{1, 9}, {5, 8}});
APlant* APutPlant(APlantType type, int row, int col)
{
    return AAsm::PutPlant(row - 1, col - 1, type);
}
APlant* APutPlant(APlantType type, AGrid grid)
{
    return APutPlant(type, grid.row, grid.col);
}
std::vector<APlant*> APutPlant(APlantType type, std::vector<AGrid> grids)
{
    std::vector<APlant*> result = {};
    for (auto each : grids)
        result.emplace_back(APutPlant(type, each));
    return result;
}

// 在指定位置生成一只僵尸，返回其内存对象
// ------------参数------------
// type 要生成的僵尸种类
// row 生成的格子行数
// col 生成的格子列数
// ------------示例------------
// // 在2行8列处生成铁桶僵尸
// APutZombie(ABUCKETHEAD_ZOMBIE, 2, 8);
// // 在1行9列和2行8列处生成蹦极僵尸
// APutZombie(ABUNGEE_ZOMBIE, {{1, 9}, {2, 8}});
AZombie* APutZombie(AZombieType type, int row, int col)
{
    return AAsm::PutZombie(row - 1, col - 1, type);
}
AZombie* APutZombie(AZombieType type, AGrid grid)
{
    return APutZombie(type, grid.row, grid.col);
}
std::vector<AZombie*> APutZombie(AZombieType type, std::vector<AGrid> grids)
{
    std::vector<AZombie*> result = {};
    for (auto each : grids)
        result.emplace_back(APutZombie(type, each));
    return result;
}

// 按键调节游戏运行速度。
// F1：原速；F2：2倍速；F3：5倍速；F4：10倍速
void AKeySetSpeed()
{
    AConnect(VK_F1, [] {
        ASetGameSpeed(1);
    });
    AConnect(VK_F2, [] {
        ASetGameSpeed(2);
    });
    AConnect(VK_F3, [] {
        ASetGameSpeed(5);
    });
    AConnect(VK_F4, [] {
        ASetGameSpeed(10);
    });
}

// 移除浓雾，启用时雾夜模式不显示浓雾
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetNoFog(bool isEnable = true)
{
    AMRef<uint16_t>(0x41A68D) = isEnable ? 0xD231 : 0xF23B;
}

// 存档只读，启用时游戏进度不会被更改
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetSaveFileNoChange(bool isEnable = true)
{
    AMRef<uint8_t>(0x482149) = isEnable ? 0x2E : 0x13;
    AMRef<uint8_t>(0x54B267) = isEnable ? 0x70 : 0x74;
}

// 设置阳光数
// ------------参数------------
// num 阳光数，不得大于9990或小于0
inline void ASetSun(int num = 8000)
{
    if (num > 9990 || num < 0)
        return;
    AMPtr<APvzStruct>(0x6A9EC0)->MPtr<APvzStruct>(0x768)->MRef<int>(0x5560) = num;
}

// 僵尸死亡时不掉落战利品（钱、礼物盒、巧克力）
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetZombieNoFalling(bool isEnable = true)
{
    AMRef<uint8_t>(0x530276) = isEnable ? 0x66 : 0x5B;
}

// 暂停出怪，启用时僵尸不会刷新
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetZombieNoSpawn(bool isEnable = true)
{
    AMRef<uint8_t>(0X4265DC) = isEnable ? 0XEB : 0X74;
}

// 植物无敌，启用时植物受到攻击时不会损失血量
// ------------参数------------
// isEnable 为true时启用，为false时停用
void ASetPlantInvincible(bool isEnable = true)
{
    if (isEnable) {
        WriteArray(0x52FCF1, std::vector<uint8_t> {0x46, 0x40, 0x00});
        AMRef<uint8_t>(0X41CC2F) = 0xEB;
        AMRef<uint8_t>(0X5276EA) = 0xEB;
        WriteArray(0X46CFEB, std::vector<uint8_t> {0x90, 0x90, 0x90});
        WriteArray(0X46D7A6, std::vector<uint8_t> {0x90, 0x90, 0x90});
        AMRef<uint8_t>(0X52E93B) = 0xEB;
        AMRef<uint8_t>(0X45EE0A) = 0x70;
        AMRef<uint8_t>(0X45EC66) = 0x00;
        WriteArray(0X462B80, std::vector<uint8_t> {0xC2, 0x04, 0x00});
    } else {
        WriteArray(0x52FCF1, std::vector<uint8_t> {0x46, 0x40, 0xFC});
        AMRef<uint8_t>(0X41CC2F) = 0x74;
        AMRef<uint8_t>(0X5276EA) = 0x75;
        WriteArray(0X46CFEB, std::vector<uint8_t> {0x29, 0x50, 0x40});
        WriteArray(0X46D7A6, std::vector<uint8_t> {0x29, 0x4E, 0x40});
        AMRef<uint8_t>(0X52E93B) = 0x74;
        AMRef<uint8_t>(0X45EE0A) = 0x75;
        AMRef<uint8_t>(0X45EC66) = 0XCE;
        WriteArray(0X462B80, std::vector<uint8_t> {0X53, 0X55, 0X8B});
    }
}
// 植物自由种植（无视阳光+种子零冷却+紫卡直接种植+随意放置+蘑菇免唤醒）
// ------------参数------------
// isEnable 为true时启用，为false时停用
void ASetFreePlant(bool isEnable = true)
{
    // 无视阳光
    AMRef<uint8_t>(0x41BA72) = isEnable ? 0X70 : 0X7F;
    AMRef<uint8_t>(0x41BA74) = isEnable ? 0X3B : 0X2B;
    AMRef<uint8_t>(0x41BAC0) = isEnable ? 0X91 : 0X9E;
    AMRef<uint8_t>(0x427A92) = isEnable ? 0X80 : 0X8F;
    AMRef<uint8_t>(0x427DFD) = isEnable ? 0X80 : 0X8F;
    AMRef<uint8_t>(0x42487F) = isEnable ? 0XEB : 0X74;
    // 取消冷却
    AMRef<uint8_t>(0x487296) = isEnable ? 0X70 : 0X7E;
    AMRef<uint8_t>(0x488250) = isEnable ? 0XEB : 0X75;
    // 紫卡直种
    AMRef<uint8_t>(0x40E477) = isEnable ? 0XEB : 0X74;
    // 随意放置
    AMRef<uint8_t>(0x40FE30) = isEnable ? 0X81 : 0X84;
    AMRef<uint8_t>(0x438E40) = isEnable ? 0XEB : 0X74;
    AMRef<uint8_t>(0x42A2D9) = isEnable ? 0X8D : 0X84;
    // 蘑菇免唤醒
    AMRef<uint8_t>(0X45DE8E) = isEnable ? 0XEB : 0X74;
    // 紫卡直接种植
    if (isEnable)
        WriteArray(0X41D7D0, std::vector<uint8_t> {0XB0, 0X01, 0XC3});
    else
        WriteArray(0X41D7D0, std::vector<uint8_t> {0X51, 0X83, 0XF8});
}

// 移除轰炸陆地的炮弹的引信延迟机制
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetNoFireDelay(bool isEnable = true)
{
    AMRef<uint16_t>(0x46D672) = isEnable ? 0x9700 : 0x96D0;
}

// 僵尸进家后不食脑
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetZombieNoEnterHome(bool isEnable = true)
{
    if (isEnable)
        WriteArray(0x52B308, std::vector<uint8_t> {0x90, 0x90});
    else
        WriteArray(0x52B308, std::vector<uint8_t> {0x74, 0x07});
    return;
}

// 立刻装填，启用时玉米炮发射后很快可以再次发射
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetCobCannonNoCd(bool isEnable = true)
{
    AMRef<uint8_t>(0x46103B) = isEnable ? 0x80 : 0x85;
}

// 选卡时植物卡直接进入卡槽
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetCardImmediatelyFlyToSlot(bool isEnable = true)
{
    AMRef<uint8_t>(0x48606A) = isEnable ? 0 : 17;
}

// 不发生旗帜波事件，即启用时泳池不出珊瑚、黑夜不冒墓碑、屋顶不落空降
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetNoFlagWaveEvent(bool isEnable = true)
{
    AMRef<uint8_t>(0x42694A) = isEnable ? 0xEB : 0x75;
    AMRef<uint8_t>(0x413083) = isEnable ? 0xEB : 0x75;
}

#endif