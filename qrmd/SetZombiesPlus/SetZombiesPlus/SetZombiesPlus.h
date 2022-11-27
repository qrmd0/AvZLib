/*
 * @Author: qrmd
 * @Date: 2022-07-05 19:56:17
 * @LastEditors: qrmd
 * @LastEditTime: 2022-11-27 22:37:24
 * @Description:SetZomies的增强版本，更自由地设置出怪
 * 使用方法：1、将本文件粘贴到AsmVsZombies/inc；
 *          2、在要应用此库的脚本开头添加 #include "SetZombiesPlus.h" ；
 *          3、在README与本库的代码注释中查看函数与变量的用法。
 * 来自AsmVsZombies的公开插件仓库：
 * 主库：https://github.com/qrmd0/AvZLib
 * 镜像库1：https://gitee.com/qrmd/AvZLib
 * 镜像库2：https://gitlab.com/avzlib/AvZLib
 * Copyright (c) 2022 by qrmd, All Rights Reserved.
 */

#include "avz.h"
#include <ctime>

using namespace AvZ;

namespace _qrmd {

// 出怪方式
enum SetZombiesMethod {
    AVERAGE,
    INTERNAL,
};

// 自然出怪的限定方式
enum SetZombiesLimitsMethod {
    TRANS_WAVE,     //变速波次
    TOTAL_WAVE = 21 //全部波次
};

// 为了满足限定条件刷新出怪列表的最长耗时（秒）
int wait_limit = 60;

// 是否显示刷新出怪列表的调试信息
bool is_show_info = false;

// 自然出怪的限定条件
struct ZombiesRefreshLimit;

// 指定波的出怪情况
struct WaveZombies;

// 游戏原生的生成出怪列表函数
void InternalSetZombies();

// 以极限（均匀摊派）方式生成出怪列表
void SetZombiesAverage(std::vector<int> types);

// 以自然（游戏原生）方式生成出怪列表
void SetZombiesInternal(std::vector<int> types);

// 显示为了满足限定条件刷新出怪列表的调试信息
void ShowInfo(bool is_successful, int count, int time);

// *** Not In Queue
// 设置为了满足限定条件刷新出怪列表的最长耗时（秒），默认为60
// 此行代码必须编写于对应的SetZombiesLimits前以生效
void SetWaitLimit(int time);

// *** Not In Queue
// 设置是否显示为了满足限定条件刷新出怪列表的调试信息，默认为否
// 此行代码必须编写于对应的SetZombiesLimits前以生效
void SetIsShowInfo(bool is_true);

// *** Not In Queue
// 设置本轮出怪种类为[types]，出怪方式为[method]
// [types]的参数命名规则同原SetZombies，填入的{伴舞僵尸、雪橇僵尸、雪人僵尸、小鬼僵尸、僵王博士}将被视为无效而被剔除
// [method]为AVERAGE时，以极限（均匀摊派）方式生成出怪列表，类似于原SetZombies，但不强制刷出旗帜僵尸和蹦极僵尸；
// [method]为INTERNAL时，以自然（游戏原生）方式生成出怪列表，强制刷出旗帜僵尸和蹦极僵尸，此方式下[types]中有效的僵尸种类数量至少为2；
// 示例：// 以自然（游戏原生）方式设置出怪为{普通僵尸、路障僵尸、撑杆僵尸、舞王僵尸、海豚僵尸、小丑僵尸、气球僵尸、矿工僵尸、跳跳僵尸、红眼巨人僵尸}
// SetZombies({ZOMBIE, CONEHEAD_ZOMBIE, POLE_VAULTING_ZOMBIE, DANCING_ZOMBIE, DOLPHIN_RIDER_ZOMBIE, JACK_IN_THE_BOX_ZOMBIE, BALLOON_ZOMBIE, DIGGER_ZOMBIE, POGO_ZOMBIE, GIGA_GARGANTUAR}, INTERNAL);
void SetZombiesPlus(std::vector<int> types, SetZombiesMethod method = INTERNAL);

// *** Not In Queue
// 不断刷新出怪列表，直至满足[zombies_limits]中的所有限定条件为止
// [zombies_limits]的参数为：{int wave, int type, string method, int number}
// [wave]为属于[1, 20]的整数时，表示以[method]方式限定第[wave]波的[type]种类僵尸的数量为[number]
// [wave]为TRANS_WAVE时，[type]仅接受GIGA_GARGANTUAR或其别名，表示以[method]方式限定变速的波数为[number]
// [wave]为TOTAL_WAVE时，表示以[method]方式限定全部波次的[type]种类僵尸的总数为[number]
// [type]表示的僵尸种类必须属于本轮出怪种类
// [method]可以取"==", "<", ">", "<=", ">=", "!="
// 示例：// 限定第9波不刷出红眼巨人僵尸、第10波刷出至少10只普通僵尸以及变速的波数不小于15
// SetZombiesLimits({{9, GIGA_GARGANTUAR, "==", 0}, {10, ZOMBIE, ">=", 10}, {TRANS_WAVE, GIGA_GARGANTUAR, ">=", 16}});
void SetZombiesLimits(std::vector<ZombiesRefreshLimit> zombies_limits);

void SetZombiesLimits(ZombiesRefreshLimit each);

void SetZombiesLimits(int wave, int type, std::string method, int number);

// 设置[wave]波的出怪列表[wave_zombies]
// [wave_zombies]的参数为：{int types, int number}
// [types]为僵尸种类，
// [number]为僵尸数量，各种类僵尸的数量之和必须为50。
// 示例：// 设置第18波的出怪为：小鬼僵尸20只、蹦极僵尸14只、巨人僵尸16只
// SetWaveZombieList(18, {{IMP, 20}, {BUNGEE_ZOMBIE, 14}, {GARGANTUAR, 16}});
void SetWaveZombieList(int wave, std::vector<WaveZombies> wave_zombies);

} // namespace _qrmd

