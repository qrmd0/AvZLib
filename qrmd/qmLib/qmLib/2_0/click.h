/*
 * @Author: qrmd
 * @Date: 2023-08-17 15:10:38
 * @LastEditors: qrmd
 * @LastEditTime: 2023-08-17 15:10:59
 * @Description:
 */
#ifndef __CLICK__
#define __CLICK__

// 从主菜单界面进入指定的游戏模式，必须在void AScipt()前使用本宏
// ------------参数------------
// mode 要进入的游戏模式，先填写前缀 APvZGameMode::，右键点击这个前缀，在弹出菜单中点击“查看类型定义”查看可选的游戏模式
// ------------示例------------
// #include "avz.h"
// #include "qmLib/main.h"
// // 进入泳池生存无尽模式
// APlayTheMode(APvZGameMode::SurvivalPoolEndless);
// void AScript()
// {
//     /*选卡和运阵等代码*/
// }
#define APlayTheMode(mode) GameModeStarter _gameModeStarter(mode);

enum class APvZGameMode {
    Adventure,
    ZomBotany,
    WallnutBowling,
    SlotMachine,
    ItsRainingSeeds,
    Beghouled,
    InvisiGhoul,
    SeeingStars,
    Zombiquarium,
    BeghouledTwist,
    BigTroubleLittleZombie,
    PortalCombat,
    ColumnLikeYouSeeEm,
    BobsledBonanza,
    ZombieNimbleZombieQuick,
    WhackAZombie,
    LastStand,
    ZomBotany2,
    WallnutBowling2,
    PogoParty,
    DrZombosssRevenge,
    Vasebreaker,
    ToTheLeft,
    ThirdVase,
    ChainReaction,
    MIsForMetal,
    ScaryPotter,
    HokeyPokey,
    AnotherChainReaction,
    AceOfVase,
    VasebreakerEndless,
    IZombie,
    IZombieToo,
    CanYouDigIt,
    TotallyNuts,
    DeadZeppelin,
    MeSmash,
    ZomBoogie,
    ThreeHitWonder,
    AllYourBrainBelongToUs,
    IZombieEndless,
    SurvivalDay,
    SurvivalNight,
    SurvivalPool,
    SurvivalFog,
    SurvivalRoof,
    SurvivalDayHard,
    SurvivalNightHard,
    SurvivalPoolHard,
    SurvivalFogHard,
    SurvivalRoofHard,
    SurvivalPoolEndless,
    ArtChallengeWallnut,
    SunnyDay,
    Unsodded,
    BigTime,
    ArtChallengeSunflower,
    AirRaid,
    IceLevel,
    ZenGarden,
    HighGravity,
    GraveDanger,
    CanYouDigIt_Hidden,
    DarkStormyNight,
    BungeeBiltz,
    Intro,
    TreeOfWisdom,
    SurvivalDayEndless,
    SurvivalNightEndless,
    SurvivalFogEndless,
    SurvivalRoofEndless,
    Upsell,
};

