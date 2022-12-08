#include "avz.h"
#include "AvZDSL/AvZDSL.h"

ALogger<APvzGui> l;

template <typename... Ts>
ARelOp InfoR(Ts... args) {
    return AMkRelOp(l.Info(args...));
}

template <typename... Ts>
ARelOp CardR(Ts... args) {
    return AMkRelOp(ACard(args...));
}

ARelOp UseIceR(int row, int col) {
    return -100_cs[
        CardR(AICE_SHROOM, row, col),
        InfoR("蓝冰")
    ];
}

ARelOp UseImitatorIceR(int row, int col) {
    ARelTime t = rand() % 2 ? 419 : 420; // 模仿者生效时间可能取 319 或 320
    return -t[
        CardR(AM_ICE_SHROOM, row, col),
        AMkRelOp(AIce3(int(t))),
        InfoR("白冰")
    ];
}

ARelOp IceNightR(int row, int col) {
    return -420_cs[AMkRelOp(
        int idx = AGetSeedIndex(AICE_SHROOM, true);
        if(idx != -1 && AGetMainObject()->SeedArray()[idx].IsUsable()) // 复制冰可用
            ANow + 420_cs[UseImitatorIceR(row, col)];
        else
            ANow + 420_cs[UseIceR(row, col)];
    )];
}

// 阵设：PE经典4去存冰
// C6u: I-PP|I-PP|N|PP 1976|1976|750|749

void AScript() {
    ASetReloadMode(AReloadMode::MAIN_UI_OR_FIGHT_UI);
    ASetZombies({AZOMBIE, APOLE_VAULTING_ZOMBIE, ADANCING_ZOMBIE, AZOMBONI, ADOLPHIN_RIDER_ZOMBIE, AJACK_IN_THE_BOX_ZOMBIE, ABALLOON_ZOMBIE, ADIGGER_ZOMBIE, ACATAPULT_ZOMBIE, ABUNGEE_ZOMBIE, AGIGA_GARGANTUAR});
    ASelectCards({AICE_SHROOM, AM_ICE_SHROOM, ADOOM_SHROOM, ALILY_PAD, ASQUASH, ACHERRY_BOMB, APUMPKIN, APUFF_SHROOM, ASUN_SHROOM, AFLOWER_POT});
    aPlantFixer.Start(APUMPKIN, {}, 4000 / 3);

    ARelOp PP = -373_cs[AMkRelOp(aCobManager.Fire({{2, 8.75}, {5, 8.75}}))];
    ARelOp N = AMkRelOp(ACard(ALILY_PAD, {{3, 9}, {4, 9}, {3, 8}})) +
               AMkRelOp(ACard(ADOOM_SHROOM, {{3, 9}, {4, 9}, {3, 8}}));

    AWave(1, 5, 9, 14, 18)[
        749_cs - 200_cs[PP]
    ].AssumeWaveLength(749);

    AWave(2, 3, 6, 7, 11, 12, 15, 16, 19)[
        11_cs[IceNightR(3, 5)],
        1976_cs - 200_cs[PP]
    ].AssumeWaveLength(1976);

    AWave(4, 8, 13, 17)[
        750_cs - 200_cs - 100_cs[N]
    ].AssumeWaveLength(750);

    AWave(10)[
        341_cs[PP],
        401_cs - 100_cs[CardR(ACHERRY_BOMB, 2, 9)],
        401_cs - 182_cs[CardR(ASQUASH, 5, 9)]
    ].AssumeWaveLength(601);

    9_wave + 749_cs[
        11_cs[IceNightR(3, 5)],
        1976_cs - 200_cs[PP]
    ];

    19_wave + 1976_cs[
        1976_cs - 200_cs[PP]
    ];

    20_wave[
        341_cs[PP],
        341_cs[AMkRelOp(aCobManager.RecoverFire({{2, 8.75}, {5, 8.75}}))],
        395_cs[IceNightR(3, 5)]
    ];
}
