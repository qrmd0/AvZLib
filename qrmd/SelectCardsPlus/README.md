<!--
 * @Author: qrmd
 * @Date: 2022-08-05 23:32:40
 * @LastEditors: qrmd
 * @LastEditTime: 2023-08-14 11:21:06
 * @Description: 
-->
# AvZ SelectCardsPlus 极速选卡

## 简介

SelectCards的增强版本，能瞬间完成选卡并自动填充剩余卡槽

![整个选卡过程仅耗时大约0.09s](演示.gif)

### 关键词

选择植物 极速 选择种子

## 运行环境

本插件针对 AvZ 2.4.4_2023_06_15 版本开发，支持大部分 AvZ 版本。

## 使用方法

1、在VsCode中运行命令AvZ:Get AvZ Extension，在列表中选择本插件；

2、在{AvZ安装目录}/inc文件夹中找到本插件，浏览README和代码注释了解调用方法；

3、在要使用本插件的脚本开头加上 

```c++
#include "SelectCardsPlus/main.h"
```

## 使用示例

```c++
// 选择植物卡{咖啡豆、睡莲叶、毁灭菇、模仿毁灭菇、寒冰菇、樱桃炸弹、南瓜头}，若卡槽未被填满，则自动选择种子将其填满
SelectCardsPlus({COFFEE_BEAN, HY_16, HMG_15, M_DOOM_SHROOM, ICE_SHROOM, CHERRY_BOMB, PUMPKIN});

// 自动选择种子将卡槽填满并快速进入战斗
SelectCardsPlus();
```

在AvZ2中调用时，按照AvZ2的函数命名风格在函数名开头和植物种类名开头加字母 A。

## 更新记录

### 2023年8月14日

更新了支持AvZ2的版本，在AvZ2中修复了进入不需要选卡的模式时游戏会意外加速的Bug。