#pragma once

#include <algorithm>
#include <array>
#include <random>
#include <set>
#include <stdint.h>
#include <unordered_map>
#include <vector>
#include "avz.h"

using namespace AvZ;

namespace _setzombies_internal {
std::mt19937 rng;

class : GlobalVar {
    void beforeScript() {
        std::seed_seq ss{int(time(nullptr)), int(clock()), GetMainObject()->globalClock()};
        rng.seed(ss);
        rng.discard(624);
    }
} __init_rng;

std::unordered_map<int, std::array<int, 2>> weight = {
    {ZOMBIE,                 {400,  400 }},
    {CONEHEAD_ZOMBIE,        {1000, 1000}},
    {POLE_VAULTING_ZOMBIE,   {2000, 2000}},
    {BUCKETHEAD_ZOMBIE,      {3000, 3000}},
    {NEWSPAPER_ZOMBIE,       {1000, 1000}},
    {SCREEN_DOOR_ZOMBIE,     {3500, 3500}},
    {FOOTBALL_ZOMBIE,        {2000, 2000}},
    {DANCING_ZOMBIE,         {1000, 1000}},
    {SNORKEL_ZOMBIE,         {2000, 2000}},
    {ZOMBONI,                {2000, 2000}},
    {DOLPHIN_RIDER_ZOMBIE,   {1500, 1500}},
    {JACK_IN_THE_BOX_ZOMBIE, {1000, 1000}},
    {BALLOON_ZOMBIE,         {2000, 2000}},
    {DIGGER_ZOMBIE,          {1000, 1000}},
    {POGO_ZOMBIE,            {1000, 1000}},
    {ZOMBIE_YETI,            {1,    1   }},
    {BUNGEE_ZOMBIE,          {0,    1000}},
    {LADDER_ZOMBIE,          {1000, 1000}},
    {CATAPULT_ZOMBIE,        {1500, 1500}},
    {GARGANTUAR,             {1500, 1500}},
    {GIGA_GARGANTUAR,        {1000, 6000}},
};

std::unordered_map<int, std::vector<int>> scene_banned = {
    {0, {SNORKEL_ZOMBIE, DOLPHIN_RIDER_ZOMBIE}},
    {1, {SNORKEL_ZOMBIE, ZOMBONI, DOLPHIN_RIDER_ZOMBIE}},
    {2, {}},
    {3, {}},
    {4, {DANCING_ZOMBIE, SNORKEL_ZOMBIE, DOLPHIN_RIDER_ZOMBIE, DIGGER_ZOMBIE}}
};
};

std::vector<int> random_type_list(const std::vector<int>& required = {}, const std::vector<int>& banned = {}) {
    int sel = _setzombies_internal::rng() % 5 ? 2 : 5, another = 7 - sel;
    std::set<int> cand_ = {3, 4, 6, 7, 8, 11, 12, 14, 15, 16, 17, 18, 20, 21, 22, 23, 32, another, -1, -2};
    for(int x : _setzombies_internal::scene_banned[GetMainObject()->scene()])
        cand_.erase(x);
    for(const auto& s : {required, banned})
        for(int x : s) {
            if(cand_.count(x))
                cand_.erase(x);
            else {
                ShowErrorNotInQueue("出怪类型限制有误");
                throw Exception("");
            }
        }
    std::vector<int> cand(cand_.begin(), cand_.end());
    std::vector<int> out = {0, 19, sel};
    for(auto x : required)
        out.push_back(x);
    for(int i = 0; i < 9 - required.size(); i++) {
        int idx = _setzombies_internal::rng() % cand.size();
        if(cand[idx] >= 0)
            out.push_back(cand[idx]);
        cand[idx] = cand[cand.size() - 1];
        cand.pop_back();
    }
    return out;
}

std::array<int, 50> simulate_wave(std::vector<int> type_list, bool is_huge_wave = false, int giga_limit = 50, int giga_count = -1) {
    if(giga_count != -1)
        type_list.erase(find(type_list.begin(), type_list.end(), GIGA_GARGANTUAR));
    std::vector<int> weights;
    for(int x : type_list)
        weights.push_back(_setzombies_internal::weight[x][is_huge_wave]);
    std::discrete_distribution<> d(weights.begin(), weights.end());
    std::array<int, 50> result;
    int cur = 0;
    if(is_huge_wave) {
        result[cur++] = FLAG_ZOMBIE;
        for(int i = 0; i < 8; i++)
            result[cur++] = ZOMBIE;
    }
    for(int i = 0; i < giga_count; i++)
        result[cur++] = GIGA_GARGANTUAR;
    while(cur < 50) {
        int zombie_type = type_list[d(_setzombies_internal::rng)];
        while(!is_huge_wave && giga_limit <= 0 && zombie_type == GIGA_GARGANTUAR)
            zombie_type = type_list[d(_setzombies_internal::rng)];
        result[cur++] = zombie_type;
        if(zombie_type == GIGA_GARGANTUAR)
            giga_limit--;
    }
    return result;
}

enum WaveType {
    DEFAULT,
    ALL_NORMAL,
    ALL_HUGE
};

void simulate_summon(const std::vector<int>& type_list, int giga_limit = 50, WaveType wave_type = DEFAULT, int giga_count = -1) {
    for(int wave = 1; wave <= 20; wave++) {
        bool is_huge = (wave_type == ALL_HUGE) || (wave_type == DEFAULT && wave % 10 == 0);
        auto zombies = simulate_wave(type_list, is_huge, giga_limit, giga_count);
        for(int i = 0; i < 50; i++) {
            GetMainObject()->zombieList()[(wave - 1) * 50 + i] = zombies[i];
            if(zombies[i] == GIGA_GARGANTUAR)
                giga_limit--;
        }
    }
}
