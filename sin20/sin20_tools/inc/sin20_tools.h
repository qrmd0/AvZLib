/*
 * @Author: SKOSKX
 * @Date: 2023-01-13
 * @Description: 反正是杂七杂八的一些东西……
 */

#ifndef _SIN20_TOOLS_H_
#define _SIN20_TOOLS_H_
#include "avz.h"
#include "avz_more.h"

using namespace AvZ;

bool temPause = false;
namespace AvZ
    { extern bool __is_advanced_pause; }

// *** Not In Queue
// 启用部分A-TAS功能（键位可以自行修改）
// 1-女仆-前进，2-女仆-跳舞，3-女仆-召唤，W-女仆-停用
// Z-高级暂停，X-延时1cs高级暂停，C-1/4倍速，N-2倍速，M-5倍速，B-置顶卡槽
void A_TAS();

namespace sin20 {

    // 检查指定格是否有墓碑
    // 使用了avz_more.h的场地物品内存数组placeArray
    bool CheckGrave(int row, int col);

    // 检查指定格是否有弹坑
    // 使用了avz_more.h的场地物品内存数组placeArray
    bool CheckCrater(int row, int col);

    // FogAdjuster配件
    struct FogAdjustSet {
        int row;                //行
        int col;                //列
        int fog_upperlim = 102; //浓度
    };

    class FogAdjuster : public TickRunner {
    public:
        // *** In Queue
        // 调整雾浓度
        // 雾浓度范围为0 ~ 255，游戏默认设置为255，本功能默认调整为102（即40%）
        // 可以不填或只填入浓度，将会修改全部雾的浓度；也可以填入行、列、浓度三个参数（浓度可不填，默认102），将修改具体格子的浓度
        // 避免将它与修改器的“全屏浓雾”功能一起使用
        // *** 使用示例：
        // fog_adjuster.start(); ——修改全部雾的浓度为40%
        // fog_adjuster.start(170); ——修改全部雾的浓度为66.7%
        // fog_adjuster.start({{2, 9}, {5, 9}}); ——修改岸路9列雾的浓度为40%
        // fog_adjuster.start({{2, 9, 170}, {5, 9, 170}}); ——修改岸路9列雾的浓度为66.7%
        void start(int concentration = 102);

        void start(const std::vector<FogAdjustSet> &lst);
    };

    FogAdjuster fog_adjuster;

    // *** Not In Queue
    // 人为控制旗帜波蹦极僵尸们偷的格子包含[row]行[col]列
    // 可以选择在第10波、第20波、第(10和20)波控制蹦极
    // 该函数改写自Elovi所作火中取栗阵型的蹦极控制部分。这里向他表示感谢
    // *** 使用示例：
    // BungeeManipulate(3, 4); ——令W10必有蹦极偷3-4
    // BungeeManipulate(3, 4, {10, 20}); ——令W10和W20都必有蹦极偷3-4
    // BungeeManipulate({{3, 4}, {4, 3}}, {20}); ——令W20必有蹦极偷3-4和4-3
    // *** 注意：请确保蹦极僵尸数量足够偷这些位置。例如，如果填写了4个格子，但只出了3只蹦极，那么蹦极将不够用，因而函数无法生效。可以使用全麦写的SetZombiesLimits函数来保证蹦极充足。
    void BungeeManipulate(int row, int col, std::vector<int> mpwaves = {10});

    void BungeeManipulate(const std::vector<Grid> &lst, std::vector<int> mpwaves = {10});

    // *** In Queue
    // 模拟神炮，向场外指定落点强制发炮
    // 第四个参数[x]的取值范围为0x0010000至0x2FFFFFF
    // *** 使用示例：
    // PaoToOutside(3, 1, 2, 0x20ABCD); ——将3-1的炮强制发射至2路x=0x20ABCD处
    void PaoToOutside(int pao_row, int pao_col, int row, int x);

    //行出怪属性，依次为不出怪、按陆路出怪、按水路出怪
    enum RowType { NONE, LAND, AQUA };

    // *** In Queue
    // 修改指定行的出怪属性
    void SetRowSpawnType(const std::vector<int> &lst, RowType type = NONE);

