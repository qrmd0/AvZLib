/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-10-30 15:19:50
 * @Description: include global Func
 */

#ifndef __AVZ1_GLOBAL_H__
#define __AVZ1_GLOBAL_H__

#include <avz.h>
#include <functional>

#include "avz1_pvzstruct.h"

#define STR_GAME_RET_MAIN_UI "game return main ui"
#define FindInAllRange(container, goal) std::find(container.begin(), container.end(), goal)

#ifdef __MINGW32__
#define Likely(x) __builtin_expect(!!(x), 1)
#define Unlikely(x) __builtin_expect(!!(x), 0)
#else
#define Likely(x) (x)
#define Unlikely(x) (x)
#endif

#define __A1Deprecated [[deprecated("This identifier is not compatible")]]

namespace AvZ {

class GlobalVar : public AStateHook {
public:
    // 此函数会在 AvZ 基本内存信息初始化完成后且调用 void Script() 之前运行
    void virtual beforeScript() { }

    // 此函数会在 AvZ 调用 void Script() 之后运行
    void virtual afterScript() { }

    // 此函数会在游戏进入战斗界面后立即运行
    void virtual enterFight() { }

    // 此函数会在游戏退出战斗界面后立即运行
    // 特别注意: 如果用户从主界面进入选卡界面但是又立即退回主界面，此函数依然会运行
    void virtual exitFight() { }

protected:
    void virtual _BeforeScript() override { beforeScript(); }
    void virtual _AfterScript() override { afterScript(); }
    void virtual _EnterFight() override { enterFight(); }
    void virtual _ExitFight() override { exitFight(); }
};

template <class ReturnType>
using VoidFunc = std::function<ReturnType()>;

constexpr int __DEFAULT_START_TIME = -0xffff;

using Grid = AGrid;
using Position = APosition;

struct TimeWave {
    int time;
    int wave;
};

// *** 函数功能：判断数字范围
// *** 使用示例：
// RangeIn(wave, {1,2,3})------如果波数在 1 2 3 范围里返回 true
inline bool RangeIn(int num, std::initializer_list<int> lst)
{
    return ARangeIn(num, lst);
}

// 寻找 vector 中相同的元素，返回其迭代器
template <typename Ele>
auto FindSameEle(const std::vector<Ele>& container, const Ele& ele_) -> std::vector<decltype(container.begin())>
{
    std::vector<decltype(container.begin())> result;
    for (auto it = container.begin(); it != container.end(); ++it) {
        if ((*it) == ele_) {
            result.push_back(it);
        }
    }

    return result;
}

// 寻找 vector 中相同的元素，返回其迭代器
template <typename Ele>
auto FindSameEle(std::vector<Ele>& container, const Ele& ele_) -> std::vector<decltype(container.begin())>
{
    std::vector<decltype(container.begin())> result;
    for (auto it = container.begin(); it != container.end(); ++it) {
        if ((*it) == ele_) {
            result.push_back(it);
        }
    }

    return result;
}

// 读取内存函数
template <typename T, typename... Args>
T ReadMemory(Args... args)
{
    std::initializer_list<uintptr_t> lst = {static_cast<uintptr_t>(args)...};
    uintptr_t buff = 0;
    T result = T();

    for (auto it = lst.begin(); it != lst.end(); ++it) {
        if (it != lst.end() - 1)
            ReadProcessMemory(INVALID_HANDLE_VALUE, (const void*)(buff + *it), &buff, sizeof(buff), nullptr);
        else
            ReadProcessMemory(INVALID_HANDLE_VALUE, (const void*)(buff + *it), &result, sizeof(result), nullptr);
    }
    return result;
}

// 改写内存函数
template <typename T, typename... Args>
void WriteMemory(T value, Args... args)
{
    std::initializer_list<uintptr_t> lst = {static_cast<uintptr_t>(args)...};
    uintptr_t buff = 0;
    for (auto it = lst.begin(); it != lst.end(); it++)
        if (it != lst.end() - 1)
            ReadProcessMemory(INVALID_HANDLE_VALUE, (const void*)(buff + *it), &buff, sizeof(buff), nullptr);
        else
            WriteProcessMemory(INVALID_HANDLE_VALUE, (void*)(buff + *it), &value, sizeof(value), nullptr);
}

template <typename T>
void LimitValue(T& value, T min_v, T max_v)
{
    if (value < min_v) {
        value = min_v;
    }
    if (value > max_v) {
        value = max_v;
    }
}

} // namespace AvZ

