/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-09-28 19:29:12
 * @Description:
 */

#ifndef __SMIDCT_H__
#define __SMIDCT_H__
#include <avz.h>

constexpr int __SM_DEFAULT_STATE = 0xffff;
constexpr int __SM_PLANT_TOTAL = 49;
constexpr int __SM_ZOMBIE_TOTAL = 33;
constexpr int __SM_PLACE_TOTAL = 14;

template <typename T>
class SMShowObj {
public:
    struct SMFindInfo {
        float threshold;
        float offsetX;
        float offsetY;

        SMFindInfo(float threshold, float offsetX, float offsetY)
            : threshold(threshold)
            , offsetX(offsetX)
            , offsetY(offsetY)
        {
        }

        SMFindInfo()
            : threshold(40)
            , offsetX(0.5f)
            , offsetY(0.5f)
        {
        }
    };
    template <typename F>
    using InfoFunc = std::function<std::string(F*)>;
    using FindInfoDict = std::vector<std::unordered_map<int, SMFindInfo>>;
    SMShowObj()
    {
    }

    // 设置显示种类
    // 使用示例
    // SetShowType({YMJNP_47, HBG_14}) ----- 仅显示玉米加农炮和寒冰菇的内存信息
    void SetShowType(const std::vector<int>& typeVec)
    {
        _typeDict.assign(_typeDict.size(), 0);
        _typeDict.back() = int(!typeVec.empty());
        for (auto&& type : typeVec) {
            if (type < _typeDict.size()) {
                _typeDict[type] = 1;
            }
        }
    }

    // 设置显示函数
    // 使用示例
    // SetShowInfoFunc([=](APlant* plant) {
    //     std::string text;
    //     text += "类型:" + _nameDict[plant->type()] + '\n';
    //     text += "血量:" + std::to_string(plant->hp());
    //     return text;
    // }); ------- 显示植物中的 类型 和 血量 信息
    template <typename Func>
        requires std::is_convertible_v<Func, InfoFunc<T>>
    void SetShowInfoFunc(Func&& func)
    {
        this->_infoFunc = std::forward<Func>(func);
    }

    T* FindObject(int mouseX, int mouseY)
    {
        return nullptr;
    }

    std::string GetShowText(int mouseX, int mouseY)
    {
        auto ptr = FindObject(mouseX, mouseY);
        if (ptr) {
            return _infoFunc(ptr);
        }
        return std::string();
    }

    // 得到鼠标寻找对象的字典信息
    // 使用示例
    // auto&& dict = GetFindDict();
    // dict[Type][State].threshold = 50;  ---- 将类型为 Type、状态为 State 的对象的寻找阈值设置为 50
    // dict[Type][State].offsetX = 0.3;  ---- 将类型为 Type、状态为 State 的对象的中心偏移量设置为 0.3
    // 进一步解释
    // threshold 指的是鼠标离游戏内对象"中心"的寻找距离，即当鼠标所在位置和游戏内对象的"中心"距离小于 50 时，就会显示对象的内存信息
    // 这里所说的距离是欧氏距离
    // offsetX 指的是计算对象"中心"时用到的偏移量，由于在 PvZ 这个游戏中，没有直接的存储对象画面中心信息的内存，
    // 这个中心需要通过计算得到， 在这里，中心横坐标的计算公式为 $centerX = 对象横坐标 + 对象的判定宽度 * offsetX$,
    // 纵坐标同理
    FindInfoDict& GetFindDict()
    {
        return _findInfoDict;
    }

    // 得到对象的字符串名称
    // 使用示例
    // auto&& dict = getNameDict();
    // dict[int(AYMJNP_47)] = "春哥";   ----- 将玉米加农炮的名称设置为 春哥
    std::vector<std::string>& GetNameDict()
    {
        return _nameDict;
    }

    std::vector<uint8_t>& GetTypeDict()
    {
        return _typeDict;
    }

    void Pause()
    {
        _typeDict.back() = 0;
    }

    void GoOn()
    {
        _typeDict.back() = 1;
    }

    bool IsPaused()
    {
        return _typeDict.back() == 0;
    }

protected:
    std::vector<uint8_t> _typeDict; // 最后一位用来记录是否用遍历
    InfoFunc<T> _infoFunc;
    FindInfoDict _findInfoDict;
    std::vector<std::string> _nameDict;
};

