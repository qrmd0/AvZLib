#include "avz.h"
#include <vector>
#include "PlantFixerPlus.h"
#include "dance_cheat.h"

using namespace AvZ;
PlantFixerPlus Cob_Cannon_fixer;
TickRunner fixer_manager;
TickRunner smart_blover;

// *** Not In Queue
// 返回[type]僵尸在[wave]波的刷新数量
//检测夹零用的判断本波红眼函数
int GetZombieNumber(int type, int wave)
{
    int result = 0;
    auto zombie_list = GetMainObject()->zombieList();
    for (int i = (wave - 1) * 50; i < wave * 50; ++i)
    {
        if (*(zombie_list + i) == type)
        {
            result += 1;
        }
    }
    return result;
}

bool IsHYExist(int row, int hp) //检查红眼状态
{
    bool result = false;
    auto zombie = GetMainObject()->zombieArray();
    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index)
    {
        if (zombie[index].type() == GIGA_GARGANTUAR && zombie[index].isExist() && !zombie[index].isDead() && zombie[index].row() == row - 1 && zombie[index].hp() >= hp && zombie[index].state() != 70)
        {
            result = true;
        }
    }
    return result;
}

bool IsBYExist(int row, int hp) //检查白眼状态
{
    bool result = false;
    auto zombie = GetMainObject()->zombieArray();
    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index)
    {
        if (zombie[index].type() == GARGANTUAR && zombie[index].isExist() && !zombie[index].isDead() && zombie[index].row() == row - 1 && zombie[index].hp() >= hp && zombie[index].state() != 70)
        {
            result = true;
        }
    }
    return result;
}

bool IsSLExist(int row, int hp) //检查水路僵尸状态
{
    bool result = false;
    auto zombie = GetMainObject()->zombieArray();
    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index)
    {
        if (zombie[index].isExist() && !zombie[index].isDead() && zombie[index].row() == row - 1 && zombie[index].hp() >= hp)
        {
            result = true;
        }
    }
    return result;
}

bool IsTTExist(int row, int hp) //检查铁桶状态
{
    bool result = false;
    auto zombie = GetMainObject()->zombieArray();
    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index)
    {
        if (zombie[index].type() == BUCKETHEAD_ZOMBIE && zombie[index].isExist() && !zombie[index].isDead() && zombie[index].row() == row - 1 && zombie[index].hp() >= hp && zombie[index].state() != 2)
        {
            result = true;
        }
    }
    return result;
}

bool IsPJExist(int row, int hp) //检查普僵状态
{
    bool result = false;
    auto zombie = GetMainObject()->zombieArray();
    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index)
    {
        if (zombie[index].type() == ZOMBIE && zombie[index].isExist() && !zombie[index].isDead() && zombie[index].row() == row - 1 && zombie[index].hp() >= hp && zombie[index].state() != 2)
        {
            result = true;
        }
    }
    return result;
}

bool IsLZExist(int row, int hp) //检查路障状态
{
    bool result = false;
    auto zombie = GetMainObject()->zombieArray();
    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index)
    {
        if (zombie[index].type() == CONEHEAD_ZOMBIE && zombie[index].isExist() && !zombie[index].isDead() && zombie[index].row() == row - 1 && zombie[index].hp() >= hp && zombie[index].state() != 2)
        {
            result = true;
        }
    }
    return result;
}

bool IsDBExist(int row, int hp) //检查读报状态
{
    bool result = false;
    auto zombie = GetMainObject()->zombieArray();
    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index)
    {
        if (zombie[index].type() == NEWSPAPER_ZOMBIE && zombie[index].isExist() && !zombie[index].isDead() && zombie[index].row() == row - 1 && zombie[index].hp() >= hp && zombie[index].state() != 2)
        {
            result = true;
        }
    }
    return result;
}

bool IsTMExist(int row, int hp) //检查铁门状态
{
    bool result = false;
    auto zombie = GetMainObject()->zombieArray();
    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index)
    {
        if (zombie[index].type() == SCREEN_DOOR_ZOMBIE && zombie[index].isExist() && !zombie[index].isDead() && zombie[index].row() == row - 1 && zombie[index].hp() >= hp && zombie[index].state() != 2)
        {
            result = true;
        }
    }
    return result;
}