    // *** Not In Queue
    // 返回炸[pao_row]路的炮弹全收第[wave]波[pogo_row]路跳跳所需的爆心x范围
    // 若无法全收，返回{0, 0}；参数[wave]填-1表示任意波
    std::pair<int, int> PaoxForPogo(int pogo_row, int pao_row, bool isShowInfo = true, int wave = -1);

    // *** In Queue
    // 设置音乐
    // 1-DE 2-NE 3-PE 4-FE 5-RE 8-禅境花园 11-x-10 12-僵王关
    void SetMusic(int musicId = 12);

    /* >>> https://www.bilibili.com/video/BV1d54y1n7NX nya~ */

    bool CheckGrave(int row, int col)
    {
        auto place = ((cresc::MainObjectAZM *)GetMainObject())->placeArray();
        for (int i = 0; i < ((cresc::MainObjectAZM *)GetMainObject())->placeTotal(); ++i, ++place)
        {
            if (!place->isDisappeared() && place->row() == row - 1 &&
                place->col() == col - 1 && place->type() == 1)
                return true;
        }
        return false;
    }
    bool CheckCrater(int row, int col)
    {
        auto place = ((cresc::MainObjectAZM *)GetMainObject())->placeArray();
        for (int i = 0; i < ((cresc::MainObjectAZM *)GetMainObject())->placeTotal(); ++i, ++place)
        {
            if (!place->isDisappeared() && place->row() == row - 1 &&
                place->col() == col - 1 && place->type() == 2)
                return true;
        }
        return false;
    }

    // 该函数是每个单帧的操作，故需要配合TickRunner帧运行功能来生效
    void FogAdjust(int fog_upperlim)
    {
        if (fog_upperlim < 0 || fog_upperlim > 255)
        {
            ShowErrorNotInQueue("雾浓度范围为0 ~ 255，您填写了 # ！", fog_upperlim);
            return;
        }
        for (int i = 0x4C8; i <= 0x5C0; i += 4)
        {
            if ((i < 0x554 || i > 0x56C) &&
                GetMainObject()->mRef<int>(i) > fog_upperlim - 3)
            {
                GetMainObject()->mRef<int>(i) -= 6;
                if (GetMainObject()->mRef<int>(i) < fog_upperlim - 3)
                    GetMainObject()->mRef<int>(i) = fog_upperlim - 3;
            }
            else if ((i >= 0x554 && i <= 0x56C) &&
                    GetMainObject()->mRef<int>(i) > fog_upperlim * 200 / 255 - 3)
            {
                GetMainObject()->mRef<int>(i) -= 6;
                if (GetMainObject()->mRef<int>(i) < fog_upperlim * 200 / 255 - 3)
                    GetMainObject()->mRef<int>(i) = fog_upperlim * 200 / 255 - 3;
            }
        }
    }

    void FogAdjust(const std::vector<FogAdjustSet> &fog_adjust_set)
    {
        for (FogAdjustSet each : fog_adjust_set)
        {
            if (each.fog_upperlim < 0 || each.fog_upperlim > 255)
            {
                ShowErrorNotInQueue("雾浓度范围为0 ~ 255，您填写了 # ！", each.fog_upperlim);
                return;
            }
            int gridId = 0x4C8 + (each.col - 1) * 28 + (each.row - 1) * 4;
            if ((gridId < 0x554 || gridId > 0x56C) &&
                GetMainObject()->mRef<int>(gridId) > each.fog_upperlim - 3)
            {
                GetMainObject()->mRef<int>(gridId) -= 6;
                if (GetMainObject()->mRef<int>(gridId) < each.fog_upperlim - 3)
                    GetMainObject()->mRef<int>(gridId) = each.fog_upperlim - 3;
            }
            else if ((gridId >= 0x554 && gridId <= 0x56C) &&
                    GetMainObject()->mRef<int>(gridId) > each.fog_upperlim * 200 / 255 - 3)
            {
                GetMainObject()->mRef<int>(gridId) -= 6;
                if (GetMainObject()->mRef<int>(gridId) < each.fog_upperlim * 200 / 255 - 3)
                    GetMainObject()->mRef<int>(gridId) = each.fog_upperlim * 200 / 255 - 3;
            }
        }
    }

