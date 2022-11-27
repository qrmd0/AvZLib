/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-10-30 15:36:43
 * @Description: 操作队列头文件
 */

#ifndef __AVZ_TIME_OPERATION_H__
#define __AVZ_TIME_OPERATION_H__

#include <avz.h>

#include "avz1_global.h"
namespace AvZ {

enum EffectiveMode {
    NONE,
    MAIN_UI,
    MAIN_UI_OR_FIGHT_UI,
};

// *** Not In Queue
// 使用此函数可使脚本一次注入多次运行
// 适用于脚本完全无误后录制视频使用
// *** 注意：使用的关闭效果按键必须在战斗界面才会生效
// *** 使用示例
// OpenMultipleEffective() -------- 脚本多次生效，默认按下 C 键取消此效果
// OpenMultipleEffective('Q')-------  脚本多次生效，按下 Q 键取消此效果
// OpenMultipleEffective('Q', AvZ::MAIN_UI_OR_FIGHT_UI)-------
// 脚本多次生效，按下 Q 键取消此效果，多次生效效果在主界面和选卡界面都会生效
// OpenMultipleEffective('Q', AvZ::MAIN_UI)-------  脚本多次生效，按下 Q
// 键取消此效果，多次生效效果仅在主界面生效
inline void OpenMultipleEffective(char close_key = 'C', int _effective_mode = MAIN_UI)
{
    ASetReloadMode(AReloadMode(_effective_mode));
    AConnect(close_key, [] {
        ASetReloadMode(AReloadMode::NONE);
    });
}

inline TimeWave time_wave = {-599, 1};

// 设定操作时间点
// *** 使用示例：
// SetTime(-95, 1)------ 将操作时间点设为第一波僵尸刷新前 95cs
// SetTime(-95)--------- 将操作时间点设为僵尸刷新前 95cs,
// 波数由上一个最近确定的波数决定
inline void SetTime(int time, int wave)
{
    time_wave.time = time;
    time_wave.wave = wave;
}

// 设定操作时间点
inline void SetTime(int time)
{
    time_wave.time = time;
}

// 延迟一定时间
// *** 注意由于操作队列的优势，此函数支持负值
// *** 使用示例：
// Delay(298) ------ 延迟 298cs
// Delay(-298) ------ 提前 298cs
inline void Delay(int time)
{
    time_wave.time += time;
}

// 设定时间为当前时间
inline void SetNowTime()
{
    auto now_time = ANowTime();
    time_wave.time = now_time.time;
    time_wave.wave = now_time.wave;
}

// 设定延迟时间
inline void SetDelayTime(int time)
{
    SetNowTime();
    time_wave.time += time;
}

// 阻塞运行直到达到目标时间点
// 使用方法与 setTime 相同
// *** 注意：WaitUntil 停止阻塞的时间点是设定的时间点的下一帧
// 例如 WaitUntil(150, 1); int time = NowTime(1)
// 此时 time 的值是 151
// *** return : 阻塞是否正常结束
// ture 阻塞正常结束
// false 阻塞异常结束
inline bool WaitUntil(int time, int wave)
{
    AWaitUntil(wave, time);
    SetNowTime();
    return true;
}

// 得到当前时间，读取失败返回 -65535
// *** 注意得到的是以参数波刷新时间点为基准的相对时间
// *** 使用示例：
// NowTime(1) -------- 得到以第一波刷新时间点为基准的当前时间
// NowTime(2) -------- 得到以第二波刷新时间点为基准的当前时间
inline int NowTime(int wave)
{
    return ANowTime(wave);
}

inline bool __is_insert_operation = true;

// 确保当前操作是否被插入操作队列
class InsertGuard {
private:
    bool _is_insert_operation;

public:
    InsertGuard(bool is_insert)
    {
        extern bool __is_insert_operation;
        _is_insert_operation = __is_insert_operation;
        __is_insert_operation = is_insert;
    }

