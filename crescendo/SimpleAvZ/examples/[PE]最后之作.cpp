/*
 * 脚本作者: Crescendo(https://space.bilibili.com/8252252), 2023
 * 可自由复制, 转载, 修改本脚本, 但请注明出处. 不可从源代码中删去本段落.
 *
 * 阵型名: [PE]最后之作
 * 布阵码: LI43bJyUlFSWYjTGVZ5Xtc0kRWQkDF3QU0kk3TS+lmRNVPidRIlW
 * 参考视频: https://www.bilibili.com/video/BV1Uo4y1f7Fa
 * 说明: 原阵型作者光锥维, 优化ch4解作者ng收割机
 */

#include "SimpleAvZ/lib.h" // v1.0.0
#include "avz.h"           // 22_10_01

using namespace AvZ;

int leng1 = 1550;
int leng2 = 1887;

void Script()
{
    OpenMultipleEffective('C');
    SetErrorMode(CONSOLE);
    SetZombies({PJ_0, LZ_2, CG_3, WW_8, BC_12, HT_14, XC_15, QQ_16, KG_17, TT_18, FT_21, BY_23, HY_32});
    SelectCards({ICE, M_ICE, COFFEE, LILY, DOOM, CHERRY, PUFF, POT, SNOW_PEA, WALL_NUT});

    // ND/NSA
    for (auto w : waves(1)) {
        C(249, {DOOM, COFFEE}, 3, 9);
        P(249, 5, 9);
        D(after(110), 1, 7.8);
        C(now, CHERRY, 5, 9);

        C(-599, LILY, 3, 9);
        C(after(751), LILY, 3, 3);
        SetIce(now, {{1, 5}, {6, 5}, {3, 3}});
        C(after(751), LILY, 4, 3);
        SetIce(now, {{1, 5}, {6, 5}, {3, 3}, {4, 3}});
    }

    // PSD/B
    for (auto w : waves(2)) {
        PP(249, {2, 2}, 9);
        D(after(110), 1, 9);
        B(304, 5, 8.225);

        C(420, until(600), {PUFF, POT}, {5, 6}, 9);
        I();
    }

    // IPP-PP
    for (auto w : waves(3)) {
        P(303, 1, 9);
        P(433, 5, 7.4125);
        PP(leng1 - 200, 8.6);

        I(after(210));
    }

    // I3PPDD-B-PP
    for (auto w : waves({4, 8}, {12, 18}, 1)) {
        PP(410, 8.6);
        DD(after(220), 8.525);
        B(1100, 3, 8.2375);
        PP(leng2 - 200, 8.6);

        I(after(210));

        if (w == 4) {
            C(800, keep(266), {PUFF, POT}, {5, 6}, 8);
        }
    }

    // I3PPDD-B-PPNDD
    for (auto w : waves(9, 19)) {
        PP(410, 8.6);
        DD(after(220), 8.525);
        B(1100, 3, 8.2375);
        PP(1558, 8.6);
        C(after(220), {LILY, DOOM, COFFEE}, 3, 8);
        DD(after(220), 8.6);

        PP(4300, 8.6);
    }

    // PSd/PAd
    for (auto w : waves(10)) {
        PP(249);
        P(333, 2, 9);
        C(333, CHERRY, 5, 9);
        DD(after(263), {2, 5}, 4.5);

        I();
    }

    // IPP-PP
    for (auto w : waves(11)) {
        PP(303, {1, 5}, 9);
        PP(leng1 - 200, 8.6);

        I(after(210));
    }

    for (auto w : waves(20)) {
        P(225, 4, 7.5);
        PP(310);
        PP(310);
        PP(after(100));
        PP(now);
        PP(now, 4.5);
    }
}