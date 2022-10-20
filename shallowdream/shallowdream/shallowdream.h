/*
    本部分是我的常用函数
    作者：B站  shallow——dream
    使用方法：   将该文件放入安装目录下的inc文件夹中
                在开头写上 #include <shallowdream.h>即可


    插件内容有：
                红眼计数器(int count())
                智能三叶草(void SanYeCao())
                检测某波红眼数量(int GetZombieNumber(type, wave))
                检测红眼是否存在(bool IsHYExist(row, hp))
                检测白眼是否存在(bool IsBYExist(row, hp))
                检测慢速僵尸是否存在(bool IsMSExist(row, hp))
                炮消珊瑚(void PXSH())
                手动设置游戏速度(void GameSpeed())
                智能樱桃处理中场刷新延迟(void DealYanchi)

    2022.10.4更新
    更新内容：添加函数
                检测冰车是否存在(void IsBCExist)
                中场智能冰杀小偷(void DealBUNGEE)
                检测蹦极是否存在(void IsBJExist)
             函数修改
                单独列出撑杆是否存在函数(void IsCGExist)

    2022.10.8更新
    更新内容：添加函数
                重新寻找炮并发炮(void FindAndPao(pao_operator, {{row, col}}))
                重新寻找炮，等待炮恢复完毕，发炮(void FindAndrecovverPao(pao_operator, {{row, col}}))
                对每个函数添加示例，对无法准确表达功能的函数添加功能
    version 2022_10_20
*/

#include "avz.h"
#include <vector>
using namespace AvZ;

int count(int Wave);
void SanYeCao();
int GetZombieNumber(int type, int wave);
void GameSpeed();
int GetZombieNumber(int type, int wave);
bool IsHYExist(int row, int hp);
bool IsBYExist(int row, int hp);
bool IsMSExist(int row, int hp);
void PXSH();
void DealYanchi();
bool IsBCExist(int row, int hp);
bool IsBJExist(int row, int abs, int hp);
bool IsCGExist(int row, int hp);
void DealBungee();
void FindAndPao(PaoOperator &_pao_operator, int row, float col);
void FindAndPao(PaoOperator &_pao_operator, const std::vector<Position> &lst);
void FindAndrecoverPao(PaoOperator &_pao_operator, int row, float col);
void FindAndrecoverPao(PaoOperator &_pao_operator, const std::vector<Position> &lst);

// 红眼计数器
// 示例：count(9);
// 功能：检测并返回第9波红眼的数量
int count(int Wave)
{
    InsertTimeOperation(1, Wave, [=]()
                        {
                            int num = 0;
                            auto zombie_list = GetMainObject()->zombieArray();
                            int zombie_count = GetMainObject()->zombieTotal();
                            for (int index = 0; index < zombie_count; ++index)
                            {
                                if (zombie_list[index].hp() == 6000)
                                {
                                    num = num + 1;
                                }
                            }
                            return num; });
}

// 当气球僵尸飞过6.5列时使用三叶草，必须写进TickRunner的pushFunc以使其时刻生效
// 示例：InsertOperation([=](){smart_blover.pushFunc(KillBallon);});
// 功能：当气球僵尸飞过6.5列时，每帧检测三叶草卡片是否可用，若可用则放于(2, 1)
void SanYeCao()
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
                    Card(SYC_27, 2, 7);
                }
                break;
            }
        }
    }
}

// *** Not In Queue
// 返回[type]僵尸在[wave]波的刷新数量
// 示例：GetZombieNumber(HY_32, 9);
// 功能：检测并返回第9波的红眼数量
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

// 手动设置游戏速度
// 示例：GameSpeed();
// 功能：按下对应的数字键，将游戏速度更改为对应的倍速
// 注意：按下右边小键盘无效，只有按下上方的数字键才生效，生效时间为本次选卡的剩余时间
void GameSpeed()
{
    KeyConnect('1', [=]()
               { SetGameSpeed(1); });
    KeyConnect('2', [=]()
               { SetGameSpeed(2); });
    KeyConnect('3', [=]()
               { SetGameSpeed(3); });
    KeyConnect('4', [=]()
               { SetGameSpeed(4); });
    KeyConnect('5', [=]()
               { SetGameSpeed(5); });
    KeyConnect('6', [=]()
               { SetGameSpeed(6); });
    KeyConnect('7', [=]()
               { SetGameSpeed(7); });
    KeyConnect('8', [=]()
               { SetGameSpeed(8); });
}

