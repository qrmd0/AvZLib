#ifndef __AVZ1_TICK_H__
#define __AVZ1_TICK_H__

#include "avz1_time_operation.h"

namespace AvZ {

enum TickStatus {
    STOPPED = 0,
    PAUSED,
    RUNNING
};

// CLASS TickRunner
// 使用此类使得操作每帧都运行
class TickRunner : public ATickRunner {
public:
    // *** Not In Queue
    // 得到线程的状态
    // *** 返回值：
    // 停止状态：return STOPPED
    // 暂停状态：return PAUSED
    // 运行状态：return RUNNING
    int getStatus() const { return status; }

    // *** Not In Queue
    template <class Func>
    void pushFunc(Func&& _run, bool is_in_fight = true)
    {
        status = RUNNING;
        ATickRunner::Start(std::forward<Func>(_run), !is_in_fight);
    }

    // *** In Queue
    void stop()
    {
        status = STOPPED;
        InsertOperation([this] { ATickRunner::Stop(); });
    }

    // *** In Queue
    void pause()
    {
        status = PAUSED;
        InsertOperation([this] { ATickRunner::Pause(); });
    }

    // *** In Queue
    void goOn()
    {
        status = RUNNING;
        InsertOperation([this] { ATickRunner::GoOn(); });
    }

private:
    int status = STOPPED;
};

class ItemCollector : public AItemCollector {

public:
    // *** In Queue
    void start()
    {
        InsertOperation([this] {
            AItemCollector::Start();
        });
    }

    // *** In Queue
    void setInterval(int _time_interval)
    {
        InsertOperation([=] {
            AItemCollector::SetInterval(_time_interval);
        });
    }
};

class IceFiller : public AIceFiller {

public:
    // *** In Queue
    // 重置冰卡
    // *** 注意：该函数需要使用在 start 函数之后才能生效
    // *** 使用示例
    // resetIceSeedList({ICE_SHROOM}) ------ 只使用原版冰
    // resetIceSeedList({M_ICE_SHROOM, ICE_SHROOM}) ----- 优先使用模仿冰，再使用原版冰
    void resetIceSeedList(const std::vector<int>& lst)
    {
        InsertOperation([=] {
            AIceFiller::SetIceSeedList(lst);
        });
    }

    // *** In Queue
    // 重置存冰位置
    // *** 使用示例：
    // resetFillList({{3,4},{5,6}})-----将存冰位置重置为{3，4}，{5，6}
    void resetFillList(const std::vector<Grid>& lst)
    {
        InsertOperation([=] {
            AIceFiller::SetList(lst);
        });
    }

    // *** In Queue
    // 线程开始工作
    // *** 使用示例：
    // start({{3,4},{5,6}})-----在{3，4}，{5，6}位置存冰
    void start(const std::vector<Grid>& lst)
    {
        InsertOperation([=] {
            AIceFiller::Start(lst);
        });
    }

    // *** In Queue
    // 使用咖啡豆函数
    // *** 使用示例：
    // coffee()-----自动使用优先级低的存冰位
    void coffee()
    {
        InsertOperation([=] {
            AIceFiller::Coffee();
        });
    }
};

class PlantFixer : public APlantFixer {

public:
    // *** In Queue
    // 重置植物修补位置
    // *** 使用示例：
    // resetFixList({{2, 3},{3, 4}})-------位置被重置为{2，3}，{3，4}
    void resetFixList(const std::vector<Grid>& lst)
    {
        InsertOperation([=] {
            APlantFixer::SetList(lst);
        });
    }
    // *** In Queue
    // 自动得到修补的位置列表
    void autoGetFixList()
    {
        InsertOperation([=] {
            APlantFixer::AutoGetList();
        });
    }

    // *** In Queue
    // 线程开始工作，此函数开销较大，不建议多次调用
    // 第一个参数为植物类型
    // 第二个参数不填默认全场
    // 第三个参数不填默认植物血量为150以下时修补
    // *** 使用示例：
    // start(23)-------修补全场的高坚果
    // start(30,{{1,3},{2,3}})-----修补位置为{1，3}，{2，3}位置的南瓜头
    // start(3,{{1,3},{2,3}},300)------修补位置为{1，3}，{2，3}位置的坚果，血量降至300开始修补
    void start(int _plant_type, const std::vector<Grid>& lst = {}, int _fix_hp = 150)
    {
        InsertOperation([=] {
            APlantFixer::Start(_plant_type, lst, _fix_hp);
        });
    }

    // *** In Queue
    // 重置修补血量
    // *** 使用示例：
    // resetFixHp(200)------将修补触发血量改为200
    void resetFixHp(int _fix_hp)
    {
        InsertOperation([=] {
            APlantFixer::SetHp(_fix_hp);
        });
    }

    // *** In Queue
    // 是否使用咖啡豆
    void isUseCoffee(bool _is_use_coffee)
    {
        InsertOperation([=] {
            APlantFixer::SetIsUseCoffee(_is_use_coffee);
        });
    }
};

template <typename Func>
    requires std::is_convertible_v<Func, AOperation>
void KeyConnect(AKey key, Func&& func)
{
    AConnect(key, [func = std::forward<Func>(func)]() mutable {
        InsertGuard ig(false);
        func();
    });
}

} // namespace AvZ

#endif