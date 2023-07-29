#pragma once

#include <string>
#include "avz.h"

namespace AvZ { void Utf8ToGbk(std::string&); };

std::string to_gbk(std::string s) {
    AvZ::Utf8ToGbk(s);
    return s;
}
