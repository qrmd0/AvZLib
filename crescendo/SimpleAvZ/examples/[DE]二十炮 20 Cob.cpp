/*
 * 脚本作者: Crescendo(https://space.bilibili.com/8252252), 2023
 * 可自由复制, 转载, 修改本脚本, 但请注明出处. 不可从源代码中删去本段落.
 *
 * 阵型名: [DE]二十炮
 * 布阵码: LI43bJyUlNTYBS1A3FRUkXBbQVQ=
 * 参考视频: https://www.bilibili.com/video/BV1mb4y1t7UD
 * 说明: 无铲种解作者冰巫师墨舞, 优化ch6-I6解作者Crescendo
 *
 * 
 * Script author: Crescendo(https://space.bilibili.com/8252252), 2023
 * You may freely copy, distribute, and modify this script, subject to that
 * you cite the source and do not remove this notice from the source code.
 *
 * Name: [DE] 20 Cob
 * PvZ Tools code: LI43bJyUlNTYBS1A3FRUkXBbQVQ=
 * Ref video: https://www.bilibili.com/video/BV1mb4y1t7UD
 * Note: non cob replacement solution by 冰巫师墨舞,
 *       improved ch6-I6 solution by Crescendo.
 */

#include "SimpleAvZ/lib.h" // v1.0.0
#include "avz.h"           // 22_10_01

using namespace AvZ;

int leng1 = 1150;
int leng2 = 1250;