using _qrmd::AVERAGE;
using _qrmd::INTERNAL;
using _qrmd::SetIsShowInfo;
using _qrmd::SetWaitLimit;
using _qrmd::SetWaveZombieList;
using _qrmd::SetZombiesLimits;
using _qrmd::SetZombiesPlus;
using _qrmd::TOTAL_WAVE;
using _qrmd::TRANS_WAVE;

void _qrmd::InternalSetZombies()
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl 0x6a9ec0, %%edi;"
        "movl 0x768(%%edi), %%edi;"
        "movl $0x4092e0, %%ebx;"
        "calll *%%ebX;"
        "popal;"
        :
        :
        :);
#else
    __asm {
        pushad
        mov edi,[0x6a9ec0]
        mov edi,[edi+0x768]
        mov ebx,0x4092e0
        call ebx
        popad
    }
#endif
}
void _qrmd::SetZombiesAverage(std::vector<int> types)
{

    const std::vector<int> ban_zombie_types = {FLAG_ZOMBIE, BUNGEE_ZOMBIE};
    std::vector<int> zombie_types = {};

    for (int each : types) {
        if (!(std::find(ban_zombie_types.begin(), ban_zombie_types.end(), each) != ban_zombie_types.end())) {
            zombie_types.emplace_back(each);
        }
    }

    auto zombie_list = GetMainObject()->zombieList();
    for (int index = 0; index < 1000; ++index, ++zombie_list) {
        (*zombie_list) = zombie_types[index % zombie_types.size()];
    }

    // 生成旗帜僵尸
    if (std::find(types.begin(), types.end(), FLAG_ZOMBIE) != types.end()) {
        for (auto index : {450, 950}) {
            (*(GetMainObject()->zombieList() + index)) = FLAG_ZOMBIE;
        }
    }

    // 生成蹦极僵尸
    /*if (std::find(types.begin(), types.end(), BUNGEE_ZOMBIE) != types.end()) {
        for (auto index : {451, 452, 453, 454, 951, 952, 953, 954}) {
            (*(GetMainObject()->zombieList() + index)) = BUNGEE_ZOMBIE;
        }
    }*/

    if (GetPvzBase()->gameUi() == 2) {
        Asm::killZombiesPreview();
        GetMainObject()->selectCardUi_m()->isCreatZombie() = false;
    }
}
void _qrmd::SetZombiesInternal(std::vector<int> types)
{

    uint8_t* zombie_type_list = GetZombieTypeList();

    for (int i = ZOMBIE; i <= GIGA_GARGANTUAR; ++i) {
        if (std::find(types.begin(), types.end(), i) != types.end()) {
            zombie_type_list[i] = true;
        } else {
            zombie_type_list[i] = false;
        }
    }

    InternalSetZombies();
}

