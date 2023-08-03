#ifndef __GLOBAL__
#define __GLOBAL__

#include "avz.h"

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

#endif