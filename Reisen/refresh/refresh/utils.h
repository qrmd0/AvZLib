#pragma once

#include <set>
#include "avz.h"

using namespace std;
using namespace AvZ;

#define Card2(...) do { \
    Card(__VA_ARGS__); \
    InsertOperation([](){ \
        for(auto& seed : BasicFilter<Seed>()) \
            seed.isUsable() = true; \
    }); \
} while(false)

#define CardNotInQueue2(...) do { \
    CardNotInQueue(__VA_ARGS__); \
    for(auto& seed : BasicFilter<Seed>()) \
        seed.isUsable() = true; \
} while(false)

int WaveTotalHp(int wave) {
    int ret;
    asm volatile(
        "pushl %[wave];"
        "movl 0x6a9ec0, %%ebx;"
        "movl 0x768(%%ebx), %%ebx;"
        "movl $0x412e30, %%ecx;"
        "calll *%%ecx;"
        "mov %%eax, %[ret];"
        : [ret] "=rm"(ret)
        : [wave] "rm"(wave - 1)
        : "ebx", "ecx", "edx"
    );
    return ret;
}

void UnifyWaves(bool huge) {
    static const set<int> excluded_types = {
        FLAG_ZOMBIE,
        POLE_VAULTING_ZOMBIE,
        ZOMBONI,
        CATAPULT_ZOMBIE,
        GARGANTUAR,
        GIGA_GARGANTUAR,
        BUNGEE_ZOMBIE
    };
    for(int wave = 1; wave <= 20; wave++) {
        if((wave % 10 == 0) == huge)
            continue;
        int offset = huge ? 40 : -40;
        InsertTimeOperation(0, wave, [=](){
            for(auto& zombie : alive_zombie_filter)
                if(zombie.existTime() == 0 && !excluded_types.count(zombie.type()))
                    zombie.abscissa() += offset;
        });
    }
}
