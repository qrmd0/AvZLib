/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-09-28 19:29:12
 * @Description:
 */

#ifndef __SMIDCT_H__
#define __SMIDCT_H__
#include "avz_global.h"
#include "libavz.h"
#include "sm_types.h"
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#define DEFAULT_STATE 0xffff

template <typename T>
class SMShowObj {
public:
    template <typename F>
    using InfoFunc = std::function<SMText(F*)>;
    using FindInfoDict = std::vector<std::unordered_map<int, SMFindInfo>>;
    SMShowObj()
    {
    }

    // 设置显示种类
    // 使用示例
    // setShowType({YMJNP_47, HBG_14}) ----- 仅显示玉米加农炮和寒冰菇的内存信息
    void setShowType(const std::vector<int>& type_vec)
    {
        type_dict.assign(type_dict.size(), 0);
        type_dict.back() = int(!type_vec.empty());
        for (auto&& type : type_vec) {
            if (type < type_dict.size()) {
                type_dict[type] = 1;
            }
        }
    }

    // 设置显示函数
    // 使用示例
    // setShowInfoFunc([=](Plant* plant) {
    //     SMText text;
    //     text += "类型:" + name_dict[plant->type()] + '\n';
    //     text += "血量:" + std::to_string(plant->hp());
    //     return text;
    // }); ------- 显示植物中的 类型 和 血量 信息
    void setShowInfoFunc(InfoFunc<Plant> func)
    {
        this->info_func = func;
    }

    T* findObject(int mouse_x, int mouse_y)
    {
        return nullptr;
    }

    SMText getShowText(int mouse_x, int mouse_y)
    {
        auto ptr = findObject(mouse_x, mouse_y);
        if (ptr) {
            return info_func(ptr);
        }
        return {};
    }

    // 得到鼠标寻找对象的字典信息
    // 使用示例
    // auto&& dict = getFindDict();
    // dict[Type][State].threshold = 50;  ---- 将类型为 Type、状态为 State 的对象的寻找阈值设置为 50
    // dict[Type][State].offset_x = 0.3;  ---- 将类型为 Type、状态为 State 的对象的中心偏移量设置为 0.3
    // 进一步解释
    // threshold 指的是鼠标离游戏内对象"中心"的寻找距离，即当鼠标所在位置和游戏内对象的"中心"距离小于 50 时，就会显示对象的内存信息
    // 这里所说的距离是欧氏距离
    // offset_x 指的是计算对象"中心"时用到的偏移量，由于在 PvZ 这个游戏中，没有直接的存储对象画面中心信息的内存，
    // 这个中心需要通过计算得到， 在这里，中心横坐标的计算公式为 $center_x = 对象横坐标 + 对象的判定宽度 * offset_x$,
    // 纵坐标同理
    FindInfoDict& getFindDict()
    {
        return find_info_dict;
    }

    // 得到对象的字符串名称
    // 使用示例
    // auto&& dict = getNameDict();
    // dict[YMJNP_47] = "春哥";   ----- 将玉米加农炮的名称设置为 春哥
    std::vector<std::string>& getNameDict()
    {
        return name_dict;
    }

    std::vector<uint8_t>& getTypeDict()
    {
        return type_dict;
    }

protected:
    std::vector<uint8_t> type_dict; // 最后一位用来记录是否用遍历
    InfoFunc<T> info_func;
    FindInfoDict find_info_dict;
    std::vector<std::string> name_dict;
};

