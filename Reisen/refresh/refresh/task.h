#pragma once

#include <functional>
#include <string>
#include <vector>
#include "avz.h"
#include "to_gbk.h"

using namespace AvZ;

struct Task {
    std::string output_folder, prefix;
    bool huge, assume_refresh, dance_cheat, debug, clear_zombies;
    int giga_count, check_time, total;
    std::vector<int> required_types, banned_types, card_selection;
    std::function<void(int)> operation;
};

#define _REISEN_TASK_H_MEMBER_REQUIRED(type, name) \
    private: \
    type name##_; \
    public: \
    void _set_##name(const type& val) { \
        name##_ = val; \
        initalized.insert(__COUNTER__); \
    } \
    auto name(const type& val) const { \
        TaskBuilder x = *this; \
        x._set_##name(val); \
        return x; \
    }

#define _REISEN_TASK_H_MEMBER_OPTIONAL(type, name, ...) \
    private: \
    type name##_ = __VA_ARGS__; \
    public: \
    void _set_##name(const type& val) { \
        name##_ = val; \
    } \
    auto name(const type& val) const { \
        TaskBuilder x = *this; \
        x._set_##name(val); \
        return x; \
    }

class TaskBuilder {
    private:
    std::set<int> initalized;
    _REISEN_TASK_H_MEMBER_REQUIRED(std::string, output_folder)
    _REISEN_TASK_H_MEMBER_REQUIRED(std::string, prefix)
    _REISEN_TASK_H_MEMBER_REQUIRED(bool, huge)
    _REISEN_TASK_H_MEMBER_REQUIRED(bool, assume_refresh)
    _REISEN_TASK_H_MEMBER_REQUIRED(int, check_time)
    _REISEN_TASK_H_MEMBER_REQUIRED(int, total)
    _REISEN_TASK_H_MEMBER_REQUIRED(std::vector<int>, required_types)
    _REISEN_TASK_H_MEMBER_REQUIRED(std::vector<int>, banned_types)
    _REISEN_TASK_H_MEMBER_REQUIRED(std::function<void(int)>, operation)
    _REISEN_TASK_H_MEMBER_OPTIONAL(bool, debug, false)
    _REISEN_TASK_H_MEMBER_OPTIONAL(bool, clear_zombies, true)
    _REISEN_TASK_H_MEMBER_OPTIONAL(bool, dance_cheat, true)
    _REISEN_TASK_H_MEMBER_OPTIONAL(int, giga_count, -1)
    _REISEN_TASK_H_MEMBER_OPTIONAL(std::vector<int>, card_selection, {ICE_SHROOM, DOOM_SHROOM,
        CHERRY_BOMB, JALAPENO, SQUASH, SPIKEWEED, BLOVER, TALL_NUT, GARLIC, POTATO_MINE})

    public:
    Task build() const {
        if(initalized.size() != 9) {
            ShowErrorNotInQueue("Task 未完成初始化");
            throw Exception("");
        }
        return {to_gbk(output_folder_), to_gbk(prefix_), huge_, assume_refresh_, dance_cheat_, debug_, clear_zombies_, giga_count_,
        std::max(check_time_, 401), total_, required_types_, banned_types_, card_selection_, operation_};
    }

    operator Task() const {
        return build();
    }
};
