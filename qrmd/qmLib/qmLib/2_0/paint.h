

#ifndef __PAINT__
#define __PAINT__

#include "avz.h"
#include "constant.h"
#include "drawBar.h"
#include "global.h"

// 显示游戏对象的属性信息条
class AStateShower {
private:
    ATickRunner _pltPainter;
    ATickRunner _zmbPainter;
    ATickRunner _plcItmPainter;
    APainterEx _qmPainterEx;
    const int _frameThickness = 1;
    const int _pltHpBarSizeX = 74;
    const int _pltHpBarSizeY = 8;
    const int _pltAttrBarSizeX = 74;
    const int _pltAttrBarSizeY = 5;
    const int _zmbHpBarSizeX = 8;
    // const int _zmbHpBarSizeY = 100;
    const int _zmbAttrBarSIzeX = 5;
    // const int _zmbHpBarSizeY = 100;
    const int _zmbIceBarSizeX = 5;
    // 格子列间距
    const int _gridXDiff = 80;
    // 格子行间距（根据场景确定，现在不能判断，否则AvZ不能正常注入）
    int _gridYDiff = 0;
    const int _plcItmAttrBarSizeX = 74;
    const int _plcItmAttrBarSizeY = 5;

public:
    // 在指定植物附近显示其属性信息条，其中属性为：
    // 所有植物：血量；
    // 射手类、投手类：发射倒计时进度；
    // 土豆地雷：准备就绪倒计时进度；
    // 向日葵、阳光菇、金盏花、双子向日葵：生产倒计时进度；
    // 樱桃炸弹、寒冰菇、毁灭菇和火爆辣椒：生效倒计时进度；
    // 三叶草：消失倒计时进度；
    // 大嘴花、磁力菇、玉米加农炮：复用倒计时进度；
    // ------------参数------------
    // pred 对象过滤器，以一个APlant*为参数的，返回bool值的lambda表达式，该断言将被用于场上的每个植物，结果为true的显示信息条。默认总为true，即始终显示所有植物的信息条
    // ------------示例------------
    // // 显示不满血的后轮在7列或8列的玉米加农炮的属性信息条
    // ShowPlants([](APlant* plt) { return plt->Type() == ACOB_CANNON && plt->Col() + 1 >= 7 && plt->Hp() < plt->HpMax(); });
    void ShowPlants(std::function<bool(APlant*)> pred = [](APlant* plt) { return true; })
    {
        if (!_pltPainter.IsStopped())
            _pltPainter.Stop();

        auto paintPltState = [=] {
            int barX, barY, sizeX, sizeY;
            float rate;
            std::vector<double> separators;
            uint32_t fillARGB, backgroundARGB;
            ABar::Direction direct = ABar::RIGHT;
            auto animArray = (AAnimation*)AGetPvzBase()->AnimationMain()->AnimationOffset()->AnimationArray();
            // 植物血量
            for (auto&& plt : AAliveFilter<APlant>(pred)) {
                if (AGetIsHave(std::vector<int> {ALILY_PAD, AFLOWER_POT}, plt.Type()))
                    continue;
                if (plt.Type() == ACOB_CANNON) {
                    barX = plt.Abscissa() + 40 * 2;
                    sizeX = _pltHpBarSizeX * 2;
                } else {
                    barX = plt.Abscissa() + 40;
                    sizeX = _pltHpBarSizeX;
                }
                sizeY = _pltHpBarSizeY;
                if (plt.Type() == APUMPKIN)
                    barY = plt.Ordinate() - _pltHpBarSizeY - _frameThickness * 2;
                else
                    barY = plt.Ordinate();
                rate = float(plt.Hp()) / plt.HpMax();
                fillARGB = AArgb(0xFF, 0x00, 0xFF, 0x00);
                backgroundARGB = AArgb(0xFF, 0x00, 0x00, 0x0);
                if (AGetIsHave(std::vector<int> {AWALL_NUT, APUMPKIN}, plt.Type()))
                    separators = {1.0 / 4, 2.0 / 4, 3.0 / 4};
                else if (plt.Type() == ATALL_NUT)
                    separators = {1.0 / 8, 2.0 / 8, 3.0 / 8, 4.0 / 8, 5.0 / 8, 6.0 / 8, 7.0 / 8};
                else
                    separators = {};
                _qmPainterEx.Draw(ABar(barX, barY, rate, sizeX, sizeY, fillARGB, backgroundARGB, _frameThickness, direct, separators));
            }
            // 植物属性
            for (auto&& plt : AAliveFilter<APlant>(pred)) {
                if (plt.Type() == ACOB_CANNON) {
                    barX = plt.Abscissa() + 40 * 2;
                    sizeX = _pltHpBarSizeX * 2;
                } else {
                    barX = plt.Abscissa() + 40;
                    sizeX = _pltHpBarSizeX;
                }
                barY = plt.Ordinate() + (_pltHpBarSizeY + _pltAttrBarSizeY) / 2 + _frameThickness * 2;
                sizeY = _pltAttrBarSizeY;
                int animCode = plt.AnimationCode();
                separators = {};
                switch (plt.Type()) {
                case APEASHOOTER:
                case ASNOW_PEA:
                case AREPEATER:
                case APUFF_SHROOM:
                case AFUME_SHROOM:
                case ASCAREDY_SHROOM:
                case ATHREEPEATER:
                case ASEA_SHROOM:
                case ACACTUS:
                case ASPLIT_PEA:
                case ASTARFRUIT:
                case ACABBAGE_PULT:
                case AKERNEL_PULT:
                case AMELON_PULT:
                case AGATLING_PEA:
                case AGLOOM_SHROOM:
                case ACATTAIL:
                case AWINTER_MELON:
                    rate = float(plt.MRef<int>(0x58)) / _plantAttackCycle[plt.Type()];
                    break;
                case ASPIKEWEED:
                case ASPIKEROCK:
                    rate = float(plt.StateCountdown()) / _plantAttackCycle[plt.Type()];
                    break;
                case APOTATO_MINE:
                    if (plt.StateCountdown() == 0)
                        continue;
                    rate = float(plt.StateCountdown()) / _potatoMineReadyTime;
                    break;
                case ASUNFLOWER:
                case ASUN_SHROOM:
                case AMARIGOLD:
                case ATWIN_SUNFLOWER:
                    rate = float(plt.MRef<int>(0x58)) / _sunFLowerProduceTime;
                    break;
                case ACHERRY_BOMB:
                case AICE_SHROOM:
                case ADOOM_SHROOM:
                case AJALAPENO:
                    rate = float(plt.ExplodeCountdown()) / _explodeTime;
                    break;
                case ACHOMPER:
                    if (plt.StateCountdown() == 0)
                        continue;
                    rate = float(plt.StateCountdown()) / _chomperChewTime;
                    break;
                case AMAGNET_SHROOM:
                    if (plt.StateCountdown() == 0)
                        continue;
                    rate = float(plt.StateCountdown()) / _magnetShroomReadyTime;
                    break;
                case ACOB_CANNON:
                    switch (plt.State()) {
                    case 35:
                        rate = float(125 + plt.StateCountdown()) / _cobCannonLoadTime;
                        break;
                    case 36:
                        rate = float(int(125 * (1 - animArray[animCode].CirculationRate()) + 0.5) + 1) / _cobCannonLoadTime;
                        break;
                    case 38:
                        rate = float(3125 + int(350 * (1 - animArray[animCode].CirculationRate()) + 0.5)) / _cobCannonLoadTime;
                        break;
                    default:
                        rate = 0;
                        break;
                    }
                    if (rate == 0)
                        continue;
                    break;
                case ABLOVER:
                    rate = float(plt.ExplodeCountdown() + plt.BloverCountdown()) / _bloverLiveTime;
                    separators = {(250.0 - 50.0) / 250.0};
                    break;
                case AGRAVE_BUSTER:
                    rate = float(plt.StateCountdown()) / _graveBuster;
                    break;
                case ASQUASH:
                    if (plt.State() >= 3 && plt.State() <= 6) {
                        if (plt.State() == 3) {
                            rate = float(plt.StateCountdown() + 45 + 50 + 7) / (80 + 45 + 50 + 7);
                        } else if (plt.State() == 4) {
                            rate = float(plt.StateCountdown() + 50 + 7) / (80 + 45 + 50 + 7);
                        } else if (plt.State() == 5) {
                            rate = float(plt.StateCountdown() + 7) / (80 + 45 + 50 + 7);
                        } else if (plt.State() == 6) {
                            rate = float(plt.StateCountdown()) / (80 + 45 + 50 + 7);
                        }
                    } else
                        continue;

                    separators = {(182.0 - 127.0) / 182};
                    break;
                case AIMITATOR:
                    switch (plt.State()) {
                    case 0:
                        rate = float(plt.StateCountdown() + 120) / _imitatorLiveTime;
                        break;
                    case 42:
                        animCode = plt.AnimationCode();
                        rate = float((1 - animArray[animCode].CirculationRate()) * 120) / _imitatorLiveTime;
                        break;
                    default:
                        break;
                    }
                    break;
                default:
                    continue;
                    break;
                }
                fillARGB = AArgb(0xFF, 0xFF, 0xC0, 0x00);
                backgroundARGB = AArgb(0xFF, 0x00, 0x00, 0x00);
                _qmPainterEx.Draw(ABar(barX, barY, rate, sizeX, sizeY, fillARGB, backgroundARGB, _frameThickness, direct, separators));
            }
        };
        _pltPainter.Start(paintPltState);
    }
    // 在指定僵尸附近显示其属性信息条，其中属性为：
    // 所有僵尸：血量（不计入持有的二类饰品）；
    // 撑杆僵尸、海豚僵尸：跳过植物动画进度；
    // 读报僵尸：失去报纸动画进度；
    // 潜水僵尸、海豚僵尸：跳入泳池动画进度；
    // 小丑僵尸：开盒倒计时进度；
    // 气球僵尸：气球破裂落地动画进度；
    // 矿工僵尸：自然出土后眩晕动画进度；
    // 蹦极僵尸：准备抓取植物动画进度；
    // 投篮僵尸：投篮动画进度；
    // 巨人僵尸：锤击动画进度、投掷小鬼动画进度；
    // 可被冰冻僵尸：冰冻倒计时进度；
    // ------------参数------------
    // pred 对象过滤器，以一个AZombie*为参数的，返回bool值的lambda表达式，该断言将被用于场上的每只僵尸，结果为true的显示信息条。默认总为true，即始终显示所有僵尸的信息条
    // ------------示例------------
    // // 显示6列以左的未被减速的巨人僵尸的属性信息条
    // ShowZombies([](AZombie* zmb) { return (zmb->Type() == AGARGANTUAR || zmb->Type() == AGIGA_GARGANTUAR) && zmb->Abscissa() <= 410 && zmb->SlowCountdown() == 0; });
    void ShowZombies(std::function<bool(AZombie*)> pred = [](AZombie* zmb) { return true; })
    {
        if (!_zmbPainter.IsStopped())
            _zmbPainter.Stop();

        auto paintZmbState = [=] {
            int barX, barY, sizeX, sizeY;
            float rate;
            std::vector<double> separators;
            uint32_t fillARGB, backgroundARGB;
            ABar::Direction direct;
            auto animArray = (AAnimation*)AGetPvzBase()->AnimationMain()->AnimationOffset()->AnimationArray();
            // 僵尸血量
            for (auto&& zmb : AAliveFilter<AZombie>(pred)) {
                barX = zmb.Abscissa() + zmb.MRef<int>(0x8C) + zmb.MRef<int>(0x94) + _zmbHpBarSizeX / 2 + _frameThickness;
                barY = zmb.Ordinate() + zmb.MRef<int>(0x90) + zmb.MRef<int>(0x98) / 2 - zmb.MRef<float>(0x84);
                sizeX = _zmbHpBarSizeX;
                sizeY = zmb.MRef<int>(0x98);
                rate = float(zmb.Hp() + zmb.OneHp()) / (zmb.MRef<int>(0xCC) + zmb.MRef<int>(0xD4));
                fillARGB = AArgb(0xFF, 0xFF, 0x00, 0x00);
                backgroundARGB = AArgb(0xFF, 0x00, 0x00, 0x00);
                if (zmb.Type() == AGARGANTUAR)
                    separators = {1800.0 / 3000};
                else if (zmb.Type() == AGIGA_GARGANTUAR)
                    separators = {1800.0 / 6000, 1800.0 * 2 / 6000, 1800.0 * 3 / 6000};
                else
                    separators = {float(_zombieHpCriticalPoint[zmb.Type()]) / (zmb.MRef<int>(0xCC) + zmb.MRef<int>(0xD4))};
                direct = ABar::UP;
                _qmPainterEx.Draw(ABar(barX, barY, rate, sizeX, sizeY, fillARGB, backgroundARGB, _frameThickness, direct, separators));
            }
            // 僵尸属性
            for (auto&& zmb : AAliveFilter<AZombie>(pred)) {
                barX = zmb.Abscissa() + zmb.MRef<int>(0x8C) + zmb.MRef<int>(0x94) - _zmbAttrBarSIzeX / 2 - _frameThickness;
                barY = zmb.Ordinate() + zmb.MRef<int>(0x90) + zmb.MRef<int>(0x98) / 2 - zmb.MRef<float>(0x84);
                sizeX = _zmbAttrBarSIzeX;
                sizeY = zmb.MRef<int>(0x98);
                uint16_t animCode = zmb.MRef<uint16_t>(0x118);
                separators = {};
                int bungeeGrabTime;
                switch (zmb.Type()) {
                case APOLE_VAULTING_ZOMBIE:
                    if (zmb.State() != 12)
                        continue;
                    rate = animArray[animCode].CirculationRate();
                    break;
                case ANEWSPAPER_ZOMBIE:
                    if (zmb.State() != 30)
                        continue;
                    rate = animArray[animCode].CirculationRate();
                    break;
                case ADANCING_ZOMBIE:
                case ABACKUP_DANCER:
                    // todo
                    break;
                case ASNORKEL_ZOMBIE:
                    if (zmb.State() != 58)
                        continue;
                    rate = animArray[animCode].CirculationRate();
                    break;
                case ADOLPHIN_RIDER_ZOMBIE:
                    if (!(zmb.State() == 52 || zmb.State() == 54))
                        continue;
                    rate = animArray[animCode].CirculationRate();
                    break;
                case AJACK_IN_THE_BOX_ZOMBIE:
                    if (zmb.State() != 15)
                        continue;
                    rate = float(zmb.StateCountdown()) / _jackInTheBoxMaxOpenBoxTime;
                    break;
                case ABALLOON_ZOMBIE:
                    if (zmb.State() != 74)
                        continue;
                    rate = animArray[animCode].CirculationRate();
                    break;
                case ADIGGER_ZOMBIE:
                    if (zmb.State() != 36)
                        continue;
                    rate = (animArray[animCode].CirculationRate() + animArray[animCode].MRef<int>(0x5C)) / 2.0;
                    break;
                case ABUNGEE_ZOMBIE:
                    if (!(zmb.State() == 6 || zmb.State() == 7))
                        continue;
                    bungeeGrabTime = zmb.SlowCountdown() > 0 ? _bungeeGrabTime * 2 : _bungeeGrabTime;
                    if (zmb.State() == 6)
                        rate = 1 - float(zmb.StateCountdown() + bungeeGrabTime) / (_bungeeRemainTime + bungeeGrabTime);
                    else // zmb.State() == 7
                        rate = float(_bungeeRemainTime + animArray[animCode].CirculationRate() * bungeeGrabTime) / (_bungeeRemainTime + bungeeGrabTime);
                    if (zmb.SlowCountdown() > 0)
                        separators = {float(_bungeeRemainTime) / (_bungeeRemainTime + bungeeGrabTime)};
                    else
                        separators = {float(_bungeeRemainTime) / (_bungeeRemainTime + bungeeGrabTime * 2)};
                    break;
                case ACATAPULT_ZOMBIE:
                    if (!(zmb.State() == 67 || zmb.State() == 68))
                        continue;
                    rate = 1 - float(zmb.StateCountdown()) / _catapultShootTime;
                    if (zmb.SlowCountdown() > 0)
                        separators = {(float(_catapultShootTime) / _catapultShootCycle) * 2};
                    else
                        separators = {float(_catapultShootTime) / _catapultShootCycle};
                    break;
                case AGARGANTUAR:
                case AGIGA_GARGANTUAR:
                    if (!(zmb.State() == 69 || zmb.State() == 70))
                        continue;
                    rate = animArray[animCode].CirculationRate();
                    if (zmb.State() == 69) {
                        separators = {float(_gargantuarThrowTime) / _gargantuarThrowCycle};
                    } else { // zmb.State() == 70
                        separators = {float(_gargantuarAttackTime) / _gargantuarAttackCycle};
                    }
                    break;
                default:
                    continue;
                    break;
                }
                fillARGB = AArgb(0xFF, 0xFF, 0xC0, 0x00);
                backgroundARGB = AArgb(0xFF, 0x00, 0x00, 0x00);
                direct = ABar::UP;
                _qmPainterEx.Draw(ABar(barX, barY, rate, sizeX, sizeY, fillARGB, backgroundARGB, _frameThickness, direct, separators));
            }
            // 僵尸被冰冻
            for (auto&& zmb : AAliveFilter<AZombie>(pred)) {
                if (zmb.FreezeCountdown() == 0)
                    continue;
                barX = zmb.Abscissa() + zmb.MRef<int>(0x8C) + zmb.MRef<int>(0x94) + _zmbHpBarSizeX + _zmbIceBarSizeX / 2 + _frameThickness * 2;
                barY = zmb.Ordinate() + zmb.MRef<int>(0x90) + zmb.MRef<int>(0x98) / 2 - zmb.MRef<float>(0x84);
                sizeX = _zmbIceBarSizeX;
                sizeY = zmb.MRef<int>(0x98);
                rate = float(zmb.FreezeCountdown()) / 600;
                fillARGB = AArgb(0xFF, 0x00, 0x00, 0xFF);
                backgroundARGB = AArgb(0x00, 0xFF, 0xFF, 0xFF);
                direct = ABar::UP;
                separators = {};
                _qmPainterEx.Draw(ABar(barX, barY, rate, sizeX, sizeY, fillARGB, backgroundARGB, 0, direct, separators));
            }
        };
        _zmbPainter.Start(paintZmbState);
    }
    // 在指定场地物品附近显示其属性信息条，其中属性为：
    // 弹坑：消失倒计时
    // ------------参数------------
    // pred 对象过滤器，以一个APlaceItem*为参数的，返回bool值的lambda表达式，该断言将被用于场上的每个场地物品，结果为true的显示信息条。默认总为true，即始终显示所有场地物品的信息条
    // ------------示例------------
    // 显示位于3行的8列和9列的弹坑的消失倒计时信息条
    // ShowPlaceItems([](APlaceItem* plcItm) { return plcItm->Type() == 2 && plcItm->Row() + 1 == 3 && plcItm->Col() + 1 >= 8; });
    void ShowPlaceItems(std::function<bool(APlaceItem*)> pred = [](APlaceItem* plcItm) { return true; })
    {
        if (!_plcItmPainter.IsStopped())
            _plcItmPainter.Stop();

        _gridYDiff = (AGetMainObject()->Scene() == 2 || AGetMainObject()->Scene() == 3) ? 85 : 100;
        auto plcItmState = [=] {
            int barX, barY, sizeX, sizeY;
            float rate;
            std::vector<double> separators;
            uint32_t fillARGB, backgroundARGB;
            ABar::Direction direct;
            // 弹坑消失倒计时
            for (auto&& plcItm : AAliveFilter<APlaceItem>(pred)) {
                barX = (plcItm.Col() + 1) * _gridXDiff;
                barY = (plcItm.Row() + 1) * _gridYDiff + 10;
                sizeX = _plcItmAttrBarSizeX;
                sizeY = _plcItmAttrBarSizeY;
                switch (plcItm.Type()) {
                case 2: // 弹坑
                    rate = float(plcItm.Value()) / 18000;
                    break;
                default:
                    continue;
                    break;
                }
                fillARGB = AArgb(0xFF, 0xFF, 0xC0, 0x00);
                backgroundARGB = AArgb(0xFF, 0x00, 0x00, 0x00);
                direct = ABar::RIGHT;
                separators = {};
                _qmPainterEx.Draw(ABar(barX, barY, rate, sizeX, sizeY, fillARGB, backgroundARGB, _frameThickness, direct, separators));
            }
        };

        _plcItmPainter.Start(plcItmState);
    }

