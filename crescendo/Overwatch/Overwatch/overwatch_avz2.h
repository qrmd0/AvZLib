#pragma once
#include "libavz.h"
#include "macro.h"

namespace ow_priv {

struct grid_with_type {
    Grid grid;
    int type;

    grid_with_type(Plant& p)
        : grid({p.Row(), p.Col()})
        , type(p.Type()) { }

    friend bool operator==(const grid_with_type& g1, const grid_with_type& g2) {
        return g1.grid == g2.grid && g1.type == g2.type;
    }

    friend bool operator<(const grid_with_type& g1, const grid_with_type& g2) {
        return std::tie(g1.grid, g1.type) < std::tie(g2.grid, g2.type);
    }

    bool is(Plant& p) const {
        return *this == grid_with_type(p);
    }

    std::string str() const {
        std::ostringstream oss;
        oss << grid.row + 1 << "-" << grid.col + 1 << "\t";
        return oss.str();
    }
};

struct zombie_info {
    int wave;
    int row;
    int type;

    zombie_info(Zombie& z)
        : wave(z.StandState())
        , row(z.Row())
        , type(z.Type()) { }

    friend bool operator<(const zombie_info& z1, const zombie_info& z2) {
        return std::tie(z1.wave, z1.row, z1.type) < std::tie(z2.wave, z2.row, z2.type);
    }

    std::string str() const {
        static const std::vector<std::string> ZOMBIE_NAMES
            = {"普", "旗", "障", "杆", "桶", "报", "门", "橄", "舞", "伴", "鸭", "潜", "车", "橇", "豚", "丑", "气",
                "矿", "跳", "雪", "偷", "梯", "篮", "白", "鬼", "", "", "", "", "", "", "", "红"};
        std::ostringstream oss;
        oss << "w" << wave + 1 << "\t" << ZOMBIE_NAMES[type] << "\t" << row + 1 << "\t";
        return oss.str();
    }
};

struct stat {
    std::map<int, int> stat_of_wave;

    int sum() const {
        int sum = 0;
        for (const auto& pair : stat_of_wave)
            sum += pair.second;
        return sum;
    }

    void add(int wave, int by) {
        stat_of_wave[wave] += by;
    }

    int by_wave(int wave) {
        return stat_of_wave[wave];
    }
};

struct rect {
    int x;
    int y;
    int width;
    int height;

    bool get_circle_rect_overlap(int circle_x, int circle_y, int radius) {
        bool res;
        rect* rect = this;
        __asm__ __volatile__("movl %[rect], %%eax;"
                             "pushl %[radius];"
                             "movl %[circle_y], %%edx;"
                             "movl %[circle_x], %%ecx;"
                             "movl $0x41C850, %%edi;"
                             "calll *%%edi;"
                             "addl $0x4, %%esp;"
                             "movb %%al, %[res];"
            : [res] "=r"(res)
            : [rect] "m"(rect), [radius] "m"(radius), [circle_y] "m"(circle_y), [circle_x] "m"(circle_x)
            : "eax", "ecx", "edx", "edi");
        return res;
    }

    int get_rect_overlap(const rect& other) {
        int res;
        rect* rect = this;
        __asm__ __volatile__("movl %[rect], %%ecx;"
                             "movl %[other], %%edi;"
                             "movl $0x41C820, %%eax;"
                             "calll *%%eax;"
                             "movl %%eax, %[res];"
            : [res] "=r"(res)
            : [rect] "m"(rect), [other] "r"(&other)
            : "eax", "ecx", "edi");
        return res;
    }
};

rect get_plant_rect(Plant& plant) {
    rect rect;
    __asm__ __volatile__("movl %[rect], %%eax;"
                         "movl %[plant], %%ecx;"
                         "movl $0x467EF0, %%edx;"
                         "calll *%%edx;"
        :
        : [rect] "r"(&rect), [plant] "r"(&plant)
        : "eax", "ecx", "edx");
    return rect;
}

rect get_zombie_attack_rect(Zombie& zombie) {
    rect rect;
    __asm__ __volatile__("movl %[rect], %%edi;"
                         "movl %[zombie], %%ebx;"
                         "movl $0x532140, %%ecx;"
                         "calll *%%ecx;"
        :
        : [rect] "r"(&rect), [zombie] "r"(&zombie)
        : "ebx", "ecx", "edx", "edi");
    return rect;
}

struct plants_on_lawn {
    Plant* base;
    Plant* pumpkin;
    Plant* coffee_bean;
    Plant* normal;

