/*
MIT License

Copyright (c) 2022 Reisen

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

#pragma once
#include <algorithm>
#include <iterator>
#include <map>
#include <utility>
#include <vector>
#include "avz.h"

using namespace AvZ;

class ZombieIterator : public std::iterator<std::forward_iterator_tag, SafePtr<Zombie>> {
    private:
    SafePtr<Zombie> cur;

    ZombieIterator(SafePtr<Zombie> cur_) : cur(cur_) {}

    void forward() {
        ++cur;
        while(cur != GetMainObject()->zombieArray() + GetMainObject()->zombieTotal() &&
            (cur->isDead() || cur->isDisappeared()))
            ++cur;
    }

    public:
    ZombieIterator() {
        cur = GetMainObject()->zombieArray();
        if(cur->isDead() || cur->isDisappeared())
            forward();
    }

    SafePtr<Zombie> operator*() const {
        return cur;
    }

    SafePtr<Zombie> operator++() {
        forward();
        return cur;
    }

    SafePtr<Zombie> operator++(int) {
        auto tmp = cur;
        forward();
        return tmp;
    }

    bool operator==(ZombieIterator& rhs) { 
        return cur.toUnsafe() == rhs.cur.toUnsafe();
    }

    bool operator!=(ZombieIterator& rhs) { 
        return !(*this == rhs);
    }

    static ZombieIterator begin() {
        return ZombieIterator();
    }

    static ZombieIterator end() {
        return ZombieIterator(GetMainObject()->zombieArray() + GetMainObject()->zombieTotal());
    }
};

class Fodder {
    private:
    std::vector<int> fodder_cards;

    public:
    // 构造垫材类。输入为垫材卡片类型，优先级高的（比如有100cs免疫啃食的花盆）在前。
    Fodder(const std::vector<PlantType>& cards) {
        InsertOperation([=](){
            fodder_cards.resize(fodder_cards.size());
            std::transform(cards.begin(), cards.end(), fodder_cards.begin(), [](auto p){
                bool is_imitator = (p >= M_PEASHOOTER);
                return GetSeedIndex(is_imitator ? p - M_PEASHOOTER : p, is_imitator);
            });
        });
    }

    // In Queue
    // 放垫。
    // rows: 用垫的行（{}代表所有陆地行）
    // removal_delay: 铲除垫材的延迟。默认值266为减速巨人锤击生效时间。
    // col: 垫材放置列数。
    // smart: 是否根据快速僵尸数量调整顺序（为false则严格按顺序放置）
    void operator()(std::vector<int> rows = {}, int removal_delay = 266, int col = 9, bool smart = true) {
        if(rows.empty()) {
            int scene = GetMainObject()->scene();
            rows = (scene == 2 || scene == 3) ? std::vector<int>{1, 2, 5, 6} : 
                std::vector<int>{1, 2, 3, 4, 5};
        }
        InsertOperation([=](){
            std::map<int, std::pair<int, int>> priority_;
            for(int r : rows)
                priority_[r] = {0, 0};
            for(auto z : ZombieIterator())
                if(priority_.count(z->row() + 1))
                    if(z->type() == LADDER_ZOMBIE)
                        priority_[z->row() + 1].first--;
                    else if(z->type() == JACK_IN_THE_BOX_ZOMBIE || z->type() == FOOTBALL_ZOMBIE)
                        priority_[z->row() + 1].second--;
            std::vector<std::pair<std::pair<int, int>, int>> priority;
            for(auto t : priority_)
                priority.push_back({t.second, t.first});
            if(smart)
                sort(priority.begin(), priority.end());
            auto sa = GetMainObject()->seedArray();
            auto cur = fodder_cards.begin();
            for(auto t : priority) {
                int r = t.second;
                while(cur != fodder_cards.end() && !sa[*cur].isUsable())
                    cur++;
                if(cur == fodder_cards.end())
                    ShowErrorNotInQueue("无可用垫材");
                else {
                    CardNotInQueue(*cur + 1, r, col);
                    if(removal_delay != -1) {
                        SetDelayTime(removal_delay); 
                        Shovel(r, col);
                    }
                }
            }
        });
    }
};
