# AvZ PlantFixerPlus

## 简介

PlantFixer的增强版本，添加了更多功能与特性：

1、支持升级植物（紫卡）和咖啡豆修补，修补玉米加农炮时不会选择可能影响其复用的时机；

2、允许不打断手动操作；

3、支持设置修补阈值为血量最大值的比例；

4、同时存在多个待修补的植物血量低于设定值时，优先修补血量最低的植物，血量最低的植物有多个时，优先修补格子位置列表中靠前的植物；

5、允许在待修补的睡莲叶或花盆上有植物时，不对其修补；

6、移除了PlantFixer在以下情况仍会尝试修补的特性：

（1）种植所需阳光不足；

（2）修补位置处存在墓碑或弹坑。

7、移除了isUseCoffee函数。

### 关键词

自动修补 补炮 补曾 补南瓜

## 运行环境

本插件针对 AvZ 2.4.4_2023_06_15 版本开发，支持 AvZ 2022_06_30 以及更高的大部分版本。

## 使用方法


1、在VsCode中运行命令AvZ:Get AvZ Extension，在列表中选择本插件；

2、在{AvZ安装目录}/inc文件夹中找到本插件，浏览README和代码注释了解调用方法；

3、在要使用本插件的脚本开头加上：
```c++
#include "PlantFixerPlus/main.h"
```

## 函数说明
```c++
// 启用自动修补
// ------------参数------------
// [type]为植物种类，填入一个属于[0, 47]的整数或APlantType（模仿植物除外）
// [grids]为待修补的格子位置列表，不填或为空时自动获取场上的[type]植物的格子位置列表
// [fixThreshold]为修补血量阈值，植物血量不高于此值时触发修补操作，小于 1 的值被视为最大血量的比例，不填时为 0.9999
// [isUseImitatorSeed]为是否同时使用模仿种子修补，不填时为 是
// ------------示例------------
// Start(APUMPKIN, {}, 1200, false) // 修补全场的南瓜头，血量阈值为1200，不使用模仿种子
// Start(AGLOOM_SHROOM) // 修补全场的忧郁菇，血量阈值为299，同时使用模仿种子
// Start(ACOB_CANNON, {{1, 1}, {2, 1}}, 0.3)------修补位于{1，1}，{2，1}的玉米加农炮，血量阈值为90，同时使用模仿种子
void Start(const int& type, const std::vector<AGrid>& grids = {}, const float& fixThreshold = 0.9999, const bool& isUseImitatorSeed = true);

// 设置是否在战斗开始时检查是否携带待修补植物的种子，默认为 是
void SetIsCheckCards(bool is_check_cards);

// 设置是否不打断手动操作，默认为 是
void SetIsNotInterrupt(bool is_not_interrupt);

// 设置是否不修补上面有植物的睡莲叶或花盆
void SetIsNotFixLilyPadOrFlowerPot(bool is_not_fix_lily_pad_or_flower_pot_with_plant);

// 设置修补阳光阈值，阳光数量小于此值时不修补，默认为 0
void SetFixSunThreshold(unsigned int fix_sun_threshold);

// 设置执行修补判断的时间间隔（厘秒），默认为 1
void SetRunInterval(unsigned int run_interval);

// 重置植物修补位置
void ResetFixList(std::vector<AGrid> grids);

// 重置修补血量阈值
void ResetFixHpThreshold(unsigned int fix_hp_threshold);
```


## 使用示例

### 控制启用自动修补的时机

自动修补咖啡豆常被用于唤醒自动修补的蘑菇类植物，但运阵往往需要使用咖啡豆，为了不影响运阵的正常使用，您可以指定一些安全的时间段启用自动修补：  

