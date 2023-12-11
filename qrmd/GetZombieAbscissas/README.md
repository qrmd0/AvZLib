<!--
 * @Author: qrmd
 * @Date: 2022-06-09 00:03:58
 * @LastEditors: qrmd
 * @LastEditTime: 2023-08-14 01:42:10
 * @Description: 
-->
# AvZ GetZombieAbscissas 僵尸坐标预测

## 简介

预测僵尸在理想状态下运动时的坐标。

### 关键词

僵尸位移 僵尸运动 预测 精确计算

## 运行环境

本插件针对 AvZ 2.4.4_2023_06_15 版本开发，支持大部分 AvZ 版本。

## 使用方法

1、在VsCode中运行命令AvZ:Get AvZ Extension，在列表中选择本插件；

2、在{AvZ安装目录}/inc文件夹中找到本插件，浏览README和代码注释了解调用方法；

3、在要使用本插件的脚本开头加上：
```c++
#include "GetZombieAbscissas/main.h"
```
## 函数说明

```c++
// 返回[index]号僵尸保持当前运动模型（未再次受到减速、冰冻、定身等限制移动效果、进入啃食状态、鸭子僵尸入水、撑杆僵尸遇到植物跳跃等）移动时，未来[timeLength]厘秒内的横坐标向量，如果该僵尸是无法预测的类型或处于无法预测的状态，则返回空向量
// 注意，本函数可能需要读取僵尸的以下内存数据：
// 类型，
// 相对速度，
// 动画循环率，
// 动画起始帧，
// 横坐标，
// 冰冻状态倒计时，
// 减速状态倒计时。
// 示例：输出10号僵尸未来300厘秒内每厘秒的横坐标
// ALogger<AConsole> consoleLogger;
// std::vector<float> abscissas = {};
// abscissas = AGetZombieAbscissas(10, 300);
// if (!abscissas.empty()) {
//     for (int time = 0; time < abscissas.size(); ++time) {
//          consoleLogger.Info("#号僵尸在#厘秒后的横坐标为：#", 10, time, abscissas[time]);
//     }
// } else {
//     consoleLogger.Info("#号僵尸是无法预测的类型或处于无法预测的状态", 10);
// }
std::vector<float> GetZombieAbscissas(int index, int timeLength);
```

在AvZ2中调用时，按照AvZ2的函数命名风格在开头加字母 A。

<font color=red>注意：如果被测僵尸是无法预测的类型或处于无法预测的状态，则返回空向量，请检查返回向量是否为空，访问空向量会导致下标溢出错误而使游戏崩溃</font>

## 原理

根据对游戏代码的逆向分析，精确还原僵尸的移动过程，详见：https://http//www.bilibili.com/video/BV1vA411G75o1。

## 更新记录

### 2023年8月14日

支持AvZ2。

### 2023年12月11日

1、修复了预测某些不支持的僵尸导致游戏程序崩溃的bug；
2、新增了对读报僵尸的支持。

## 更新预告

1、水路僵尸的预测。由于水路领带僵尸的入水机制尚不明确，因此无法模拟水路领带僵尸入水前到入水后的移动过程；

2、优化算法。提供新的接口，无需每帧计算，直接算出非匀速移动的僵尸的给定厘秒后的坐标或移动到给定坐标所需的时间；

3、纵坐标的预测。跳跳等僵尸在移动过程中纵坐标也会发生变化，而灰烬的判定也与纵坐标有关。

4、模糊预测，对于僵尸属于无法预测的类型或僵尸处于无法预测的状态时，给出模糊结果。

## 性能分析

【待完善】