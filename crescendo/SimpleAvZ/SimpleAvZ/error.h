#pragma once

#include "libavz.h"

namespace _SimpleAvZInternal {

template <typename... Args>
void error(const std::string& func_name, const std::string& content, Args... args)
{
    AvZ::SetErrorMode(AvZ::POP_WINDOW);
    AvZ::ShowErrorNotInQueue("[调用 " + func_name + " 时出错]\n" + content, args...);

    throw "";
}

} // namespace _SimpleAvZInternal