void _qrmd::SetZombiesPlus(std::vector<int> types, SetZombiesMethod method)
{

    const std::vector<int> ban_zombie_types = {BACKUP_DANCER, ZOMBIE_BOBSLED_TEAM, ZOMBIE_YETI, IMP, DR_ZOMBOSS};
    std::vector<int> zombie_types = {};

    for (int each : types) {
        if (!(std::find(ban_zombie_types.begin(), ban_zombie_types.end(), each) != ban_zombie_types.end())) {
            zombie_types.emplace_back(each);
        }
    }

    if (zombie_types.empty()) {
        ShowErrorNotInQueue("SetZombiesPlus的有效僵尸种类数量不得为0");
        return;
    }

    switch (method) {
    case AVERAGE:
        SetZombiesAverage(zombie_types);
        break;
    case INTERNAL:
        if (types.size() < 2) {
            ShowErrorNotInQueue("SetZombiesPlus以自然方式出怪时的有效僵尸种类数量不得小于2，如果您想仅刷出一种僵尸，请使用极限方式出怪");
            return;
        } else {
            SetZombiesInternal(zombie_types);
        }
        break;
    default:
        break;
    }
}

void _qrmd::SetIsShowInfo(bool is_true)
{
    is_show_info = is_true ? true : false;
}
struct _qrmd::ZombiesRefreshLimit {
    int wave;
    int type;
    std::string method;
    int number;
};

