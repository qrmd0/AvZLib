#ifndef _CRESC_AVERAGE_SPAWN_H
#define _CRESC_AVERAGE_SPAWN_H

#include "libavz.h"

namespace _CrescAverageSpawn {

struct _Zombie : public Zombie {
    int& layer() { return (int&)((uint8_t*)this)[0x20]; }
};

} // namespace _CrescAverageSpawn

// *** Not In Queue
// 平均分配指定种类僵尸所在行数
// 默认只平均分配红眼
// *** 注意：只能读取DE、NE、PE、FE、RE五大场景，无法辨认的场景默认是ME
// *** 使用示例
// AverageSpawn()-------------------------------不填参数，平均分配每波红眼
// AverageSpawn({POGO_ZOMBIE, ZOMBONI})---------平均分配每波跳跳和冰车
// AverageSpawn({GIGA_GARGANTUAR}, true)--------仅在收尾波(w9, w19)平均分配红眼
// AverageSpawn({GIGA_GARGANTUAR}, true, 1)-----仅在收尾波平均分配红眼，从1路起顺次分配
void AverageSpawn(const std::vector<ZombieType>& lst = {GIGA_GARGANTUAR},
    bool onlyEndingWave = false, int start_row = -1)
{
    int scene = AvZ::GetMainObject()->scene();
    std::vector<int> zombieRow = {0, 1, 2, 3, 4};
    int height = 85;
    if (scene == 2 || scene == 3) {
        zombieRow = {0, 1, 4, 5};
    }
    if (scene == 0 || scene == 1) {
        height = 100;
    }
    int baseY = 40;
    if (scene == 0 || scene == 1 || scene == 2 || scene == 3) {
        baseY = 50;
    }

    for (int wave = 1; wave <= 20; ++wave) {
        if (onlyEndingWave && (wave != 9 && wave != 19))
            continue;
        AvZ::InsertTimeOperation(
            0, wave,
            [=]() {
                auto zombie = (_CrescAverageSpawn::_Zombie*)AvZ::GetMainObject()->zombieArray();
                int zombies_count_max = AvZ::GetMainObject()->zombieTotal();
                const int ZOMBIE_TYPE_NUM = 33;
                int spawnRow[ZOMBIE_TYPE_NUM];
                for (int i = 0; i < ZOMBIE_TYPE_NUM; ++i) {
                    if (std::find(zombieRow.begin(), zombieRow.end(), start_row - 1) == zombieRow.end()) {
                        spawnRow[i] = rand() % (zombieRow.size());
                    } else {
                        spawnRow[i] = std::find(zombieRow.begin(), zombieRow.end(), start_row - 1) - zombieRow.begin();
                    }
                }
                for (int i = 0; i < zombies_count_max; ++i, ++zombie) {
                    if (zombie->isDead() || zombie->isDisappeared())
                        continue;

                    // 只移动本波僵尸
                    if (zombie->existTime() > 100)
                        continue;

                    // 检测类型
                    int type = zombie->type();
                    if (std::find(lst.begin(), lst.end(), type) == lst.end())
                        continue;

                    // 不移动水路僵尸
                    int row = zombie->row();
                    if ((scene == 2 || scene == 3) && (row == 2 || row == 3)) {
                        continue;
                    }

                    // 不移动前院场地的舞王
                    if ((scene == 0 || scene == 1) && (type == DANCING_ZOMBIE)) {
                        continue;
                    }

                    if (type >= 0 && type < ZOMBIE_TYPE_NUM && spawnRow[type] >= 0 && spawnRow[type] < zombieRow.size()) {
                        int diff = zombieRow[spawnRow[type]] - row;
                        zombie->row() = zombieRow[spawnRow[type]];
                        zombie->ordinate() = baseY + zombieRow[spawnRow[type]] * height;
                        zombie->layer() += 10000 * diff;
                        spawnRow[type]++;
                        if (spawnRow[type] >= zombieRow.size()) {
                            spawnRow[type] -= zombieRow.size();
                        }
                    }
                }
            },
            "AverageSpawn");
    }
}
#endif