using PlantType = APlantType;
using ZombieType = AZombieType;

constexpr APlantType PEASHOOTER = APEASHOOTER;         // 豌豆射手
constexpr APlantType SUNFLOWER = ASUNFLOWER;           // 向日葵
constexpr APlantType CHERRY_BOMB = ACHERRY_BOMB;       // 樱桃炸弹
constexpr APlantType WALL_NUT = AWALL_NUT;             // 坚果
constexpr APlantType POTATO_MINE = APOTATO_MINE;       // 土豆地雷
constexpr APlantType SNOW_PEA = ASNOW_PEA;             // 寒冰射手
constexpr APlantType CHOMPER = ACHOMPER;               // 大嘴花
constexpr APlantType REPEATER = AREPEATER;             // 双重射手
constexpr APlantType PUFF_SHROOM = APUFF_SHROOM;       // 小喷菇
constexpr APlantType SUN_SHROOM = ASUN_SHROOM;         // 阳光菇
constexpr APlantType FUME_SHROOM = AFUME_SHROOM;       // 大喷菇
constexpr APlantType GRAVE_BUSTER = AGRAVE_BUSTER;     // 墓碑吞噬者
constexpr APlantType HYPNO_SHROOM = AHYPNO_SHROOM;     // 魅惑菇
constexpr APlantType SCAREDY_SHROOM = ASCAREDY_SHROOM; // 胆小菇
constexpr APlantType ICE_SHROOM = AICE_SHROOM;         // 寒冰菇
constexpr APlantType DOOM_SHROOM = ADOOM_SHROOM;       // 毁灭菇
constexpr APlantType LILY_PAD = ALILY_PAD;             // 荷叶
constexpr APlantType SQUASH = ASQUASH;                 // 倭瓜
constexpr APlantType THREEPEATER = ATHREEPEATER;       // 三发射手
constexpr APlantType TANGLE_KELP = ATANGLE_KELP;       // 缠绕海藻
constexpr APlantType JALAPENO = AJALAPENO;             // 火爆辣椒
constexpr APlantType SPIKEWEED = ASPIKEWEED;           // 地刺
constexpr APlantType TORCHWOOD = ATORCHWOOD;           // 火炬树桩
constexpr APlantType TALL_NUT = ATALL_NUT;             // 高坚果
constexpr APlantType SEA_SHROOM = ASEA_SHROOM;         // 水兵菇
constexpr APlantType PLANTERN = APLANTERN;             // 路灯花
constexpr APlantType CACTUS = ACACTUS;                 // 仙人掌
constexpr APlantType BLOVER = ABLOVER;                 // 三叶草
constexpr APlantType SPLIT_PEA = ASPLIT_PEA;           // 裂荚射手
constexpr APlantType STARFRUIT = ASTARFRUIT;           // 杨桃
constexpr APlantType PUMPKIN = APUMPKIN;               // 南瓜头
constexpr APlantType MAGNET_SHROOM = AMAGNET_SHROOM;   // 磁力菇
constexpr APlantType CABBAGE_PULT = ACABBAGE_PULT;     // 卷心菜投手
constexpr APlantType FLOWER_POT = AFLOWER_POT;         // 花盆
constexpr APlantType KERNEL_PULT = AKERNEL_PULT;       // 玉米投手
constexpr APlantType COFFEE_BEAN = ACOFFEE_BEAN;       // 咖啡豆
constexpr APlantType GARLIC = AGARLIC;                 // 大蒜
constexpr APlantType UMBRELLA_LEAF = AUMBRELLA_LEAF;   // 叶子保护伞
constexpr APlantType MARIGOLD = AMARIGOLD;             // 金盏花
constexpr APlantType MELON_PULT = AMELON_PULT;         // 西瓜投手
constexpr APlantType GATLING_PEA = AGATLING_PEA;       // 机枪射手
constexpr APlantType TWIN_SUNFLOWER = ATWIN_SUNFLOWER; // 双子向日葵
constexpr APlantType GLOOM_SHROOM = AGLOOM_SHROOM;     // 忧郁菇
constexpr APlantType CATTAIL = ACATTAIL;               // 香蒲
constexpr APlantType WINTER_MELON = AWINTER_MELON;     // 冰西瓜投手
constexpr APlantType GOLD_MAGNET = AGOLD_MAGNET;       // 吸金磁
constexpr APlantType SPIKEROCK = ASPIKEROCK;           // 地刺王
constexpr APlantType COB_CANNON = ACOB_CANNON;         // 玉米加农炮
constexpr APlantType IMITATOR = AIMITATOR;             // 模仿者