// 判断红眼是否存在
// 示例：IsHYExist(1, 0);
// 功能：检测1路是否有血量大于0的红眼僵尸
bool IsHYExist(int row, int hp)
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

// 判断白眼是否存在
// 示例：IsBYExist(1, 0);
// 功能：检测1路是否有血量大于0的伽刚特尔僵尸
bool IsBYExist(int row, int hp)
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

// 判断慢速僵尸是否存在
// 示例：IsMSExist(1, 0);
// 功能：检测1路是否有血量大于0的慢速僵尸
bool IsMSExist(int row, int hp)
{
    bool result = false;
    bool result2 = false;
    bool result3 = false;
    bool result4 = false;
    bool result5 = false;
    bool result6 = false;
    auto zombie = GetMainObject()->zombieArray();

    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index)
    {
        //铁门僵尸
        if (zombie[index].type() == BUCKETHEAD_ZOMBIE && zombie[index].isExist() && !zombie[index].isDead() && zombie[index].row() == row - 1 && zombie[index].hp() >= hp && zombie[index].state() != 2)
        {
            result2 = true;
        }

        //普通僵尸
        if (zombie[index].type() == ZOMBIE && zombie[index].isExist() && !zombie[index].isDead() && zombie[index].row() == row - 1 && zombie[index].hp() >= hp && zombie[index].state() != 2)
        {
            result3 = true;
        }

        //路障僵尸
        if (zombie[index].type() == CONEHEAD_ZOMBIE && zombie[index].isExist() && !zombie[index].isDead() && zombie[index].row() == row - 1 && zombie[index].hp() >= hp && zombie[index].state() != 2)
        {
            result4 = true;
        }

        //读报僵尸
        if (zombie[index].type() == NEWSPAPER_ZOMBIE && zombie[index].isExist() && !zombie[index].isDead() && zombie[index].row() == row - 1 && zombie[index].hp() >= hp && zombie[index].state() != 2)
        {
            result5 = true;
        }

        //铁桶僵尸
        if (zombie[index].type() == SCREEN_DOOR_ZOMBIE && zombie[index].isExist() && !zombie[index].isDead() && zombie[index].row() == row - 1 && zombie[index].hp() >= hp && zombie[index].state() != 2)
        {
            result6 = true;
        }
    }

    if (result2 || result3 || result4 || result5 || result6)
    {
        result = true;
    }
    return result;
}

// 炮消珊瑚
// 示例：PXSH();
void PXSH()
{
    SetTime(-128, 20);
    pao_operator.pao({{4, 7.6}});
}

// 智能樱桃处理中场刷新延迟
// 注意：本函数只对出红眼的关卡处理中场
// 示例：DealYanChi();
void DealYanchi()
{
    auto zombie_type = GetMainObject()->zombieTypeList();
    SetTime(1, 10);
    InsertOperation([=]()
                    {
        int GGCount_up = 0;
        int GGCount_down = 0;
        auto zombie = GetMainObject()->zombieArray();
        for (int index = 0; index < GetMainObject()->zombieTotal(); ++index)
        {
            if (zombie[index].type() == GIGA_GARGANTUAR && zombie[index].isExist() && !zombie[index].isDead() && (zombie[index].row() == 0 || zombie[index].row() == 1)) {
                GGCount_up = GGCount_up + 1;
            }
        }
        for (int index = 0; index < GetMainObject()->zombieTotal(); ++index)
        {
            if (zombie[index].type() == GIGA_GARGANTUAR && zombie[index].isExist() && !zombie[index].isDead() && (zombie[index].row() == 4 || zombie[index].row() == 5)) {
                GGCount_down = GGCount_down + 1;
            }
        }
        SetTime(401, 10);
        InsertOperation([=]()
                    {
        if (GGCount_up < GGCount_down && zombie_type[GIGA_GARGANTUAR] && GetMainObject()->refreshCountdown() > 200)
        {
            SetNowTime();
            Card(CHERRY_BOMB, 5, 9);
        }
        
        else if (GGCount_up >= GGCount_down && zombie_type[GIGA_GARGANTUAR] && GetMainObject()->refreshCountdown() > 200)
        {
            SetNowTime();
            Card(CHERRY_BOMB, 2, 9);
        }
                    }
                        ); });
}