    void FogAdjuster::start(int concentration)
    {
        InsertOperation([=](){
            pushFunc([=](){ FogAdjust(concentration); });
        });
    }

    void FogAdjuster::start(const std::vector<FogAdjustSet>& lst)
    {
        InsertOperation([=](){
            pushFunc([=](){ FogAdjust(lst); });
        });
    }

    std::vector<Grid> grids; // BungeeManipulate函数配件

    // BungeeManipulate函数配件，将蹦极僵尸的行列转换为横纵坐标
    std::pair<int, int> BungeeCoord(int row, int col)
    {
        auto scene = GetMainObject()->scene();
        if (RangeIn(scene, {0, 1, 6, 8})) // DE、NE、MGE、AQE
            return {40 + (col - 1) * 80, 50 + (row - 1) * 100};
        if (scene == 2 || scene == 3) //后院
            return {40 + (col - 1) * 80, 50 + (row - 1) * 85};
        if (scene == 4 || scene == 5) //屋顶
        {
            int slopeHeight = 0;
            if (col <= 5)
                slopeHeight = 10 + 20 * (5 - col);
            return {40 + (col - 1) * 80, 40 + (row - 1) * 85 + slopeHeight};
        }
    }

    void BungeeManipulate(const std::vector<Grid> &lst, std::vector<int> mpwaves)
    {
        for (auto wave : mpwaves)
        {
            /*第一轮遍历，检查蹦极僵尸数量是否足够*/
            int bungeeTotal = 0; // 该波蹦极的总数
            for (int i = (wave - 1) * 50; i < wave * 50; ++i)
            {
                if (*(GetMainObject()->zombieList() + i) == BUNGEE_ZOMBIE)
                    bungeeTotal += 1;
            }
            if (bungeeTotal < lst.size())
            {
                ShowErrorNotInQueue("您指定了 # 只蹦极偷的位置，但第 # 波只有 # 只蹦极！", lst.size(), wave, bungeeTotal);
                return;
            }

            SetTime(1, wave);
            InsertOperation([=](){
                grids = lst; // 把它换成全局变量
                auto za = GetMainObject()->zombieArray();
                std::vector<int> bungee_idx; // 偷其他格子的蹦极的序号。它们是第三轮遍历时修改的对象
                /*第二轮遍历，找到哪些蹦极恰好偷了指定的格子，并记下其余蹦极的序号*/
                for (int i = 0; i < GetMainObject()->zombieTotal(); ++i)
                {
                    if (za[i].isExist() &&
                        za[i].type() == BUNGEE_ZOMBIE &&
                        !za[i].isDead())
                    {
                        bool isSatisfying = false;
                        for (int g = 0; g < grids.size(); ++g)
                        {
                            if (za[i].row() + 1 == grids[g].row &&
                                za[i].abscissa() == BungeeCoord(grids[g].row, grids[g].col).first)
                            {
                                isSatisfying = true; // 表示蹦极偷的格子包含于指定的格子
                                grids.erase(grids.begin() + g);
                                break;
                            }
                        }
                        if (!isSatisfying)
                            bungee_idx.push_back(i); // 记下这个蹦极的序号
                    }
                }

                /*第三轮遍历，对于没有蹦极偷的被指定的格子，强行修改蹦极位置去偷*/
                for (int i = 0; i < grids.size(); ++i)
                {
                    auto bungee_coord = BungeeCoord(grids[i].row, grids[i].col);
                    auto oldRow = za[bungee_idx[i]].row();
                    za[bungee_idx[i]].row() = grids[i].row - 1;
                    za[bungee_idx[i]].mRef<int>(0x80) = grids[i].col - 1;
                    za[bungee_idx[i]].abscissa() = bungee_coord.first;
                    za[bungee_idx[i]].ordinate() = bungee_coord.second;
                    za[bungee_idx[i]].mRef<int>(0x20) -= 10000 * (oldRow - (grids[i].row - 1));
                }
            });
        }
    }

    void BungeeManipulate(int row, int col, std::vector<int> mpwaves)
    {
        Grid temp1 = {row, col};
        std::vector<Grid> temp2 = {temp1};
        BungeeManipulate(temp2, mpwaves);
    }