    Plant* top_plant() const {
        if (normal)
            return normal;
        if (pumpkin)
            return pumpkin;
        if (base)
            return base;
        return nullptr;
    }
};

plants_on_lawn get_plants_on_lawn(int row, int col) {
    plants_on_lawn plants_on_lawn;
    __asm__ __volatile__("pushl %[row];"
                         "pushl %[col];"
                         "movl %[plants_on_lawn], %%ebx;"
                         "movl 0x6a9ec0, %%edx;"
                         "movl 0x768(%%edx), %%edx;"
                         "movl $0x40D2A0, %%eax;"
                         "calll *%%eax;"
        :
        : [row] "m"(row), [col] "m"(col), [plants_on_lawn] "r"(&plants_on_lawn)
        : "eax", "ebx", "ecx", "edx");
    return plants_on_lawn;
}

enum class RunningState { Running, ShuttingDown, Done };

void remove_plant(Plant& plant) {
    __asm__ __volatile__("pushl %[plant];"
                         "movl $0x4679B0, %%edx;"
                         "calll *%%edx;"
        :
        : [plant] "r"(&plant)
        : "eax", "ecx", "edx", "memory");
}

bool garg_about_to_hammer(Zombie& garg) {
    if (garg.IsHammering() && garg.FreezeCountdown() <= 0 && garg.FixationCountdown() <= 0) {
        auto animation_code = garg.MRef<uint16_t>(0x118);
        auto animation_array = GetPvzBase()->AnimationMain()->AnimationOffset()->AnimationArray();
        auto circulation_rate = animation_array[animation_code].CirculationRate();
        return std::fabs(circulation_rate - 0.644f) < 0.05f;
    } else
        return false;
}

int get_enter_home_thres(int type) {
    switch (type) {
    case GARGANTUAR:
    case GIGA_GARGANTUAR:
    case POLE_VAULTING_ZOMBIE:
        return -150;
    case CATAPULT_ZOMBIE:
    case FOOTBALL_ZOMBIE:
    case ZOMBONI:
        return -175;
    case BACKUP_DANCER:
    case DANCING_ZOMBIE:
    case SNORKEL_ZOMBIE:
        return -130;
    default:
        return -100;
    }
}

bool zombie_about_to_enter_home(Zombie& z) {
    return static_cast<int>(z.Abscissa()) <= get_enter_home_thres(z.Type()) + 5;
}

std::string get_prev_wave_stat(int prev_wave_num) {
    int w = NowWave();;
    std::ostringstream oss;
    oss << "w" << w << " 至" << NowTime(w) << "cs\n";
    for (int i = 1; i < prev_wave_num; i++) {
        w--;
        if (w < 1)
            break;
        oss << "w" << w << " 共" << NowTime(w) - NowTime(w + 1) << "cs\n";
    }
    return oss.str();
}

}; // namespace ow_priv

class overwatch : public TickRunner {
    using grid_with_type = ow_priv::grid_with_type;
    using zombie_info = ow_priv::zombie_info;
    using stat = ow_priv::stat;
    using RunningState = ow_priv::RunningState;

private:
    RunningState running_state = RunningState::Running;
    std::set<grid_with_type> watched_plants;
    std::set<grid_with_type> safe_plants;
    std::map<grid_with_type, stat> explode_stats;
    std::map<grid_with_type, stat> smash_stats;
    std::map<grid_with_type, stat> hp_stats;
    std::vector<zombie_info> enter_home_records;
    const int INIT_HP = 1'000'000;
    int shutting_down_time = -1;
    bool stop_when_zombie_enter_home = false;

