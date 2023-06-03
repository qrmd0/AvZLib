#include "avz.h"
#include "AvZDSL/AvZDSL.h"

ALogger<AConsole> l;

// 定义返回 ARelOp 的函数时，推荐将返回类型写作 RFunc
// 这会在返回值未被绑定时使编译器发出警告，降低出错概率
// 注意 static 和 RFunc 同时使用时 RFunc 要在 static 前面
template <typename... Ts>
RFunc InfoR(Ts... args) {
    // AMkRelOp(...) 就是 ARelOp([=]{ ...; }) 的语法糖
    return AMkRelOp(l.Info(args...));
}

#define CardR(...) AMkRelOp(ACard(__VA_ARGS__))

RFunc UseIceR(int row, int col) {
    // 构建复合操作的基本语法是 时间[操作1, 操作2, ...]
    // 相当于 ARelOp(-100, CardR(AICE_SHROOM, row, col) + InfoR("蓝冰"))
    return -100_cs[
        CardR(AICE_SHROOM, row, col),
        InfoR("蓝冰")
    ];
}

RFunc UseImitatorIceR(int row, int col) {
    ARelTime t = rand() % 2 ? 419 : 420; // 模仿者生效时间可能取 319 或 320
    return -t[
        CardR(AM_ICE_SHROOM, row, col),
        AMkRelOp(AIce3(t)),
        InfoR("白冰")
    ];
}

RFunc IceNightR(int row, int col) {
    return -420_cs[AMkRelOp(
        // 由于它被 AMkRelOp 包裹，这段代码会在操作时间点执行
        if(AIsSeedUsable(AM_ICE_SHROOM))
            // ANow(420) 就是在当前时间 420cs 后执行
            ANow(420)[UseImitatorIceR(row, col)];
        else
            ANow(420)[UseIceR(row, col)];
        // 为什么要先 -420cs 再 +420cs 呢？因为 -420cs 是「决策」的时间（你得在复制冰生效前 420cs 就想到把它放下去）

        // 让 ARelOp 立即执行也需要把它用 ANow[] 包裹
        // 得益于 RFunc，编译器会警告这种错误用法
        InfoR("这条语句不会被执行");
    )];
}

// 阵设：PE经典4去存冰
// 邻C6u: I-PP|I-PP|N|PP 1976|1976|750|749

void AScript() {
    ASetInternalLogger(l);
    ASetReloadMode(AReloadMode::MAIN_UI_OR_FIGHT_UI);
    ASetZombies({AZOMBIE, APOLE_VAULTING_ZOMBIE, ADANCING_ZOMBIE, AZOMBONI, ADOLPHIN_RIDER_ZOMBIE, AJACK_IN_THE_BOX_ZOMBIE, ABALLOON_ZOMBIE, ADIGGER_ZOMBIE, ACATAPULT_ZOMBIE, ABUNGEE_ZOMBIE, AGIGA_GARGANTUAR});
    ASelectCards({AICE_SHROOM, AM_ICE_SHROOM, ADOOM_SHROOM, ALILY_PAD, ASQUASH, ACHERRY_BOMB, APUMPKIN, APUFF_SHROOM, ASUN_SHROOM, AFLOWER_POT});
    aPlantFixer.Start(APUMPKIN, {}, 4000 / 3);

    // 定义发炮操作：在 373cs 前发射一对炮
    // 这样绑定的时间就对应炮生效时间，使代码更加直观和简短
    // 这样只构建复合操作，并不运行
    ARelOp PP = -373_cs[AMkRelOp(aCobManager.Fire({{2, 8.75}, {5, 8.75}}))];
    ARelOp N = AMkRelOp(ACard(ALILY_PAD, {{3, 9}, {4, 9}, {3, 8}})) +
               AMkRelOp(ACard(ADOOM_SHROOM, {{3, 9}, {4, 9}, {3, 8}}));

    // AWave 可以接受任意多参数
    // 参数可以是以下几种：
    // 1. 数字或其字符串形式
    // 2. 形如 "a-b" 的字符串（如 "2-5" 与 2, 3, 4, 5 等效）
    // 3. 形如 "a-b+c" 的字符串（如 "2-15+3" 与 2, 5, 8, 11, 14 等效）
    // C6u 是4波一循环，所以 +4
    AWave("1-9+4", 14, "18")[
        749_cs - 200_cs[PP] // 注意，749_cs - 200_cs可以不加括号
    ].AssumeWaveLength(749);
    // 第 9 波的假定波长被自动忽略了，不会报错

    // 字符串可以包含用空格或逗号分隔的多个区间
    // "..."_wave 与 AWave("...") 等效
    // 这种写法是笔者最推荐的
    "2-9+4 3-9+4 11-19+4 12-19+4"_wave[
        11_cs[IceNightR(1, 1)],
        1976_cs[-200_cs[PP]]
        // 不同于 AvZ1 的 delay，同级的时间是互不影响的，嵌套的时间会累加。
        // IceNightR 在 11cs 生效，PP 在 1776cs 生效
    ].AssumeWaveLength(1976);

    "4, 8, 13, 17"_wave[
        // ARelTime 支持 int * time 运算，但不能调换顺序
        -200_cs - 100_cs - 75 * -10_cs[N]
    ].AssumeWaveLength(750);

    const int w10_card_time = 401;
    // w10 特化：PPAa 消延迟
    // 窝瓜放置到生效 182cs
    " 10-10 "_wave[
        341_cs[PP],
        // 只有数字才能接_cs，w10_card_time_cs 是不可以的！
        // 有如下两种方法将 int 转化为时间：
        ARelTime(w10_card_time) - 100_cs[CardR(ACHERRY_BOMB, 2, 9)],
        w10_card_time * 1_cs - 182_cs[CardR(ASQUASH, 5, 9)]
    ].AssumeWaveLength(601);

    // 收尾
    9_wave + 749_cs[
        11_cs[IceNightR(3, 5)],
        1976_cs - 200_cs[PP]
        // 这个 PP 会在 (9, 749+1976-200-373) 发射，(9, 749+1976-200) 生效
        // 可见，多级嵌套的相对时间可以很大程度上提高可读性
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
