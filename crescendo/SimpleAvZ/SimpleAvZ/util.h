// This file is for pure C++ helper functions only.

#pragma once

#include <iterator> // std::advance
#include <numeric>
#include <string>
#include <type_traits> // std::is_same
#include <vector>

namespace _SimpleAvZInternal {

template <typename T>
std::string to_string(const T& val, std::true_type)
{
    return val;
}

template <typename T>
std::string to_string(const T& val, std::false_type)
{
    return std::to_string(val);
}

template <template <typename, typename...> class Container, typename T, typename... Args>
std::string concat(const Container<T, Args...>& container, const std::string& sep)
{
    std::string output = "";
    bool first = true;

    for (const auto& val : container) {
        if (first) {
            first = false;
        } else {
            output += sep;
        }

        output += to_string(val, std::is_same<T, std::string>());
    }

    return output;
}

template <typename T>
bool contains(const std::vector<T>& vec, const T& elem)
{
    return std::find(vec.begin(), vec.end(), elem) != vec.end();
}

template <typename T>
typename std::vector<T>::const_iterator select_random_elem(const std::vector<T>& vec)
{
    auto it = vec.begin();
    std::advance(it, std::rand() % vec.size());
    return it;
}

} // namespace _SimpleAvZInternal
