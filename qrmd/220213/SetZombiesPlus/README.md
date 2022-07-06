# AvZ SetZombiesPlus 20220706

## 简介

SetZombies的增强版本，支持更多的出怪方式。

1、极限出怪不再强制刷出旗帜僵尸和蹦极僵尸；

2、支持使用游戏原生方式自然出怪。

### 关键词

自然出怪 游戏内部出怪 游戏原生出怪 模拟出怪

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
// 以极限（均匀摊派）方式设置出怪为 {普通僵尸、读报僵尸、小丑僵尸、冰车僵尸、扶梯僵尸、跳跳僵尸、舞王僵尸、潜水僵尸、海豚僵尸、铁门僵尸}
    SetZombies({0, DB_5, XC_15, ZOMBONI, FT_21, POGO_ZOMBIE, WW_8, 11, 14, SCREEN_DOOR_ZOMBIE}, AVERAGE);
    
    // 以自然（游戏原生）方式设置出怪为 {普通僵尸、路障僵尸、撑杆僵尸、舞王僵尸、海豚僵尸、小丑僵尸、气球僵尸、矿工僵尸、跳跳僵尸、巨人僵尸、红眼巨人僵尸}
    SetZombies({ZOMBIE, LZ_2, POLE_VAULTING_ZOMBIE, 8, HT_14, JACK_IN_THE_BOX_ZOMBIE, QQ_16, 17, TT_18, BY_23, 32}, INTERNAL);
```

## 更新计划

加入模拟方式出怪，满足夹零、变速等测试需求。