// 判断冰车是否存在
// 示例：IsBCExist(1, 0);
// 功能：检测1路是否有血量大于0的冰车僵尸
bool IsBCExist(int row, int hp)
{
    bool result = false;
    auto zombie = GetMainObject()->zombieArray();
    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index)
    {
        if (zombie[index].type() == ZOMBONI && zombie[index].isExist() && !zombie[index].isDead() && zombie[index].row() == row - 1 && zombie[index].hp() >= hp && zombie[index].state() != 2)
        {
            result = true;
        }
    }
    return result;
}

// 判断撑杆是否存在
// 示例：IsBJExist(1, 0);
// 功能：检测1路是否有血量大于0的撑杆僵尸
bool IsCGExist(int row, int hp)
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

// 中场智能冰杀小偷
// 示例：DealBunGee();
void DealBungee()
{
    for (auto wave : {10, 20})
    {
        if (GetZombieTypeList()[BUNGEE_ZOMBIE])
        {
            SetTime(100, wave);
            Card({{LILY_PAD, 1, 9}, {ICE_SHROOM, 3, 9}, {COFFEE_BEAN, 3, 9}});
            Delay(400);
            Shovel({{3, 9}});
        }
    }
}

// 检测蹦极是否存在
// 示例：IsBJExist(1, 80, 0);
// 功能：检测(1, 1)位置有无血量大于0的蹦极僵尸
bool IsBJExist(int row, int abs, int hp)
{
    bool result = false;
    auto zombie = GetMainObject()->zombieArray();
    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index)
    {
        if (zombie[index].type() == BUNGEE_ZOMBIE && zombie[index].isExist() && !zombie[index].isDead() && zombie[index].row() == row - 1 && zombie[index].abscissa() < abs && zombie[index].hp() >= hp && zombie[index].state() != 2)
        {
            result = true;
        }
    }
    return result;
}

// *** In Queue
// 更新炮列表并发炮
// 示例：FindAndPao(pao_operator, 2, 9);
// 功能：重新寻找炮并向（2, 9）位置发炮
void FindAndPao(PaoOperator &_pao_operator, int row, float col)
{
    _pao_operator.autoGetPaoList();
    _pao_operator.pao(row, col);
}
// *** In Queue
// 更新炮列表并发炮
// 示例：FindAndPao(pao_operator, {{2, 9}, {5, 9}});
// 功能：重新寻找炮并向（2, 9）和（5, 9）位置发炮
void FindAndPao(PaoOperator &_pao_operator, const std::vector<Position> &lst)
{
    for (const auto &each : lst)
    {
        FindAndPao(_pao_operator, each.row, each.col);
    }
}

// *** In Queue
// 更新炮列表,等待炮回复完毕,发炮
// 示例：FindAndrecoverPao(pao_operator, 2, 9);
// 功能：重新寻找炮，等待炮回复完毕，并向（2, 9）位置发炮
void FindAndrecoverPao(PaoOperator &_pao_operator, int row, float col)
{
    _pao_operator.autoGetPaoList();
    _pao_operator.recoverPao(row, col);
}
// *** In Queue
// 更新炮列表,等待炮回复完毕,发炮
// 示例：FindAndrecoverPao(pao_operator, {{2, 9}, {5, 9}});
// 功能：重新寻找炮，等待炮回复完毕，向（2, 9）和（5, 9）位置发炮
void FindAndrecoverPao(PaoOperator &_pao_operator, const std::vector<Position> &lst)
{
    for (const auto &each : lst)
    {
        FindAndPao(_pao_operator, each.row, each.col);
    }
}