void _qrmd::SetZombiesLimits(std::vector<ZombiesRefreshLimit> zombies_limits)
{
    bool is_succeed;
    int zombie_number;
    int trans_wave;
    int trans_zombie_number;
    auto zombie_list = GetMainObject()->zombieList();
    clock_t time0 = std::clock();
    for (int count = 0;; ++count) {
        if (std::clock() - time0 >= wait_limit * 1000.0) {
            ShowInfo(false, count, std::clock() - time0);
            return;
        }
        is_succeed = true;
        for (ZombiesRefreshLimit each : zombies_limits) {
            if (!GetZombieTypeList()[each.type]) {
                ShowErrorNotInQueue("SetZombiesLimits限定的僵尸种类必须属于本轮出怪种类");
                return;
            }
            if (each.wave == TRANS_WAVE) {
                if (each.type != GIGA_GARGANTUAR) {
                    ShowErrorNotInQueue("SetZombiesLimits的限定条件为变速时，第二个参数必须为GIGA_GARGANTUAR或其别名");
                    return;
                }
                trans_wave = 22;
                trans_zombie_number = 0;
                for (int wave = 1; wave <= 20; ++wave) {
                    for (int i = (wave - 1) * 50; i < wave * 50; ++i) {
                        if (*(zombie_list + i) == each.type)
                            trans_zombie_number += 1;
                    }
                    if (trans_zombie_number >= 50) {
                        trans_wave = wave + 1;
                        break;
                    }
                }
                if (each.method == "==") {
                    if (trans_wave != each.number) {
                        InternalSetZombies();
                        is_succeed = false;
                    }
                } else if (each.method == "<") {
                    if (trans_wave >= each.number) {
                        InternalSetZombies();
                        is_succeed = false;
                    }
                } else if (each.method == ">") {
                    if (trans_wave <= each.number) {
                        InternalSetZombies();
                        is_succeed = false;
                    }
                } else if (each.method == "<=") {
                    if (trans_wave > each.number) {
                        InternalSetZombies();
                        is_succeed = false;
                    }
                } else if (each.method == ">=") {
                    if (trans_wave < each.number) {
                        InternalSetZombies();
                        is_succeed = false;
                    }
                } else if (each.method == "!=") {
                    if (trans_wave == each.number) {
                        InternalSetZombies();
                        is_succeed = false;
                    }
                }
            } else if (each.wave >= 1 && each.wave <= 20) {
                zombie_number = 0;
                for (int i = (each.wave - 1) * 50; i < each.wave * 50; ++i) {
                    if (*(zombie_list + i) == each.type)
                        zombie_number += 1;
                }
                if (each.method == "==") {
                    if (zombie_number != each.number) {
                        InternalSetZombies();
                        is_succeed = false;
                    }
                } else if (each.method == "<") {
                    if (zombie_number >= each.number) {
                        InternalSetZombies();
                        is_succeed = false;
                    }
                } else if (each.method == ">") {
                    if (zombie_number <= each.number) {
                        InternalSetZombies();
                        is_succeed = false;
                    }
                } else if (each.method == "<=") {
                    if (zombie_number > each.number) {
                        InternalSetZombies();
                        is_succeed = false;
                    }
                } else if (each.method == ">=") {
                    if (zombie_number < each.number) {
                        InternalSetZombies();
                        is_succeed = false;
                    }
                } else if (each.method == "!=") {
                    if (zombie_number == each.number) {
                        InternalSetZombies();
                        is_succeed = false;
                    }
                }
            } else if (each.wave == TOTAL_WAVE) {
                zombie_number = 0;
                for (int wave = 1; wave <= 20; ++wave) {
                    for (int i = (wave - 1) * 50; i < wave * 50; ++i) {
                        if (*(zombie_list + i) == each.type)
                            zombie_number += 1;
                    }
                }
                if (each.method == "==") {
                    if (zombie_number != each.number) {
                        InternalSetZombies();
                        is_succeed = false;
                    }
                } else if (each.method == "<") {
                    if (zombie_number >= each.number) {
                        InternalSetZombies();
                        is_succeed = false;
                    }
                } else if (each.method == ">") {
                    if (zombie_number <= each.number) {
                        InternalSetZombies();
                        is_succeed = false;
                    }
                } else if (each.method == "<=") {
                    if (zombie_number > each.number) {
                        InternalSetZombies();
                        is_succeed = false;
                    }
                } else if (each.method == ">=") {
                    if (zombie_number < each.number) {
                        InternalSetZombies();
                        is_succeed = false;
                    }
                } else if (each.method == "!=") {
                    if (zombie_number == each.number) {
                        InternalSetZombies();
                        is_succeed = false;
                    }
                }
            } else {
                ShowErrorNotInQueue("SetZombiesLimits的限定条件的第一个参数必须属于[1, 20]或为TRANS/TOTAL_WAVE");
                return;
            }
        }
        if (is_succeed) {
            if (is_show_info)
                ShowInfo(true, count, std::clock() - time0);
            return;
        }
    }
}
void _qrmd::SetZombiesLimits(ZombiesRefreshLimit each)
{
    std::vector<ZombiesRefreshLimit> temp = {each};
    SetZombiesLimits(temp);
}
void _qrmd::SetZombiesLimits(int wave, int type, std::string method, int number)
{
    ZombiesRefreshLimit temp1 = {wave, type, method, number};
    std::vector<ZombiesRefreshLimit> temp2 = {temp1};
    SetZombiesLimits(temp2);
}
void _qrmd::SetWaitLimit(int time)
{
    wait_limit = time;
    return;
}
void _qrmd::ShowInfo(bool is_successful, int count, int time)
{
    if (is_successful)
        ShowErrorNotInQueue("找到了符合限定条件的出怪列表\n刷新次数：#\n耗时：#秒", count, time / 1000.0);
    else
        ShowErrorNotInQueue("未能找到符合限定条件的出怪列表，您可以延长等待时间或放宽限定条件重试\n刷新次数：#\n耗时：#秒", count, time / 1000.0);
}
struct _qrmd::WaveZombies {
    int type;
    int number;
};
void _qrmd::SetWaveZombieList(int wave, std::vector<WaveZombies> wave_zombies)
{
    int number = 0;
    auto zombie_list = GetMainObject()->zombieList();

    for (WaveZombies each : wave_zombies)
        number += each.number;
    if (number != 50) {
        ShowErrorNotInQueue("SetWaveZombieList设置的一波僵尸的数量必须为50，而当前的数量为#", number);
        return;
    }
    int index = (wave - 1) * 50;
    for (WaveZombies each : wave_zombies) {
        for (int i = index; i < index + each.number; ++i) {
            *(zombie_list + i) = each.type;
        }
        index += each.number;
    }
}