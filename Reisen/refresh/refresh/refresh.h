#include <numeric>
#include "avz.h"
#include "avz_testing.h"
#include "DanceCheat/DanceCheat.h"
#include "mod/mod.h"
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
int initial_total_hp[21];

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

void initialize_task() {
    progress = 0;
    refresh_data.clear();
    sort(cur_task->check_time.begin(), cur_task->check_time.end());
    for(int i = 0; i < cur_task->check_time.size(); i++) {
        RefreshData rd;
        rd.result = rd.sum = 0;
        memset(rd.hist, 0, sizeof(rd.hist));
        refresh_data.push_back(rd);
    }
}

void Script() {
    if(first_run) {
        first_run = false;
        start_time = clock();
        all_tasks = get_tasks();
        cur_task = all_tasks.begin();
        initialize_task();
        extract_exe();
    }
    if(progress == cur_task->total) {
        vector<string> idents;
        for(int i = 0; i < cur_task->check_time.size(); i++)
            if(cur_task->check_time.size() != 1)
                idents.push_back("(" + to_string(cur_task->check_time[i]) + ")");
            else
                idents.push_back("");
        merge_csv(idents);
        cur_task++;
        if(cur_task == all_tasks.end()) {
            double elapsed = 1.0 * (clock() - start_time) / CLOCKS_PER_SEC;
            ShowErrorNotInQueue("完成\n用时：#s", int(elapsed + 0.5));
            return;
        }
        initialize_task();
    }

    EnableModsScoped(SaveDataReadOnly, FreePlantingCheat, PlantAnywhere, CobInstantRecharge,
        DisableItemDrop, PlantInvincible, DisableSpecialAttack, CobFixedDelay);
    OpenMultipleEffective('Q', MAIN_UI_OR_FIGHT_UI);
    if(!cur_task->debug)
        SkipTick([](){ return true; });
    SelectCardsPlus(cur_task->card_selection);

    type_list = random_type_list(cur_task->required_types, cur_task->banned_types);
    simulate_summon(type_list, 1000, cur_task->huge ? ALL_HUGE : ALL_NORMAL, cur_task->giga_count);
    unifyAllWaves(cur_task->huge ? BIG_WAVE : NORMAL_WAVE);
    SetTime(-599, 1); pao_operator.autoGetPaoList();
    if(cur_task->dance_cheat) {
        DanceCheat(cur_task->assume_refresh ? DanceCheatMode::FAST : DanceCheatMode::SLOW);
        if(cur_task->assume_refresh)
            MaidCheats::move();
        else
            MaidCheats::dancing();
    }

    int last_time = cur_task->check_time[cur_task->check_time.size() - 1];
    for(int wave = 1; wave <= 20; wave++) {
        InsertTimeOperation(1, wave, [=](){
            initial_total_hp[wave] = GetMainObject()->mRef<int>(0x5598);
            if(cur_task->clear_zombies)
                GetMainObject()->zombieRefreshHp() = 0;
        });
        SetTime(0, wave); cur_task->operation(wave);
        for(int i = 0; i < cur_task->check_time.size(); i++)
            InsertTimeOperation(cur_task->check_time[i], wave, [=](){
                auto& rd = refresh_data[i];
                int hp = 0;
                rd.left_count[wave].clear();
                for(auto& z : alive_zombie_filter)
                    if(z.standState() == wave - 1 && !z.mRef<bool>(0xb8)) {
                        if(!RangeIn(z.type(), {BACKUP_DANCER, BUNGEE_ZOMBIE}))
                            hp += z.hp() + z.oneHp() + z.twoHp() / 5 + z.mRef<int>(0xe4);
                        int type = z.type();
                        if(RangeIn(z.type(), {GARGANTUAR, GIGA_GARGANTUAR}))
                            type = z.type() * 10 + ceil(z.hp() / 1800.0);
                        rd.left_count[wave][type]++;
                    }
                int hp2 = WaveTotalHp(wave);
                if(hp != hp2) {
                    ShowErrorNotInQueue("计算总血量 = #\n实际总血量 = #", hp, hp2);
                    throw Exception("");
                }
                rd.hp_ratio[wave] = 1.0 * hp / initial_total_hp[wave];
                double prob = (0.65 - min(max(rd.hp_ratio[wave], 0.5), 0.65)) / 0.15;
                rd.wave_prob[wave] = cur_task->assume_refresh ? 1 - prob : prob;
            });
        if(cur_task->clear_zombies || RangeIn(wave, {9, 19, 20}))
            killAllZombie({wave}, {}, last_time);
    }

    InsertTimeOperation(0, 20, [](){ progress = refresh_data[0].ranking.size() + 1; });
    for(int i = 0; i < cur_task->check_time.size(); i++)
        InsertTimeOperation(cur_task->check_time[i], 20, [=](){
            auto& rd = refresh_data[i];
            rd.result = accumulate(rd.wave_prob + 1, rd.wave_prob + 21, 0.0);
            rd.sum += rd.result;
            rd.hist[int(ceil(rd.result * 2))]++;
            string ident = "(" + to_string(cur_task->check_time[i]) + ")";
            if(cur_task->check_time.size() == 1)
                ident = "";
            generate_stats(rd, ident);
            generate_data(rd, ident);
            generate_raw(rd, ident);
        });
}
