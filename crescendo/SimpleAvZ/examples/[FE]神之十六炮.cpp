/*
 * 脚本作者: Crescendo(https://space.bilibili.com/8252252), 2023
 * 可自由复制, 转载, 修改本脚本, 但请注明出处. 不可从源代码中删去本段落.
 *
 * 阵型名: [FE]神之十六炮
 * 布阵码: LI43NJRUU4Vk15cVU0DYGVAHQ3JUVNBNREVX
 * 参考视频: https://www.bilibili.com/video/BV1EL4y18795
 * 说明: 原阵型作者冰巫师墨舞, 优化C7u解作者Crescendo
 */

#include "SimpleAvZ/lib.h" // v1.1.0
#include "avz.h"           // 22_10_01

using namespace AvZ;

int leng1 = 1526;
int leng2 = 1642;
int leng3 = 631;
int leng4 = 1600;

void Script()
{
    OpenMultipleEffective('C');
    SetErrorMode(CONSOLE);
    SetZombies({PJ_0, LZ_2, CG_3, WW_8, BC_12, HT_14, XC_15, KG_17, TT_18, FT_21, TL_22, BY_23, HY_32});
    SelectCards({ICE, M_ICE, DOOM, LILY, CHERRY, JALA, TALL_NUT, BLOVER, GARLIC, POT});

    // AD/PD
    for (auto w : waves(1)) {
        A(225, 2, 9);
        D(after(110), 1, 7.8);
        P(359, 5, 9);
        D(after(107), 5, 7.8);

        C(563, until(642), BLOVER, 2, 9);
    }

    // PPDD
    for (auto w : waves(2, 11)) {
        PP(249, 9);
        if (w == 2) {
            DD(after(110), 8);
        } else {
            D(after(110), 1, 8);
            D(after(244 - 110), 5, 3);
        }
        I(1, 1);

        C(384, until(445), GARLIC, 2, 9);
        C(420, keep(134), POT, 5, 9);
    }

    // IPP-B-PPDD
    for (auto w : waves(3, 12)) {
        PP(429, {1, 5}, 7.425);
        B(849, 3, 8.3625);
        PP(leng1 - 200, 8.8);
        M_I(after(211), 1, 1);
        DD(after(220 - 211), 7.425);

        C(700, until(800), TALL_NUT, 2, 9);
        C(800, until(1289), POT, 2, 9);
        C(after(751), POT, 6, 2);
        C(775, until(1289), GARLIC, 5, 9);
    }

    // I'PP-B-PPDD
    for (auto w : waves(4, 13)) {
        PP(429, {1, 5}, 7.425);
        B(1175, 3, 8.1625);
        PP(leng2 - 200, 8.3875);
        DD(after(218), 8.3125);

        C(808, until(1175), POT, 2, 9);
        C(433, until(525), BLOVER, 5, 9);
        C(793, until(1262), GARLIC, 5, 9);
    }

    // NDD
    for (auto w : waves(5, 14)) {
        N(236, w == 5 ? 3 : 4, 9);
        DD(after(114), 8.15);

        C(557, until(618), {GARLIC, BLOVER}, {2, 5}, 9);
    }

    // PA/PJd
    for (auto w : waves(6, 15)) {
        P(243, 2, 9);
        A(after(120), 1, 8);
        P(243, 5, 9);
        J(after(201), 5, 9);
        P(after(110), 5, 8.8);

        C(405, keep(134), TALL_NUT, 2, 9);
    }

    // PPdd
    for (auto w : waves(7, 16)) {
        PP(leng3 - 200, 8.475);
        D(after(231), 5, 3.525);
        D(after(250 - 231), 1, 4);
        I(leng3, 5, 9);

        C(73, keep(1), POT, 2, 9);
        C(252, until(335), {GARLIC, BLOVER}, {2, 5}, 9);
        RM(leng3 + 50, 6, 2);
    }

    // IPP-B-PPDD
    for (auto w : waves(8, 17)) {
        PP(433, {1, 5}, 7.4125);
        B(1175, 3, 8.1625);
        PP(leng2 - 200, 8.6);
        M_I(after(211), 1, 1);
        DD(after(220 - 211), 7.4125);

        C(403, until(601), BLOVER, 2, 9);
        C(1014, until(1175), POT, 2, 9);
        C(804, until(1289), GARLIC, 5, 9);
    }

    // IPP-收尾
    for (auto w : waves(9)) {
        PP(433, {1, 5}, 7.4125);
        P(1174, 3, 9);
        DD(1429, 8.3875);
        DD(after(227), 8.3875);
        DD(after(227), 8.675);
        PP(2271, {3, 4}, 9);
        PP(3275, {1, 6}, 1);
        PP(4300, {1, 6}, 6);
        PP(now, {1, 6}, 8.5);

        C(803, until(1174), POT, 2, 9);
        C(433, until(525), BLOVER, 5, 9);
        C(793, until(1249), GARLIC, 5, 9);

        C(1544, until(4300), GARLIC, 2, 9);
        C(after(751), until(4300), GARLIC, 5, 9);
        C(4146, TALL_NUT, 1, 2);

        EnsureExist({GIGA, 1, 6});
    }

    // PD/AJD
    for (auto w : waves(10)) {
        P(333, 2, 9);
        D(after(240), 1, 4);
        C(285, {JALA, CHERRY}, {5, 6}, 9);
        D(after(110), 5, 9);

        C(563, until(642), BLOVER, 2, 9);
    }

    // IPP-B-PPDD
    for (auto w : waves(18)) {
        PP(433, {1, 5}, 7.4125);
        B(1175, 3, 8.1625);
        PP(leng4 - 200, 8.3875);
        DD(after(233), {7.4125, 8.325});

        C(774, until(1220), POT, 2, 9);
        C(433, until(585), BLOVER, 5, 9);
        C(999, until(1220), GARLIC, 5, 9);
    }

    // PND/BPJA-收尾
    for (auto w : waves(19)) {
        P(318, 2, 9);
        N(after(235), 1, 7);
        D(after(113), 1, 8.675);
        B(264, 5, 8.2625);
        P(729, 5, 8.6);
        J(after(242), 5, 9);
        A(after(218), 5, 9);

        P(1408, 2, 9);
        P(1675, 5, 9);
        P(now, 6, 1);
        P(2438, 6, 2);
        P(now, 6, 7);

        C(555, until(1064 - 134), POT, 2, 9);
        C(1064, BLOVER, 2, 9);
        C(442, keep(20), GARLIC, 5, 9);
        C(after(751), until(1403 - 134), GARLIC, 5, 9);
        C(1403, POT, 5, 9);

        EnsureExist({{GIGA}, {GARG}});
    }

    for (auto w : waves(20)) {
        P(229, 4, 7.5);
        PP(310);
        PP(now);
        PP(410);
        PP(now);
    }
}