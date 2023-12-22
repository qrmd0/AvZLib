#pragma once

#include "error.h"
#include "global.h"
#include "libavz.h"
#include "util.h"

namespace _SimpleAvZInternal {

const std::set<ZombieType> AQUATIC_ZOMBIES = {SNORKEL_ZOMBIE, DOLPHIN_RIDER_ZOMBIE};
const std::set<ZombieType> INVALID_ZOMBIES = {BACKUP_DANCER, ZOMBIE_BOBSLED_TEAM, BUNGEE_ZOMBIE, IMP};

struct EnsureExistInfo {
private:
    std::set<int> rows;

public:
    ZombieType zombie_type;

    template <typename... Args>
    EnsureExistInfo(ZombieType _zombie_type, Args... args)
        : zombie_type(_zombie_type)
    {
        for (int r : {args...})
            rows.insert(r);
    }

    EnsureExistInfo(ZombieType _zombie_type)
        : zombie_type(_zombie_type)
    {
    }

    void validate() const
    {
        if (INVALID_ZOMBIES.count(zombie_type)) {
            error("EnsureExist", "Unsupported zombie type: #", zombie_type);
        }

        for (const auto& row : rows) {
            if (row < 1 || row > get_max_spawn_row()) {
                error("EnsureExist", "Zombie row should be within 1~#: #", get_max_spawn_row(), row);
            }
        }

        auto detail_str = "Zombie type: " + std::to_string(static_cast<int>(zombie_type)) + "\nZombie rows: " + concat(rows, ",");

        if (has_water_rows()) {
            if (AQUATIC_ZOMBIES.count(zombie_type)) {
                if (AvZ::GetMainObject()->scene() == 8) {
                    error("EnsureExist", "Cannot spawn snorkel or dolphin in aquarium scene\n" + detail_str);
                } else if (rows.count(3) + rows.count(4) != rows.size()) {
                    error("EnsureExist", "Cannot spawn snorkel or dolphin in non-water rows\n" + detail_str);
                }
            } else if (zombie_type != BALLOON_ZOMBIE) {
                if ((rows.count(3) || rows.count(4))) {
                    error("EnsureExist", "Setting zombies except snorkel, dolphin, or balloon in water rows is not supported yet\n" + detail_str);
                }
            }
        } else {
            if (AQUATIC_ZOMBIES.count(zombie_type)) {
                error("EnsureExist", "Cannot spawn snorkel or dolphin in non-backyard scenes\n" + detail_str);
            }
            if (no_dancing_in_side_rows()
                && zombie_type == DANCING_ZOMBIE
                && (rows.count(1) || rows.count(get_max_spawn_row()))) {
                error("EnsureExist", "Cannot spawn dancer in row 1 or 5\n" + detail_str);
            }
            if (no_zomboni() && zombie_type == ZOMBONI) {
                error("EnsureExist", "Cannot spawn zomboni in night scene\n" + detail_str);
            }
            if (is_roof()
                && (zombie_type == DANCING_ZOMBIE || zombie_type == DIGGER_ZOMBIE)) {
                error("EnsureExist", "Cannot spawn dancer or digger in roof scenes\n" + detail_str);
            }
        }
    }

    std::set<int> get_rows() const
    {
        if (!rows.empty()) {
            return rows;
        } else {
            std::set<int> new_rows;
            if (has_water_rows()) {
                new_rows = {1, 2, 3, 4, 5, 6};
                if (AQUATIC_ZOMBIES.count(zombie_type)) {
                    new_rows = {3, 4};
                } else if (zombie_type == BALLOON_ZOMBIE) {
                    new_rows = {1, 2, 3, 4, 5, 6};
                } else {
                    new_rows = {1, 2, 5, 6};
                }
            } else {
                for (int i = 1; i <= get_max_spawn_row(); i++) {
                    new_rows.insert(i);
                }
                if (no_dancing_in_side_rows() && zombie_type == DANCING_ZOMBIE) {
                    new_rows.erase(1);
                    new_rows.erase(get_max_spawn_row());
                }
            }
            return new_rows;
        }
    }
};

void move_zombie_row(Zombie* zombie, int target_row)
{
    const int BASE_Y = is_roof() ? 40 : 50;
    const int GRID_HEIGHT = is_visually_six_rows() ? 85 : 100;

    int diff = (target_row - 1) - zombie->row();
    zombie->row() = target_row - 1;
    zombie->ordinate() = static_cast<float>(BASE_Y + (target_row - 1) * GRID_HEIGHT);
    zombie->mRef<int>(0x20) += 10000 * diff; // 僵尸层数
}

} // namespace _SimpleAvZInternal