    // void ShowSeeds(std::function<bool(ASeed*)> pred = [](ASeed* sd) { return true; })
    // {
    // todo
    // }

    // 显示所有植物、僵尸和场地物品的属性信息条
    void ShowAll()
    {
        ShowPlants();
        ShowZombies();
        ShowPlaceItems();
        // ShowSeeds();
    }
    // 不再显示植物的属性信息条
    void StopPlants()
    {
        _pltPainter.Stop();
    }
    // 不再显示僵尸的属性信息条
    void StopZombies()
    {
        _zmbPainter.Stop();
    }
    // 不再显示场地物品的属性信息条
    void StopPlaceItems()
    {
        _plcItmPainter.Stop();
    }
    // 不再显示植物、僵尸和场地物品的属性信息条
    void StopAll()
    {
        _plcItmPainter.Stop();
        _zmbPainter.Stop();
        _plcItmPainter.Stop();
    }
};

APainterEx _qmPainter;
ATickRunner _qmTickPainter;
// 在游戏窗口底部显示僵尸刷新信息，格式为：
// 波次：[当前波次]/[总波次] 时间：[本波时刻]|[刷新倒计时]/[刷新倒计时初始值]
// 刷新：[当前本波总血量]/[本波总血量初始值]([本波当前血量占总血量百分比]/[本波刷新血量占总血量百分比])
// ------------参数------------
// isEnable 为true时启用，为false时停用
void AShowRefreshingInfo(bool isEnable = true)
{
    const int _barLength = 400;
    const int _barWidth = 23;
    const int _barX = 0;
    const int _barY = 600 - _barWidth;
    const int _textSize = _barWidth;
    if (isEnable) {
        if (!_qmTickPainter.IsStopped())
            return;
        _qmPainter.SetFont("宋体");
        _qmPainter.SetFontSize(_textSize);
        std::string _temp;
        auto paintInfo = [=] {
            // 本波时刻
            int timeNowWave = ANowTime().time;
            std::string timeNowWaveStr = AGetString(timeNowWave, 0, 5);
            // 僵尸刷新倒计时
            int refreshingCountdown = AMPtr<APvzStruct>(0x6A9EC0)->MPtr<APvzStruct>(0x768)->MRef<int>(0x559C);
            std::string refreshingCountdownStr = AGetString(refreshingCountdown, 0, 4);
            // 僵尸刷新倒计时初始值
            int initialRefreshingCountdown = AMPtr<APvzStruct>(0x6A9EC0)->MPtr<APvzStruct>(0x768)->MRef<int>(0x55A0);
            std::string initialRefreshingCountdownStr = AGetString(initialRefreshingCountdown, 0, 4);
            // 当前所在波数
            int NowWave = ANowWave();
            std::string NowWaveStr = AGetString(NowWave, 0, 2);
            // 当前关卡总波数
            int allWaves = AMPtr<APvzStruct>(0x6A9EC0)->MPtr<APvzStruct>(0x768)->MRef<int>(0x5564);
            std::string allWavesStr = AGetString(allWaves, 0, 2);
            // 当前本波总血量
            int zmbHpAtWaveNow = 0;
            const std::vector<int> zmbTypeException = {ABACKUP_DANCER, ABUNGEE_ZOMBIE};
            // 未被魅惑的本波非伴舞蹦极僵尸
            for (auto&& zmb : aAliveZombieFilter) {
                if (zmb.AtWave() + 1 == ANowWave() && std::find(zmbTypeException.begin(), zmbTypeException.end(), zmb.Type()) == zmbTypeException.end() && !(zmb.MRef<bool>(0xB8)))
                    zmbHpAtWaveNow += zmb.Hp() + zmb.OneHp() + zmb.TwoHp() / 5;
            }
            std::string zmbHpAtWaveNowStr = AGetString(zmbHpAtWaveNow, 0, 6);
            // 达到刷新条件的血量
            int zmbHpToRefreshing = AMPtr<APvzStruct>(0x6A9EC0)->MPtr<APvzStruct>(0x768)->MRef<int>(0x5594);
            std::string zmbHpToRefreshingStr = AGetString(zmbHpToRefreshing, 0, 6);
            // 本波总血量
            int zmbHpAtWave = AMPtr<APvzStruct>(0x6A9EC0)->MPtr<APvzStruct>(0x768)->MRef<int>(0x5598);
            std::string zmbHpAtWaveStr = AGetString(zmbHpAtWave, 0, 6);
            // 本波当前血量占总血量百分比
            std::string hpNowPercentage = std::to_string(float(zmbHpAtWaveNow) * 100 / zmbHpAtWave).substr(0, 5);
            // 本波刷新血量占总血量百分比
            std::string refreshHpPercentage = std::to_string(float(zmbHpToRefreshing) * 100 / zmbHpAtWave).substr(0, 5);

            std::string timeInfo = "波次：" + NowWaveStr + "/" + allWavesStr + " 时间：" + timeNowWaveStr + "|" + refreshingCountdownStr + "/" + initialRefreshingCountdownStr;
            std::string refreshInfo = "血量：" + zmbHpAtWaveNowStr + "/" + zmbHpAtWaveStr + "(" + hpNowPercentage + "%/" + refreshHpPercentage + "%)";

            // 进度条
            _qmPainter.SetRectColor(AArgb(0xFF, 0x00, 0x00, 0x00));
            _qmPainter.Draw(ARect(_barX, _barY, _barLength, _barWidth));

            _qmPainter.Draw(ABar(_barLength / 2, 600 - (_barWidth / 2), float(timeNowWave) / initialRefreshingCountdown, _barLength, _barWidth, AArgb(0xFF, 0x12, 0x56, 0x28), AArgb(0x00, 0x00, 0x00, 0x00), 1, ABar::RIGHT));

            uint32_t tempARGB = refreshingCountdown > 200 ? AArgb(0xFF, 0xA1, 0x48, 0xB2) : AArgb(0xFF, 0xFF, 0xFF, 0x00);
            _qmPainter.Draw(ABar(_barLength / 2, 600 - (_barWidth / 2), float(refreshingCountdown) / initialRefreshingCountdown, _barLength, _barWidth, tempARGB, AArgb(0x00, 0x00, 0x00, 0x00), 1, ABar::LEFT));

            _qmPainter.SetRectColor(AArgb(0xFF, 0x00, 0x00, 0x00));
            _qmPainter.Draw(ARect(_barX + 400, _barY, _barLength, _barWidth));

            tempARGB = zmbHpAtWaveNow > zmbHpToRefreshing ? AArgb(0xFF, 0xF9, 0x00, 0x93) : AArgb(0xFF, 0xFF, 0xFF, 0x00);
            _qmPainter.Draw(ABar(_barLength + _barLength / 2, 600 - (_barWidth / 2), float(zmbHpAtWaveNow) / zmbHpAtWave, _barLength, _barWidth, tempARGB, AArgb(0xFF, 0x00, 0x00, 0x00), 1, ABar::RIGHT, {float(zmbHpToRefreshing) / zmbHpAtWave}));

            _qmPainter.SetRectColor(AArgb(0xFF, 0x00, 0x00, 0x00));
            _qmPainter.Draw(ARect(_barLength + (float(zmbHpToRefreshing) / zmbHpAtWave) * _barLength, 600 - _barWidth, 4, _barWidth));

            _qmPainter.SetRectColor(AArgb(0x00, 0x00, 0x00, 0x00));
            _qmPainter.SetTextColor(AArgb(0xFF, 0xFD, 0x93, 0x00));

            _qmPainter.Draw(AText(timeInfo, 0, 600 - _barWidth));
            _qmPainter.Draw(AText(refreshInfo, _barLength, 600 - _barWidth));
        };
        _qmTickPainter.Start(paintInfo);
    } else {
        _qmTickPainter.Stop();
    }
}

#endif