#include "refresh/refresh.h"

auto base = TaskBuilder()
    .output_folder("C:\\ProgramData\\PopCap Games\\PlantsVsZombies\\refresh\\无炮")
    .total(1000)
    .huge(false)
    .dance_cheat(false);

vector<Task> get_tasks() {
    vector<Task> tasks;
    tasks.push_back(base
        .prefix("两仪-变-2500")
        .required_types({GIGA_GARGANTUAR, JACK_IN_THE_BOX_ZOMBIE})
        .banned_types({GARGANTUAR})
        .operation([](int wave){
            SetTime(783 - 100); Card(ICE_SHROOM, 1, 1);
        })
        .assume_refresh(false)
        .check_time(2300)
    );
    tasks.push_back(base
        .prefix("两仪-变-2500-blover")
        .required_types({GIGA_GARGANTUAR, JACK_IN_THE_BOX_ZOMBIE})
        .banned_types({GARGANTUAR})
        .operation([](int wave){
            SetTime(783 - 100); Card(ICE_SHROOM, 1, 1);
            Card(BLOVER, 6, 1);
        })
        .assume_refresh(false)
        .check_time(2300)
    );
    tasks.push_back(base
        .prefix("电波钟-变-700")
        .required_types({GIGA_GARGANTUAR, JACK_IN_THE_BOX_ZOMBIE, ZOMBONI})
        .banned_types({GARGANTUAR})
        .giga_count(0)
        .operation([](int wave){
            SetTime(299 - 100); Card(ICE_SHROOM, 1, 6);
            if(RangeIn(wave, {9, 19, 20}))
                killAllZombie({wave}, {}, 501);
        })
        .assume_refresh(false)
        .clear_zombies(false)
        .check_time(500)
    );
    tasks.push_back(base
        .prefix("电波钟-极-700")
        .required_types({JACK_IN_THE_BOX_ZOMBIE, ZOMBONI})
        .banned_types({GIGA_GARGANTUAR, GARGANTUAR})
        .operation([](int wave){
            SetTime(299 - 100); Card(ICE_SHROOM, 1, 6);
            if(RangeIn(wave, {9, 19, 20}))
                killAllZombie({wave}, {}, 501);
        })
        .assume_refresh(false)
        .clear_zombies(false)
        .check_time(500)
    );
    tasks.push_back(base
        .prefix("电波钟-无车-变-700")
        .required_types({GIGA_GARGANTUAR, JACK_IN_THE_BOX_ZOMBIE})
        .banned_types({GARGANTUAR, ZOMBONI})
        .giga_count(0)
        .operation([](int wave){
            SetTime(299 - 100); Card(ICE_SHROOM, 1, 6);
            if(RangeIn(wave, {9, 19, 20}))
                killAllZombie({wave}, {}, 501);
        })
        .assume_refresh(false)
        .clear_zombies(false)
        .check_time(500)
    );
    tasks.push_back(base
        .prefix("电波钟-无车-极-700")
        .required_types({JACK_IN_THE_BOX_ZOMBIE})
        .banned_types({GIGA_GARGANTUAR, GARGANTUAR, ZOMBONI})
        .operation([](int wave){
            SetTime(299 - 100); Card(ICE_SHROOM, 1, 6);
            if(RangeIn(wave, {9, 19, 20}))
                killAllZombie({wave}, {}, 501);
        })
        .assume_refresh(false)
        .clear_zombies(false)
        .check_time(500)
    );
    return tasks;
}
