/* 脚本作者：阳光 */
/*！！第一种情况！！*/
#include "Overwatch/overwatch.h"
#include "SelectCardsPlus/SelectCardsPlus.h"
#include "SimpleAvZ/lib.h"
#include "avz.h"
#include "mod/mod.h"

const int TOTAL_ROUND = 100;
int round_counter = 0;
overwatch ow;

void Script()
{
    round_counter++;
    if (round_counter > TOTAL_ROUND) {
        SetErrorMode(CONSOLE);
        ShowErrorNotInQueue("Ran for # rounds", round_counter - 1);
        ow.dump_stats();
        return;
    }
    SetGameSpeed(5);
    SetTime(-599, 1);
    ow.start({{1, 7}, {2, 7}, {5, 7}, {6, 7}});
    SkipTick([=]() {
        return ow.keep_going();
    });
    EnableModsScoped(RemoveFog, UnlimitedSun, DisableItemDrop, DisableSpecialAttack, CobFixedDelay);

    KeyConnect('A', [=] {
        ow.dump_stats();
    });
    SetTime(0, 19);
    MaidCheats::dancing();
    SetWavelength({{6, 1672}, {15, 1672}});

    SetZombies({CG_3, HT_14, FT_21, BC_12, TT_18, HY_32, WW_8, BY_23});
    SetWaveZombies(20, {CG_3, FT_21, BC_12, HY_32, TT_18, HT_14, BY_23, PJ_0, LZ_2, WW_8, HY_32});
    SetWaveZombies(9, {CG_3, FT_21, BC_12, HY_32, TT_18, HT_14, BY_23, PJ_0, LZ_2, WW_8, HY_32});
    SetWaveZombies(19, {CG_3, FT_21, BC_12, HY_32, TT_18, HT_14, BY_23, PJ_0, LZ_2, WW_8, HY_32});
    SelectCardsPlus({
        ICE_SHROOM,    // 蓝冰
        M_ICE_SHROOM,  // 白冰
        LILY_PAD,      // 荷叶
        CHERRY_BOMB,   // 樱桃
        DOOM_SHROOM,   // 核弹
        JALAPENO,      // 辣椒
        FUME_SHROOM,   // 掌
        FLOWER_POT,    // 花盆
        CACTUS,        // 大嘴
        UMBRELLA_LEAF, // 伞
    });
    SetWavelength({{7, 1650}, {16, 1714}, {18, 601}});
    OpenMultipleEffective('C', MAIN_UI_OR_FIGHT_UI);
    KeyConnect('Q', [=]() { Card(7, 1, 9); });
    KeyConnect('W', [=]() { Card(8, 1, 9); });
    KeyConnect('E', [=]() { Card(9, 1, 9); });
    KeyConnect('R', [=]() { Card(10, 1, 9); });
    //  W1 PPSDD
    for (auto w : waves(1)) {
        PP(225);
        P(225, 2, 9);
        DD(after(110), {9, 7.8});

        C(-599, {LILY, FUME_SHROOM}, {4, 4}, 1);
        C(after(751), {LILY, CACTUS}, {3, 3}, 2);
        C(after(751), LILY, 4, 8);
        C(after(751), LILY, 3, 7);

        C(-599, {POT, CACTUS, UMBRELLA_LEAF}, {1, 1, 6}, 9);
        C(after(751), FUME_SHROOM, 5, 9);
        C(after(751), CACTUS, 2, 9);
        RM(490, {5, 6}, 9);
        RM(601, 1, 9);
        C(602, {UMBRELLA_LEAF, POT}, {1, 2}, 9);

        RM(0, 2, 9);
        M_I(2, 9);
    }
    // W2,11 IPP-PPDD 1639
    for (auto w : waves(2, 11)) {
        P(433, 5, 7.4125);
        P(317 - 33, 1, 9.8125);
        RM(1292, {1, 1, 2, 2}, 9);
        PP(1439, {8.75, 8.6625});
        DD(after(217), 7.4125);

        C(824, keep(266), {UMBRELLA_LEAF, POT}, {5, 6}, 9);
        C(after(266), keep(148), {CACTUS, FUME_SHROOM}, {5, 6}, 9);

        EnsureExist({GIGA, 2});
    }
    // W3 NDD
    for (auto w : waves(3)) {
        N(359, 4, 9);
        C(after(751 - 99), LILY, 3, 4);
        C(after(751), LILY, 3, 3);
        C(after(751), LILY, 3, 1);
        DD(359 + 108, 7.925);

        C(359, until(615), {FUME_SHROOM, CACTUS}, {5, 6}, 9);
        C(now, until(505), UMBRELLA_LEAF, 1, 9);
        C(now, until(745), POT, 2, 9);

        EnsureExist({GIGA, 1, 2});
    }
    // W4,13 BD/JAD
    for (auto w : waves(4, 13)) {
        B(318, 5, 9);
        D(after(110), 5, 8);
        J(325, 1, 9);
        A(440, 1, 9);
        D(359 + 110, 1, 7.8125); // 原本的炸点会伤到红眼

        C(539 + 33, keep(50), {FUME_SHROOM, UMBRELLA_LEAF}, {5, 6}, 9);
        C(after(751), {FUME_SHROOM, UMBRELLA_LEAF}, {5, 6}, 9);
        C(509, until(745), POT, 2, 9);
        C(after(751), POT, 5, 9);

        EnsureExist({{WW_8, 1}, {HT_14, 3}});
    }
    // W5 PSD/PD
    for (auto w : waves(5)) {
        PP(225 + 33, {5, 5}, 9);
        D(after(110), 5, 8.8);
        P(359, 2, 9);
        D(after(107), 1, 7.825);

        C(446, keep(25), CACTUS, 1, 9);

        I(3, 7);

        EnsureExist({GIGA, 1, 2});
    }
    // W6,15 IPP-P/PD 1672/1673
    for (auto w : waves(6, 15)) {
        P(466, 1, 7.4125);
        P(317, 5, 9.5375);
        RM(1262, {5, 5, 6, 6}, 9);
        P(1472, 2, 8.75);
        D(after(217), 1, 7.4125);
        P(1473, 5, 8.6625);

        M_I(after(210), 3, 7);

        C(809, until(1292), POT, 1, 9);
        C(1655, keep(1), {POT, CACTUS}, {1, 2}, 9);
        C(800, keep(266), CACTUS, 2, 9);
        C(after(266), keep(148), FUME_SHROOM, 2, 9);
        C(after(751), FUME_SHROOM, 3, 1);
        RM(1050, 6, 9);
        C(after(1), UMBRELLA_LEAF, 6, 9);

        EnsureExist({GIGA, 1, 2});
    }
    // W7 I3PD/IP-B-PDD 1650
    for (auto w : waves(7)) {
        P(410, 5, 8.75);
        D(after(220), 5, 8.575);
        P(525, 1, 7.4125);
        P(1450, 2, 8.75);
        D(after(217), 1, 7.4125);
        D(now, 5, 8.5875);
        B(1150, 5, 8.1);

        C(796, keep(266), {POT, CACTUS}, {1, 2}, 9);
        C(after(266), keep(148), {UMBRELLA_LEAF, FUME_SHROOM}, {1, 2}, 9);
        C(after(751 - 266), until(1595), {POT, CACTUS}, {1, 2}, 9);
    }
    // W8 ND/PSN
    for (auto w : waves(8)) {
        N(359, 3, 9);
        C(after(751 - 99), LILY, 4, 2);
        D(359 + 108, 1, 7.925);
        PP(225, {5, 5}, 9);

        C(359, until(590), {UMBRELLA_LEAF, FUME_SHROOM}, {1, 2}, 9);
    }
    // W9 JPADD
    for (auto w : waves(9)) {
        A(359, 5, 9);
        D(after(108), 5, 7.825);
        J(298, 1, 9);
        P(393, 2, 9);
        D(after(122), 1, 7.6125);
        PP(1065);
        DD(after(789), {5, 5}, 9);
        D(1241 + 5, 3, 9);
        D(1634 + 5, 3, 9);

        C(199, keep(100), CACTUS, 2, 9);
        C(after(751), CACTUS, 1, 9);
        C(509, until(756), {UMBRELLA_LEAF, FUME_SHROOM}, {5, 6}, 9);
        C(after(751), UMBRELLA_LEAF, 4, 2);
        C(after(751), UMBRELLA_LEAF, 4, 8);
        C(446, until(756), POT, 2, 9);

        I(1065 + 105, 3, 7);

        // 垫1路红
        C(1800, FUME_SHROOM, 1, 9);
        C(after(414), POT, 1, 9);
        C(after(414), FUME_SHROOM, 1, 9);
        C(after(414), POT, 1, 9);
        C(after(207), CACTUS, 1, 9);
        C(after(207), UMBRELLA_LEAF, 1, 9);
        C(after(207), FUME_SHROOM, 1, 9);
        C(after(207), POT, 1, 9);
        C(after(207), CACTUS, 1, 9);

        EnsureExist(GIGA);

        P(4300, 1, 9);
        RM(now, 1, 9);
    }

    // W10 PPSDD
    for (auto w : waves(10)) {
        PP(225, {2, 2}, 9);
        D(after(110), 1, 8.8);
        P(332, 5, 9);
        D(after(110), 5, 7.9375);

        C(-77, {POT, UMBRELLA_LEAF}, {1, 1}, 9);
        C(after(751), {POT, UMBRELLA_LEAF}, {2, 2}, 9);
        C(332, until(490), {CACTUS, FUME_SHROOM}, {6, 5}, 9);

        M_I(3, 7);
    }

    // W12 NDD
    for (auto w : waves(12)) {
        N(359, 3, 8);
        DD(after(108), 7.925);

        C(359, until(615), {FUME_SHROOM, CACTUS}, {5, 6}, 9);
        C(now, until(505), UMBRELLA_LEAF, 1, 9);
        C(now, until(745), POT, 2, 9);

        EnsureExist({GIGA, 1});
    }
    // W14 PDD/PD
    for (auto w : waves(14)) {
        P(225 + 33, 5, 9);
        D(after(110), 5, 8.8);
        D(after(110), 5, 8.8);
        P(359, 2, 9);
        D(after(107), 1, 7.825);

        C(446, keep(25), CACTUS, 1, 9);

        I(3, 7);

        EnsureExist({GIGA, 1, 2});
    }
    // W16 I3PD/IP-B-PDD 1714
    for (auto w : waves(16)) {
        P(410, 5, 8.75);
        D(after(220), 5, 8.575);
        P(525, 1, 7.4125);
        P(1514, 2, 8.75);
        D(after(220), 1, 7.375); // 原本的炸点漏跳跳, 但这个炸点仍有漏的概率
        D(now, 5, 8.5375);
        B(1150, 5, 8.1);

        C(796, keep(266), {POT, CACTUS}, {1, 2}, 9);
        C(after(266), keep(148), {UMBRELLA_LEAF, FUME_SHROOM}, {1, 2}, 9);
        C(after(751 - 266), until(1680), {POT, CACTUS}, {1, 2}, 9);
    }
    // W17 ND/PSN
    for (auto w : waves(17)) {
        N(359, 4, 9);
        D(359 + 108, 1, 7.925);
        PP(225, {5, 5}, 9);

        C(359, until(505), UMBRELLA_LEAF, 2, 9);
        C(now, until(730), FUME_SHROOM, 1, 9);

        EnsureExist({GIGA, 2});
    }
    // W18 JAD/B 601
    for (auto w : waves(18)) {
        B(304, 5, 8.225);
        J(325, 2, 9);
        A(440, 2, 9);
        D(359 + 110, 1, 7.825);

        C(360, until(535), POT, 1, 9); // 359->360, 否则会在红眼受伤前垫到

        EnsureExist({{WW_8, 2}, {BY_23, 5, 6}});
    }
    // W19 PD-PDD/B-I3P-PSD
    for (auto w : waves(19)) {
        P(359, 2, 9);
        D(after(107), 1, 7.8125);
        P(188, 5, 8.3625);
        P(901, 5, 9);
        PP(after(210), {5, 5}, 9);
        D(after(210), 5, 9);
        P(1101 + 416 + 416, 2, 9);
        D(after(212), 1, 9);
        D(2293, 2, 9);

        I(500, 3, 7);

        C(145, keep(1), CACTUS, 1, 9);
        C(187, until(330), {UMBRELLA_LEAF, FUME_SHROOM}, {5, 6}, 9);
        C(1101, keep(1), {POT, CACTUS}, {1, 2}, 9);
        C(after(416), keep(1), {UMBRELLA_LEAF, FUME_SHROOM}, {1, 2}, 9);

        EnsureExist(GIGA);
    }

    // W20
    for (auto w : waves(20)) {
        // 铲除多余植物
        RM(0, FUME_SHROOM);
        RM(0, CACTUS);
        RM(0, UMBRELLA_LEAF);
        RM(0, LILY_PAD);
        // 秒杀僵尸，恢复冷却，去除弹坑
        At(359, [=] {
            for (auto& z : alive_zombie_filter)
                z.state() = 3;
            EnableModsGlobal(CobInstantRecharge, FreePlantingCheat);
            for (auto& p : alive_place_item_filter)
                if (p.type() == 2 && p.value() > 0)
                    p.value() = 1;
        });
        At(459, [=] {
            DisableMods(CobInstantRecharge, FreePlantingCheat);
        });
    }
}
/*！！第一种情况！！*/