# AvZ More 更多AvZ功能 220822

## 运行环境

本插件针对 AvZ 220213/220630/220817(Nightly) 版本开发，不确保对其它 AvZ 版本的兼容性。

## 使用方法
将 xxx.h 置于 AvZ/inc 目录后，编写脚本并运行即可。

需在 Script() 外添加:
```c++
#include "avz_more.h"
using namespace cresc;
```

> 各个类与函数的使用方式，可以参见头文件里的注释。

## 内含功能

### avz_more.h

```c++
/** ===自定义类部分===**/

// 自定义僵尸子类
struct ZombieAZM

// 获得动画对象
Animation *GetAnimation(SafePtr<ZombieAZM> z)

// 获得动画对象
Animation *GetAnimation(SafePtr<Plant> p)

// 自定义场物类
struct Place

// 自定义MainObject子类
struct MainObjectAZM

// 获得MainObjectAZM对象
MainObjectAZM *MyMainObject()

// 完成关卡数
int GetCurrentLevel()

// 当前阳光数
int GetSun()

// 自定义IceFiller子类
// 用于白昼简化点冰
class IceFillerAZM 

// ***Not In Queue
// 便捷原版冰放置
void nice(int row, float col, int time = 601, int wave = -1)

// ***Not In Queue
// 便捷复制冰放置
void mice(int row, float col, int time = 601, int wave = -1)


/** ===判定部分===**/

// 获得row行col列无偏移植物的内存坐标
std::pair<int, int> GetPlantCoord(int row, int col)

// 获得某特定类型植物的防御域
// 使用现行标准，内存坐标 → 90,0)基准的调试模式https://wiki.crescb.com/timespace/ts/
std::pair<int, int> GetDefenseRange(PlantType type)

// 获得某特定类型植物对小丑爆炸的防御域
std::pair<int, int> GetExplodeDefenseRange(PlantType type)

// 判断两个区间是否重叠
bool IntervalIntersectInterval(int x1, int w1, int x2, int w2)

// 判断某僵尸攻击域和某植物防御域是否重叠
bool JudgeHit(SafePtr<Zombie> zombie, int plant_row, int plant_col, PlantType plant_type = PEASHOOTER)

// 判断小丑爆炸范围和某植物防御域是否重叠
bool JudgeExplode(SafePtr<Zombie> zombie, int plant_row, int plant_col, PlantType plant_type = PEASHOOTER)


/** ===读僵尸内存部分===**/

// ***Not In Queue
// 检测僵尸是否存在
// type: 类型；row: 行；left_lim: 坐标左限；right_lim: 坐标右限；wave: 来自波次；hp_min: 最低血量
bool CheckZombie(int type = -1, int row = -1, int left_lim = -1,
                 int right_lim = -1, int wave = -1, int hp_min = -1)

// ***Not In Queue
// 数僵尸数量
int CountZombie(int type = -1, int row = -1, int left_lim = -1,
                int right_lim = -1, const std::set<int> &states = {}, int from_wave = -1)


/** ===卡片部分===**/

// 某卡片是否可用
bool IsUsable(PlantType type)

// 某格子的核坑倒计时
int DoomCD(int row, int col)

// 某行的冰道倒计时
int IceTrailCD(int row)

// 某格子的冰道倒计时
int IceTrailCD(int row, int col)

// 某各自的核坑倒计时和冰道倒计时的较大值
int GridCD(int row, int col)

// 返回某卡能否成功用在某处
// 主要用于防止Card函数一帧内调用多次Card导致不明原因崩溃
bool Plantable(PlantType type, int row, int col)

// 返回某卡的冷却倒计时
int CardCD(PlantType type)

// 如果放置成功或下一帧尝试，返回true；否则返回false
bool CardNIQ(PlantType plant_type, int row, int col, bool force = false)
void CardNIQ(PlantType plant_type, const std::vector<Grid> &lst, bool force = false)

// ***In Queue
// 根据僵尸是否存在判断用卡
void SmartCard(PlantType plant_type, int row, int col,
               const std::set<int> &_zombie_type,
               const std::set<int> &_check_rows = {},
               const std::pair<int, int> &_limits = {-1, -1},
               const std::set<int> &_states = {})

// ***In Queue
// 等卡片冷却完毕后使用，与PaoOperator::recoverPao类似
void RecoverCard(PlantType plant_type, int row, int col, bool force = true)
void RecoverCard(PlantType plant_type, const std::vector<Grid> &lst, bool force = true)

// ***In Queue
// 等待上下三行内小丑炸完后用卡（默认卡片需要100cs生效）
void SafeCard(PlantType plant_type, int row, int col, bool force = true, int need_time = 100)
void SafeCard(PlantType plant_type, const std::vector<Grid> &lst, bool force = true, int need_time = 100)
void SafeCard(const std::vector<PlantType> &lst, int row, int col, bool force = true, int need_time = 100)

// ***In Queue
// RecoverCard和SafeCard的结合
void RecoverSafeCard(PlantType plant_type, int row, float col, bool force = true, int need_time = 100)
void RecoverSafeCard(PlantType plant_type, const std::vector<Grid> &lst, bool force = true, int need_time = 100)

// ***In Queue
// 智能垫垫材
// 函数默认若垫材卡中含有花盆，则优先级最高（刚种植100cs内免疫啃食），其余垫材卡按参数传入的顺序
// 每条路的威胁程度计算方法为梯子数与小丑数相加
void SmartStall(int time, int wave, const std::vector<int> &r = {1, 2, 5, 6},
                const std::vector<PlantType> &t = {FLOWER_POT, SCAREDY_SHROOM,
                                                   PUFF_SHROOM, SUN_SHROOM})


/** ===其它功能===**/

// ***Not In Queue
// 平均分配指定种类僵尸所在行数
// 默认只平均分配红眼
void AverageSpawn(const std::vector<ZombieType> &lst = {GIGA_GARGANTUAR},
                  bool onlyHugeWave = false, int start_row = 0)

// ***Not In Queue
// 设置弹坑CD
// 注意：该位置必须先有一个弹坑
void SetDoomPlace(int row, int col, int cd)

// *** Not In Queue
// 得到指定位置和类型的植物对象序列
// 当参数type为默认值-1时该函数无视南瓜、花盆、荷叶、咖啡豆
// 注意：玉米炮两格都算
int CheckPlant(int row, int col, int type = -1,
               const std::set<int> &ignore_types = {16, 30, 33, 35})

// 某两波（包含）之间所有波的波长总和
// 如果输入不符合预期，或有波次尚未刷出，返回-1
int WaveLength(int start_wave, int end_wave)

// 返回某波的波长
int WaveLength(int wave)

// 是否出现以下出怪之一
bool TypeOr(const std::vector<int> &zombie_types)

// 是否包含以下所有出怪
bool TypeAnd(const std::vector<int> &zombie_types)

// 包含以下出怪的个数
int TypeCount(const std::vector<int> &zombie_types)

// 检查当前时间是否超过某个时间点
// -1：未知；0：未超过；1：超过
int Past(TimeWave time_wave)
```

