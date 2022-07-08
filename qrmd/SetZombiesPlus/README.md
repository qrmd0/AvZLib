# AvZ SetZombiesPlus 20220708

## 简介

SetZombies的增强版本，更自由地设置出怪。

1、极限出怪不再强制刷出旗帜僵尸和蹦极僵尸；

2、支持在夹零、变速等限定条件下使用游戏原生方式自然出怪。

3、支持逐波设置出怪列表。

### 关键词

自然出怪 游戏内部出怪 游戏原生出怪

## 运行环境

本插件针对 AvZ 220213 版本开发，不确保对其它 AvZ 版本的兼容性。

## 使用方法

将 SetZombiesPlus.h 置于 AsmVsZombies/inc 目录后，编写脚本并运行即可。

在 Script() 外添加下面的代码以在编译时包含此库:

```c++
#include "SetZombiesPlus.h"
```

## 使用示例

```c++
    // 以自然（游戏原生）方式设置出怪为{普通僵尸、路障僵尸、撑杆僵尸、舞王僵尸、海豚僵尸、小丑僵尸、气球僵尸、矿工僵尸、跳跳僵尸、红眼巨人僵尸}
    SetZombiesPlus({PJ_0, LZ_2, CG_3, WW_8, HT_14, XC_15, QQ_16, KG_17, TT_18, HY_32}, INTERNAL);
    // 显示为了满足限定条件刷新出怪列表的调试信息
    SetIsShowInfo(true);
    // 限定第9波不刷出红眼巨人僵尸、第10波刷出至少10只普通僵尸以及变速的波数不小于15
    SetZombiesLimits({{9, GIGA_GARGANTUAR, "==", 0}, {10, ZOMBIE, ">=", 10}, {TRANS_WAVE, GIGA_GARGANTUAR, ">=", 15}});
    // 设置第18波的出怪为：小鬼僵尸20只、蹦极僵尸14只和巨人僵尸16只
    SetWaveZombieList(18, {{IMP, 20}, {BUNGEE_ZOMBIE, 14}, {GARGANTUAR, 16}});
```

## 关于出怪机制

### 变速

变速是在生存无尽模式中红眼巨人僵尸在一轮游戏内累计刷出 50 只便不再在非旗帜波刷出的游戏特性。

了解更多出怪机制请访问：https://pvz.tools/wiki/#%E5%87%BA%E6%80%AA%E8%A7%84%E5%BE%8B

## 相关链接

如果您想设置僵尸出现的行数，请访问：
https://github.com/qrmd0/AvZLib/tree/main/crescendo/avz-more

如果您想查看出怪列表，可以使用第三方辅助工具 Plants vs. Zombies Toolkit：
https://pvz.tools/toolkit/