// 模仿者命名 + M
constexpr APlantType M_PEASHOOTE = AM_PEASHOOTER;          // 豌豆射手
constexpr APlantType M_SUNFLOWER = AM_SUNFLOWER;           // 向日葵
constexpr APlantType M_CHERRY_BOMB = AM_CHERRY_BOMB;       // 樱桃炸弹
constexpr APlantType M_WALL_NUT = AM_WALL_NUT;             // 坚果
constexpr APlantType M_POTATO_MINE = AM_POTATO_MINE;       // 土豆地雷
constexpr APlantType M_SNOW_PEA = AM_SNOW_PEA;             // 寒冰射手
constexpr APlantType M_CHOMPER = AM_CHOMPER;               // 大嘴花
constexpr APlantType M_REPEATER = AM_REPEATER;             // 双重射手
constexpr APlantType M_PUFF_SHROOM = AM_PUFF_SHROOM;       // 小喷菇
constexpr APlantType M_SUN_SHROOM = AM_SUN_SHROOM;         // 阳光菇
constexpr APlantType M_FUME_SHROOM = AM_FUME_SHROOM;       // 大喷菇
constexpr APlantType M_GRAVE_BUSTER = AM_GRAVE_BUSTER;     // 墓碑吞噬者
constexpr APlantType M_HYPNO_SHROOM = AM_HYPNO_SHROOM;     // 魅惑菇
constexpr APlantType M_SCAREDY_SHROOM = AM_SCAREDY_SHROOM; // 胆小菇
constexpr APlantType M_ICE_SHROOM = AM_ICE_SHROOM;         // 寒冰菇
constexpr APlantType M_DOOM_SHROOM = AM_DOOM_SHROOM;       // 毁灭菇
constexpr APlantType M_LILY_PAD = AM_LILY_PAD;             // 荷叶
constexpr APlantType M_SQUASH = AM_SQUASH;                 // 倭瓜
constexpr APlantType M_THREEPEATER = AM_THREEPEATER;       // 三发射手
constexpr APlantType M_TANGLE_KELP = AM_TANGLE_KELP;       // 缠绕海藻
constexpr APlantType M_JALAPENO = AM_JALAPENO;             // 火爆辣椒
constexpr APlantType M_SPIKEWEED = AM_SPIKEWEED;           // 地刺
constexpr APlantType M_TORCHWOOD = AM_TORCHWOOD;           // 火炬树桩
constexpr APlantType M_TALL_NUT = AM_TALL_NUT;             // 高坚果
constexpr APlantType M_SEA_SHROOM = AM_SEA_SHROOM;         // 水兵菇
constexpr APlantType M_PLANTERN = AM_PLANTERN;             // 路灯花
constexpr APlantType M_CACTUS = AM_CACTUS;                 // 仙人掌
constexpr APlantType M_BLOVER = AM_BLOVER;                 // 三叶草
constexpr APlantType M_SPLIT_PEA = AM_SPLIT_PEA;           // 裂荚射手
constexpr APlantType M_STARFRUIT = AM_STARFRUIT;           // 杨桃
constexpr APlantType M_PUMPKIN = AM_PUMPKIN;               // 南瓜头
constexpr APlantType M_MAGNET_SHROOM = AM_MAGNET_SHROOM;   // 磁力菇
constexpr APlantType M_CABBAGE_PULT = AM_CABBAGE_PULT;     // 卷心菜投手
constexpr APlantType M_FLOWER_POT = AM_FLOWER_POT;         // 花盆
constexpr APlantType M_KERNEL_PULT = AM_KERNEL_PULT;       // 玉米投手
constexpr APlantType M_COFFEE_BEAN = AM_COFFEE_BEAN;       // 咖啡豆
constexpr APlantType M_GARLIC = AM_GARLIC;                 // 大蒜
constexpr APlantType M_UMBRELLA_LEAF = AM_UMBRELLA_LEAF;   // 叶子保护伞
constexpr APlantType M_MARIGOLD = AM_MARIGOLD;             // 金盏花
constexpr APlantType M_MELON_PULT = AM_MELON_PULT;         // 西瓜投手

