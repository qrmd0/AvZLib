/*
 * @Author: qrmd
 * @Date: 2022-07-05 19:56:17
 * @LastEditors: qrmd
 * @LastEditTime: 2022-07-06 09:53:26
 * @Description:SetZomies的增强版本，支持多种出怪方式
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

using namespace AvZ;

namespace _qrmd {

// 游戏原生的生成出怪列表函数
void InternalSetZombies();

// 以极限（均匀摊派）方式生成出怪列表
void SetZombiesAverage(std::vector<int> types);

// 以自然（游戏原生）方式生成出怪列表
void SetZombiesInternal(std::vector<int> types);

enum SetZombiesMethod {
    AVERAGE,
    INTERNAL,
    SIMULATE
};
// *** Not In Queue
// 以[method]方式设置本轮出怪种类为[types]
// [types]的参数命名规则同原SetZombies，填入的{伴舞僵尸、雪橇僵尸、雪人僵尸、小鬼僵尸、僵王博士}将被视为无效而剔除
// [method]为AVERAGE时，以极限（均匀摊派）方式生成出怪列表，同原SetZombies，但不强制刷出旗帜僵尸和蹦极僵尸；
// [method]为INTERNAL时，以自然（游戏原生）方式生成出怪列表，强制刷出旗帜僵尸和蹦极僵尸；
void SetZombies(std::vector<int> types, SetZombiesMethod method);

} // namespace _qrmd
using _qrmd::AVERAGE;
using _qrmd::INTERNAL;
using _qrmd::SetZombies;
using _qrmd::SIMULATE;

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
    if (std::find(types.begin(), types.end(), BUNGEE_ZOMBIE) != types.end()) {
        for (auto index : {451, 452, 453, 454, 951, 952, 953, 954}) {
            (*(GetMainObject()->zombieList() + index)) = BUNGEE_ZOMBIE;
        }
    }

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
// *** Not In Queue
// 以[method]方式设置本轮出怪种类为[types]
// [types]的参数命名规则同原SetZombies，填入的{伴舞僵尸、雪橇僵尸、雪人僵尸、小鬼僵尸、僵王博士}将被视为无效而剔除
// [method]为AVERAGE时，以极限（均匀摊派）方式生成出怪列表，同原SetZombies，但不强制刷出旗帜僵尸和蹦极僵尸；
// [method]为INTERNAL时，以自然（游戏原生）方式生成出怪列表，强制刷出旗帜僵尸和蹦极僵尸，[types]中有效的僵尸种类数量至少为2；
void _qrmd::SetZombies(std::vector<int> types, SetZombiesMethod method)
{

    const std::vector<int> ban_zombie_types = {BACKUP_DANCER, ZOMBIE_BOBSLED_TEAM, ZOMBIE_YETI, IMP, DR_ZOMBOSS};
    std::vector<int> zombie_types = {};

    for (int each : types) {
        if (!(std::find(ban_zombie_types.begin(), ban_zombie_types.end(), each) != ban_zombie_types.end())) {
            zombie_types.emplace_back(each);
        }
    }

    if (zombie_types.empty()) {
        ShowErrorNotInQueue("SetZombies的有效僵尸种类数量不得为0");
        return;
    }

    switch (method) {
    case AVERAGE:
        SetZombiesAverage(zombie_types);
        break;
    case INTERNAL:
        if (types.size() < 2) {
            ShowErrorNotInQueue("以自然方式出怪时，SetZombies的有效僵尸种类数量不得小于2");
            return;
        } else {
            SetZombiesInternal(zombie_types);
        }
        break;
    case SIMULATE:

        break;
    default:
        break;
    }
}