/*
Copyright (C) 2022 vector-wlc, Reisen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef REISEN_AVZ_FIX_H
#define REISEN_AVZ_FIX_H
#include "avz_global.h"
#include "avz_memory.h"
#include "avz_time_operation.h"
#include "pvzfunc.h"

namespace AvZ {
extern MainObject* __main_object;
extern PvZ* __pvz_base;

void SetWavelength(const std::initializer_list<WaveTime>& lst)
{
    extern std::vector<OperationQueue> __operation_queue_vec;
    extern TimeWave __time_wave_insert;
    extern std::vector<OperationQueue>::iterator __wavelength_it;
    // 需要恢复的状态
    auto temp = __time_wave_insert;

    for (const auto& ele : lst) {
        __operation_queue_vec[ele.wave - 1].wave_length = ele.time;

        InsertGuard insert_guard(true);
        SetTime(1, ele.wave);
        InsertOperation([=]() {
            __main_object->zombieRefreshHp() = 0;
            int now_time = __main_object->gameClock() - __operation_queue_vec[ele.wave - 1].refresh_time;
            int countdown = ele.time - now_time;
            if (countdown < 0) {
                ShowErrorNotInQueue(
                    "您在第 # 波设定的波长为 #，但是当前时刻点为 "
                    "#，已超出可设定的时间范围，波长设定失败！",
                    ele.wave, ele.time, now_time);
                return;
            }
            __main_object->refreshCountdown() = countdown;
            __main_object->initialCountdown() = ele.time;

            if (__wavelength_it - __operation_queue_vec.begin() < ele.wave - 1) {
                __wavelength_it = __operation_queue_vec.begin() + ele.wave - 1;
            }

            // 设定刷新时间点
            for (; __wavelength_it != __operation_queue_vec.end() - 1; ++__wavelength_it) {
                if (__wavelength_it->refresh_time == __DEFAULT_START_TIME || __wavelength_it->wave_length == -1) {
                    break;
                }
                (__wavelength_it + 1)->refresh_time = __wavelength_it->refresh_time + __wavelength_it->wave_length;
            }
        },
            "writeWavelength");
    }

    SetTime(temp);
}

void SetZombies(const std::initializer_list<int>& zombie_type)
{
    std::vector<int> zombie_type_vec;
    bool include_bungee = false;

    for (const auto& type : zombie_type) {
        // 做一些处理，出怪生成不应大量含有 旗帜 舞伴 雪橇小队 雪人 蹦极 小鬼
        if (type == BJ_20)
            include_bungee = true;

        if (!RangeIn(type, {QZ_1, BW_9, XQ_13, XR_19, BJ_20, XG_24})) {
            zombie_type_vec.push_back(type);
        }
    }
    auto zombie_list = __main_object->zombieList();
    for (int index = 0; index < 1000; ++index, ++zombie_list) {
        (*zombie_list) = zombie_type_vec[index % zombie_type_vec.size()];
    }

    // 生成旗帜
    for (auto index : {450, 950}) {
        (*(__main_object->zombieList() + index)) = QZ_1;
    }

    // 生成蹦极
    if (include_bungee)
        for (auto i : {451, 951}) 
            for (int index = i; index < i + 49 / (zombie_type_vec.size() + 1); ++index) {
                (*(__main_object->zombieList() + index)) = BJ_20;
            }

    if (__pvz_base->gameUi() == 2) {
        // 去掉当前画面上的僵尸
        Asm::killZombiesPreview();
        // 重新生成僵尸
        __main_object->selectCardUi_m()->isCreatZombie() = false;
    }
}

class ZombieIterator : public std::iterator<std::forward_iterator_tag, SafePtr<Zombie>> {
    private:
    SafePtr<Zombie> cur;

    ZombieIterator(SafePtr<Zombie> cur_) : cur(cur_) {}

    void forward() {
        ++cur;
        while(cur != GetMainObject()->zombieArray() + GetMainObject()->zombieTotal() &&
            (cur->isDead() || cur->isDisappeared()))
            ++cur;
    }

    public:
    ZombieIterator() {
        cur = GetMainObject()->zombieArray();
        if(cur->isDead() || cur->isDisappeared())
            forward();
    }

    SafePtr<Zombie> operator*() const {
        return cur;
    }

    SafePtr<Zombie> operator++() {
        forward();
        return cur;
    }

    SafePtr<Zombie> operator++(int) {
        auto tmp = cur;
        forward();
        return tmp;
    }

    bool operator==(ZombieIterator& rhs) { 
        return cur.toUnsafe() == rhs.cur.toUnsafe();
    }

    bool operator!=(ZombieIterator& rhs) { 
        return !(*this == rhs);
    }

    static ZombieIterator begin() {
        return ZombieIterator();
    }

    static ZombieIterator end() {
        return ZombieIterator(GetMainObject()->zombieArray() + GetMainObject()->zombieTotal());
    }
};
} // namespace AvZ
#endif