void Script()
{
    OpenMultipleEffective('C');
    SetErrorMode(CONSOLE);
    SetZombies({PJ_0, LZ_2, CG_3, WW_8, BC_12, XC_15, KG_17, TT_18, FT_21, TL_22, BY_23, HY_32});
    SelectCards({ICE, M_ICE, DOOM, COFFEE, CHERRY, JALA, PUMPKIN, PUFF, POT, SCAREDY});

    auto PPSSDA = [=]() {
        PP(225);
        PP(249);
        D(after(110), 1, 9);
        C(now, CHERRY, 4, 9);
    };
    auto PSD_Pd = [=]() {
        PP(225, {1, 2}, 9);
        P(225, 4, 9);
        D(after(110), 1, 8.8);
        D(after(265 - 110), 4, 3.8);
    };

    // IPP-PPDD
    for (auto w : waves(1)) {
        P(429, 2, 7.525);
        P(180, 4, 8.625);
        PP(leng1 - 200, {2, 5}, 8.825);
        D(after(206), 4, 8.8);
        D(after(212 - 206), 1, 7.9);

        C(-599, M_ICE, 5, 9);
        C(100, COFFEE, 1, 9);
    }

    // PPSSDA
    for (auto w : waves(2)) {
        PPSSDA();

        C(400, keep(130), PUMPKIN, 5, 9);
        C(601 + 100, COFFEE, 5, 9);
    }

    // IPP-NDD
    for (auto w : waves(3)) {
        P(180, 1, 8.625);
        P(429, 4, 7.525);
        C(leng1 - 200, {DOOM, COFFEE}, 2, 9);
        D(after(206), 2, 8.825);
        D(after(212 - 206), 4, 8.825);

        C(101, ICE, 1, 9);
    }

    // PSD/Pd
    for (auto w : waves(4)) {
        PSD_Pd();

        C(420, until(601), {PUFF, POT}, {4, 5}, 9);
        C(603, COFFEE, 1, 9);
    }

    // IPP-PPDD
    for (auto w : waves(5, 7, 15, 17)) {
        P(303, 2, 9);
        P(429, 4, 7.525);
        PP(leng2 - 200, {1, 4}, 8.8);
        D(after(206), 2, 8.8);
        if (w != 17) {
            D(after(212 - 206), 4, 8.8);
        } else {
            D(after(231 - 206), 4, 8.6);
            C(910, keep(1), PUFF, 4, 9);
        }
    }

    // PSD/Pd
    for (auto w : waves(6)) {
        PSD_Pd();

        C(-199, M_ICE, 1, 9);
        C(after(5001), M_ICE, 3, 9);
        C(420, until(601), {PUFF, POT}, {4, 5}, 9);
        C(603, COFFEE, 1, 9);
    }

    // PA'd/AD
    for (auto w : waves(8)) {
        P(225, 2, 9);
        C(225, {JALA, CHERRY}, {1, 4}, 9);
        D(after(110), 4, 8.175);
        D(after(265 - 110), 2, 3.8);

        C(336, keep(130), PUMPKIN, 5, 9);
        C(420, until(601), {PUFF, POT}, {4, 5}, 9);
        C(603, {ICE, COFFEE}, 5, 9);
    }

    // IPd/P-收尾
    for (auto w : waves(9, 19)) {
        P(597, 2, 8.6375);
        D(after(340), 2, 3.8);
        P(429, 4, 7.525);

        PP(leng2 - 200, {1, 4}, 8.8);
        D(after(206), 2, 8.8);
        D(after(212 - 206), 4, 8.8);
        D(after(220), 4, 8.8);
        PP(now, {1, 2}, 8.8);
        PP(4300);

        EnsureExist({{GIGA, 5}});
    }

    // PSDA'/PD
    for (auto w : waves(10)) {
        P(225, 2, 9);
        P(267, 2, 9);
        D(after(126), 2, 8.8);
        C(now, JALA, 1, 9);

        P(286, 4, 9);
        D(after(110), 4, 8.0625);

        C(434, until(598), {PUFF, POT}, {4, 5}, 9);
        C(601 + 100, {ICE, COFFEE}, 1, 9);
    }

    // IPP-PPDD
    for (auto w : waves(11)) {
        P(180, 2, 8.625);
        P(429, 4, 7.525);
        PP(leng1 - 200, {1, 4}, 8.825);
        D(after(206), 4, 8.8);
        D(after(212 - 206), 1, 7.9);

        C(910, keep(1), {PUFF, POT}, {4, 5}, 9);
    }

    // PPSSDA
    for (auto w : waves(12)) {
        PPSSDA();

        C(400, M_ICE, 1, 9);
        C(400, keep(130), PUMPKIN, 1, 9);
        C(601 + 100, COFFEE, 3, 9);
    }

    // IPP-NDD
    for (auto w : waves(13)) {
        P(180, 1, 8.625);
        P(429, 4, 7.525);
        C(leng1 - 200 + 1, keep(-200), {PUFF, COFFEE}, 3, 9);
        C(after(-200), DOOM, 3, 9);
        D(leng1 - 200 + 206, 2, 8.825);
        D(after(227 - 206), 4, 8.65);
    }

    // PSD/Pd
    for (auto w : waves(14)) {
        P(226, 1, 9);
        P(258, 4, 8.8);
        P(260, 2, 8.825);
        D(after(110), 1, 8.7);
        D(after(265 - 110), 4, 3.8);

        C(420, until(601), {PUFF, POT}, {4, 5}, 9);
        C(603, COFFEE, 1, 9);
    }

    // PSD/Pd
    for (auto w : waves(16)) {
        PSD_Pd();

        C(420, until(548), {PUFF, POT}, {4, 5}, 9);
        C(603, {ICE, COFFEE}, 1, 9);
    }

    // PA'd/AD
    for (auto w : waves(18)) {
        P(225, 2, 9);
        C(now, JALA, 1, 9);
        D(after(265), 2, 3.8);
        C(243, CHERRY, 4, 9);
        D(after(110), 4, 8.15);

        C(-17, M_ICE, 5, 9);
        C(336, keep(130), PUMPKIN, 5, 9);
        C(420, until(601), PUFF, 4, 9);
        C(603, COFFEE, 5, 9);
    }

    for (auto w : waves(20)) {
        P(191, 1, 9);
        PP(310);
        DD(now);
        PP(410);
        PP(now);
    }
}