bool IsCGExist(int row, int hp) //检查撑杆状态
{
    bool result = false;
    auto zombie = GetMainObject()->zombieArray();
    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index)
    {
        if (zombie[index].type() == POLE_VAULTING_ZOMBIE && zombie[index].isExist() && !zombie[index].isDead() && zombie[index].row() == row - 1 && zombie[index].hp() >= hp && zombie[index].state() != 2)
        {
            result = true;
        }
    }
    return result;
}

//当气球僵尸飞过6.5列时使用三叶草，必须写进TickRunner的pushFunc以使其时刻生效
void KillBallon()
{
    auto zombie_array = GetMainObject()->zombieArray();
    auto seed_array = GetMainObject()->seedArray();
    bool is_clover_usable = seed_array[GetSeedIndex(27)].isUsable();
    if (is_clover_usable)
    {
        for (int index = 0; index < GetMainObject()->zombieTotal(); ++index)
        {
            if (zombie_array[index].isExist() && zombie_array[index].type() == QQ_16 && !zombie_array[index].isDead() && zombie_array[index].abscissa() <= 6.5 * 80)
            {
                {
                    //确保操作成功插入进去
                    InsertGuard ig(false);
                    //使用三叶草，坐标可改
                    Card(BLOVER, 1, 9);
                }
                break;
            }
        }
    }
}

