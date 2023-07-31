/*
 * @Author: qrmd
 * @Date: 2023-07-31 18:56:37
 * @LastEditors: qrmd
 * @LastEditTime: 2023-07-31 19:03:47
 * @Description:AvZ2插件自动垫材
 */

#include "avz.h"

namespace _qrmd {
// 取被b包含的a中的元素组成的向量，顺序与a相同，包含重复项
// ------------参数------------
// a 任意支持"=="运算符的类型或其组成的向量
// b 同a
// ------------示例------------
// std::vector<int> v1 = {1, 2, 2, 3};
// std::vector<int> v2 = {2, 2, 2, 1, 4};
// std::vector<int> v3 = AGetIntersection(v1, v2);
// // v3的值为{1, 2, 2}
template <typename T>
std::vector<T> AGetIntersection(const std::vector<T>& a, const std::vector<T>& b)
{
    std::vector<T> intersection;
    for (const auto& element : a) {
        if (std::find(b.begin(), b.end(), element) != b.end())
            intersection.emplace_back(element);
    }
    return intersection;
}

template <typename T>
std::vector<T> AGetIntersection(const T& a, const std::vector<T>& b)
{
    std::vector<T> temp = {a};
    return AGetIntersection(temp, b);
}

template <typename T>
std::vector<T> AGetIntersection(const std::vector<T>& a, const T& b)
{
    std::vector<T> temp = {b};
    return AGetIntersection(a, temp);
}

// 合并两个向量，返回合并后的向量
template <typename T>
std::vector<T> AMerge(const std::vector<T>& a, const std::vector<T>& b)
{
    std::vector<T> result;
    result.insert(result.end(), a.begin(), a.end());
    result.insert(result.end(), b.begin(), b.end());
    return result;
}

template <typename T>
std::vector<T> AMerge(const T& a, const std::vector<T>& b)
{
    std::vector<T> temp = {a};
    return AMerge(temp, b);
}

template <typename T>
std::vector<T> AMerge(const std::vector<T>& a, const T& b)
{
    std::vector<T> temp = {b};
    return AMerge(a, temp);
}

// 移除向量中的重复元素
template <typename T>
void ARemoveDuplicates(std::vector<T>& vec)
{
    std::unordered_set<T> uniqueElements;
    std::vector<T> result;

    for (auto each : vec) {
        if (uniqueElements.find(each) == uniqueElements.end()) {
            uniqueElements.insert(each);
            result.push_back(each);
        }
    }
    vec = result;
}

// 移除向量中的指定元素
template <typename T>
void ARemoveElement(std::vector<T>& vec, T element)
{
    auto newEnd = std::remove_if(vec.begin(), vec.end(), [element](T t) {
        return t == element;
    });
    if (newEnd != vec.end())
        vec.erase(newEnd, vec.end());
}

// 将向量中的指定元素移动至指定位置，如果失败，返回false
template <typename T>
bool AMoveElement(std::vector<T>& vec, T element, int toIndex)
{
    if (toIndex < 0 || toIndex >= vec.size())
        return false;

    auto it = std::find(vec.begin(), vec.end(), element);
    if (it == vec.end())
        return false;

    vec.erase(vec.begin() + std::distance(vec.begin(), it));
    vec.insert(vec.begin() + toIndex, element);
    return true;
}

// 自动垫僵尸，在指定格子处检测到指定僵尸时放置指定植物拖延其推进，用于运阵时垫舞王、垫巨人，拖收尾，抗压等
// ------------参数------------
// grids 要放垫的格子列表，如果传空向量，则取所有陆地行的最靠右的不在垫材列表中的非一次性植物的右边的格子
// zombies 要阻挡的僵尸列表，如果传空向量，则取橄榄、小丑、梯子、白眼、红眼，不得包含：冰车、气球、矿工、投篮、蹦极，因为垫材通常对它们无效
// plants 作为垫材的种子列表，如果传空向量，则取所有冷却时间最快的种子及其模仿者版本，阳光花费少者优先
// removalDelay 放置垫材后多久将其铲除，如果为-1则不铲除。常用时间：减速巨人锤击生效时间266cs，防止跳跳起跳时间79cs。长期连接时建议不铲除，否则可能会意外铲除原垫材消失后在此格子放置的新植物
// bufferPx 横坐标缓冲量，允许僵尸在指定格子处推进多远再放垫，例如巨人僵尸推进到横坐标不大于440时会对5列植物举锤，如果bufferPx填入30，巨人僵尸推进到横坐标不大于410时才放垫。允许为负值
// isSynchronizeGargantuar 是否同步巨人动作，即当此处有巨人处于举锤状态时不放垫
// isSmartFlowerpot 是否将花盆放在啃食威胁最大的位置，花盆被种下后短时间内免疫啃食伤害，因此需要同时放置多个垫材时，花盆适合放在最容易被啃穿的位置
// ------------示例------------
// // NE舞夜惊魂(Pc)垫舞王
// for (auto wave = 1; wave <= 20; ++wave)
//     AConnect(ATime(wave, 195), [] { AAutoFodder({{3, 9}, {4, 9}}, {ADANCING_ZOMBIE}, {}, 4); });
// // PE纯二十炮(P5u)垫跳跳和舞王
// for (auto wave : {4, 6, 9, 12, 14, 17, 19}) {
//     AConnect(ATime(wave, 170), [] { AAutoFodder({{1, 9}}, {APOGO_ZOMBIE}, {}, 79); });
//     AConnect(ATime(wave, 195), [] { AAutoFodder({{1, 9}, {2, 9}, {5, 9}, {6, 9}}, {ADANCING_ZOMBIE}, {}, 4); });
// }
// // DE双冰六炮(ch5)垫红眼拖收尾
// for (auto wave : {9, 19, 20}) {
//     AConnect([=] { return ANowWave() == wave && ANowTime(wave) >= 1700; }, [] { AAutoFodder(); });
//     // 场上无红眼时铲垫
//     const std::vector<AGrid> fodderGrids = {{1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7}};
//     for (AGrid each : fodderGrids) {
//         AConnect([=] { return AGetPlantIndex(each.row, each.col) >= 0 && !AIsZombieExist(AGIGA_GARGANTUAR); }, [=] { AShovel(each.row, each.col); });
//     }
// }
// PE电波钟无炮抗压
// AConnect([] { return true; }, [] { AAutoFodder({{1, 5}, {1, 6}, {6, 5}, {6, 6}, {5, 7}, {2, 7}}, {}, {}, -1, 25); });
void AAutoFodder(std::vector<AGrid> grids = {}, std::vector<AZombieType> zombies = {}, std::vector<APlantType> plants = {}, int removalDelay = -1, int bufferPx = 0, bool isSynchronizeGargantuar = true, bool isSmartFlowerpot = true)
{
    std::vector<AZombieType> _zombies = zombies;
    for (auto each : {AZOMBONI, ABALLOON_ZOMBIE, ADIGGER_ZOMBIE, ACATAPULT_ZOMBIE, ABUNGEE_ZOMBIE})
        ARemoveElement(_zombies, each);
    if (_zombies.empty())
        _zombies = {AFOOTBALL_ZOMBIE, AJACK_IN_THE_BOX_ZOMBIE, ALADDER_ZOMBIE, AGARGANTUAR, AGIGA_GARGANTUAR};

    std::vector<APlantType> _plants = plants;
    if (_plants.empty())
        _plants = {AM_PUFF_SHROOM, APUFF_SHROOM, ASCAREDY_SHROOM, AM_SCAREDY_SHROOM, AFLOWER_POT, AM_FLOWER_POT, AM_SUN_SHROOM, ASUN_SHROOM, ALILY_PAD, AM_LILY_PAD, ASUNFLOWER, AM_SUNFLOWER, AGARLIC, AM_GARLIC, AFUME_SHROOM, AM_FUME_SHROOM, APEASHOOTER, AM_PEASHOOTER, ABLOVER, AM_BLOVER, AUMBRELLA_LEAF, AM_UMBRELLA_LEAF};
    std::vector<APlantType> tempVec = {};
    for (auto plt : _plants) {
        int seedIndex = -1;
        seedIndex = int(plt) > int(AIMITATOR) ? AGetSeedIndex(int(plt) - 49, true) : AGetSeedIndex(int(plt), false);
        if (seedIndex != -1)
            tempVec.emplace_back(plt);
    }
    _plants = tempVec;

    const std::vector<APlantType> disposablePlants = {ACHERRY_BOMB, APOTATO_MINE, AGRAVE_BUSTER, AHYPNO_SHROOM, AICE_SHROOM, ADOOM_SHROOM, ASQUASH, ATANGLE_KELP, AJALAPENO, ASPIKEROCK, ABLOVER, ACOFFEE_BEAN};
    std::vector<APlantType> tempPlants = AMerge(_plants, disposablePlants);
    ARemoveDuplicates(tempPlants);
    std::vector<int> fodderRows = ARangeIn(AGetMainObject()->Scene(), {2, 3}) ? std::vector<int> {1, 2, 5, 6} : std::vector<int> {1, 2, 3, 4, 5};
    std::vector<AGrid> _grids = grids;

    if (_grids.empty()) {
        int tempCol;
        AAliveFilter<APlant> permanentPlant([=](APlant* plt) {
            return AGetIntersection(APlantType(plt->Type()), tempPlants).empty();
        });
        for (auto row : fodderRows) {
            tempCol = 0;
            for (auto&& plt : permanentPlant) {
                if (plt.Row() == row - 1) {
                    int pltCol = plt.Type() == ACOB_CANNON ? plt.Col() + 3 : plt.Col() + 2;
                    tempCol = pltCol > tempCol ? pltCol : tempCol;
                }
            }
            if (tempCol > 0 && tempCol <= 9) {
                AGrid tempAGrid = {row, tempCol};
                _grids.emplace_back(tempAGrid);
            }
        }
    }

    // 僵尸对某列普通植物产生威胁时的坐标偏移量，其中撑杆和海豚的状态视为跳前，跳跳的状态视为跳跃
    const std::vector<int> zombiesAttackAbscissaOffsetLowBound = {-80, -80, -80, -51, -80, -80, -80, -80, -80, -80, -80, -60, 0, 0, -51, -80, 0, 0, -50, 0, 0, -70, 0, -70, -80, 0, 0, 0, 0, 0, 0, 0, -70};
    const std::vector<int> zombiesAttackAbscissaOffsetUpBound = {-10, -10, -10, 39, -10, -10, -10, -40, -40, -40, -10, 15, 0, 0, 39, -10, 0, 0, 0, 0, 0, 0, 0, 40, -40, 0, 0, 0, 0, 0, 0, 0, 40};
    std::vector<AGrid> targetGrids = {};
    for (auto each : _grids) {
        AAliveFilter<AZombie> zombieAtGrid([=](AZombie* zmb) {
            return !AGetIntersection(AZombieType(zmb->Type()), _zombies).empty() && zmb->Row() == each.row - 1 && (int(zmb->Abscissa()) >= each.col * 80 + zombiesAttackAbscissaOffsetLowBound[zmb->Type()] && int(zmb->Abscissa()) <= each.col * 80 + zombiesAttackAbscissaOffsetUpBound[zmb->Type()] - bufferPx);
        });
        for (auto&& zmb : zombieAtGrid) {
            (void)zmb;
            targetGrids.emplace_back(each);
            break;
        }
    }

    if (isSmartFlowerpot && !AGetIntersection(_plants, AFLOWER_POT).empty() && targetGrids.size() > 1) {
        AMoveElement(_plants, AFLOWER_POT, 0);
        auto maxDangerGridPtr = std::max_element(targetGrids.begin(), targetGrids.end(), [=](AGrid a, AGrid b) {
            auto GetZombieCanEatNum = [=](AGrid grid) -> float {
                float result = 0;
                const std::vector<AZombieType> zombiesCanEat = {AZOMBIE, AFLAG_ZOMBIE, ACONEHEAD_ZOMBIE, ABUCKETHEAD_ZOMBIE, ANEWSPAPER_ZOMBIE, ASCREEN_DOOR_ZOMBIE, AFOOTBALL_ZOMBIE, ADANCING_ZOMBIE, ABACKUP_DANCER, AJACK_IN_THE_BOX_ZOMBIE, AZOMBIE_YETI, ALADDER_ZOMBIE, AIMP};
                AAliveFilter<AZombie> zombieCanEatAtGrid([=](AZombie* zmb) {
                    return !AGetIntersection(AZombieType(zmb->Type()), zombiesCanEat).empty() && zmb->Row() == grid.row - 1 && (int(zmb->Abscissa()) >= grid.col * 80 + zombiesAttackAbscissaOffsetLowBound[zmb->Type()] && int(zmb->Abscissa()) <= grid.col * 80 + zombiesAttackAbscissaOffsetUpBound[zmb->Type()]);
                });
                for (auto&& zmb : zombieCanEatAtGrid)
                    result += zmb.SlowCountdown() > 0 ? 0.5 : 1;

                return result;
            };
            return GetZombieCanEatNum(a) < GetZombieCanEatNum(b);
        });
        AGrid tempAGrid = *(maxDangerGridPtr);
        AMoveElement(targetGrids, tempAGrid, 0);
    }

    bool isNeedPlant = true;
    for (auto each : targetGrids) {
        isNeedPlant = true;
        if (isSynchronizeGargantuar && !AGetIntersection(_zombies, {AGARGANTUAR, AGIGA_GARGANTUAR}).empty()) {
            AAliveFilter<AZombie> gargantuarAtGrid([=](AZombie* zmb) {
                return !AGetIntersection(AZombieType(zmb->Type()), {AGARGANTUAR, AGIGA_GARGANTUAR}).empty() && zmb->Row() == each.row - 1 && (int(zmb->Abscissa()) >= each.col * 80 + zombiesAttackAbscissaOffsetLowBound[zmb->Type()] && int(zmb->Abscissa()) <= each.col * 80 + zombiesAttackAbscissaOffsetUpBound[zmb->Type()]);
            });
            for (auto&& zmb : gargantuarAtGrid) {
                if (zmb.State() == 70) {
                    isNeedPlant = false;
                    break;
                }
            }
        }
        if (!isNeedPlant)
            continue;

        for (auto plt : _plants) {
            auto AGetIsExistPlant = [](int row, int col) -> bool {
                for (auto&& plt : aAlivePlantFilter) {
                    if (plt.Row() == row - 1 && plt.Col() == col - 1)
                        return true;
                }
                return false;
            };
            if (AIsSeedUsable(plt) && !AGetIsExistPlant(each.row, each.col) && AAsm::GetPlantRejectType(plt, each.row - 1, each.col - 1) == AAsm::NIL) {
                ACard(plt, each.row, each.col);
                auto GetIsCardSuccessful = [=](APlantType plt, AGrid grid) -> bool {
                    if (int(plt) > int(AIMITATOR))
                        return AGetPlantIndex(each.row, each.col, AIMITATOR) >= 0;
                    else
                        return AGetPlantIndex(each.row, each.col, plt) >= 0;
                };
                if (removalDelay >= 0 && GetIsCardSuccessful(plt, each)) {
                    AConnect(ANowDelayTime(removalDelay), [=] {
                        ARemovePlant(each.row, each.col);
                    });
                }
            }
        }
    }
}
}

using _qrmd::AAutoFodder;