template <>
inline SMShowObj<Plant>::SMShowObj()
{
    type_dict.assign(48 + 1, 1);

    // 初始化字符串生成函数
    info_func = [=](Plant* plant) {
        SMText text;
        text += "类型:" + name_dict[plant->type()] + '\n';
        text += "血量:" + std::to_string(plant->hp());
        return text;
    };

    name_dict = {
        "豌豆射手", "向日葵", "樱桃炸弹", "坚果", "土豆雷",
        "寒冰射手", "大嘴花", "双发射手", "小喷菇", "阳光菇",
        "大喷菇", "墓被吞噬者", "魅惑菇", "胆小菇", "寒冰菇",
        "毁灭菇", "睡莲", "倭瓜", "三线射手", "缠绕海藻",
        "火爆辣椒", "地刺", "火炬树桩", "高坚果", "海蘑菇",
        "路灯花", "仙人掌", "三叶草", "裂荚射手", "杨桃",
        "南瓜头", "磁力菇", "卷心菜投手", "花盆", "玉米投手",
        "咖啡豆", "大蒜", "叶子保护伞", "金盏花", "西瓜投手",
        "机枪射手", "双子向日葵", "忧郁蘑菇", "香蒲", "冰瓜",
        "吸金磁", "地刺王", "玉米加农炮", "模仿者"};

    std::pair<int, SMFindInfo> temp(DEFAULT_STATE, SMFindInfo(30, 0.5, 0.5));
    for (int type = 0; type < 48; ++type) {
        find_info_dict.push_back({temp});
    }
    find_info_dict[16][DEFAULT_STATE].offset_y = 1;
    find_info_dict[30][DEFAULT_STATE].offset_x = 0.33;
    find_info_dict[30][DEFAULT_STATE].offset_y = 0.66;
    find_info_dict[33][DEFAULT_STATE].offset_y = 1;
}

template <>
inline Plant* SMShowObj<Plant>::findObject(int mouse_x, int mouse_y)
{

    float min_distance = 0xffff;
    Plant* find_ptr = nullptr;
    for (auto&& obj : AvZ::alive_plant_filter) {
        auto type = obj.type();
        if (!type_dict[type]) {
            continue;
        }

        auto&& find_map = find_info_dict[type];
        auto info_iter = find_map.find(obj.state());
        auto&& info = info_iter == find_map.end() ? find_map[DEFAULT_STATE] : info_iter->second;
        float x_distance = mouse_x - obj.abscissa() - obj.hurtWidth() * info.offset_x;
        float y_distance = mouse_y - obj.ordinate() - obj.hurtHeight() * info.offset_y;
        float distance2 = x_distance * x_distance + y_distance * y_distance;
        float threshold2 = info.threshold * info.threshold;
        if (distance2 > threshold2) {
            continue;
        }
        if (distance2 < min_distance) {
            min_distance = distance2;
            find_ptr = &obj;
        }
    }
    return find_ptr;
}

template <>
inline SMShowObj<Zombie>::SMShowObj()
{
    type_dict.assign(33 + 1, 1);
    info_func = [=](Zombie* zombie) {
        SMText text;
        text += "类型:" + name_dict[zombie->type()] + '\n';
        text += "血量:" + std::to_string(zombie->hp());
        return text;
    };
    name_dict = {
        "普僵", "旗帜", "路障", "撑杆", "铁桶", "读报",
        "铁门", "橄榄", "舞王", "伴舞", "鸭子", "潜水",
        "冰车", "雪橇", "海豚", "小丑", "气球", "矿工",
        "跳跳", "雪人", "蹦极", "扶梯", "投篮", "白眼",
        "小鬼", "僵博", "豌豆", "坚果", "辣椒", "机枪",
        "倭瓜", "高墙", "红眼"};

    std::pair<int, SMFindInfo> temp(DEFAULT_STATE, SMFindInfo(30, 1.2, 0.5));

    for (int type = 0; type < 48; ++type) {
        find_info_dict.push_back({temp});
    }
    find_info_dict[11][59] = {50, 0.5, 1};
    find_info_dict[12][DEFAULT_STATE] = {50, 0.5, 0.5};
    find_info_dict[14][DEFAULT_STATE] = {50, 1.2, 0.8};
    find_info_dict[16][75] = {50, 1.2, 0.8};
    find_info_dict[17][32] = {50, 2, 1};
    find_info_dict[17][DEFAULT_STATE] = {50, 2, 0.5};
    find_info_dict[20][DEFAULT_STATE] = {50, 0.4, 0.5};
    find_info_dict[22][DEFAULT_STATE] = {50, 0.5, 0.5};
    find_info_dict[23][DEFAULT_STATE] = {50, 0.5, 0.3};
    find_info_dict[24][DEFAULT_STATE] = {50, 1.3, 0.8};
    find_info_dict[32][DEFAULT_STATE] = {50, 0.5, 0.3};
}