constexpr APlantType WDSS_0 = APEASHOOTER;       // 豌豆射手
constexpr APlantType XRK_1 = ASUNFLOWER;         // 向日葵
constexpr APlantType YTZD_2 = ACHERRY_BOMB;      // 樱桃炸弹
constexpr APlantType JG_3 = AWALL_NUT;           // 坚果
constexpr APlantType TDDL_4 = APOTATO_MINE;      // 土豆地雷
constexpr APlantType HBSS_5 = ASNOW_PEA;         // 寒冰射手
constexpr APlantType DZH_6 = ACHOMPER;           // 大嘴花
constexpr APlantType SCSS_7 = AREPEATER;         // 双重射手
constexpr APlantType XPG_8 = APUFF_SHROOM;       // 小喷菇
constexpr APlantType YGG_9 = ASUN_SHROOM;        // 阳光菇
constexpr APlantType DPG_10 = AFUME_SHROOM;      // 大喷菇
constexpr APlantType MBTSZ_11 = AGRAVE_BUSTER;   // 墓碑吞噬者
constexpr APlantType MHG_12 = AHYPNO_SHROOM;     // 魅惑菇
constexpr APlantType DXG_13 = ASCAREDY_SHROOM;   // 胆小菇
constexpr APlantType HBG_14 = AICE_SHROOM;       // 寒冰菇
constexpr APlantType HMG_15 = ADOOM_SHROOM;      // 毁灭菇
constexpr APlantType HY_16 = ALILY_PAD;          // 荷叶
constexpr APlantType WG_17 = ASQUASH;            // 倭瓜
constexpr APlantType SFSS_18 = ATHREEPEATER;     // 三发射手
constexpr APlantType CRHZ_19 = ATANGLE_KELP;     // 缠绕海藻
constexpr APlantType HBLJ_20 = AJALAPENO;        // 火爆辣椒
constexpr APlantType DC_21 = ASPIKEWEED;         // 地刺
constexpr APlantType HJSZ_22 = ATORCHWOOD;       // 火炬树桩
constexpr APlantType GJG_23 = ATALL_NUT;         // 高坚果
constexpr APlantType SBG_24 = ASEA_SHROOM;       // 水兵菇
constexpr APlantType LDH_25 = APLANTERN;         // 路灯花
constexpr APlantType XRZ_26 = ACACTUS;           // 仙人掌
constexpr APlantType SYC_27 = ABLOVER;           // 三叶草
constexpr APlantType LJSS_28 = ASPLIT_PEA;       // 裂荚射手
constexpr APlantType YT_29 = ASTARFRUIT;         // 杨桃
constexpr APlantType NGT_30 = APUMPKIN;          // 南瓜头
constexpr APlantType CLG_31 = AMAGNET_SHROOM;    // 磁力菇
constexpr APlantType JXCTS_32 = ACABBAGE_PULT;   // 卷心菜投手
constexpr APlantType HP_33 = AFLOWER_POT;        // 花盆
constexpr APlantType YMTS_34 = AKERNEL_PULT;     // 玉米投手
constexpr APlantType KFD_35 = ACOFFEE_BEAN;      // 咖啡豆
constexpr APlantType DS_36 = AGARLIC;            // 大蒜
constexpr APlantType YZBHS_37 = AUMBRELLA_LEAF;  // 叶子保护伞
constexpr APlantType JZH_38 = AMARIGOLD;         // 金盏花
constexpr APlantType XGTS_39 = AMELON_PULT;      // 西瓜投手
constexpr APlantType JQSS_40 = AGATLING_PEA;     // 机枪射手
constexpr APlantType SZXRK_41 = ATWIN_SUNFLOWER; // 双子向日葵
constexpr APlantType YYG_42 = AGLOOM_SHROOM;     // 忧郁菇
constexpr APlantType XP_43 = ACATTAIL;           // 香蒲
constexpr APlantType BXGTS_44 = AWINTER_MELON;   // 冰西瓜投手
constexpr APlantType XJC_45 = AGOLD_MAGNET;      // 吸金磁
constexpr APlantType DCW_46 = ASPIKEROCK;        // 地刺王
constexpr APlantType YMJNP_47 = ACOB_CANNON;     // 玉米加农炮
constexpr APlantType MFZ_48 = AIMITATOR;         // 模仿者

