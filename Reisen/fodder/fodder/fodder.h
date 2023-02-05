/*
MIT License

Copyright (c) 2022-2023 Reisen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef _REISEN_FODDER_FODDER_H
#define _REISEN_FODDER_FODDER_H
#include <algorithm>
#include <iterator>
#include <map>
#include <utility>
#include <vector>
#include "avz.h"

class Fodder {
private:
    std::vector<APlantType> cards;

    static bool CheckUsable(const APlantType p) {
        int idx = AGetCardIndex(p);
        if(idx == -1) {
            __aInternalGlobal.loggerPtr->Error("未携带卡�?#", p);
            return false;
        }
        return AGetMainObject()->SeedArray()[idx].IsUsable();
    }

public:
    Fodder(const std::vector<APlantType>& c = {}) : cards(c) {}

    void SetCards(const std::vector<APlantType>& c) { cards = c; }

    void operator()(std::vector<int> rows = {}, int removal_delay = 266, int col = 9, bool smart = true) const {
        if(rows.empty())
            rows = ARangeIn(AGetMainObject()->Scene(), {2, 3}) ?
                std::vector<int>{1, 2, 5, 6} : std::vector<int>{1, 2, 3, 4, 5};
        if(smart) {
            std::map<int, std::pair<int, int>> num_zombies;
            for(int r : rows)
                num_zombies[r] = {0, 0};
            for(auto& z : aAliveZombieFilter)
                if(num_zombies.count(z.Row() + 1))
                    if(z.Type() == ALADDER_ZOMBIE)
                        num_zombies[z.Row() + 1].first--;
                    else if(z.Type() == AJACK_IN_THE_BOX_ZOMBIE)
                        num_zombies[z.Row() + 1].second--;
            std::vector<std::pair<std::pair<int, int>, int>> priority;
            for(auto t : num_zombies)
                priority.push_back({t.second, t.first});
            std::sort(priority.begin(), priority.end());
            rows.clear();
            for(auto t : priority)
                rows.push_back(t.second);
        }
        auto cur = cards.begin();
        for(auto r : rows) {
            while(cur != cards.end() && !CheckUsable(*cur))
                cur++;
            if(cur == cards.end()) {
                __aInternalGlobal.loggerPtr->Error("无可用垫�?);
                break;
            }
            auto p = *cur;
            ACard(p, r, col);
            if(removal_delay != -1)
                AConnect(ANowDelayTime(removal_delay), [=]{
                    if(AGetPlantIndex(r, col, p % 49) >= 0)
                        AShovel(r, col, p % 49 == APUMPKIN);
                    else if(p >= 49 && AGetPlantIndex(r, col, AIMITATOR) >= 0)
                        AShovel(r, col, false);
                });
        }
    }
};

class FodderR {
private:
    Fodder f;

public:
    FodderR(const std::vector<APlantType>& c = {}) : f(c) {}

    [[nodiscard("ARelOp 需要绑定到时间才会执行")]]
    ARelOp SetCards(const std::vector<APlantType>& c) {
        return ARelOp([=](){ f.SetCards(c); });
    }

    [[nodiscard("ARelOp 需要绑定到时间才会执行")]]
    ARelOp operator()(const std::vector<int>& rows = {}, int removal_delay = 266, int col = 9, bool smart = true) const {
        return ARelOp([=](){ f(rows, removal_delay, col, smart); });
    }
};
#endif