    // 本次游戏期间神炮爆心的横坐标
    int GetGodPaoX()
    {
        return GetPvzBase()->mRef<int>(0x320) - 1;
    }

    // *** Not In Queue
    // 神炮爆心横坐标转换成十六进制后的后四位数
    // 当打开的游戏exe一定时，这四位数是一个定值
    int GetGodPaoLastNums()
    {
        return GetGodPaoX() % 0x10000;
    }

    void PaoToOutside(int pao_row, int pao_col, int row, int x)
    {
        pao_operator.rawPao(pao_row, pao_col, row, 9);
        InsertOperation([=](){
            int id = GetPlantIndex(pao_row, pao_col, COB_CANNON);
            GetMainObject()->plantArray()[id].mRef<int>(0x80) = x - 47;
        });
    }

    void SetRowSpawnType(const std::vector<int> &lst, RowType type)
    {
        InsertOperation([=](){
            for (auto row : lst)
                { GetMainObject()->mRef<int>(0x5D8 + 4 * (row - 1)) = type; }
        });
    }

    struct Pogostu { float x; float y; int dt; int paox; };

    // 计算跳跳僵尸的炮右距或炮左距
    int PaoDistance(int y, int paoy, bool right = true, int type = TT_18)
    {
        int yLower = y + 115; //y下边界
        int ydt = (paoy > yLower) ? (paoy - yLower) : ((paoy < y) ? (y - paoy) : 0); //y距离
        if (ydt > 115) return -2000; //提前结束，防止下面的sqrt给负数开平方
        int xdt = int(sqrt(115 * 115 - ydt * ydt));
        if (right) return xdt + 78;
        else       return xdt - 36;
    }

    std::pair<int, int> PaoxForPogo(int pogo_row, int pao_row, bool isShowInfo, int wave)
    {
        if (GetPvzBase()->gameUi() != 3) {
            ShowErrorNotInQueue("当前不在战斗界面，无法搜索跳跳！");
            return {0, 0};
        }
        if (abs(pao_row - pogo_row) > 1) {
            ShowErrorNotInQueue("炮只能炸三行，请检查输入的参数！");
            return {0, 0};
        }
        /*下面这三行可以根据需要换成全局变量*/
        Pogostu pogo1 = {1000, 1000, -100, 1000}; //x最小的跳跳
        Pogostu pogo2 = {1000, 1000, -100, 1000}; //右侧paox最小的跳跳
        Pogostu pogo3 = {10, 1000, -100, 10}; //左侧paox最大的跳跳
        int paoy = 35 + 85 * pao_row; //炮弹y坐标
        auto aa = GetPvzBase()->animationMain()->animationOffset()->animationArray();
        bool isPogoFound = false;
        for (auto& zf : alive_zombie_filter)
        {
            if (zf.type() == TT_18 && zf.row() == pogo_row - 1 &&
                (wave == -1 || zf.standState() == wave - 1))
            {
                isPogoFound = true;
                uint16_t& animationCode = zf.mRef<uint16_t>(0x118);
                float x = zf.abscissa();
                float y = zf.ordinate() + aa[animationCode].mRef<float>(0x38) - 8;
                int rdt = PaoDistance(y, paoy); //炮右距
                int ldt = rdt - 78 - 36;        //炮左距
                int rpaox = int(x) + rdt;
                int lpaox = int(x) - ldt;
                if (x < pogo1.x || (x == pogo1.x && y < pogo1.y)) {
                    pogo1.x = x;    pogo1.y = y;
                    pogo1.dt = rdt; pogo1.paox = rpaox;
                }
                if (rpaox < pogo2.paox || (rpaox == pogo2.paox && x < pogo2.x)) {
                    pogo2.x = x;    pogo2.y = y;
                    pogo2.dt = rdt; pogo2.paox = rpaox;
                }
                if (lpaox > pogo3.paox || (lpaox == pogo3.paox && x > pogo3.x)) {
                    pogo3.x = x;    pogo3.y = y;
                    pogo3.dt = ldt; pogo3.paox = lpaox;
                }
            }
        }
        if (!isPogoFound) {
            if (isShowInfo) ShowErrorNotInQueue("未找到符合要求的跳跳！");
            return {0, 0};
        }
        if (pogo2.paox < pogo3.paox) { //囊括了炮无法兼收的情况和炮炸不着的情况
            pogo2.paox = 0;    pogo3.paox = 0;
        }
        if (pogo1.paox < 0) pogo1.paox = 0;
        if (isShowInfo)
        {
            std::string rpaocol = pogo2.paox ? std::to_string(float(pogo2.paox + 7) / 80) : "-1.00000";
            std::string lpaocol = pogo3.paox ? std::to_string(float(pogo3.paox + 7) / 80) : "-1.00000";
            rpaocol.erase(rpaocol.find('.') + 5); //保留4位小数
            lpaocol.erase(lpaocol.find('.') + 5);
            ShowErrorNotInQueue("横坐标最小的跳跳，x=#，y=#，dt=#，paox=#；\n所需炮落点最左的跳跳，x=#，y=#，dt=#，paox=#。\n所需炮落点最右的跳跳，x=#，y=#，dt=#，paox=#。\n炮瞄点应在" + lpaocol + "~" + rpaocol + "列。", pogo1.x, pogo1.y, pogo1.dt, pogo1.paox, pogo2.x, pogo2.y, pogo2.dt, pogo2.paox, pogo3.x, pogo3.y, pogo3.dt, pogo3.paox);
        }
        return {pogo3.paox, pogo2.paox};
    }

