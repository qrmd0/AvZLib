<!--
 * @Author: qrmd
 * @Date: 2022-07-06 09:56:07
 * @LastEditors: qrmd
 * @LastEditTime: 2023-08-14 11:23:32
 * @Description: 
-->
# AvZ SetZombiesPlus

## 简介

SetZombies的增强版本，更自由地设置出怪。

1、极限出怪不再强制刷出旗帜僵尸和蹦极僵尸；

2、支持在夹零、变速等限定条件下使用游戏原生方式自然出怪。

3、支持逐波设置出怪列表。

### 关键词

自然出怪 游戏内部出怪 游戏原生出怪

## 运行环境

本插件针对 AvZ 2.4.4_2023_06_15 版本开发，支持自 AvZ 220213 到 AvZ 2.4.4_2023_06_15 大部分版本。

## 使用方法

1、在VsCode中运行命令AvZ:Get AvZ Extension，在列表中选择本插件；

2、在{AvZ安装目录}/inc文件夹中找到本插件，浏览README和代码注释了解调用方法；

3、在要使用本插件的脚本开头加上：
```c++
#include "SetZombiesPlus/main.h"
```
## 使用示例

```c++
    // 以自然（游戏原生）方式设置出怪为{普通僵尸、路障僵尸、撑杆僵尸、舞王僵尸、海豚僵尸、小丑僵尸、气球僵尸、矿工僵尸、跳跳僵尸、红眼巨人僵尸}
    ASetZombiesPlus({APJ_0, ALZ_2, ACG_3, AWW_8, AHT_14, AXC_15, AQQ_16, AKG_17, ATT_18, AHY_32}, INTERNAL);
    // 显示为了满足限定条件刷新出怪列表的调试信息
    ASetIsShowInfo(true);
    // 限定第9波不刷出小丑僵尸、全部波次刷出的红眼巨人僵尸合计不少于60只以及变速的波数不小于15
    ASetZombiesLimits({{9, AJACK_IN_THE_BOX, "==", 0}, {TOTAL_WAVE, AGIGA_GARGANTUAR, ">=", 60}, {TRANS_WAVE, AGIGA_GARGANTUAR, ">=", 15}});
    // 设置第18波的出怪为：小鬼僵尸20只、蹦极僵尸14只和巨人僵尸16只
    ASetWaveZombieList(18, {{AIMP, 20}, {ABUNGEE_ZOMBIE, 14}, {AGARGANTUAR, 16}});

```
在AvZ1中调用时，应去掉函数名和僵尸名开头的"A"字母。

## 更新记录


### 20230731

支持在AvZ2中使用。

### 20221127

SetZombiesLimits函数支持全部波次（第1波到第20波）的限定条件。

感谢 @SKOSKX 提供的相关代码。

## 关于出怪机制

### 变速

变速是在生存无尽模式中红眼巨人僵尸在一轮游戏内累计刷出 50 只便不再在非旗帜波刷出的游戏特性。

了解更多出怪机制请访问：https://pvz.tools/wiki/#%E5%87%BA%E6%80%AA%E8%A7%84%E5%BE%8B

## 相关链接

如果您想设置僵尸出现的行数，请访问：
https://github.com/qrmd0/AvZLib/tree/main/crescendo/avz-more

如果您想查看出怪列表，可以使用第三方辅助工具 Plants vs. Zombies Toolkit：
https://pvz.tools/toolkit/