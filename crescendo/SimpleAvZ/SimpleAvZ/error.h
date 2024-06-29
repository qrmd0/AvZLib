#pragma once

#include "global.h"
#include "libavz.h"

namespace _SimpleAvZInternal {

template <typename... Args>
void error(const std::string& func_name, const std::string& content, Args... args)
{
    if (!global.disable_errors) {
        global.disable_errors = true;
        AvZ::SetErrorMode(AvZ::POP_WINDOW);
        AvZ::ShowErrorNotInQueue("[调用 " + func_name + " 时出错]\n" + content, args...);
    }
}

} // namespace _SimpleAvZInternal
