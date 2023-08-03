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

#endif