// Simple AvZ is an AvZ 1 extension to simplify scripting syntax.
// Copyright (C) 2023 Crescendo

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "cannon.h"
#include "card.h"
#include "ice_filler.h"
#include "iterator.h"
#include "macro.h"
#include "zombie.h"

#ifndef __AVZ_VERSION__
#error "Unknown AvZ version. Cannot perform version check."
#elif __AVZ_VERSION__ < 221001
#warning "Simple AvZ is made for AvZ 221001. You are using an older version of AvZ, which may cause compatibility issues."
#elif __AVZ_VERSION__ > 221001
#error "Simple AvZ is designed for AvZ 1, which is incompatible with AvZ 2."
#endif

using namespace AvZ;