// Ensure some zombies appear in certain rows for the current wave.
// Must be used within waves() loops.
// *** Usage:
// EnsureExist(GIGA)----------------------------- Ensure giga appears in all reasonable rows
// EnsureExist({GIGA, 2, 3})--------------------- Ensure giga appears in row 2 & 3
// EnsureExist({{GIGA, 2, 3}, {ZOMBONI, 4}})----- Ensure giga appears in row 2 & 3 and zomboni appears in row 4
void EnsureExist(const std::vector<_SimpleAvZInternal::EnsureExistInfo>& ensure_exist_info_list)
{
    std::set<ZombieType> seen_zombie_types;
    for (const auto& info : ensure_exist_info_list) {
        info.validate();

        if (seen_zombie_types.count(info.zombie_type)) {
            _SimpleAvZInternal::error("EnsureExist", "Duplicate zombie type: #", info.zombie_type);
        }
        seen_zombie_types.insert(info.zombie_type);
    }

    _SimpleAvZInternal::set_time_inside(0, "EnsureExist");
    AvZ::InsertOperation([=] {
        std::vector<int> zombie_index[GIGA_GARGANTUAR + 1][6 + 1]; // 从僵尸种类与行数查询符合条件的僵尸下标

        auto zombie_array = AvZ::GetMainObject()->zombieArray();
        for (int i = 0; i < AvZ::GetMainObject()->zombieTotal(); i++) {
            auto zombie = zombie_array + i;
            if (zombie->isDead() || zombie->isDisappeared())
                continue;

            if (zombie->existTime() > 5) // 只考虑本波僵尸
                continue;

            if (_SimpleAvZInternal::has_water_rows()
                && (zombie->row() + 1 == 3 || zombie->row() + 1 == 4)
                && !_SimpleAvZInternal::AQUATIC_ZOMBIES.count(static_cast<ZombieType>(zombie->type()))
                && zombie->type() != BALLOON_ZOMBIE) // 无视水路潜水,海豚,气球以外的僵尸
                continue;

            zombie_index[zombie->type()][zombie->row() + 1].push_back(i);
        }

        for (const auto& info : ensure_exist_info_list) {
            auto zombie_type = info.zombie_type;
            auto target_rows = info.get_rows();

            for (const auto& target_row : target_rows) {
                if (!zombie_index[zombie_type][target_row].empty())
                    continue;

                // 优先寻找非移动目标行
                int best_src_row = 0;
                for (int row = 1; row <= 6; row++) {
                    if (!target_rows.count(row) && zombie_index[zombie_type][row].size() > zombie_index[zombie_type][best_src_row].size())
                        best_src_row = row;
                }

                // 如果找不到, 则从其它目标行中选择最优的
                if (zombie_index[zombie_type][best_src_row].empty()) {
                    for (const auto& row : target_rows) {
                        if (target_row != row && zombie_index[zombie_type][row].size() > zombie_index[zombie_type][best_src_row].size())
                            best_src_row = row;
                    }
                    if (zombie_index[zombie_type][best_src_row].size() <= 1) {
                        AvZ::ShowErrorNotInQueue("Not enough zombies of type # to ensure it appears in row #", zombie_type, target_row);
                        continue;
                    }
                }

                assert(!zombie_index[zombie_type][best_src_row].empty());
                auto src_index_it = _SimpleAvZInternal::select_random_elem(zombie_index[zombie_type][best_src_row]);
                _SimpleAvZInternal::move_zombie_row(zombie_array + *src_index_it, target_row);
                zombie_index[zombie_type][target_row].push_back(*src_index_it);
                zombie_index[zombie_type][best_src_row].erase(src_index_it);
            }
        }
    },
        "EnsureExist");
}

void EnsureExist(const _SimpleAvZInternal::EnsureExistInfo& ensure_exist_info)
{
    EnsureExist(std::vector<_SimpleAvZInternal::EnsureExistInfo> {ensure_exist_info});
}