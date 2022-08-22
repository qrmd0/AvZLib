#pragma once
#include "avz_more.h"
#include "avz_more_autoplay.h"
#include "libavz.h"

using namespace AvZ;
using namespace cresc;

namespace cresc {

// ***In Queue
// 读取僵尸图层并显示，主要供调试用
void ReadLayer() {
    InsertOperation([=]() {
        SafePtr<ZombieAZM> zombie = (ZombieAZM *)GetMainObject()->zombieArray();
        int zombies_count_max = GetMainObject()->zombieTotal();
        for (int i = 0; i < zombies_count_max; ++i, ++zombie) {
            if (zombie->isDead() || zombie->isDisappeared()) {
                continue;
            }
            ShowErrorNotInQueue("类型# 行数# 图层#", zombie->type(),
                                zombie->row(), zombie->layer());
        }
    });
}

// ***Not In Queue
// 读取当前所有场物并显示，主要供调试用
void ShowPlace() {
    SafePtr<Place> place = ((MainObjectAZM *)GetMainObject())->placeArray();
    for (int i = 0; i < ((MainObjectAZM *)GetMainObject())->placeTotal();
         ++i, ++place) {
        if (!place->isDisappeared())
            ShowErrorNotInQueue("#, #, #, #", place->row(), place->col(),
                                place->type(), place->countdown());
    }
}

void ShowGridCD() {
    for (auto row : {1, 2, 5, 6})
        ShowErrorNotInQueue("| # | # | # |", GridCD(row, 6), GridCD(row, 7), GridCD(row, 8));
}
}  // namespace cresc
