#ifndef __GLOBAL__
#define __GLOBAL__

#include "avz.h"
#include "constant.h"
#include <math.h>
#include <windows.h>

// 返回能否在向量A中找到b
template <typename T>
inline bool AGetIsHave(const std::vector<T>& A, T& b)
{
    return std::find(A.begin(), A.end(), b) != A.end();
}

// 根据键名返回一个映射的键值，如果键名不存在抛出异常
template <typename KeyType, typename ValueType>
ValueType AGetValue(const std::map<KeyType, ValueType>& myMap, const KeyType& key)
{
    auto it = myMap.find(key);
    if (it != myMap.end())
        return it->second;
    else
        throw std::out_of_range("Key not found in the map.");
}

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

// 返回数字的字符串
// ------------参数------------
// t 要转换的数字
// decimalDigits 四舍五入保留的小数位数，默认0位
// integerDigits 要保留的整数位数，不足则前导补0
// ------------示例------------
// AGetString(35.475, 2, 4); // 返回 0035.48
template <typename T>
std::string AGetString(const T& num, int decimalDigits, int integerDigits)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(decimalDigits);
    ss << num;
    std::string numStr = ss.str();
    int integerSize = std::to_string(int(num)).size();

    if (integerDigits > integerSize) {
        numStr = std::string(integerDigits - integerSize, '0') + numStr;
    }
    if (num < 0) {
        std::string tempStr = "-";
        for (int i = 0; i < numStr.size(); ++i) {
            if (numStr[i] != '-')
                tempStr = tempStr + numStr[i];
        }
        numStr = tempStr;
    }
    return numStr;
}

// 返回指定格子的状态是否为允许种植指定种子的状态
// ------------参数------------
// plt 种子类型
// row 要检查是否允许种植的格子行数
// col 要检查是否允许种植的格子列数
inline bool AGetIsPlantAllowable(APlantType plt, int row, int col)
{
    return AAsm::GetPlantRejectType(plt, row - 1, col - 1) == AAsm::NIL;
}

// 指定键均被按下时，返回true。
// ------------参数------------
// keys 要检测的按键的虚拟键码，详见：https://learn.microsoft.com/zh-cn/windows/win32/inputdev/virtual-key-codes
// isRequirePvZActive 是否要求PvZ窗口是当前窗口
// ------------示例------------
// AConnect([] { return true; }, [] {
//     if(AGetIsKeysDown({VK_CONTROL, VK_SHIFT, 'D'}, false)){
//         ALogger<AMsgBox> _log;
//         _log.Debug("您按下了全局快捷键 Ctrl+Shift+D");
//     } });
bool AGetIsKeysDown(const std::vector<int>& keys, bool isRequirePvZActive = true)
{
    auto pvzHandle = AGetPvzBase()->MRef<HWND>(0x350);
    if (isRequirePvZActive && GetForegroundWindow() != pvzHandle)
        return false;
    for (auto each : keys) {
        if ((GetAsyncKeyState(each) & 0x8000) == 0)
            return false;
    }
    return true;
}
bool AGetIsKeysDown(const int& key, bool isRequirePvZActive = true)
{
    std::vector<int> vec = {key};
    return AGetIsKeysDown(vec, isRequirePvZActive);
}

#endif