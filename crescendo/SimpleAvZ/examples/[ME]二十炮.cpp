/*
 * 脚本作者: Crescendo(https://space.bilibili.com/8252252), 2023
 * 可自由复制, 转载, 修改本脚本, 但请注明出处. 不可从源代码中删去本段落.
 *
 * 阵型名: [ME]二十炮
 * 布阵码: LI4/bIyUhNTQTQQswNxUVCU0d0FR
 * 参考视频: https://www.bilibili.com/video/BV1pQ4y1d7rM
 * 说明: C7u-C8u融合解作者Crescendo
 */

#include "SimpleAvZ/lib.h" // v1.0.0
#include "avz.h"           // 22_10_01

using namespace AvZ;

CobOperator c1(1);
CobOperator c3(3);
CobOperator c5(5);
CobOperator c7(7);

int leng1 = 1540;

void Script()
{
    OpenMultipleEffective('C');
    SetErrorMode(CONSOLE);
    SetZombies({PJ_0, LZ_2, CG_3, GL_7, BC_12, XC_15, TT_18, FT_21, TL_22, BY_23, HY_32});
    SelectCards({ICE, M_ICE, POT, DOOM, CHERRY, JALA, SQUASH, CHOMPER, TALL_NUT, PUMPKIN});

    // PPSSDD
    for (auto w : waves(1, 10)) {
        c1.P(295, 2, 9.2);
        c3.P(295, 2, 9);
        c5.P(295, 4, 9);
        c7.P(295, 4, 9);
        c3.D(after(110), 2, 9);
        c7.D(now, 4, 9);

        C(601, {POT, ICE}, 1, 9);
    }

    // IPP-B-PPDD
    for (auto w : waves(2, 11)) {
        c3.P(303, 2, 9);
        c1.P(303, 4, 9);
        c5.B(1100, 3, 8.15);
        c3.P(leng1 - 200, 2, 9);
        c5.P(leng1 - 200, 4, 9);
        c7.D(after(207), 2, 9);
        c1.D(after(13), 4, 7.425);

        C(243, until(1290), CHOMPER, 1, 9);
        C(after(751), until(1290), CHOMPER, 5, 9);
        C(750, POT, 5, 9);
    }

    // BDD/PDD
    for (auto w : waves(3, 12)) {
        c3.P(225, 2, 8.7875);
        c1.D(after(110), 2, 9.1625);
        c1.D(after(110), 2, 9.1625);
        c5.P(379, 4, 9);
        c7.D(after(150), 4, 8.3625);
        c5.D(after(107), 4, 8.7125);

        C(8, POT, 2, 9);
        M_I(601 + 34, 2, 9);

        EnsureExist({GARG, 4});
    }

    // I'Ja-B-PPDD
    for (auto w : waves(4, 13)) {
        C(258, {POT, JALA}, 1, 9);
        C(302, SQUASH, 2, 9);
        c7.B(1100, 4, 8.15);
        c5.P(leng1 - 200, 2, 8.4125);
        c1.P(leng1 - 200, 5, 8.4125);
        c3.D(after(207), 2, 9);
        c7.D(now, 4, 9);

        C(260, until(1250), CHOMPER, 1, 9);
        C(after(751), until(1250), CHOMPER, 2, 9);

        EnsureExist({ZOMBONI, 2});
    }

    // APDD
    for (auto w : waves(5, 14)) {
        C(27, POT, 1, 9);
        C(227, CHERRY, 1, 9);
        c3.P(now, 4, 8.7875);
        c3.D(after(110), 2, 8.175);
        c7.D(now, 4, 8.175);

        if (w == 5) {
            C(396, keep(134), TALL_NUT, 1, 9);
        } else {
            RM(228, 1, 9);
        }
    }

    // NDDDD
    for (auto w : waves(6, 15)) {
        C(293, {POT, DOOM}, w == 6 ? 4 : 3, 9);
        c1.D(after(121), 2, 8.4);
        c5.D(now, 4, 8.4);
        c3.D(after(119), 2, 8.3875);
        c5.D(now, 4, 8.4);
    }

    // PPDDDD
    for (auto w : waves(7, 16)) {
        c1.P(379, 2, 9);
        c7.P(379, 4, 9);
        c3.D(after(121), 2, 9);
        c1.D(now, 4, 9);
        c1.D(after(134), 2, 8.9125);
        c5.D(now, 4, 8.7125);

        C(606, {POT, ICE}, 1, 9);
    }

    // Ia-B-PPDD
    for (auto w : waves(8, 17)) {
        C(188, SQUASH, 1, 9);
        c7.B(1100, 3, 8.15);
        c5.P(leng1 - 200, 1, 8.4125);
        c7.P(leng1 - 200, 4, 8.45);
        c5.D(after(207), 2, 9);
        c1.D(after(13), 4, 7.4125);

        C(160, until(1160), CHOMPER, 1, 9);
        C(after(751), until(1160), CHOMPER, 5, 9);
        C(750, POT, 5, 9);

        EnsureExist({ZOMBONI, 1});
    }

    // PDD/PD-I'收尾
    for (auto w : waves(9)) {
        C(530, keep(0), {POT, M_ICE}, 1, 9);

        c3.P(236, 2, 8.7875);
        c3.D(after(141), 2, 8.5375);
        c3.D(after(120), 2, 8.7875);
        c7.P(379, 4, 9);
        c7.D(after(153), 4, 7.55);

        c5.P(1147, 4, 9);
        c1.D(after(220), 4, 9);

        c5.P(2071, 2, 9);
        c1.P(2071, 4, 9);
    }

    // PDD/PD
    for (auto w : waves(18)) {
        c3.P(236, 2, 8.7875);
        c3.D(after(141), 2, 8.5375);
        c3.D(after(120), 2, 8.7875);
        c7.P(379, 4, 9);
        c7.D(after(153), 4, 7.55);

        C(8, POT, 1, 9);
        C(601, keep(0), M_ICE, 1, 9);
    }

    // I'P/PD-PPDDDD收尾
    for (auto w : waves(19)) {
        c1.P(546, 2, 9);
        c5.P(590, 4, 9);
        c3.D(after(223), 4, 8.2875);

        c5.P(1084, 2, 9);
        c1.D(after(220), 2, 9.2);
        c1.D(after(220), 2, 9.2);
        c7.P(1112, 4, 9);
        c3.D(after(220), 4, 9.2);
        c1.D(after(220), 4, 9.2);

        C(2383, {POT, CHERRY}, 2, 9);
        c5.P(2383, 4, 9);
    }

    for (auto w : waves(20)) {
        C(-1, keep(0), ICE, 2, 9);

        PP(310);
        PP(310);
        PP(410);
        PP(410);
    }
}