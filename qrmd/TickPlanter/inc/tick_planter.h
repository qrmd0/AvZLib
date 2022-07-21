/*
 * @字符编码: UTF-8
 * @作者: qrmd
 * @时间: 2022-06-04 22:07:38
 * @描述: 智能种植操作类
 * @AvZ版本：220213
 */

#include "avz.h"
using namespace AvZ;
#include <vector>

namespace AvZ {
class TickPlanter : public TickRunner {
public:
    // *** Not In Queue
    // 在种植操作队列尾部添加操作，如果这些操作失败则什么也不做，相当于Card
    // 示例：在2行8列种植向日葵，如果种植失败则什么也不做
    // tick_planter.PlantNotInQuene(SUNFLOWER, {{2, 8}});
    void PlantNotInQuene(int type, std::vector<Grid> grid_list)
    {
        for (Grid each : grid_list) {
            Planting temp = {type, {each.row, each.col}, ABANDON};
            plant_list[1].emplace_back(temp);
        }
    }
    // *** Not In Queue
    // 在种植操作队列尾部添加操作，如果这些操作失败则将其保留在队列中
    // 示例：先后在1行1列、2行1列、5行1列种植向日葵，如果种植失败则重试，直到全部种植操作执行成功
    // tick_planter.delayPlantNotInQuene(SUNFLOWER, {{1, 1}, {2, 1}, {5, 1}});
    void delayPlantNotInQuene(int type, std::vector<Grid> grid_list)
    {
        for (Grid each : grid_list) {
            Planting temp = {type, {each.row, each.col}, DELAY};
            plant_list[1].emplace_back(temp);
        }
    }
    // *** Not In Queue
    // 在种植操作队列尾部添加操作，如果这些操作仅因位置被占用而失败则铲除占位的植物重试，如果仍失败，操作被保留在队列中
    // 示例：在1行4列种植倭瓜，如果仅因1行4列被其他植物占用而种植失败，则将其铲除并重试，如果仍失败，则继续重试直到种植成功
    // tick_planter.shovelPlantNotInQuene(SQUASH, {{1, 4}});
    void shovelPlantNotInQuene(int type, std::vector<Grid> grid_list)
    {
        for (Grid each : grid_list) {
            Planting temp = {type, {each.row, each.col}, SHOVEL_AND_TRY_AGAIN};
            plant_list[1].emplace_back(temp);
        }
    }
    // *** Not In Queue
    // 在种植操作队列尾部添加操作，如果这些操作失败则暂停其他种植操作并将此操作保留
    // 示例：先后在1行1列、2行1列、5行1列种植大喷菇，仅当最靠前的种植操作成功时，才会尝试其他种植操作
    // tick_planter.waitPlantNotInQuene(FUME_SHROOM, {{1, 1}, {2, 1}, {5, 1}});
    void waitPlantNotInQuene(int type, std::vector<Grid> grid_list)
    {
        for (Grid each : grid_list) {
            Planting temp = {type, {each.row, each.col}, WAIT};
            plant_list[0].emplace_back(temp);
        }
    }
    // *** In Queue
    // 启用智能种植操作
    // tick_planter.start();
    void start()
    {
        InsertOperation([=]() {
            pushFunc([=]() {
                run();
            });
        },
            "StartTickPlanter");
    }
    // *** In Queue
    // 清空种植操作队列
    // tick_planter.clear();
    void clear()
    {
        InsertOperation([=]() {
            prior_plant_list.clear();
            routine_plant_list.clear();
            plant_list.clear();
            plant_list = {prior_plant_list, routine_plant_list};
        },
            "CleanPlantQuene");
    }
    // *** In Queue
    // 设置tick_planter遍历种植操作队列时间间隔，默认为1厘秒
    // 示例：设置tick_planter遍历种植操作队列时间间隔为10厘秒
    // tick_planter.setInterval(10);
    void setInterval(int _time)
    {
        InsertOperation([=]() {
            time_interval = _time;
        },
            "SetTickPlantInterval");
    }

private:
    enum IfFailDo {
        // 放弃，不会再执行此操作
        ABANDON,
        // 推迟，下次仍会执行此操作
        DELAY,
        // 阻塞，跳过所有其他操作直到此操作被执行成功
        WAIT,
        // 如果仅因为位置被占用而失败，则铲除占位植物并重试，如果重试失败，则推迟
        SHOVEL_AND_TRY_AGAIN
    };
    // 种植操作数据
    struct Planting {
        // 种植的种子类型
        int type;
        // 种植的位置：{row, col}:[row]行[col]列
        Grid position;
        // 种植失败时的处理
        IfFailDo if_fail_do = DELAY;
    };
    // 优先队列
    std::vector<Planting> prior_plant_list;
    // 常规队列
    std::vector<Planting> routine_plant_list;
    // 种植操作队列向量
    std::vector<std::vector<Planting>> plant_list = {prior_plant_list, routine_plant_list};
    // 遍历种植操作队列的时间间隔
    int time_interval = 1;
    void run()
    {
        if (GetMainObject()->gameClock() % time_interval != 0)
            return;
        auto seeds = GetMainObject()->seedArray();
        for (int i = 0; i < 2; ++i) {
            for (int index = 0; index < this->plant_list[i].size(); ++index) {
                auto it = plant_list[i][index];
                int plant_type = it.type > IMITATOR ? it.type - IMITATOR - 1 : it.type;
                int index_seed = it.type > IMITATOR ? GetSeedIndex(it.type - IMITATOR - 1, true) : GetSeedIndex(it.type);
                int sun_cost = *(int*)(0x69F2C0 + plant_type * 0x24);
                int present_sun = GetMainObject()->sun();
                int plant_reject_type = Asm::getPlantRejectType(plant_type, it.position.row - 1, it.position.col - 1);
                bool is_plant_successful = false;
                if (plant_reject_type == Asm::PlantReject::NIL && seeds[index_seed].isUsable() && present_sun >= sun_cost) {
                    CardNotInQueue(index_seed + 1, it.position.row, it.position.col);
                    if (!seeds[index_seed].isUsable()) {
                        is_plant_successful = true;
                        plant_list[i].erase(plant_list[i].begin() + index);
                    }
                }
                if (!is_plant_successful) {
                    switch (it.if_fail_do) {
                    case ABANDON:
                        plant_list[i].erase(plant_list[i].begin() + index);
                        break;
                    case WAIT:
                        return;
                        break;
                    case SHOVEL_AND_TRY_AGAIN:
                        if (plant_reject_type == Asm::PlantReject::NOT_HERE && seeds[index_seed].isUsable() && present_sun >= sun_cost) {
                            ShovelNotInQueue(it.position.row, it.position.col);
                            index--;
                        }
                        break;

                    default:
                        break;
                    }
                }
            }
        }
    }
};
TickPlanter tick_planter;
}