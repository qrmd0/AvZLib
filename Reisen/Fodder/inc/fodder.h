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

#ifndef REISEN_FODDER_FODDER_H
#define REISEN_FODDER_FODDER_H
#include <algorithm>
#include <iterator>
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include "avz.h"

using namespace AvZ;

class Fodder {
    private:
    std::vector<PlantType> cards;

    static std::vector<int> cards_to_id(const std::vector<PlantType>& c) {
        std::vector<int> fodder_cards;
        for(auto p : c) {
            bool is_imitator = (p >= M_PEASHOOTER);
            auto x = GetSeedIndex(is_imitator ? p - M_PEASHOOTER : p, is_imitator);
            if(x != -1)
                fodder_cards.push_back(x);
        }
        return fodder_cards;
    }

    public:
    Fodder(const std::vector<PlantType>& c = {}) : cards(c) {}

    void setCards(const std::vector<PlantType>& c) { cards = c; }

    void operator()(std::vector<int> rows = {}, int removal_delay = 266, int col = 9, bool smart = true) const {
        if(rows.empty()) {
            int scene = GetMainObject()->scene();
            rows = (scene == 2 || scene == 3) ? std::vector<int>{1, 2, 5, 6} :
                std::vector<int>{1, 2, 3, 4, 5};
        }
        auto c = std::make_shared<std::vector<PlantType>>(cards);
        InsertOperation([=](){
            auto fodder_cards = cards_to_id(*c);
            std::map<int, std::pair<int, int>> priority_;
            for(int r : rows)
                priority_[r] = {0, 0};
            for(auto z : AliveFilter<Zombie>([=](auto z){return priority_.count(z->row() + 1);}))
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
} fodder;
#endif