    void SetMusic(int musicId)
    {
        if (musicId <= 0 || musicId >= 13) {
            ShowErrorNotInQueue("音乐编号为1~12，您输入了#", musicId);
            return;
        }
        InsertOperation([=](){
            __asm__ __volatile__(
                "pushal;"
                "movl %[musicId],%%edi;"
                "movl 0x6a9ec0,%%eax;"
                "movl 0x83c(%%eax), %%eax;"
                "movl $0x45b750, %%edx;"
                "calll *%%edx;"
                "popal;"
                :
                : [musicId] "m"(musicId)
                :);
        });
    }

} // namespace sin20

void A_TAS()
{
    KeyConnect('1', [=]()
               { WriteMemory<uint32_t>(0x00E9B890, 0x52DFC9); }); //move
    KeyConnect('2', [=]()
               { WriteMemory<uint32_t>(0x0140B890, 0x52DFC9); }); //dancing
    KeyConnect('3', [=]()
               { WriteMemory<uint32_t>(0x00F0B890, 0x52DFC9); }); //callPartner
    KeyConnect('W', [=]()
               { WriteMemory<uint32_t>(0x838808B, 0x52DFC9); }); //stop
    KeyConnect('Z', [=](){
        temPause = false;
        __is_advanced_pause = !__is_advanced_pause;
    }); //高级暂停
    KeyConnect('X', [=](){
        temPause = false;
        InsertGuard ig(true);
        SetNowTime(); 
        InsertOperation([=](){ SetAdvancedPause(false); });
        Delay(1);
        InsertOperation([=](){ SetAdvancedPause(true); });
    }); //延时高级暂停
    KeyConnect('C', [=](){
        if (__is_advanced_pause) {
            __is_advanced_pause = false;
            temPause = true;
            GetPvzBase()->tickMs() = 40;
        }
        else if (temPause) {
            GetPvzBase()->tickMs() = 10;
            temPause = false;
            __is_advanced_pause = true;
        }
        else
            GetPvzBase()->tickMs() = (GetPvzBase()->tickMs() == 40) ? 10 : 40;
    }); //1/4倍速
    KeyConnect('N', [=](){
        GetPvzBase()->tickMs() = (GetPvzBase()->tickMs() == 5) ? 10 : 5;
    }); //2倍速
    KeyConnect('M', [=](){
        GetPvzBase()->tickMs() = (GetPvzBase()->tickMs() == 2) ? 10 : 2;
    }); //5倍速
    KeyConnect('B', [=](){
        if (ReadMemory<int>(0x416dbe) == 100001)
            WriteMemory<int>(699999, 0x416dbe);
        else if (ReadMemory<int>(0x416dbe) == 699999)
            WriteMemory<int>(100001, 0x416dbe);
    }); //置顶卡槽
}

#endif