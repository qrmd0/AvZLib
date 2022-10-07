#include "avz.h"

using namespace AvZ;

void Script()
{
    SetErrorMode(CONSOLE);
    // 在各移动片段停留的时长统计向量
    std::vector<int> time_in_fragments(46, 0);
    // 移动片段序号
    int index_fragment = 0;
    // 逐帧统计在各移动片段停留的时长
    for (int t = 0; t < 100; ++t) {
        // 计算移动片段序号（向下取整（动画进度 * 移动片段数））
        index_fragment = 1;
        // 将此帧计入在此移动片段停留的时长
        time_in_fragments[index_fragment]++;
    }

    for (int i = 0; i < 46; ++i) {
        ShowErrorNotInQueue("第#个运动片段停留的时长为#", i, time_in_fragments[i]);
    }
}