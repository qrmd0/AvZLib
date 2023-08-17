/*
 * @Author: qrmd
 * @Date: 2023-08-03 23:07:35
 * @LastEditors: qrmd
 * @LastEditTime: 2023-08-10 20:59:33
 * @Description:
 */
#ifndef __MEMORY__
#define __MEMORY__

#include "avz.h"

// 写内存数组
// ------------参数------------
// dstAddr 内存地址
// srcArr 要写入的向量
// ------------示例------------
// WriteArray(0x52B308, std::vector<uint8_t> {0x90, 0x90});
template <class DstAddr, class SrcArray>
void WriteArray(DstAddr dstAddr, SrcArray&& srcArr)
{
    auto ptr = AMVal<uint8_t*>(dstAddr);
    std::copy(srcArr.begin(), srcArr.end(), ptr);
}

// 返回当前游戏模式（关卡序号）
inline int AGetGameMode()
{
    return AMPtr<APvzStruct>(0x6a9ec0)->MRef<int>(0x7F8);
}
#endif