// 模仿者植物

constexpr APlantType M_WDSS_0 = AM_PEASHOOTER;      // 豌豆射手
constexpr APlantType M_XRK_1 = AM_SUNFLOWER;        // 向日葵
constexpr APlantType M_YTZD_2 = AM_CHERRY_BOMB;     // 樱桃炸弹
constexpr APlantType M_JG_3 = AM_WALL_NUT;          // 坚果
constexpr APlantType M_TDDL_4 = AM_POTATO_MINE;     // 土豆地雷
constexpr APlantType M_HBSS_5 = AM_SNOW_PEA;        // 寒冰射手
constexpr APlantType M_DZH_6 = AM_CHOMPER;          // 大嘴花
constexpr APlantType M_SCSS_7 = AM_REPEATER;        // 双重射手
constexpr APlantType M_XPG_8 = AM_PUFF_SHROOM;      // 小喷菇
constexpr APlantType M_YGG_9 = AM_SUN_SHROOM;       // 阳光菇
constexpr APlantType M_DPG_10 = AM_FUME_SHROOM;     // 大喷菇
constexpr APlantType M_MBTSZ_11 = AM_GRAVE_BUSTER;  // 墓碑吞噬者
constexpr APlantType M_MHG_12 = AM_HYPNO_SHROOM;    // 魅惑菇
constexpr APlantType M_DXG_13 = AM_SCAREDY_SHROOM;  // 胆小菇
constexpr APlantType M_HBG_14 = AM_ICE_SHROOM;      // 寒冰菇
constexpr APlantType M_HMG_15 = AM_DOOM_SHROOM;     // 毁灭菇
constexpr APlantType M_HY_16 = AM_LILY_PAD;         // 荷叶
constexpr APlantType M_WG_17 = AM_SQUASH;           // 倭瓜
constexpr APlantType M_SFSS_18 = AM_THREEPEATER;    // 三发射手
constexpr APlantType M_CRHZ_19 = AM_TANGLE_KELP;    // 缠绕海藻
constexpr APlantType M_HBLJ_20 = AM_JALAPENO;       // 火爆辣椒
constexpr APlantType M_DC_21 = AM_SPIKEWEED;        // 地刺
constexpr APlantType M_HJSZ_22 = AM_TORCHWOOD;      // 火炬树桩
constexpr APlantType M_GJG_23 = AM_TALL_NUT;        // 高坚果
constexpr APlantType M_SBG_24 = AM_SEA_SHROOM;      // 水兵菇
constexpr APlantType M_LDH_25 = AM_PLANTERN;        // 路灯花
constexpr APlantType M_XRZ_26 = AM_CACTUS;          // 仙人掌
constexpr APlantType M_SYC_27 = AM_BLOVER;          // 三叶草
constexpr APlantType M_LJSS_28 = AM_SPLIT_PEA;      // 裂荚射手
constexpr APlantType M_YT_29 = AM_STARFRUIT;        // 杨桃
constexpr APlantType M_NGT_30 = AM_PUMPKIN;         // 南瓜头
constexpr APlantType M_CLG_31 = AM_MAGNET_SHROOM;   // 磁力菇
constexpr APlantType M_JXCTS_32 = AM_CABBAGE_PULT;  // 卷心菜投手
constexpr APlantType M_HP_33 = AM_FLOWER_POT;       // 花盆
constexpr APlantType M_YMTS_34 = AM_KERNEL_PULT;    // 玉米投手
constexpr APlantType M_KFD_35 = AM_COFFEE_BEAN;     // 咖啡豆
constexpr APlantType M_DS_36 = AM_GARLIC;           // 大蒜
constexpr APlantType M_YZBHS_37 = AM_UMBRELLA_LEAF; // 叶子保护伞
constexpr APlantType M_JZH_38 = AM_MARIGOLD;        // 金盏花
constexpr APlantType M_XGTS_39 = AM_MELON_PULT;     // 西瓜投手

