<!--
 * @Author: qrmd
 * @Date: 2023-07-31 19:06:25
 * @LastEditors: qrmd
 * @LastEditTime: 2023-08-04 00:14:27
 * @Description: 
-->
# AvZ qmLib

## 简介

全麦的常用AvZ函数，包含了多个用于测试和战斗的函数，并提供了一个用于快速生成测试用脚本的代码模板。

### 关键词

函数库 PTK 

## 运行环境

本插件针对 AvZ 2.4.4_2023_06_15 版本开发，支持大部分 AvZ2 版本。

## 使用方法

1、在VsCode中运行命令AvZ:Get AvZ Extension，在列表中选择本插件；

2、在{AvZ安装目录}/inc文件夹中找到本插件，浏览README和代码注释了解调用方法；

3、在要使用本插件的脚本开头加上 #include "qmLib/main.h"

## 函数说明
```c++
// --------------全局--------------

// 取被b包含的a中的元素组成的向量，顺序与a相同，包含重复项
// ------------参数------------
// a 任意支持"=="运算符的类型或其组成的向量
// b 同a
// ------------示例------------
// std::vector<int> v1 = {1, 2, 2, 3};
// std::vector<int> v2 = {2, 2, 2, 1, 4};
// std::vector<int> v3 = AGetIntersection(v1, v2);
// // v3的值为{1, 2, 2}
std::vector<T> AGetIntersection(const std::vector<T>& a, const std::vector<T>& b);
std::vector<T> AGetIntersection(const T& a, const std::vector<T>& b);
std::vector<T> AGetIntersection(const std::vector<T>& a, const T& b);

// 合并两个向量，返回合并后的向量
std::vector<T> AMerge(const std::vector<T>& a, const std::vector<T>& b);
std::vector<T> AMerge(const T& a, const std::vector<T>& b);
std::vector<T> AMerge(const std::vector<T>& a, const T& b);

// 移除向量中的重复元素
void ARemoveDuplicates(std::vector<T>& vec);

// 移除向量中的指定元素
void ARemoveElement(std::vector<T>& vec, T element);

// 将向量中的指定元素移动至指定位置，如果失败，返回false
bool AMoveElement(std::vector<T>& vec, T element, int toIndex);

// --------------------------------

// --------------工具--------------

// 移除场上所有的植物
void ARemoveAllPlants();

// 移除场上所有的僵尸
void ARemoveAllZombies();

// 移除场上所有的场地物品（钉耙除外）
// 警告：实现方式为将其移动至255行，待优化
void ARemoveAllPlaceItems();

// 清空场地
void AClearGround();

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
APlant* APutPlant(APlantType type, int row, int col);
APlant* APutPlant(APlantType type, AGrid grid);
std::vector<APlant*> APutPlant(APlantType type, std::vector<AGrid> grids);

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
AZombie* APutZombie(AZombieType type, int row, int col);
AZombie* APutZombie(AZombieType type, AGrid grid);
std::vector<AZombie*> APutZombie(AZombieType type, std::vector<AGrid> grids);

// 按键调节游戏运行速度。
// F1：原速；F2：2倍速；F3：5倍速；F4：10倍速
void AKeySetSpeed();

// 移除浓雾，启用时雾夜模式不显示浓雾
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetNoFog(bool isEnable = true);

// 存档只读，启用时游戏进度不会被更改
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetSaveFileNoChange(bool isEnable = true);

// 设置阳光数
// ------------参数------------
// num 阳光数，不得大于9990或小于0
inline void ASetSun(int num = 8000);

// 僵尸死亡时不掉落战利品（钱、礼物盒、巧克力）
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetZombieNoFalling(bool isEnable = true);

// 暂停出怪，启用时僵尸不会刷新
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetZombieNoSpawn(bool isEnable = true);

// 植物无敌，启用时植物受到攻击时不会损失血量
// ------------参数------------
// isEnable 为true时启用，为false时停用
void ASetPlantInvincible(bool isEnable = true);

// 植物自由种植（无视阳光+种子零冷却+紫卡直接种植+随意放置+蘑菇不睡觉）
// ------------参数------------
// isEnable 为true时启用，为false时停用
void ASetFreePlant(bool isEnable = true);

// 移除轰炸陆地的炮弹的引信延迟机制
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetNoFireDelay(bool isEnable = true);

// 僵尸进家后不食脑
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetZombieNoEnterHome(bool isEnable = true);

// 立刻装填，启用时玉米炮发射后很快可以再次发射
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetCobCannonNoCd(bool isEnable = true);

// 选卡时植物卡直接进入卡槽
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetCardImmediatelyFlyToSlot(bool isEnable = true);

// 不发生旗帜波事件，即启用时泳池不出珊瑚、黑夜不冒墓碑、屋顶不落空降
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetNoFlagWaveEvent(bool isEnable = true);

// --------------内存--------------

// 写内存数组
// ------------参数------------
// dstAddr 内存地址
// srcArr 要写入的向量
// ------------示例------------
// WriteArray(0x52B308, std::vector<uint8_t> {0x90, 0x90});
void WriteArray(DstAddr dstAddr, SrcArray&& srcArr);

// --------------------------------

// --------------战斗--------------

// 计算指定波次僵尸在指定行的血量之和，用于智能激活或抗压
// ------------参数------------
// wave 波次
int GetZombiesHp(int wave, int row);

// 不断开炮直到前场不存在本波僵尸，用于收尾
// ------------参数------------
// wave 要收尾的波次
// time 第一次开炮的本波时刻
// delay 开炮间隔时间
// ------------示例------------
// // PE经典八炮的收尾操作
// for (auto wave : {9, 19, 20}) {
//     PPEndingTheWave(wave, 869 - 200 + 869 - 373, 869);
// }
void PPEndingTheWave(int wave, int time, int delay);

// --------------------------------
```

## 代码模板说明

将AvZTemplate.code-snippets置于{AvZ安装目录}/.vscode文件夹中，在AvZ安装目录下新建cpp文件时，键入AvZTemplate时智能感知将列出测试脚本模板。