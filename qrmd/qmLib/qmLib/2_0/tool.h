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

extern ALogger<AConsole> _qmConsoleLogger;
// 按键调节游戏运行速度
// ------------参数------------
// _1x 设置游戏速度倍率为1.0的快捷键，详见：https://learn.microsoft.com/zh-cn/windows/win32/inputdev/virtual-key-codes
// _2x 设置游戏速度倍率为2.0的快捷键
// _5x 设置游戏速度倍率为5.0的快捷键
// _10x 设置游戏速度倍率为10.0的快捷键
void AKeySetSpeed(const int& _1x = VK_F1, const int& _2x = VK_F2, const int& _5x = VK_F3, const int& _10x = VK_F4)
{
    AConnect(_1x, [] { ASetGameSpeed(1); });
    AConnect(_2x, [] { ASetGameSpeed(2); });
    AConnect(_5x, [] { ASetGameSpeed(5); });
    AConnect(_10x, [] { ASetGameSpeed(10); });
    _qmConsoleLogger.Info("\n游戏变速控制已启用，快捷键为：\n    #：1.0倍速\n    #：2.0倍速\n    #：5.0倍速\n    #：10.0倍速", AGetValue(_keyNames, _1x), AGetValue(_keyNames, _2x), AGetValue(_keyNames, _5x), AGetValue(_keyNames, _10x));
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
// 植物自由种植（不花费阳光+种子零冷却+紫卡直接种植+随意放置+蘑菇免唤醒）
// ------------参数------------
// isEnable 为true时启用，为false时停用
void ASetFreePlant(bool isEnable = true)
{
    // 不花费阳光
    AMRef<uint8_t>(0x41BA72) = isEnable ? 0X70 : 0X7F;
    AMRef<uint8_t>(0x41BA74) = isEnable ? 0X3B : 0X2B;
    AMRef<uint8_t>(0x41BAC0) = isEnable ? 0X91 : 0X9E;
    AMRef<uint8_t>(0x427A92) = isEnable ? 0X80 : 0X8F;
    AMRef<uint8_t>(0x427DFD) = isEnable ? 0X80 : 0X8F;
    AMRef<uint8_t>(0x42487F) = isEnable ? 0XEB : 0X74;
    // 种子零冷却
    AMRef<uint8_t>(0x487296) = isEnable ? 0X70 : 0X7E;
    AMRef<uint8_t>(0x488250) = isEnable ? 0XEB : 0X75;
    // 紫卡直接种植
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

// 解锁所有游戏模式，再次进入游戏主界面生效
void AUnlockAllMode()
{
    AMRef<uint8_t>(0x0044A514) = 0x70;
    WriteArray(0x0042E440, std::vector<uint8_t> {0x31, 0xC0, 0xC3});
    AMRef<uint8_t>(0x0044A5AF) = 0x70;
    AMRef<uint8_t>(0x00454109) = 0x70;
    AMRef<uint8_t>(0x00449E7A) = 0xEB;
    AMRef<uint8_t>(0x00449E9D) = 0xEB;
    AMRef<uint8_t>(0x00453AD1) = 0xEB;
    AMRef<int>(0x00403A10) = 0x5BEB01B0;
    WriteArray(0x00403B30, std::vector<uint8_t> {0xB0, 0x01, 0xC3});
    AMRef<int>(0x0069DCA0) = 0x00;
    WriteArray(0x0048AAD0, std::vector<uint8_t> {0x30, 0xC0, 0xC3});
    AMRef<uint8_t>(0x0048A54C) = 0xEB;
    AMRef<uint8_t>(0x0048D32B) = 0xEB;
    AMRef<uint8_t>(0x0048C491) = 0xEB;
}

// 设置当前已存活轮数，适用于Survival:Endless、模式
// ------------参数------------
// num 已存活轮数，在Survival:Endless模式中为旗帜数的一半
inline void ASetSurvivalRound(int num)
{
    int mode = AGetGameMode();
    if (mode == 60 || mode == 70 || (mode >= 11 && mode <= 15))
        AMPtr<APvzStruct>(0x6A9EC0)->MPtr<APvzStruct>(0x768)->MPtr<APvzStruct>(0x160)->MRef<int>(0x6C) = num;
}

// 设置钱数
// ------------参数------------
// num 单位为1美元，与游戏界面显示的一致
inline void ASetMoney(int num = 999990)
{
    AMPtr<APvzStruct>(0x6A9EC0)->MPtr<APvzStruct>(0x82C)->MRef<int>(0x28) = num / 10;
}

// 锁定黄油或玉米粒，启用时玉米投手只发射黄油或玉米粒
// ------------参数------------
// mode 锁定方式，为1时锁定黄油，为2时锁定玉米粒，为0时还原默认
void ALockKernelPult(int mode = 1)
{
    switch (mode) {
    case 0:
        AMRef<uint8_t>(0x0045f1ec) = 0x75;
        break;
    case 1:
        AMRef<uint8_t>(0x0045f1ec) = 0x70;
        break;
    case 2:
        AMRef<uint8_t>(0x0045f1ec) = 0xEB;
        break;
    default:
        break;
    }
}

// 毁灭菇爆炸不留弹坑
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetNoCrater(bool isEnable = true)
{
    AMRef<uint8_t>(0x41D79E) = isEnable ? 0x70 : 0x75;
}

// 设置植物（樱桃炸弹、寒冰菇、毁灭菇、火爆辣椒）爆炸准备时长
// ------------参数------------
// mode 准备方式，为1时立即爆炸，为2时永不爆炸，为0时还原默认
void ASetPlantExplodeMode(int mode)
{
    switch (mode) {
    case 0:
        AMRef<uint8_t>(0x00463408) = 0x75;
        break;
    case 1:
        AMRef<uint8_t>(0x00463408) = 0x74;
        break;
    case 2:
        AMRef<uint8_t>(0x00463408) = 0xEB;
        break;
    default:
        break;
    }
}

// 设置僵尸（小丑僵尸、辣椒僵尸）爆炸准备时长
// ------------参数------------
// mode 准备方式，为1时立即爆炸，为2时永不爆炸，为0时还原默认
void ASetZombieExplodeMode(int mode)
{
    switch (mode) {
    case 0:
        AMRef<uint8_t>(0x00526afc) = 0x8f;
        AMRef<uint8_t>(0x005275dd) = 0x85;
        break;
    case 1:
        AMRef<uint8_t>(0x00526afc) = 0x80;
        AMRef<uint8_t>(0x005275dd) = 0x80;
        break;
    case 2:
        AMRef<uint8_t>(0x00526afc) = 0x81;
        AMRef<uint8_t>(0x005275dd) = 0x81;
        break;
    default:
        break;
    }
}
// 僵尸停滞不前。启用时僵尸仍做行进动作但横坐标不会变化
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetZombieNotMove(bool isEnable = true)
{
    AMRef<uint8_t>(0x0052ab2b) = isEnable ? 0x54 : 0x64;
    AMRef<uint8_t>(0x0052ab34) = isEnable ? 0x54 : 0x44;
}

// 移除冰道，启用时移除场上的冰道且冰车不会留下冰道
// ------------参数------------
// isEnable 为true时启用，为false时停用
void ASetNoIceTrail(bool isEnable = true)
{
    AMRef<uint8_t>(0x0052a7b0) = isEnable ? 0xc3 : 0x51;
    AMRef<uint8_t>(0x0041f79a) = isEnable ? 0xeb : 0x75;
    if (isEnable)
        for (size_t i = 0; i < 6; ++i)
            AMPtr<APvzStruct>(0x6A9EC0)->MPtr<APvzStruct>(0x768)->MRef<int>(0x624 + i * 4) = 1;
}

// 巨人不扔小鬼，启用时巨人即使血量低于一半也不会投掷小鬼
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void AGargNotThrowImp(bool isEnable = true)
{
    AMRef<uint8_t>(0x00527205) = isEnable ? 0xeb : 0x7d;
}

// 立即刷新下一波僵尸
inline void ASpawningNextWave()
{
    AMPtr<APvzStruct>(0x6A9EC0)->MPtr<APvzStruct>(0x768)->MRef<int>(0x559C) = 1;
}

// 启用内部调试功能
// ------------参数------------
// mode 为1时显示僵尸刷新信息，为2时显示背景音乐信息为3时显示内存信息，为4时显示游戏对象的碰撞判定信息（红色框为攻击域，绿色为防御域），为0时停用
inline void ASetDebugMode(int mode)
{
    AMPtr<APvzStruct>(0x6A9EC0)->MPtr<APvzStruct>(0x768)->MRef<int>(0x55F8) = mode;
}


#endif