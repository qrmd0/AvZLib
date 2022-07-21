# AvZ More 更多AvZ功能 220721

## 运行环境

本插件针对 AvZ 220213/220630 版本开发，不确保对其它 AvZ 版本的兼容性。

## 使用方法
将 avz_more.h 置于 AvZ/inc 目录后，编写脚本并运行即可。

需在 Script() 外添加:
```c++
#include "avz_more.h"
using namespace cresc;
```

> 各个类与函数的使用方式，可以参见头文件里的注释。

## 内含功能
- 用卡相关：SmartCard, RecoverCard, SafeCard, RecoverSafeCard
- 智能垫材：SmartStall
- 快捷用冰：IceFillerAZM(ice), nice, mice
- 平均分配僵尸行数：AverageSpawn
- 常用修改器功能：IgnoreSun, NoFog
- 杂项功能：CountReadyPao, SetDoomPlace等

## 其他插件

本插件可以配合 [!AvZ Smart Cannon 智能发炮](https://github.com/qrmd0/AvZLib/tree/main/crescendo/avz-smart-cannon)插件一起使用。

[![GitHub](https://badgen.net/badge/GitHub/avz_testing/cyan?icon=github)](https://github.com/Rottenham/avz-testing)


https://badgen.net/badge/GitHub/avz_smart_cannon/orange?icon=github