    int check_enter_home(Zombie& z) {
        using namespace ow_priv;
        if (zombie_about_to_enter_home(z)) {
            enter_home_records.push_back(zombie_info(z));
            if (stop_when_zombie_enter_home) {
                GetInternalLogger()->Error("僵尸进家: " + zombie_info(z).str());
                stop_run();
                return 1;
            }
            z.State() = 3;
        }
        return 0;
    }

    void check_jack(Zombie& jack) {
        using namespace ow_priv;
        if (jack.StateCountdown() == 1 && jack.State() == 16) {
            std::vector<Plant*> plants_to_kill;
            for (auto& p : alivePlantFilter)
                if (get_plant_rect(p).get_circle_rect_overlap(jack.Abscissa() + 60, jack.Ordinate() + 60, 90)) {
                    if (watched_plants.count(grid_with_type(p)))
                        explode_stats[grid_with_type(p)].add(jack.StandState(), 1);
                    else
                        plants_to_kill.push_back(&p);
                }
            for (auto p : plants_to_kill)
                remove_plant(*p);
            jack.State() = 3;
        }
    }

    void check_garg(Zombie& garg, std::set<AGrid>& grids_about_to_be_smashed) {
        using namespace ow_priv;
        if (garg_about_to_hammer(garg)) {
            auto zombie_attack_rect = get_zombie_attack_rect(garg);
            for (auto& p : alivePlantFilter) {
                if (garg.Row() == p.Row() && zombie_attack_rect.get_rect_overlap(get_plant_rect(p)) >= 20
                    && !grids_about_to_be_smashed.count({p.Row(), p.Col()})) {
                    if (watched_plants.count(grid_with_type(p))) {
                        smash_stats[grid_with_type(p)].add(AGetMainObject()->Wave() - 1, 1);
                        garg.State() = 3;
                    } else
                        grids_about_to_be_smashed.insert({p.Row(), p.Col()});
                    break;
                }
            }
        }
    }

    void restore_health() {
        for (auto& p : alivePlantFilter) {
            if (watched_plants.count(grid_with_type(p)) && p.Hp() && p.Hp() < INIT_HP) {
                hp_stats[grid_with_type(p)].add(AGetMainObject()->Wave() - 2, INIT_HP - p.Hp());
                p.Hp() = INIT_HP;
            }
        }
    }

    void stop_run() {
        using namespace ow_priv;
        GetInternalLogger()->Debug(get_prev_wave_stat(3));
        running_state = RunningState::ShuttingDown;
        shutting_down_time = NowTime(0);
    }

