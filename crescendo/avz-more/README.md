# AvZ More 更多AvZ功能 220605

## 运行环境

本插件针对 AvZ 220213 版本开发，不确保对其它 AvZ 版本的兼容性。

## 使用方法
将 avz_more.h 置于 AvZ/inc 目录后，编写脚本并运行即可。

需在Script()外添加:
```c++
#include "avz_more.h"
using namespace cresc;
```

> 各个类与函数的使用方式，可以参见头文件里的注释。

## 内含功能
- 用卡相关：SmartCard, RecoverCard, SafeCard, RecoverSafeCard
- 智能垫材：SmartStall
- 快捷发炮：PaoOperatorAZM(pp, dd, pd, bd)
- 快捷用冰：IceFillerAZM(ice), nice, mice
- 平均分配僵尸行数：AverageSpawn
- 常用修改器功能：IgnoreSun, NoFog
- 杂项功能：CountReadyPao, SetDoomPlace等