std::map<APvZGameMode, std::vector<std::array<int, 2>>> _pvzGameModePos = {
    {APvZGameMode::Adventure, {{550, 140}}},
    {APvZGameMode::ZomBotany, {{550, 235}, {88, 153}, {290, 379}}},
    {APvZGameMode::WallnutBowling, {{550, 235}, {243, 153}, {290, 379}}},
    {APvZGameMode::SlotMachine, {{550, 235}, {398, 153}, {290, 379}}},
    {APvZGameMode::ItsRainingSeeds, {{550, 235}, {553, 153}, {290, 379}}},
    {APvZGameMode::Beghouled, {{550, 235}, {708, 153}, {290, 379}}},
    {APvZGameMode::InvisiGhoul, {{550, 235}, {88, 272}, {290, 379}}},
    {APvZGameMode::SeeingStars, {{550, 235}, {243, 272}, {290, 379}}},
    {APvZGameMode::Zombiquarium, {{550, 235}, {398, 272}, {290, 379}}},
    {APvZGameMode::BeghouledTwist, {{550, 235}, {553, 272}, {290, 379}}},
    {APvZGameMode::BigTroubleLittleZombie, {{550, 235}, {708, 272}, {290, 379}}},
    {APvZGameMode::PortalCombat, {{550, 235}, {88, 391}, {290, 379}}},
    {APvZGameMode::ColumnLikeYouSeeEm, {{550, 235}, {243, 391}, {290, 379}}},
    {APvZGameMode::BobsledBonanza, {{550, 235}, {398, 391}, {290, 379}}},
    {APvZGameMode::ZombieNimbleZombieQuick, {{550, 235}, {553, 391}, {290, 379}}},
    {APvZGameMode::WhackAZombie, {{550, 235}, {708, 391}, {290, 379}}},
    {APvZGameMode::LastStand, {{550, 235}, {88, 510}, {290, 379}}},
    {APvZGameMode::ZomBotany2, {{550, 235}, {243, 510}, {290, 379}}},
    {APvZGameMode::WallnutBowling2, {{550, 235}, {398, 510}, {290, 379}}},
    {APvZGameMode::PogoParty, {{550, 235}, {553, 510}, {290, 379}}},
    {APvZGameMode::DrZombosssRevenge, {{550, 235}, {708, 510}, {290, 379}}},
    {APvZGameMode::Vasebreaker, {{550, 315}, {88, 153}, {290, 379}}},
    {APvZGameMode::ToTheLeft, {{550, 315}, {243, 153}, {290, 379}}},
    {APvZGameMode::ThirdVase, {{550, 315}, {398, 153}, {290, 379}}},
    {APvZGameMode::ChainReaction, {{550, 315}, {553, 153}, {290, 379}}},
    {APvZGameMode::MIsForMetal, {{550, 315}, {708, 153}, {290, 379}}},
    {APvZGameMode::ScaryPotter, {{550, 315}, {88, 272}, {290, 379}}},
    {APvZGameMode::HokeyPokey, {{550, 315}, {243, 272}, {290, 379}}},
    {APvZGameMode::AnotherChainReaction, {{550, 315}, {398, 272}, {290, 379}}},
    {APvZGameMode::AceOfVase, {{550, 315}, {553, 272}, {290, 379}}},
    {APvZGameMode::VasebreakerEndless, {{550, 315}, {708, 272}, {290, 379}}},
    {APvZGameMode::IZombie, {{550, 315}, {88, 391}, {290, 379}}},
    {APvZGameMode::IZombieToo, {{550, 315}, {243, 391}, {290, 379}}},
    {APvZGameMode::CanYouDigIt, {{550, 315}, {398, 391}, {290, 379}}},
    {APvZGameMode::TotallyNuts, {{550, 315}, {553, 391}, {290, 379}}},
    {APvZGameMode::DeadZeppelin, {{550, 315}, {708, 391}, {290, 379}}},
    {APvZGameMode::MeSmash, {{550, 315}, {88, 510}, {290, 379}}},
    {APvZGameMode::ZomBoogie, {{550, 315}, {243, 510}, {290, 379}}},
    {APvZGameMode::ThreeHitWonder, {{550, 315}, {398, 510}, {290, 379}}},
    {APvZGameMode::AllYourBrainBelongToUs, {{550, 315}, {553, 510}, {290, 379}}},
    {APvZGameMode::IZombieEndless, {{550, 315}, {708, 510}, {290, 379}}},
    {APvZGameMode::SurvivalDay, {{550, 385}, {88, 185}, {290, 379}}},
    {APvZGameMode::SurvivalNight, {{550, 385}, {243, 185}, {290, 379}}},
    {APvZGameMode::SurvivalPool, {{550, 385}, {398, 185}, {290, 379}}},
    {APvZGameMode::SurvivalFog, {{550, 385}, {553, 185}, {290, 379}}},
    {APvZGameMode::SurvivalRoof, {{550, 385}, {708, 185}, {290, 379}}},
    {APvZGameMode::SurvivalDayHard, {{550, 385}, {88, 330}, {290, 379}}},
    {APvZGameMode::SurvivalNightHard, {{550, 385}, {243, 330}, {290, 379}}},
    {APvZGameMode::SurvivalPoolHard, {{550, 385}, {398, 330}, {290, 379}}},
    {APvZGameMode::SurvivalFogHard, {{550, 385}, {553, 330}, {290, 379}}},
    {APvZGameMode::SurvivalRoofHard, {{550, 385}, {708, 330}, {290, 379}}},
    {APvZGameMode::SurvivalPoolEndless, {{550, 385}, {398, 475}, {290, 379}}},
    {APvZGameMode::ArtChallengeWallnut, {{550, 385}, {447, 582}, {88, 153}, {290, 379}}},
    {APvZGameMode::SunnyDay, {{550, 385}, {447, 583}, {243, 153}, {290, 379}}},
    {APvZGameMode::Unsodded, {{550, 385}, {447, 584}, {398, 153}, {290, 379}}},
    {APvZGameMode::BigTime, {{550, 385}, {447, 585}, {553, 153}, {290, 379}}},
    {APvZGameMode::ArtChallengeSunflower, {{550, 385}, {447, 586}, {708, 153}, {290, 379}}},
    {APvZGameMode::AirRaid, {{550, 385}, {447, 587}, {88, 272}, {290, 379}}},
    {APvZGameMode::IceLevel, {{550, 385}, {447, 588}, {243, 272}, {290, 379}}},
    {APvZGameMode::ZenGarden, {{550, 385}, {447, 589}, {398, 272}, {290, 379}}},
    {APvZGameMode::HighGravity, {{550, 385}, {447, 590}, {553, 272}, {290, 379}}},
    {APvZGameMode::GraveDanger, {{550, 385}, {447, 591}, {708, 272}, {290, 379}}},
    {APvZGameMode::CanYouDigIt_Hidden, {{550, 385}, {447, 592}, {88, 391}, {290, 379}}},
    {APvZGameMode::DarkStormyNight, {{550, 385}, {447, 593}, {243, 391}, {290, 379}}},
    {APvZGameMode::BungeeBiltz, {{550, 385}, {447, 594}, {398, 391}, {290, 379}}},
    {APvZGameMode::Intro, {{550, 385}, {447, 595}, {553, 391}, {290, 379}}},
    {APvZGameMode::TreeOfWisdom, {{550, 385}, {447, 596}, {708, 391}, {290, 379}}},
    {APvZGameMode::SurvivalDayEndless, {{550, 385}, {447, 597}, {88, 510}, {290, 379}}},
    {APvZGameMode::SurvivalNightEndless, {{550, 385}, {447, 598}, {243, 510}, {290, 379}}},
    {APvZGameMode::SurvivalFogEndless, {{550, 385}, {447, 599}, {398, 510}, {290, 379}}},
    {APvZGameMode::SurvivalRoofEndless, {{550, 385}, {447, 600}, {553, 510}, {290, 379}}},
    {APvZGameMode::Upsell, {{550, 385}, {447, 601}, {708, 510}, {290, 379}}},
};

class GameModeStarter {
public:
    GameModeStarter(const APvZGameMode& mode)
    {
        HWND pvzHWND = FindWindowA("MainWindow", "Plants vs. Zombies");
        if (!pvzHWND) {
            auto ret = MessageBoxW(NULL, L"未检测到正在运行的PvZ英文原版游戏程序\n 请手动打开游戏后进入主界面，再点击确定尝试注入", L"Warning", MB_ICONWARNING | MB_OK | MB_OKCANCEL);
            if (ret == 2)
                return;
        }
        for (auto each : _pvzGameModePos[mode]) {
            SendMessage(pvzHWND, WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(each[0], each[1]));
            SendMessage(pvzHWND, WM_RBUTTONUP, 0, MAKELPARAM(each[0], each[1]));
            Sleep(250);
        }
        ShowWindow(pvzHWND, SW_RESTORE);
        SetForegroundWindow(pvzHWND);
    }
};

#endif