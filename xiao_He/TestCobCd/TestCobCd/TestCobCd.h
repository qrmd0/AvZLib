/*
MIT License

Copyright (c) 2022-2024 xiao_He

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

/*
version 1.0.0
2024.12.15
*/

#ifndef __TEST_COB_CD_H__
#define __TEST_COB_CD_H__

#include <avz.h>
#include <vector>
using namespace AvZ;
AvZ::TickRunner tick_runner;

struct CobInfo
{
    int id;
    int row;
    int col;
    int shoot_time;
    int last_state;
};
std::vector<CobInfo> Cobs;

enum state
{
    UNARMED = 35, // 冷却中
    CHARGING,     // 填充
    READY,        // 待发射
    SHOOTING,     // 发射
};

auto function()
{
    for (auto &&plant : AvZ::alive_plant_filter)
    {
        if (plant.type() == COB_CANNON)
        {
            int id = plant.mRef<int>(0x148);
            int cob_num = Cobs.size();
            if (cob_num > 0)
            {
                bool if_exist = 0;
                for (int i = 0; i < cob_num; i++)
                {
                    if (id == Cobs[i].id)
                    {
                        // 在容器Cobs中找到对应炮
                        if_exist = 1;
                        int state = plant.state();
                        if (Cobs[i].last_state == READY && state == SHOOTING)
                        {
                            Cobs[i].shoot_time = GetMainObject()->gameClock();
                        }
                        else if (Cobs[i].last_state == CHARGING && state == READY)
                        {
                            if (Cobs[i].shoot_time != -1)
                            {
                                int cd = GetMainObject()->gameClock() - Cobs[i].shoot_time;
                                ShowErrorNotInQueue("编号为#、位于(#,#)的炮的冷却为#cs。", (short int)id, Cobs[i].row + 1, Cobs[i].col + 1, cd);
                                Cobs[i].shoot_time = -1;
                            }
                        }
                        Cobs[i].last_state = plant.state();
                    }
                }

                if (if_exist == 0)
                {
                    // 容器Cobs中未找到对应炮
                    Cobs.push_back({id, plant.row(), plant.col(), -1, -1});
                }
            }
            else
            {
                // 容器Cobs为空
                Cobs.push_back({id, plant.row(), plant.col(), -1, -1});
            }
        }
    }
};

void TestCobCd()
{
    Cobs.clear();
    tick_runner.pushFunc(function);
}

#endif