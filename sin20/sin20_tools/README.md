# AvZ sin20_tools 20230215

## 运行环境

本插件针对 AvZ 221001 版本开发，不确保对其它 AvZ 版本的兼容性。

## 使用方法

先安装插件 avz-more ，然后将 sin20_tools.h 置于 AsmVsZombies/inc 目录，编写脚本并运行。

在 Script() 外添加下面的代码以在编译时包含此库:

```c++
#include "sin20_tools.h"
using namespace sin20;
```

## 内含功能

```c++
// *** Not In Queue
// 若干组按键绑定，包括女仆、高级暂停、倍速等
void A_TAS();

// 检查指定格是否有墓碑
bool CheckGrave(int row, int col);

// 检查指定格是否有弹坑
bool CheckCrater(int row, int col);

// *** In Queue
// 调整雾浓度，可选择全屏或具体格子
class FogAdjuster : public TickRunner {
public:
    void start(int concentration = 102);
    void start(const std::vector<FogAdjustSet> &lst);
};
FogAdjuster fog_adjuster;

// *** Not In Queue
// 人为控制旗帜波蹦极僵尸们偷的格子包含[row]行[col]列
// 可以选择在第10波、第20波、第(10和20)波控制蹦极
void BungeeManipulate(int row, int col, std::vector<int> mpwaves = {10});
void BungeeManipulate(const std::vector<Grid> &lst, std::vector<int> mpwaves = {10});

// *** In Queue
// 模拟神炮，向场外指定落点强制发炮
void PaoToOutside(int pao_row, int pao_col, int row, int x);

// *** In Queue
// 修改指定行的出怪属性（不出怪、按陆路出怪、按水路出怪）
void SetRowSpawnType(const std::vector<int> &lst, RowType type = RowType::NONE);

// *** Not In Queue
// 返回炸[row]路的炮弹全收第[wave]波[pogo_row]路跳跳所需的爆心x范围
std::pair<int, int> GetPaoxForPogo(int row, int pogo_row, bool isShowInfo = true, int wave = -1, int cob_col = 1);

// *** In Queue
// 设置音乐
void SetMusic(int musicId = 12);
    
// *** Not In Queue
// 僵尸计数器，返回各行来自第[wave]波的[types]僵尸的数量
std::vector<int> GetZombieCount(int wave = -1, std::initializer_list<int> types = {HY_32});

// *** Not In Queue
// 返回来自第[wave]波的[types]僵尸最多的行
int GetMostZombiesRow(int wave = -1, std::initializer_list<int> types = {FT_21, XC_15});

// *** Not In Queue
// 返回来自第[wave]波的[types]僵尸最多的半场
int GetMoreZombiesField(int wave = -1, std::initializer_list<int> types = {HY_32});
    
// *** Not In Queue
// 智能骗锤，检测是否有巨人即将锤击外层南瓜、花盆上的高坚果或偏右蘑菇，如有则铲除
void SmartShovelPlant(bool autoShovel = true);

// *** Not In Queue
// 屋顶场合根据冰车投篮的存在情况和速度情况智能放[row]行9列灰烬，保证其生效时间为[time]
void SafeCol9Bomb(PlantType type, int row, int time, int wave = -1);
```