    ~InsertGuard()
    {
        extern bool __is_insert_operation;
        __is_insert_operation = _is_insert_operation;
    }
};

// 设置 insertOperation 属性函数
// *** 使用示例：
// SetInsertOperation(false) ---- insertOperation 将不会把操作插入操作队列中
// SetInsertOperation(true) ---- insertOperation 将会把操作插入操作队列中
inline void SetInsertOperation(bool _is_insert_operation = true)
{
    __is_insert_operation = _is_insert_operation;
}

// 将操作插入操作队列中
// description 将不在生效
template <class Func>
    requires std::is_convertible_v<Func, AOperation>
void InsertOperation(Func&& _operation,
    const std::string& description = "unknown")
{
    if (!__is_insert_operation) {
        _operation();
        return;
    }

    AConnect(ATime(time_wave.wave, time_wave.time), std::forward<Func>(_operation));
}

// description 将不在生效
template <class Func>
void InsertTimeOperation(int time, int wave, Func&& operation,
    const std::string& description = "unknown")
{
    SetTime(time, wave);
    InsertOperation(operation, description);
}

// 得到当前正在运行的波数
inline int GetRunningWave() { return AGetMainObject()->Wave(); }

__A1Deprecated inline std::vector<int> GetRefreshedWave()
{
    return std::vector<int>();
}
__A1Deprecated inline void ShowQueue(const std::vector<int>& lst) { }
template <class Func>
__A1Deprecated inline void ScriptExitDeal(Func&& func) { }
__A1Deprecated inline void SetScriptStartTime(int time, int wave) { }

// *** Not In Queue
// 设定高级暂停
// *** 注意开启高级暂停时不能使用跳帧
// *** 特别注意的是 `SetAdvancedPause` 一旦使得程序进入高级暂停状态后，便再也不会执行 Script 中的代码，
// 此时必须使用 `SetAdvancedPauseKey` 才能解除高级暂停状态。
// *** 使用示例
// SetAdvancedPause(true) ------ 开启高级暂停
// SetAdvancedPause(false) ------ 关闭高级暂停
inline void SetAdvancedPause(bool is_advanced_pause)
{
    ASetAdvancedPause(is_advanced_pause);
}

// *** Not In Queue
// 设定高级暂停按键
// *** 注意开启高级暂停时不能使用跳帧
// *** 使用示例
// SetAdvancedPauseKey('Q') ------ 将 Q 键设定为高级暂停管理按键，即按下 Q
// 游戏暂停，再按 Q 游戏继续运行
inline void SetAdvancedPauseKey(char key)
{
    AConnect(key, [] {
        static int is_advanced_pause = false;
        is_advanced_pause = !is_advanced_pause;
        ASetAdvancedPause(is_advanced_pause);
    });
}

// *** In Queue
// 跳到游戏指定时刻
// *** 注意使用此函数时不能使用高级暂停
// *** 使用示例
// SkipTick(200, 1) ------ 跳到时刻点 (200, 1)
inline void SkipTick(int time, int wave)
{
    InsertOperation([=]() {
        ASkipTick(wave, time);
    });
}

// *** In Queue
// 跳到指定条件为 false 的游戏帧
// *** 注意使用此函数时不能使用高级暂停
// *** 使用示例 : 直接跳到位置为 {1, 3}, {1, 5} 春哥死亡时的游戏帧
// auto condition = [=]() {
//     std::vector<int> results;
//     GetPlantIndices({{1, 3}, {1, 5}}, YMJNP_47, results);
//
//     for (auto result : results) {
//         if (result < 0) {
//             return false;
//         }
//     }
//     return true;
// };
//
// auto callback = [=]() {
//     ShowErrorNotInQueue("春哥无了，嘤嘤嘤");
// };
//
// SkipTick(condition, callback);
template <class Condition, class Callback>
void SkipTick(Condition&& condition, Callback&& callback)
{
    InsertOperation([condition = std::forward<Condition>(condition),
                        callback = std::forward<Callback>(callback)]() mutable {
        ASkipTick(std::move(condition), std::move(callback));
    },
        "SkipTick");
}

template <class Condition>
void SkipTick(Condition&& condition)
{
    SkipTick(condition = std::forward<Condition>(condition), []() {});
}

} // namespace AvZ
#endif