以下代码适用于AvZ1：
```c++
PlantFixerPlus lily_pad_fixer;
PlantFixerPlus pumpkin_fixer;
PlantFixerPlus gloom_shroom_fixer;
PlantFixerPlus coffee_bean_fixer;
TickRunner fixer_manager;

void ManageFixer(PlantFixerPlus& fixer)
{
    struct TimeRange {
        int wave;
        int time_lower_bound;
        int time_upper_bound;
    };
    // 启用自动修补的时间段
    const std::vector<TimeRange> enable_time_ranges = {{2, -200, 740}, {6, -200, 740}, {9, 300, 4500}, {11, -200, 740}, {15, -200, 740}, {19, -200, 4500}, {20, 300, 5500}};

    bool is_enable_time;
    int now_wave;

    is_enable_time = false;
    for (TimeRange each : enable_time_ranges) {
        now_wave = GetRunningWave();
        if (now_wave == each.wave && NowTime(now_wave) >= each.time_lower_bound && NowTime(now_wave) <= each.time_upper_bound) {
            is_enable_time = true;
            break;
        }
    }

    if (is_enable_time) {
        if (fixer.getStatus() != RUNNING) {
            {
                InsertGuard ig(false);
                fixer.goOn();
            }
        }
    } else {
        if (fixer.getStatus() == RUNNING) {
            {
                InsertGuard ig(false);
                fixer.pause();
            }
        }
    }
}
void Script()
{
    SetTime(-599, 1);
    lily_pad_fixer.start(LILY_PAD, {{3, 7}, {3, 8}, {3, 9}, {4, 7}, {4, 8}, {4, 9}});
    gloom_shroom_fixer.start(GLOOM_SHROOM, {{2, 6}, {5, 6}, {3, 7}, {3, 8}, {3, 9}, {4, 7}, {4, 8}, {4, 9}});
    pumpkin_fixer.start(PUMPKIN, {{3, 7}, {3, 8}, {3, 9}, {4, 7}, {4, 8}, {4, 9}}, 0.3);
    coffee_bean_fixer.start(COFFEE_BEAN, {{2, 6}, {5, 6}, {3, 7}, {3, 8}, {3, 9}, {4, 7}, {4, 8}, {4, 9}});
    InsertOperation([=]() {
        fixer_manager.pushFunc([]() {
            ManageFixer(coffee_bean_fixer);
        });
    });
}
```

由于模仿者的血量较低且变身需要一定时间，使用模仿种子修补坚果类植物时模仿植物可能会在变身完成前就被僵尸啃食掉：

![演示](演示.gif)

为了避免上述情况，您可以定时判断场上僵尸的状态，在有啃食威胁的时机停用自动修补：

以下代码适用于AvZ1：

```c++
PlantFixerPlus lily_pad_fixer;
PlantFixerPlus dolphinrider_pumpkin_fixer;
TickRunner fixer_manager;

void ManageFixer(PlantFixerPlus& fixer)
{
    bool is_enable_time;

    is_enable_time = true;
    for (auto&& zombie : alive_zombie_filter) {
        if (zombie->type() == DOLPHIN_RIDER_ZOMBIE && int(zombie->abscissa()) <= 720) {
            is_enable_time = false;
            break;
        }
    }

    if (is_enable_time) {
        if (fixer.getStatus() != RUNNING) {
            {
                InsertGuard ig(false);
                fixer.goOn();
            }
        }
    } else {
        if (fixer.getStatus() == RUNNING) {
            {
                InsertGuard ig(false);
                fixer.pause();
            }
        }
    }
}
void Script()
{
    SetTime(-599, 1);
    lily_pad_fixer.start(LILY_PAD, {{3, 7}, {4, 7}}, 0);
    dolphinrider_pumpkin_fixer.start(PUMPKIN, {{3, 7}, {4, 7}}, 0.3);
    InsertOperation([=]() {
        fixer_manager.pushFunc([]() {
            ManageFixer(dolphinrider_pumpkin_fixer);
        });
    });
}
```

在AvZ2中，可以使用条件AConnet更简洁地实现以上效果。

## 更新记录

### 2023年8月14日

支持AvZ2。