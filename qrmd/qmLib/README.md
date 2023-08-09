<!--
 * @Author: qrmd
 * @Date: 2023-07-31 19:06:25
 * @LastEditors: qrmd
 * @LastEditTime: 2023-08-09 21:41:26
 * @Description: 
-->
# AvZ qmLib

## 简介

全麦的常用AvZ函数，包含了多个用于测试和战斗的函数，并提供了一个用于快速生成测试用脚本的代码模板。

### 关键词

函数库 PTK 

## 运行环境

本插件针对 AvZ 2.4.4_2023_06_15 版本开发，支持大部分 AvZ2 版本。

## 使用方法

1、在VsCode中运行命令AvZ:Get AvZ Extension，在列表中选择本插件；

2、在{AvZ安装目录}/inc文件夹中找到本插件，浏览README和代码注释了解调用方法；

3、在要使用本插件的脚本开头加上 #include "qmLib/main.h"

## 更新记录

### 2023_08_09

新增信息条绘制、智能跳帧等新函数。

## 函数说明
```c++
// --------------------------------全局--------------------------------

// 返回能否在向量A中找到b
inline bool AGetIsHave(const std::vector<T>& A, T& b);

// 取被b包含的a中的元素组成的向量，顺序与a相同，包含重复项
// ------------参数------------
// a 任意支持"=="运算符的类型或其组成的向量
// b 同a
// ------------示例------------
// std::vector<int> v1 = {1, 2, 2, 3};
// std::vector<int> v2 = {2, 2, 2, 1, 4};
// std::vector<int> v3 = AGetIntersection(v1, v2);
// // v3的值为{1, 2, 2}
std::vector<T> AGetIntersection(const std::vector<T>& a, const std::vector<T>& b);
std::vector<T> AGetIntersection(const T& a, const std::vector<T>& b);
std::vector<T> AGetIntersection(const std::vector<T>& a, const T& b);

// 合并两个向量，返回合并后的向量
std::vector<T> AMerge(const std::vector<T>& a, const std::vector<T>& b);
std::vector<T> AMerge(const T& a, const std::vector<T>& b);
std::vector<T> AMerge(const std::vector<T>& a, const T& b);

// 移除向量中的重复元素
void ARemoveDuplicates(std::vector<T>& vec);

// 移除向量中的指定元素
void ARemoveElement(std::vector<T>& vec, T element);

// 将向量中的指定元素移动至指定位置，如果失败，返回false
bool AMoveElement(std::vector<T>& vec, T element, int toIndex);

// 根据键名返回一个映射的键值，如果键名不存在抛出异常
ValueType AGetValue(const std::map<KeyType, ValueType>& myMap, const KeyType& key);

// 返回数字的字符串
// ------------参数------------
// t 要转换的数字
// decimalDigits 四舍五入保留的小数位数，默认0位
// integerDigits 要保留的整数位数，不足则前导补0
// ------------示例------------
// AGetString(35.475, 2, 4); // 返回 0035.48
std::string AGetString(const T& num, int decimalDigits, int integerDigits);

// 返回指定格子的状态是否为允许种植指定种子的状态
// ------------参数------------
// plt 种子类型
// row 要检查是否允许种植的格子行数
// col 要检查是否允许种植的格子列数
inline bool AGetIsPlantAllowable(APlantType plt, int row, int col);


// 当前窗口为PvZ且指定键均被按下时，返回true。
// ------------参数------------
// keys 要检测的按键的虚拟键码，详见：https://learn.microsoft.com/zh-cn/windows/win32/inputdev/virtual-key-codes
bool AGetIsKeysDown(const std::vector<int>& keys);
// --------------------------------------------------------------------

// --------------------------------工具--------------------------------

// 移除场上所有的植物
void ARemoveAllPlants();

// 移除场上所有的僵尸
void ARemoveAllZombies();

// 移除场上所有的场地物品（钉耙除外）
// 警告：实现方式为将其移动至255行，待优化
void ARemoveAllPlaceItems();

// 清空场地
void AClearGround();

// 在指定位置生成一株植物，返回其内存对象
// ------------参数------------
// type 要生成的植物种类
// row 生成的格子行数
// col 生成的格子列数
// ------------示例------------
// // 在2行1列处生成向日葵
// APutPlant(ASUNFLOWER, 2, 1);
// // 在1行9列和5行8列处生成模仿小喷菇
// APutPlant(AM_PUFF_SHROOM, {{1, 9}, {5, 8}});
APlant* APutPlant(APlantType type, int row, int col);
APlant* APutPlant(APlantType type, AGrid grid);
std::vector<APlant*> APutPlant(APlantType type, std::vector<AGrid> grids);

// 在指定位置生成一只僵尸，返回其内存对象
// ------------参数------------
// type 要生成的僵尸种类
// row 生成的格子行数
// col 生成的格子列数
// ------------示例------------
// // 在2行8列处生成铁桶僵尸
// APutZombie(ABUCKETHEAD_ZOMBIE, 2, 8);
// // 在1行9列和2行8列处生成蹦极僵尸
// APutZombie(ABUNGEE_ZOMBIE, {{1, 9}, {2, 8}});
AZombie* APutZombie(AZombieType type, int row, int col);
AZombie* APutZombie(AZombieType type, AGrid grid);
std::vector<AZombie*> APutZombie(AZombieType type, std::vector<AGrid> grids);

// 按键调节游戏运行速度
// ------------参数------------
// _1x 设置游戏速度倍率为1.0的快捷键，详见：https://learn.microsoft.com/zh-cn/windows/win32/inputdev/virtual-key-codes
// _2x 设置游戏速度倍率为2.0的快捷键
// _5x 设置游戏速度倍率为5.0的快捷键
// _10x 设置游戏速度倍率为10.0的快捷键
void AKeySetSpeed(const int& _1x = VK_F1, const int& _2x = VK_F2, const int& _5x = VK_F3, const int& _10x = VK_F4);

// 移除浓雾，启用时雾夜模式不显示浓雾
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetNoFog(bool isEnable = true);

// 存档只读，启用时游戏进度不会被更改
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetSaveFileNoChange(bool isEnable = true);

// 设置阳光数
// ------------参数------------
// num 阳光数，不得大于9990或小于0
inline void ASetSun(int num = 8000);

// 僵尸死亡时不掉落战利品（钱、礼物盒、巧克力）
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetZombieNoFalling(bool isEnable = true);

// 暂停出怪，启用时僵尸不会刷新
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetZombieNoSpawn(bool isEnable = true);

// 植物无敌，启用时植物受到攻击时不会损失血量
// ------------参数------------
// isEnable 为true时启用，为false时停用
void ASetPlantInvincible(bool isEnable = true);

// 植物自由种植（无视阳光+种子零冷却+紫卡直接种植+随意放置+蘑菇不睡觉）
// ------------参数------------
// isEnable 为true时启用，为false时停用
void ASetFreePlant(bool isEnable = true);

// 移除轰炸陆地的炮弹的引信延迟机制
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetNoFireDelay(bool isEnable = true);

// 僵尸进家后不食脑
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetZombieNoEnterHome(bool isEnable = true);

// 立刻装填，启用时玉米炮发射后很快可以再次发射
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetCobCannonNoCd(bool isEnable = true);

// 选卡时植物卡直接进入卡槽
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetCardImmediatelyFlyToSlot(bool isEnable = true);

// 不发生旗帜波事件，即启用时泳池不出珊瑚、黑夜不冒墓碑、屋顶不落空降
// ------------参数------------
// isEnable 为true时启用，为false时停用
inline void ASetNoFlagWaveEvent(bool isEnable = true);

// --------------------------------------------------------------------

// --------------------------------内存--------------------------------

// 写内存数组
// ------------参数------------
// dstAddr 内存地址
// srcArr 要写入的向量
// ------------示例------------
// WriteArray(0x52B308, std::vector<uint8_t> {0x90, 0x90});
void WriteArray(DstAddr dstAddr, SrcArray&& srcArr);

// --------------------------------------------------------------------


// --------------------------------战斗--------------------------------

// 计算指定波次僵尸在指定行的血量之和，用于智能激活或抗压
// ------------参数------------
// wave 波次
// row 行数
int GetZombiesHp(int wave, int row);

// 不断开炮直到前场不存在本波僵尸，用于收尾
// ------------参数------------
// wave 要收尾的波次
// time 第一次开炮的本波时刻
// delay 开炮间隔时间
// ------------示例------------
// // PE经典八炮的收尾操作
// for (auto wave : {9, 19, 20}) {
//     PPEndingTheWave(wave, 869 - 200 + 869 - 373, 869);
// }
void PPEndingTheWave(int wave, int time, int delay);

// 自动种植三叶草吹走漏掉的气球僵尸
// ------------参数------------
// row 种植三叶草的格子行数
// col 种植三叶草的格子列数
// threshold 存在横坐标小于多少的气球僵尸时种植三叶草
void AAutoBlover(int row, int col, float threshold = 250);

// 在控制台中显示本轮出怪逐波统计表
void AShowZombiesList();

// 智能跳帧，按键启停，检测到指定位置的指定植物不血量全满时自动停止，用于挂机冲关
// ------------参数------------
// startKey 启动键的虚拟按键码，详见：https://learn.microsoft.com/zh-cn/windows/win32/inputdev/virtual-key-codes
// stopKey 停止键的虚拟按键码
// grids 要检测的格子列表，玉米加农炮为后轮格子
// types 要检测的植物种类，默认为{玉米加农炮、忧郁蘑菇、冰瓜、双子向日葵}
void ASmartSkipTick(const int& startKey, const int& stopKey, const std::vector<AGrid>& grids, const std::vector<APlantType>& types = {ACOB_CANNON, AGLOOM_SHROOM, AWINTER_MELON, ATWIN_SUNFLOWER});

// --------------------------------------------------------------------

// --------------------------------绘制--------------------------------

class APainterEx{
public:
    // 绘制按指定比例填充的信息条，ABar的参数为：
    // ------------参数------------
    // x 信息条中心的横坐标
    // y 信息条中心的纵坐标
    // rate 信息条的填充比例
    // sizeX 信息条内容的水平长度
    // sizeY 信息条内容的垂直长度
    // fillARGB 填充颜色与透明度
    // backgroundARGB 背景颜色和透明度
    // frameThickness 边框粗细
    // direction 信息条的填充方向
    // separators 在指定比例处绘制与填充方向垂直的分节线
    void Draw(const ABar& bar, int duration = 1)
}

// 在游戏窗口底部显示僵尸刷新信息，格式为：
// 波次：[当前波次]/[总波次] 时间：[本波时刻]|[刷新倒计时]/[刷新倒计时初始值]
// 血量：[当前本波总血量]/[本波总血量初始值]([本波当前血量占总血量百分比]/[本波刷新血量占总血量百分比])
// ------------参数------------
// isEnable 为true时启用，为false时停用
void AShowRefreshingInfo(bool isEnable = true);

class AStateShower{
public:
    // 在指定植物附近显示其属性信息条，其中属性为：
    // 所有植物：血量；
    // 射手类、投手类：发射倒计时进度；
    // 土豆地雷：准备就绪倒计时进度；
    // 向日葵、阳光菇、金盏花、双子向日葵：生产倒计时进度；
    // 樱桃炸弹、寒冰菇、毁灭菇和火爆辣椒：生效倒计时进度；
    // 三叶草：消失倒计时进度；
    // 大嘴花、磁力菇、玉米加农炮：复用倒计时进度；
    // ------------参数------------
    // pred 对象过滤器，以一个APlant*为参数的，返回bool值的lambda表达式，该断言将被用于场上的每个植物，结果为true的显示信息条。默认总为true，即始终显示所有植物的信息条
    // ------------示例------------
    // // 显示不满血的后轮在7列或8列的玉米加农炮的属性信息条
    // ShowPlants([](APlant* plt) { return plt->Type() == ACOB_CANNON && plt->Col() + 1 >= 7 && plt->Hp() < plt->HpMax(); });
    void ShowPlants(std::function<bool(APlant*)> pred = [](APlant* plt) { return true; });
    // 在指定僵尸附近显示其属性信息条，其中属性为：
    // 所有僵尸：血量（不计入持有的二类饰品）；
    // 撑杆僵尸、海豚僵尸：跳过植物动画进度；
    // 读报僵尸：失去报纸动画进度；
    // 潜水僵尸、海豚僵尸：跳入泳池动画进度；
    // 小丑僵尸：开盒倒计时进度；
    // 气球僵尸：气球破裂落地动画进度；
    // 矿工僵尸：自然出土后眩晕动画进度；
    // 蹦极僵尸：准备抓取植物动画进度；
    // 投篮僵尸：投篮动画进度；
    // 巨人僵尸：锤击动画进度、投掷小鬼动画进度；
    // 可被冰冻僵尸：冰冻倒计时进度；
    // ------------参数------------
    // pred 对象过滤器，以一个AZombie*为参数的，返回bool值的lambda表达式，该断言将被用于场上的每只僵尸，结果为true的显示信息条。默认总为true，即始终显示所有僵尸的信息条
    // ------------示例------------
    // // 显示6列以左的未被减速的巨人僵尸的属性信息条
    // ShowZombies([](AZombie* zmb) { return (zmb->Type() == AGARGANTUAR || zmb->Type() == AGIGA_GARGANTUAR) && zmb->Abscissa() <= 410 && zmb->SlowCountdown() == 0; });
    void ShowZombies(std::function<bool(AZombie*)> pred = [](AZombie* zmb) { return true; });
    // 在指定场地物品附近显示其属性信息条，其中属性为：
    // 弹坑：消失倒计时
    // ------------参数------------
    // pred 对象过滤器，以一个APlaceItem*为参数的，返回bool值的lambda表达式，该断言将被用于场上的每个场地物品，结果为true的显示信息条。默认总为true，即始终显示所有场地物品的信息条
    // ------------示例------------
    // 显示位于3行的8列和9列的弹坑的消失倒计时信息条
    // ShowPlaceItems([](APlaceItem* plcItm) { return plcItm->Type() == 2 && plcItm->Row() + 1 == 3 && plcItm->Col() + 1 >= 8; });
    void ShowPlaceItems(std::function<bool(APlaceItem*)> pred = [](APlaceItem* plcItm) { return true; });
    // 显示所有植物、僵尸和场地物品的属性信息条
    void ShowAll();
    // 不再显示植物的属性信息条
    void StopPlants();
    // 不再显示僵尸的属性信息条
    void StopZombies();
    // 不再显示场地物品的属性信息条
    void StopPlaceItems();
    // 不再显示植物、僵尸和场地物品的属性信息条
    void StopAll();
}

```

## 代码模板说明

将AvZTemplate.code-snippets置于{AvZ安装目录}/.vscode文件夹中，在AvZ安装目录下新建cpp文件时，键入AvZTemplate时智能感知将列出测试脚本模板。