constexpr AZombieType ZOMBIE = AZOMBIE;                                 // 普僵
constexpr AZombieType FLAG_ZOMBIE = AFLAG_ZOMBIE;                       // 旗帜
constexpr AZombieType CONEHEAD_ZOMBIE = ACONEHEAD_ZOMBIE;               // 路障
constexpr AZombieType POLE_VAULTING_ZOMBIE = APOLE_VAULTING_ZOMBIE;     // 撑杆
constexpr AZombieType BUCKETHEAD_ZOMBIE = ABUCKETHEAD_ZOMBIE;           // 铁桶
constexpr AZombieType NEWSPAPER_ZOMBIE = ANEWSPAPER_ZOMBIE;             // 读报
constexpr AZombieType SCREEN_DOOR_ZOMBIE = ASCREEN_DOOR_ZOMBIE;         // 铁门
constexpr AZombieType FOOTBALL_ZOMBIE = AFOOTBALL_ZOMBIE;               // 橄榄
constexpr AZombieType DANCING_ZOMBIE = ADANCING_ZOMBIE;                 // 舞王
constexpr AZombieType BACKUP_DANCER = ABACKUP_DANCER;                   // 伴舞
constexpr AZombieType DUCKY_TUBE_ZOMBIE = ADUCKY_TUBE_ZOMBIE;           // 鸭子
constexpr AZombieType SNORKEL_ZOMBIE = ASNORKEL_ZOMBIE;                 // 潜水
constexpr AZombieType ZOMBONI = AZOMBONI;                               // 冰车
constexpr AZombieType ZOMBIE_BOBSLED_TEAM = AZOMBIE_BOBSLED_TEAM;       // 雪橇
constexpr AZombieType DOLPHIN_RIDER_ZOMBIE = ADOLPHIN_RIDER_ZOMBIE;     // 海豚
constexpr AZombieType JACK_IN_THE_BOX_ZOMBIE = AJACK_IN_THE_BOX_ZOMBIE; // 小丑
constexpr AZombieType BALLOON_ZOMBIE = ABALLOON_ZOMBIE;                 // 气球
constexpr AZombieType DIGGER_ZOMBIE = ADIGGER_ZOMBIE;                   // 矿工
constexpr AZombieType POGO_ZOMBIE = APOGO_ZOMBIE;                       // 跳跳
constexpr AZombieType ZOMBIE_YETI = AZOMBIE_YETI;                       // 雪人
constexpr AZombieType BUNGEE_ZOMBIE = ABUNGEE_ZOMBIE;                   // 蹦极
constexpr AZombieType LADDER_ZOMBIE = ALADDER_ZOMBIE;                   // 扶梯
constexpr AZombieType CATAPULT_ZOMBIE = ACATAPULT_ZOMBIE;               // 投篮
constexpr AZombieType GARGANTUAR = AGARGANTUAR;                         // 白眼
constexpr AZombieType IMP = AIMP;                                       // 小鬼
constexpr AZombieType DR_ZOMBOSS = ADR_ZOMBOSS;                         // 僵博
constexpr AZombieType GIGA_GARGANTUAR = AGIGA_GARGANTUAR;               // 红眼

