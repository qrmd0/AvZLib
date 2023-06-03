#ifndef _REISEN_MOUSE_POS_PATCH_PATCH_H
#define _REISEN_MOUSE_POS_PATCH_PATCH_H
#include "avz.h"

namespace AvZ {
extern HWND __pvz_hwnd;

namespace _ReisenMousePosPatch {
class : GlobalVar {
    void beforeScript() override {
        __pvz_hwnd = AvZ::GetPvzBase()->mRef<HWND>(0x350);
    }
} hwndPatcher;

int PixelToRow(int x, int y) {
    x = (x < 0) ? 0 : ((x > 799) ? 799 : x);
    y = (y < 0) ? 0 : ((y > 599) ? 599 : y);
    int ret = 0;
    __asm__ __volatile__(
        "movl %[y], %%ecx;"
        "movl %[x], %%eax;"
        "movl 0x6a9ec0, %%edx;"
        "movl 0x768(%%edx), %%edx;"
        "movl $0x41c550, %%ebx;"
        "calll *%%ebx;"
        "movl %%eax, %[ret];"
        :
        : [x] "m"(x), [y] "m"(y), [ret] "m"(ret)
        : "esp", "ecx", "eax", "ebx", "edx");
    return ret;
}

int PatchedMouseRow() {
    POINT point;
    GetCursorPos(&point);
    ScreenToClient(__pvz_hwnd, &point);
    return PixelToRow(point.x, point.y) + 1;
}

float PatchedMouseCol() {
    POINT point;
    GetCursorPos(&point);
    ScreenToClient(__pvz_hwnd, &point);
    int x = (point.x < 0) ? 0 : ((point.x > 799) ? 799 : point.x);
    return x / 80.0f;
}
}
}

#define MouseRow _ReisenMousePosPatch::PatchedMouseRow
#define MouseCol _ReisenMousePosPatch::PatchedMouseCol
#endif