//智能收尾
void Endwave()
{
    for (auto wave : {9, 19, 20})
    {
        SetTime(341 - 3 - 373, wave);
        pao_operator.pao({{2, 9}, {5, 9}});
        Delay(500);
        InsertOperation([=]()
                        {
            if (IsHYExist(1, 3500) || IsHYExist(2, 3500) || IsHYExist(5, 3500) || IsHYExist(6, 3500)) //有无本波红眼
            {
                SetNowTime();
                pao_operator.recoverPao({{2, 9}, {5, 9}, {2, 9}});
                Delay(500);
                InsertOperation([=]()
                        {
                            if (IsHYExist(1, 0) || IsHYExist(2, 0) || IsHYExist(5, 0) || IsHYExist(6, 0))
                            {
                                SetNowTime();
                                pao_operator.recoverPao({{2, 9}, {5, 9}, {5, 9}});
                            }
                        });
            }
            else if (IsHYExist(1, 1800) || IsHYExist(2, 1800) || IsHYExist(5, 1800) || IsHYExist(6, 1800))
            {
                SetNowTime();
                pao_operator.recoverPao({{2, 9}, {5, 9}});
                Delay(500);
                InsertOperation([=]()
                        {
                            if (IsHYExist(1, 0) || IsHYExist(2, 0) || IsHYExist(5, 0) || IsHYExist(6, 0))
                            {
                                SetNowTime();
                                Card({{LILY_PAD, 3, 9}, {DOOM_SHROOM, 3, 9}, {COFFEE_BEAN, 3, 9}});
                            }
                        });
            }

            else if (IsBYExist(1, 500) || IsBYExist(2, 500) || IsBYExist(5, 500) || IsBYExist(6, 500))
            {
                SetNowTime();
                pao_operator.pao({{2, 9}, {5, 9}});
            }

            else if (IsTTExist(1, 0) || IsTTExist(2, 0) || IsTTExist(5, 0) || IsTTExist(6, 0) ||//铁桶是否存在
                     IsSLExist(3, 0) || IsSLExist(4, 0) ||                                      //水路僵尸是否存在
                     IsPJExist(1, 0) || IsPJExist(2, 0) || IsPJExist(5, 0) || IsPJExist(6, 0) ||//普僵是否存在
                     IsLZExist(1, 0) || IsLZExist(2, 0) || IsLZExist(5, 0) || IsLZExist(6, 0) ||//路障是否存在
                     IsDBExist(1, 0) || IsDBExist(2, 0) || IsDBExist(5, 0) || IsDBExist(6, 0) ||//读报是否存在
                     IsTMExist(1, 0) || IsTMExist(2, 0) || IsTMExist(5, 0) || IsTMExist(6, 0) ||//铁门是否存在
                     IsCGExist(1, 0) || IsCGExist(2, 0) || IsCGExist(5, 0) || IsCGExist(6, 0))  //撑杆是否存在
                    {
                        SetNowTime();
                        pao_operator.pao({{2, 9}, {5, 9}});
                    } });
    }
}
void Script()
{
    //连续运行(摁0停止运行，但是再次摁0不会重新启动)
    OpenMultipleEffective('0', AvZ::MAIN_UI_OR_FIGHT_UI);

    //选卡（除了玉米加农炮套件、樱桃、荷叶、三叶草必带，其他可以替换）
    SelectCards({
        KERNEL_PULT,   //玉米投手
        COB_CANNON,    //玉米加农炮
        M_KERNEL_PULT, //模仿玉米投手
        DOOM_SHROOM,   //核弹
        LILY_PAD,      //荷叶
        ICE_SHROOM,    //寒冰菇
        CHERRY_BOMB,   //樱桃
        COFFEE_BEAN,   //咖啡豆
        PUMPKIN,       //南瓜头
        BLOVER,        //阳光菇
    });

    //八倍速冲关
    SetGameSpeed(8);

    //智能三叶草，pushfunc是not in queue属性，因此必须在前面写insertoperation
    InsertOperation([=]()
                    { smart_blover.pushFunc(KillBallon); });

    //开局自动检测炮位
    SetTime(-599, 1);
    pao_operator.autoGetPaoList();

    int GGnumber = 0;

    DanceCheat(FAST);

    //智能修补位于{1，1}，{2，1}，{3，1}，{4，1}的玉米加农炮，血量阈值为90，同时使用模仿种子
    Cob_Cannon_fixer.start(COB_CANNON, {{1, 1}, {2, 1}, {5, 1}, {6, 1}}, 0.3);

    for (auto wave : {1, 4, 7, 10, 13, 16})
    {
        GGnumber = GGnumber + GetZombieNumber(GIGA_GARGANTUAR, wave);
        SetTime(341 - 5 - 373, wave);
        InsertOperation([=]()
                        {
        if (GGnumber >= 50)
        {
            SetTime(341 - 3 - 373, wave);
            pao_operator.pao({{2, 9}, {5, 9}});
        }
        else
        {
            SetTime(341 - 3 - 373, wave);
            pao_operator.pao({{2, 9}, {5, 9}});
            Delay(100);
            pao_operator.pao({{2, 9}});
        } });
    }

    SetTime(-199, 9);
    GGnumber = GGnumber + GetZombieNumber(GIGA_GARGANTUAR, 9);

    // P6循环，发三炮，两炮炸下
    for (auto wave : {2, 5, 8, 11, 14, 17})
    {
        GGnumber = GGnumber + GetZombieNumber(GIGA_GARGANTUAR, wave);
        SetTime(341 - 5 - 373, wave);
        InsertOperation([=]()
                        {
        if (GGnumber >= 50)
        {
            SetTime(341 - 3 - 373, wave);
            pao_operator.pao({{2, 9}, {5, 9}});
        }
        else
        {
            SetTime(341 - 3 - 373, wave);
            pao_operator.pao({{2, 9}, {5, 9}});
            Delay(100);
            pao_operator.pao({{5, 9}});
        } });
    }

    for (auto wave : {3, 6, 12, 15, 18})
    {
        SetTime(341 - 3 - 373, wave);
        pao_operator.pao({{2, 9}, {5, 9}});
    }

    //智能收尾（我也不知道能不能写for）
    Endwave();

    //炮消珊瑚，注意，这里时机一定是-128，-150不能使用这个炸点，且必须炸4路，炸3路会漏
    SetTime(-128, 20);
    pao_operator.pao({{4, 7.5875}});

    //每波检测刷新延迟，如果有延迟且樱桃可用，就放樱桃
    for (auto wave : {1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 12, 13, 14, 15, 16, 17, 18})
    {
        SetTime(601, wave);
        InsertOperation([=]()
                        {
                        auto seed= GetMainObject()->seedArray();  //种子内存
                        auto YT_seed=GetMainObject()->seedArray() + GetSeedIndex(CHERRY_BOMB);
                        auto zombie = GetMainObject()->zombieArray();
                        //检测是否延迟刷新
                        if (GetMainObject()->refreshCountdown() >601 - 200 + 1 && wave == GetMainObject()->wave()) 
                        {
                        SetNowTime();
                        //检测樱桃是否可用
                        if (YT_seed[CHERRY_BOMB].isUsable())
                        {
                            //樱桃放在（5,9），因为大多数是中场可能延迟刷新，而中场我是选择炸2路，所以智能樱桃放5路
                            Card({{CHERRY_BOMB, 5, 9}});
                        }
                        } });
    }
}