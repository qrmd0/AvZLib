# AvZ Smart Cannon 智能发炮 220607

## 运行环境

本插件针对 AvZ 220213 版本开发，不确保对其它 AvZ 版本的兼容性。

## 使用方法
> 本插件需要和 [AvZ 精确预测僵尸坐标插件](https://github.com/qrmd0/AvZLib/tree/main/qrmd/GetZombieAbscissas) 一起使用。

当前仓库的inc文件夹下已有 GetZombieAbscissas.h 文件（220608版）。

将 avz_smart_cannon.h 与 GetZombieAbscissas.h 置于 AvZ/inc 目录后，编写脚本并运行即可。

需在 Script() 外添加:
```c++
#include "avz_smart_cannon.h"
using namespace cresc;
```

> 各个类与函数的使用方式，可以参见头文件里的注释。

## 内含功能
```c++
// ***In Queue
// 发射并炸炮，五行场地炸2、4路，六行场地炸2、5路
void pp(float col1 = 9, float col2 = -1)
```

```c++
// ***In Queue
// 发射拦截炮，五行场地炸1、4路，六行场地炸1、5路
void dd(float col1 = 7.4, float col2 = -1)
```

```c++
// ***Not In Queue
// 精准之舞，默认359激活，466拦截
// 可以指定要执行操作的具体半场
void jw(int wave, PaoOperationType type = ALL_FIELD)
```

```c++
// ***Not In Queue
// 执行PDc时机的PD，默认225激活，延迟107拦截
// 可以指定要执行操作的具体半场
void pd(int wave, PaoOperationType type = ALL_FIELD)
```

```c++
// ***Not In Queue
// 执行连拦BD，默认240激活，延迟107拦截
// 可以指定要执行操作的具体半场
void bd(int wave, PaoOperationType type = ALL_FIELD)
```

```c++
// ***In Queue
// （简易函数）精确预测僵尸位置，智能确定炮落点
// ***使用示例：
// SetTime(800 - 373, 2);
// p.smartHitFull(2, 7.5, {GARGANTUAR, LADDER});
// 炮炸2路，默认炸7.5列，根据巨人与扶梯的实际位置推测修改落点
// 要求全收当前波次的巨人与扶梯
void smartHit(int row, float default_col, const std::vector<ZombieType> &zombie_types, bool prefer_left = true)
```

```c++
// ***In Queue
// （完整函数）精确预测僵尸位置，智能确定炮落点
// ***使用示例：
// SetTime(800 - 373, 2);
// p.smartHitFull(2, 7.5, {{GARGANTUAR, {1}}, {LADDER_ZOMBIE, {-1}, {5, 6}}}, false, 300);
// 炮炸2路，默认炸7.5列，根据巨人与扶梯的实际位置推测修改落点
// 要求全收第一波的巨人，并在此基础上全收当前波次（-1代表当前波）的5、6路的扶梯
// 存在多个可行的炮落点时，倾向于使用靠右的落点（false的含义）；僵尸坐标按未来300cs折算
// 因为预测坐标函数无法预知垫材逻辑，若在途中放垫，可用此时机进行修正
// p.smartHitFull(5, 2.5, {{IMP, {}}}, false);
// 炮炸5路，默认2.5列，要求全收任意波次的小鬼，落点尽可能取右
// {IMP, {}}中第二个参数填{}代表任意波次
void smartHitFull(int row, float default_col, const std::vector<SmartHitInfo> zombie_hit_info, bool prefer_left = true, int calc_delay = 373)
```