    void run() {
        using namespace ow_priv;
        if (running_state != RunningState::Running) {
            if (running_state == RunningState::ShuttingDown && NowTime(0) >= shutting_down_time + 2) {
                running_state = RunningState::Done;
                SetAdvancedPause(true);
            }
            return;
        }

        std::set<AGrid> grids_about_to_be_smashed;
        for (auto& z : aliveZombieFilter) {
            if (check_enter_home(z))
                return;
            if (z.Type() == JACK_IN_THE_BOX_ZOMBIE)
                check_jack(z);
            else if (z.Type() == GARGANTUAR || z.Type() == GIGA_GARGANTUAR)
                check_garg(z, grids_about_to_be_smashed);
        }
        for (auto& p : alivePlantFilter) {
            if (safe_plants.count(grid_with_type(p)) && p.Hp() != p.HpMax()) {
                GetInternalLogger()->Error("#-# 安全位受损, hp=#", p.Row() + 1, p.Col() + 1, p.Hp());
                stop_run();
                return;
            }
        }
        if (NowTime(AGetMainObject()->Wave()) == 0)
            restore_health();
    }

public:
    // watched_grids: 需测定的炮，这些炮会进入“无敌”状态，无视任何炸/砸/啃，但是会统计次数
    // safe_grids: 不应该被伤害的炮。默认为测定炮外的所有炮。这些炮受伤时，暂停测试
    // stop_when_zombie_enter_home: 僵尸进家时是否暂停测试，默认为false
    void start(const std::set<AGrid>& watched_grids, const std::set<AGrid>& safe_grids = {},
        bool stop_when_zombie_enter_home = false) {
        using namespace ow_priv;

        watched_plants.clear();
        safe_plants.clear();
        this->stop_when_zombie_enter_home = stop_when_zombie_enter_home;

        auto insert_grids = [](const auto& grids, std::set<grid_with_type>& plants, const std::string& tag,
                                const std::set<grid_with_type>& existing_plants = {}) {
            for (const auto& grid : grids) {
                auto top_plant = get_plants_on_lawn(grid.row - 1, grid.col - 1).top_plant();
                if (!top_plant) {
                    GetInternalLogger()->Error(
                        "指定的" + tag + "位 #-# 无植物, 请检查传给 start() 的参数", grid.row, grid.col);
                    return 1;
                }
                auto insert_result = plants.insert(grid_with_type(*top_plant));
                if (!insert_result.second || existing_plants.count(grid_with_type(*top_plant))) {
                    GetInternalLogger()->Error(
                        "指定的" + tag + "位 #-# 与其它观察位/安全位重复, 请检查传给 start() 的参数", grid.row,
                        grid.col);
                    return 1;
                }
            }
            return 0;
        };
        if (insert_grids(watched_grids, watched_plants, "观察"))
            return;
        if (safe_grids.empty()) {
            for (auto& p : alivePlantFilter)
                if (p.Type() == COB_CANNON && !watched_plants.count(grid_with_type(p)))
                    safe_plants.insert(grid_with_type(p));
        } else {
            insert_grids(safe_grids, safe_plants, "安全", watched_plants);
        }

        for (auto& p : alivePlantFilter)
            if (watched_plants.count(grid_with_type(p)))
                p.Hp() = p.HpMax() = INIT_HP;


        Start([this] { run(); });
    }

    // 若使用跳帧，请在跳帧条件中调用此函数（若keep_going为false，应停止跳帧）
    bool keep_going() const {
        return running_state == RunningState::Running;
    }

    // 输出测定炮的炸/砸/啃统计
    void dump_stats() {
        std::ostringstream oss;
        for (int wave = 0; wave < 20; wave++) {
            std::ostringstream local_oss;
            for (auto pos : watched_plants) {
                auto explode_count = explode_stats[pos].by_wave(wave), smash_count = smash_stats[pos].by_wave(wave),
                     hp_loss = hp_stats[pos].by_wave(wave);
                if (explode_count + smash_count + hp_loss > 0) {
                    local_oss << pos.str() << explode_count << "\t" << smash_count << "\t" << hp_loss << "\t"
                              << (explode_count + smash_count) * 300 + hp_loss << "\n";
                }
            }
            if (!local_oss.str().empty())
                oss << "\n== 第 " << wave + 1 << " 波==\n位置\t被炸\t被锤\t炮损\t炮伤\n" << local_oss.str();
        }
        oss << "\n== 总炮伤 ==\n位置\t被炸\t被锤\t炮损\t炮伤\n";
        for (auto pos : watched_plants) {
            auto explode_count = explode_stats[pos].sum(), smash_count = smash_stats[pos].sum(),
                 hp_loss = hp_stats[pos].sum();
            oss << pos.str() << explode_count << "\t" << smash_count << "\t" << hp_loss << "\t"
                << (explode_count + smash_count) * 300 + hp_loss << "\n";
        }
        if (!enter_home_records.empty()) {
            std::sort(enter_home_records.begin(), enter_home_records.end());
            oss << "\n== 僵尸进家 ==\n波数\t种类\t行数\n";
            for (auto zombie_info : enter_home_records)
                oss << zombie_info.str() << "\n";
        }
        oss << "注：巨人砸炮按砸炮波数显示\n";
        GetInternalLogger()->Debug(oss.str());
    }
};