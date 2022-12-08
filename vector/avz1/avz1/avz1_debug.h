/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-10-30 15:40:16
 * @Description : debug api
 */
#ifndef __AVZ1_DEBUG_H__
#define __AVZ1_DEBUG_H__

#include <avz.h>

#include "avz1_time_operation.h"

namespace AvZ {

__A1Deprecated inline void SetErrorMode(int _error_mode) { }

// *** Not In Queue
// 弹出窗口提示错误或者调试信息
// 使用方法同 printf 但是其中的 %d %f 等需要替换为 #

inline ALogger<AMsgBox> __logger;

template <typename FormatStr, typename... Args>
    requires std::is_convertible_v<FormatStr, std::string>
void ShowErrorNotInQueue(FormatStr&& formatStr, Args&&... args)
{
    __logger.Error(std::forward<FormatStr>(formatStr), std::forward<Args>(args)...);
}

// *** In Queue
// 弹出窗口提示错误或者调试信息
// 使用方法同 printf 但是其中的 %d %f 等需要替换为 #
template <typename FormatStr, typename... Args>
    requires std::is_convertible_v<FormatStr, std::string>
void ShowError(FormatStr&& formatStr, Args&&... args)
{
    InsertOperation([... args = std::forward<Args>(args), formatStr = std::forward<FormatStr>(formatStr)]() mutable {
        ShowErrorNotInQueue(std::move(formatStr), std::move(args)...);
    });
};

} // namespace AvZ
#endif