template <>
inline SMShowObj<APlant>::SMShowObj()
{
    _typeDict.assign(__SM_PLANT_TOTAL + 1, 1);

    // 初始化字符串生成函数
    _infoFunc = [=](APlant* plant) {
        std::string text;
        text += "类型:" + _nameDict[plant->Type()] + '\n';
        text += "血量:" + std::to_string(plant->Hp());
        return text;
    };

    _nameDict = {
        "豌豆射手", "向日葵", "樱桃炸弹", "坚果", "土豆雷",
        "寒冰射手", "大嘴花", "双发射手", "小喷菇", "阳光菇",
        "大喷菇", "墓碑吞噬者", "魅惑菇", "胆小菇", "寒冰菇",
        "毁灭菇", "睡莲", "倭瓜", "三线射手", "缠绕海藻",
        "火爆辣椒", "地刺", "火炬树桩", "高坚果", "海蘑菇",
        "路灯花", "仙人掌", "三叶草", "裂荚射手", "杨桃",
        "南瓜头", "磁力菇", "卷心菜投手", "花盆", "玉米投手",
        "咖啡豆", "大蒜", "叶子保护伞", "金盏花", "西瓜投手",
        "机枪射手", "双子向日葵", "忧郁蘑菇", "香蒲", "冰瓜",
        "吸金磁", "地刺王", "玉米加农炮", "模仿者"};

    std::pair<int, SMFindInfo> temp(__SM_DEFAULT_STATE, SMFindInfo(30, 0.5, 0.5));
    for (int type = 0; type < __SM_PLANT_TOTAL; ++type) {
        _findInfoDict.push_back({temp});
    }
    _findInfoDict[16][__SM_DEFAULT_STATE].offsetY = 1;
    _findInfoDict[30][__SM_DEFAULT_STATE].offsetX = 0.33;
    _findInfoDict[30][__SM_DEFAULT_STATE].offsetY = 0.66;
    _findInfoDict[33][__SM_DEFAULT_STATE].offsetY = 1;
}

template <>
inline APlant* SMShowObj<APlant>::FindObject(int mouseX, int mouseY)
{

    float minDistance = 0xffff;
    APlant* findPtr = nullptr;
    for (auto&& obj : aAlivePlantFilter) {
        auto type = obj.Type();
        if (!_typeDict[type]) {
            continue;
        }

        auto&& findMap = _findInfoDict[type];
        auto infoIter = findMap.find(obj.State());
        auto&& info = infoIter == findMap.end() ? findMap[__SM_DEFAULT_STATE] : infoIter->second;
        float xDistance = mouseX - obj.Abscissa() - obj.HurtWidth() * info.offsetX;
        float yDistance = mouseY - obj.Ordinate() - obj.HurtHeight() * info.offsetY;
        float distance2 = xDistance * xDistance + yDistance * yDistance;
        float threshold2 = info.threshold * info.threshold;
        if (distance2 > threshold2) {
            continue;
        }
        if (distance2 < minDistance) {
            minDistance = distance2;
            findPtr = &obj;
        }
    }
    return findPtr;
}

template <>
inline SMShowObj<AZombie>::SMShowObj()
{
    _typeDict.assign(__SM_ZOMBIE_TOTAL + 1, 1);
    _infoFunc = [=](AZombie* zombie) {
        std::string text;
        text += "类型:" + _nameDict[zombie->Type()] + '\n';
        text += "血量:" + std::to_string(zombie->Hp());
        return text;
    };
    _nameDict = {
        "普僵", "旗帜", "路障", "撑杆", "铁桶", "读报",
        "铁门", "橄榄", "舞王", "伴舞", "鸭子", "潜水",
        "冰车", "雪橇", "海豚", "小丑", "气球", "矿工",
        "跳跳", "雪人", "蹦极", "扶梯", "投篮", "白眼",
        "小鬼", "僵博", "豌豆", "坚果", "辣椒", "机枪",
        "倭瓜", "高墙", "红眼"};

    std::pair<int, SMFindInfo> temp(__SM_DEFAULT_STATE, SMFindInfo(30, 1.2, 0.5));

    for (int type = 0; type < __SM_ZOMBIE_TOTAL; ++type) {
        _findInfoDict.push_back({temp});
    }
    _findInfoDict[11][59] = {50, 0.5, 1};
    _findInfoDict[12][__SM_DEFAULT_STATE] = {50, 0.5, 0.5};
    _findInfoDict[14][__SM_DEFAULT_STATE] = {50, 1.2, 0.8};
    _findInfoDict[16][75] = {50, 1.2, 0.8};
    _findInfoDict[17][32] = {50, 2, 1};
    _findInfoDict[17][__SM_DEFAULT_STATE] = {50, 2, 0.5};
    _findInfoDict[20][__SM_DEFAULT_STATE] = {50, 0.4, 0.5};
    _findInfoDict[22][__SM_DEFAULT_STATE] = {50, 0.5, 0.5};
    _findInfoDict[23][__SM_DEFAULT_STATE] = {50, 0.5, 0.3};
    _findInfoDict[24][__SM_DEFAULT_STATE] = {50, 1.3, 0.8};
    _findInfoDict[32][__SM_DEFAULT_STATE] = {50, 0.5, 0.3};
}

