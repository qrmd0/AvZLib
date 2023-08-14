<!--
 * @Author: qrmd
 * @Date: 2023-07-31 19:06:25
 * @LastEditors: qrmd
 * @LastEditTime: 2023-08-14 11:22:47
 * @Description: 
-->
# AvZ AutoFodder

## 简介

自动垫僵尸，在指定格子处检测到指定僵尸时放置指定植物拖延其推进，用于运阵时垫舞王、垫巨人，拖收尾，抗压等。

### 关键词

垫僵尸 智能垫材

## 运行环境

本插件针对 AvZ 2.4.4_2023_06_15 版本开发，支持大部分 AvZ2 版本。

## 使用方法

1、在VsCode中运行命令AvZ:Get AvZ Extension，在列表中选择本插件；

2、在{AvZ安装目录}/inc文件夹中找到本插件，浏览README和代码注释了解调用方法；

3、在要使用本插件的脚本开头加上：
```c++
#include "AutoFodder/main.h"
```
## 函数说明

```c++
// 自动垫僵尸，在指定格子处检测到指定僵尸时放置指定植物拖延其推进，用于运阵时垫舞王、垫巨人，拖收尾，抗压等
// ------------参数------------
// grids 要放垫的格子列表，如果传空向量，则取所有陆地行的最靠右的不在垫材列表中的非一次性植物的右边的格子
// zombies 要阻挡的僵尸列表，如果传空向量，则取橄榄、小丑、梯子、白眼、红眼，不得包含：冰车、气球、矿工、投篮、蹦极，因为垫材通常对它们无效
// plants 作为垫材的种子列表，如果传空向量，则取所有冷却时间最快的种子及其模仿者版本，阳光花费少者优先
// removalDelay 放置垫材后多久将其铲除，如果为-1则不铲除。常用时间：减速巨人锤击生效时间266cs，防止跳跳起跳时间79cs。长期连接时建议不铲除，否则可能会意外铲除原垫材消失后在此格子放置的新植物
// bufferPx 横坐标缓冲量，允许僵尸在指定格子处推进多远再放垫，例如巨人僵尸推进到横坐标不大于440时会对5列植物举锤，如果bufferPx填入30，巨人僵尸推进到横坐标不大于410时才放垫。允许为负值
// isSynchronizeGargantuar 是否同步巨人动作，即当此处有巨人处于举锤状态时不放垫
// isSmartFlowerpot 是否将花盆放在啃食威胁最大的位置，花盆被种下后短时间内免疫啃食伤害，因此需要同时放置多个垫材时，花盆适合放在最容易被啃穿的位置
// ------------示例------------
// // NE舞夜惊魂(Pc)垫舞王
// for (auto wave = 1; wave <= 20; ++wave)
//     AConnect(ATime(wave, 195), [] { AAutoFodder({{3, 9}, {4, 9}}, {ADANCING_ZOMBIE}, {}, 4); });
// // PE纯二十炮(P5u)垫跳跳和舞王
// for (auto wave : {4, 6, 9, 12, 14, 17, 19}) {
//     AConnect(ATime(wave, 170), [] { AAutoFodder({{1, 9}}, {APOGO_ZOMBIE}, {}, 79); });
//     AConnect(ATime(wave, 195), [] { AAutoFodder({{1, 9}, {2, 9}, {5, 9}, {6, 9}}, {ADANCING_ZOMBIE}, {}, 79); });
// }
// // DE双冰六炮(ch5)垫红眼拖收尾
// for (auto wave : {9, 19, 20}) {
//     AConnect([=] { return ANowWave() == wave && ANowTime(wave) >= 1700; }, [] { AAutoFodder(); });
//     // 场上无红眼时铲垫
//     const std::vector<AGrid> fodderGrids = {{1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7}};
//     for (AGrid each : fodderGrids) {
//         AConnect([=] { return AGetPlantIndex(each.row, each.col) >= 0 && !AIsZombieExist(AGIGA_GARGANTUAR); }, [=] { AShovel(each.row, each.col); });
//     }
// }
// PE电波钟无炮抗压
// AConnect([] { return true; }, [] { AAutoFodder({{1, 5}, {1, 6}, {6, 5}, {6, 6}, {5, 7}, {2, 7}}, {}, {}, -1, 25); });
void AAutoFodder(std::vector<AGrid> grids = {}, std::vector<AZombieType> zombies = {}, std::vector<APlantType> plants = {}, int removalDelay = -1, int bufferPx = 0, bool isSynchronizeGargantuar = true, bool isSmartFlowerpot = true);
```
