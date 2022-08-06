# AvZ SelectCardsPlus 220805

## 简介

SelectCards的增强版本，能瞬间完成选卡并自动填充剩余卡槽

![演示](演示.gif)

### 关键词

选择植物 极速

## 运行环境

本插件针对 AvZ 220213 版本开发，不确保对其它 AvZ 版本的兼容性。

## 使用方法

将 SelectCardsPlus.h 置于 AsmVsZombies/inc 目录后，编写脚本并运行即可。

在 Script() 外添加下面的代码以在编译时包含此库:

```c++
#include "SelectCardsPlus.h"
```

## 使用示例

```c++
    // 选择植物卡{咖啡豆、睡莲叶、毁灭菇、模仿毁灭菇、寒冰菇、樱桃炸弹、南瓜头}，若卡槽未被填满，则自动选择植物将其填满
    SelectCardsPlus({COFFEE_BEAN, HY_16, HMG_15, M_DOOM_SHROOM, 14, 2, PUMPKIN});
```