constexpr AZombieType PJ_0 = ZOMBIE;                  // 普僵
constexpr AZombieType QZ_1 = FLAG_ZOMBIE;             // 旗帜
constexpr AZombieType LZ_2 = CONEHEAD_ZOMBIE;         // 路障
constexpr AZombieType CG_3 = POLE_VAULTING_ZOMBIE;    // 撑杆
constexpr AZombieType TT_4 = BUCKETHEAD_ZOMBIE;       // 铁桶
constexpr AZombieType DB_5 = NEWSPAPER_ZOMBIE;        // 读报
constexpr AZombieType TM_6 = SCREEN_DOOR_ZOMBIE;      // 铁门
constexpr AZombieType GL_7 = FOOTBALL_ZOMBIE;         // 橄榄
constexpr AZombieType WW_8 = DANCING_ZOMBIE;          // 舞王
constexpr AZombieType BW_9 = BACKUP_DANCER;           // 伴舞
constexpr AZombieType YZ_10 = DUCKY_TUBE_ZOMBIE;      // 鸭子
constexpr AZombieType QS_11 = SNORKEL_ZOMBIE;         // 潜水
constexpr AZombieType BC_12 = ZOMBONI;                // 冰车
constexpr AZombieType XQ_13 = ZOMBIE_BOBSLED_TEAM;    // 雪橇
constexpr AZombieType HT_14 = DOLPHIN_RIDER_ZOMBIE;   // 海豚
constexpr AZombieType XC_15 = JACK_IN_THE_BOX_ZOMBIE; // 小丑
constexpr AZombieType QQ_16 = BALLOON_ZOMBIE;         // 气球
constexpr AZombieType KG_17 = DIGGER_ZOMBIE;          // 矿工
constexpr AZombieType TT_18 = POGO_ZOMBIE;            // 跳跳
constexpr AZombieType XR_19 = ZOMBIE_YETI;            // 雪人
constexpr AZombieType BJ_20 = BUNGEE_ZOMBIE;          // 蹦极
constexpr AZombieType FT_21 = LADDER_ZOMBIE;          // 扶梯
constexpr AZombieType TL_22 = CATAPULT_ZOMBIE;        // 投篮
constexpr AZombieType BY_23 = GARGANTUAR;             // 白眼
constexpr AZombieType XG_24 = IMP;                    // 小鬼
constexpr AZombieType JB_25 = DR_ZOMBOSS;             // 僵博
constexpr AZombieType HY_32 = GIGA_GARGANTUAR;        // 红眼
#endif