template <>
inline Zombie* SMShowObj<Zombie>::findObject(int mouse_x, int mouse_y)
{
    float min_distance = 0xffff;
    Zombie* find_ptr = nullptr;
    for (auto&& obj : AvZ::alive_zombie_filter) {
        auto type = obj.type();
        if (!type_dict[type]) {
            continue;
        }

        auto&& find_map = find_info_dict[type];
        auto info_iter = find_map.find(obj.state());
        auto&& info = info_iter == find_map.end() ? find_map[DEFAULT_STATE] : info_iter->second;
        float x_distance = mouse_x - obj.abscissa() - obj.hurtWidth() * info.offset_x;
        float y_distance = mouse_y - obj.ordinate() - obj.hurtHeight() * info.offset_y;
        float distance2 = x_distance * x_distance + y_distance * y_distance;
        float threshold2 = info.threshold * info.threshold;
        if (distance2 > threshold2) {
            continue;
        }
        if (distance2 < min_distance) {
            min_distance = distance2;
            find_ptr = &obj;
        }
    }
    return find_ptr;
}

template <>
inline SMShowObj<Seed>::SMShowObj()
{
    type_dict.assign(48 + 1, 1);
    info_func = [=](Seed* seed) {
        SMText text;
        text += "类型:" + name_dict[seed->type()] + '\n';
        text += "冷却:" + std::to_string(seed->initialCd() - seed->cd());
        return text;
    };
    name_dict = {
        "豌豆射手", "向日葵", "樱桃炸弹", "坚果", "土豆雷",
        "寒冰射手", "大嘴花", "双发射手", "小喷菇", "阳光菇",
        "大喷菇", "墓被吞噬者", "魅惑菇", "胆小菇", "寒冰菇",
        "毁灭菇", "睡莲", "倭瓜", "三线射手", "缠绕海藻",
        "火爆辣椒", "地刺", "火炬树桩", "高坚果", "海蘑菇",
        "路灯花", "仙人掌", "三叶草", "裂荚射手", "杨桃",
        "南瓜头", "磁力菇", "卷心菜投手", "花盆", "玉米投手",
        "咖啡豆", "大蒜", "叶子保护伞", "金盏花", "西瓜投手",
        "机枪射手", "双子向日葵", "忧郁蘑菇", "香蒲", "冰瓜",
        "吸金磁", "地刺王", "玉米加农炮", "模仿者"};
}

template <>
inline Seed* SMShowObj<Seed>::findObject(int mouse_x, int mouse_y)
{
    float min_distance = 0xffff;
    for (auto&& obj : AvZ::alive_seed_filter) {
        auto type = obj.type();
        if (!type_dict[type]) {
            continue;
        }
        int seed_x = obj.abscissa() + obj.xOffset();
        int seed_y = obj.ordinate();
        int seed_width = obj.width();
        int seed_height = obj.height();
        if (mouse_x - seed_x > 0 && mouse_x - seed_x < seed_width && mouse_y - seed_y > 0 && mouse_y - seed_y < seed_height) {
            return &obj;
        }
    }
    return nullptr;
}

template <>
inline SMShowObj<PlaceItem>::SMShowObj()
{
    type_dict.assign(14 + 1, 0);
    type_dict[2] = 1;
    type_dict.back() = 1;
    info_func = [=](PlaceItem* place_item) {
        SMText text;
        text += "类型:" + name_dict[place_item->type()] + '\n';
        text += "特殊值:" + std::to_string(place_item->value());
        return text;
    };
    name_dict = {
        "无", "墓碑", "弹坑", "梯子",
        "PORTAL_WHITE", "PORTAL_BLACK",
        "BRAIN_AQ", "VASE", "SQUIRRE",
        "USING_TOO", "SNAI", "RAKE",
        "BRAIN_IZ", "ENM_BEGIN"};
}

template <>
inline PlaceItem* SMShowObj<PlaceItem>::findObject(int mouse_x, int mouse_y)
{
    for (auto&& obj : AvZ::alive_place_item_filter) {
        auto type = obj.type();
        if (!type_dict[type]) {
            continue;
        }
        if ((AvZ::MouseRow() == (obj.row() + 1)) && (std::abs(AvZ::MouseCol() - obj.col() - 1) < 0.5)) {
            return &obj;
        }
    }
    return nullptr;
}

#endif