template <>
inline AZombie* SMShowObj<AZombie>::FindObject(int mouseX, int mouseY)
{
    float minDistance = 0xffff;
    AZombie* findPtr = nullptr;
    for (auto&& obj : aAliveZombieFilter) {
        auto type = obj.Type();
        if (!_typeDict[type]) {
            continue;
        }

        auto&& findMap = _findInfoDict[type];
        auto infoIter = findMap.find(obj.State());
        auto&& info = infoIter == findMap.end() ? findMap[__SM_DEFAULT_STATE] : infoIter->second;
        float xDistance = mouseX - obj.Abscissa() - obj.HurtWidth() * info.offsetX;
        float yDistance = mouseY - obj.Ordinate() - obj.HurtHeight() * info.offsetY;
        float distance2 = xDistance * xDistance + yDistance * yDistance;
        float threshold2 = info.threshold * info.threshold;
        if (distance2 > threshold2) {
            continue;
        }
        if (distance2 < minDistance) {
            minDistance = distance2;
            findPtr = &obj;
        }
    }
    return findPtr;
}

template <>
inline SMShowObj<ASeed>::SMShowObj()
{
    _typeDict.assign(__SM_PLANT_TOTAL + 1, 1);
    _infoFunc = [=](ASeed* seed) {
        std::string text;
        text += "类型:" + _nameDict[seed->Type()] + '\n';
        text += "冷却:" + std::to_string(seed->IsUsable() ? 0 : seed->InitialCd() - seed->Cd());
        return text;
    };
    _nameDict = {
        "豌豆射手", "向日葵", "樱桃炸弹", "坚果", "土豆雷",
        "寒冰射手", "大嘴花", "双发射手", "小喷菇", "阳光菇",
        "大喷菇", "墓碑吞噬者", "魅惑菇", "胆小菇", "寒冰菇",
        "毁灭菇", "睡莲", "倭瓜", "三线射手", "缠绕海藻",
        "火爆辣椒", "地刺", "火炬树桩", "高坚果", "海蘑菇",
        "路灯花", "仙人掌", "三叶草", "裂荚射手", "杨桃",
        "南瓜头", "磁力菇", "卷心菜投手", "花盆", "玉米投手",
        "咖啡豆", "大蒜", "叶子保护伞", "金盏花", "西瓜投手",
        "机枪射手", "双子向日葵", "忧郁蘑菇", "香蒲", "冰瓜",
        "吸金磁", "地刺王", "玉米加农炮", "模仿者"};
}

template <>
inline ASeed* SMShowObj<ASeed>::FindObject(int mouseX, int mouseY)
{
    int seedCnt = AGetMainObject()->SeedArray()->Count();
    auto seedArray = AGetMainObject()->SeedArray();
    for (int idx = 0; idx < seedCnt; ++idx) {
        auto&& obj = seedArray[idx];
        auto type = obj.Type();
        if (!_typeDict[type]) {
            continue;
        }
        int seedX = obj.Abscissa() + obj.XOffset();
        int seedY = obj.Ordinate();
        int seedWidth = obj.Width();
        int seedHeight = obj.Height();
        if (mouseX - seedX > 0 && mouseX - seedX < seedWidth && mouseY - seedY > 0 && mouseY - seedY < seedHeight) {
            return &obj;
        }
    }

    return nullptr;
}

template <>
inline SMShowObj<APlaceItem>::SMShowObj()
{
    _typeDict.assign(__SM_PLACE_TOTAL + 1, 0);
    _typeDict[2] = 1;
    _typeDict.back() = 1;
    _infoFunc = [=](APlaceItem* place_item) {
        std::string text;
        text += "类型:" + _nameDict[place_item->Type()] + '\n';
        text += "特殊值:" + std::to_string(place_item->Value());
        return text;
    };
    _nameDict = {
        "无", "墓碑", "弹坑", "梯子",
        "PORTAL_WHITE", "PORTAL_BLACK",
        "BRAIN_AQ", "VASE", "SQUIRRE",
        "USING_TOO", "SNAI", "RAKE",
        "BRAIN_IZ", "ENM_BEGIN"};
}

template <>
inline APlaceItem* SMShowObj<APlaceItem>::FindObject(int mouseX, int mouseY)
{
    for (auto&& obj : aAlivePlaceItemFilter) {
        auto type = obj.Type();
        if (!_typeDict[type]) {
            continue;
        }
        if ((AMouseRow() == (obj.Row() + 1)) && (std::abs(AMouseCol() - obj.Col() - 1) < 0.5)) {
            return &obj;
        }
    }
    return nullptr;
}

#endif