#include "avz.h"
#include "avz_testing.h"
#include "DanceCheat/DanceCheat.h"
#include "SelectCardsPlus/SelectCardsPlus.h"

#include "merge_csv.h"
#include "global_vars.h"
#include "io.h"
#include "summon_simulate.h"
#include "task.h"

using namespace std;
using namespace AvZ;
using namespace cresc;

vector<Task> get_tasks();

bool first_run = true;
clock_t start_time;
vector<Task> all_tasks;

void Script() {
    if(first_run) {
        first_run = false;
        start_time = clock();
        all_tasks = get_tasks();
        cur_task = all_tasks.begin();
        extract_exe();
    }
    if(progress == cur_task->total) {
        merge_csv();
        cur_task++;
        double elapsed = 1.0 * (clock() - start_time) / CLOCKS_PER_SEC;
        if(cur_task == all_tasks.end()) {
            ShowErrorNotInQueue("完成\n用时：#s", int(elapsed + 0.5));
            return;
        }
        progress = sum = 0;
        memset(hist, 0, sizeof(hist));
        ranking.clear();
    }

    OpenMultipleEffective('Q', MAIN_UI_OR_FIGHT_UI);
    if(!cur_task->debug)
        SkipTick([](){ return true; });
    SelectCardsPlus(cur_task->card_selection);
    type_list = random_type_list(cur_task->required_types, cur_task->banned_types);
    simulate_summon(type_list, 1000, cur_task->huge ? ALL_HUGE : ALL_NORMAL, cur_task->giga_count);
    cobCannonNoCD();
    forbidItemDrop();
    freePlanting();
    plantInvincible();
    forbid3zGroup();
    forbidFireDelay();
    unifyAllWaves(cur_task->huge ? BIG_WAVE : NORMAL_WAVE);
    WriteMemory<uint8_t>(0x2e, 0x482149);
    WriteMemory<uint8_t>(0x70, 0x54b267);
    SetTime(-599, 1); pao_operator.autoGetPaoList();
    if(cur_task->dance_cheat) {
        DanceCheat(cur_task->assume_refresh ? DanceCheatMode::FAST : DanceCheatMode::SLOW);
        if(cur_task->assume_refresh)
            MaidCheats::move();
        else
            MaidCheats::dancing();
    }

    for(int wave = 1; wave <= 20; ++wave) {
        SetTime(0, wave); cur_task->operation(wave);
        if(cur_task->clear_zombies)
            InsertTimeOperation(1, wave, [](){ GetMainObject()->zombieRefreshHp() = 0; });
        InsertTimeOperation(cur_task->check_time, wave, [=](){
            int hp = 0;
            left_count[wave].clear();
            for(auto& z : alive_zombie_filter)
                if(z.standState() == wave - 1) {
                    if(!RangeIn(z.type(), {BACKUP_DANCER, BUNGEE_ZOMBIE}))
                        hp += z.hp() + z.oneHp() + z.twoHp() / 5 + z.mRef<int>(0xe4);
                    int type = z.type();
                    if(RangeIn(z.type(), {GARGANTUAR, GIGA_GARGANTUAR}))
                        type = z.type() * 10 + ceil(z.hp() / 1800.0);
                    left_count[wave][type]++;
                }
            int total_hp = GetMainObject()->mRef<int>(0x5598);
            hp_ratio[wave] = 1.0 * hp / total_hp;
            double prob = (0.65 - min(max(hp_ratio[wave], 0.5), 0.65)) / 0.15;
            wave_prob[wave] = cur_task->assume_refresh ? 1 - prob : prob;
        });
        if(cur_task->clear_zombies)
            killAllZombie({wave}, {}, cur_task->check_time);
    }

    InsertTimeOperation(cur_task->check_time, 20, [](){
        result = 0;
        for(int wave = 1; wave <= 20; wave++)
            result += wave_prob[wave];
        sum += result;
        hist[int(ceil(result * 2))]++;
        progress++;
        generate_stats();
        generate_data();
        generate_raw();
    });
}