### avz_autoplay.h

```c++
// ***Not In Queue
// 计算可供使用的炮数
int CountReadyPao(int time = 0)

// 返回真实倒计时（若为w9/w19，考虑大波倒计时接管的额外部分）
int RealCountdown()

// 某格子植物受锤最短用时（不受锤返回0）
int GridHitCD(int row, int col, PlantType plant_type = PEASHOOTER)

// 某格子植物受炸最短用时（不被炸返回0）
int GridJackCD(int row, int col, PlantType plant_type = PEASHOOTER)

// 描述一组僵尸的描述信息
struct ZombieInfo

// 获得能攻击到某格植物的僵尸的描述信息
ZombieInfo GridZomInfo(int row, int col, int zombie_type = GIGA_GARGANTUAR, PlantType plant_type = PEASHOOTER, bool ignore_iced, bool ignore_slow, int min_hp)

// 最大血量
int GridZomHP(int row, int col, int zombie_type = GIGA_GARGANTUAR, PlantType plant_type = PEASHOOTER, bool ignore_iced = false, bool ignore_slow = false, int min_hp = 0)

// 格子锁
class GridLocker
```

### avz_autoplay.h

> 存放一些调试用函数，请自行翻阅

## 其他插件

本插件可以配合 [AvZ Smart Cannon 智能发炮](https://github.com/qrmd0/AvZLib/tree/main/crescendo/avz-smart-cannon) 插件一起使用。

[![GitHub](
https://badgen.net/badge/GitHub/avz_smart_cannon/orange?icon=github)](https://github.com/qrmd0/AvZLib/tree/main/crescendo/avz-smart-cannon)


