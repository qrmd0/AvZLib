#ifndef _REISEN_MOUSE_POS_PATCH_PATCH_H
#define _REISEN_MOUSE_POS_PATCH_PATCH_H
#include "avz.h"

namespace AvZ {
extern HWND __pvz_hwnd;

namespace _ReisenMousePosPatch {
class : GlobalVar {
    void beforeScript() override {
        __pvz_hwnd = GetPvzBase()->mRef<HWND>(0x350);
    }
} hwndPatcher;

struct MouseMoveTracker : TickRunner, GlobalVar {
    int row;
    float col;
    POINT last_pos;

    void run() {
        POINT p;
        GetCursorPos(&p);
        if(p.x == last_pos.x && p.y == last_pos.y)
            return;
        last_pos = p;
        row = MouseRow();
        col = MouseCol();
    }

    void beforeScript() override {
        pushFunc(std::bind(&MouseMoveTracker::run, this), false);
    }
} mousePosTracker;

int PatchedMouseRow() {
    return mousePosTracker.row;
}

float PatchedMouseCol() {
    return mousePosTracker.col;
}
}
}

#define MouseRow _ReisenMousePosPatch::PatchedMouseRow
#define MouseCol _ReisenMousePosPatch::PatchedMouseCol
#endif
