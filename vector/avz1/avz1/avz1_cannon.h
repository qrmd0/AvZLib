/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-10-30 16:39:44
 * @Description: cannon api
 */
#ifndef __AVZ1_CANNON_H__
#define __AVZ1_CANNON_H__

#include "avz1_time_operation.h"

namespace AvZ {
class PaoOperator : public ACobManager {
public:
    using PaoDrop = FireDrop;

    // *** In Queue
    // 发炮函数：用户自定义位置发射，屋顶修正飞行时间发炮.
    // *** 注意：尽量不要使用此函数操作位于炮列表中的炮，因为使用此函数后自动识别的炮序与 resetPaolist 更新的炮序将无效！
    // *** 使用示例：
    // rawRoofPao(1,2,2,9)-----------------------将位置为（1，2）的炮发射到（2，9）
    // rawRoofPao({ {1,2,2,9},{1,3,5,9}})-------将位置为（1，2）的炮发射到（2，9），将位置为（1，3）的炮发射到（5，9）
    static void rawRoofPao(int pao_row, int pao_col, int drop_row, float drop_col)
    {
        InsertOperation([=] { RawRoofFire(pao_row, pao_col, drop_row, drop_col); });
    }

    // *** In Queue
    // 发炮函数：用户自定义位置发射，屋顶修正飞行时间发炮.
    // *** 注意：尽量不要使用此函数操作位于炮列表中的炮，因为使用此函数后自动识别的炮序与 resetPaolist 更新的炮序将无效！
    // *** 使用示例：
    // rawRoofPao(1,2,2,9)-----------------------将位置为（1，2）的炮发射到（2，9）
    // rawRoofPao({ {1,2,2,9},{1,3,5,9}})-------将位置为（1，2）的炮发射到（2，9），将位置为（1，3）的炮发射到（5，9）
    static void rawRoofPao(const std::vector<PaoDrop>& lst)
    {
        InsertOperation([=] { RawRoofFire(lst); });
    }

    // *** In Queue
    // 发炮函数：用户自定义位置发射
    // *** 注意：尽量不要使用此函数操作位于有炮序炮列表中的炮，因为使用此函数后自动识别的炮序与 resetPaolist 更新的炮序将无效！
    // *** 使用示例：
    // rawPao(1,2,2,9)-----------------------将位置为（1，2）的炮发射到（2，9）
    // rawPao({{1, 2, 2, 9}, {1, 3, 5, 9}})-------将位置为（1，2）的炮发射到（2，9），将位置为（1，3）的炮发射到（5，9）
    static void rawPao(int pao_row, int pao_col, int drop_row, float drop_col)
    {
        InsertOperation([=] { RawFire(pao_row, pao_col, drop_row, drop_col); });
    }

    // *** In Queue
    // 发炮函数：用户自定义位置发射
    // *** 注意：尽量不要使用此函数操作位于有炮序炮列表中的炮，因为使用此函数后自动识别的炮序与 resetPaolist 更新的炮序将无效！
    // *** 使用示例：
    // rawPao(1,2,2,9)-----------------------将位置为（1，2）的炮发射到（2，9）
    // rawPao({{1, 2, 2, 9}, {1, 3, 5, 9}})-------将位置为（1，2）的炮发射到（2，9），将位置为（1，3）的炮发射到（5，9）
    static void rawPao(const std::vector<PaoDrop>& lst)
    {
        InsertOperation([=] { RawFire(lst); });
    }

    // *** In Queue
    // 种植炮函数
    // *** 使用示例
    // plantPao(3, 4)------在三行四列位置种炮
    static void plantPao(int row, int col)
    {
        InsertOperation([=] { Plant(row, col); });
    }

    // *** In Queue
    // 立即修补上一枚已经发射的炮
    void fixLatestPao()
    {
        InsertOperation([=] { this->FixLatest(); });
    }

    // *** In Queue
    // 设置炮序模式
    // *** 使用示例：
    // setSequentialMode(AvZ::PaoOperator::TIME) ---- 设置时间使用模式
    // setSequentialMode(AvZ::PaoOperator::SPACE) ---- 设置空间使用模式
    void setSequentialMode(int _sequential_mode)
    {
        InsertOperation([=] { this->SetSequentialMode(ACobManager::SequentialMode(_sequential_mode)); });
    }

    // *** In Queue
    // 设置即将发射的下一门炮
    // 此函数只有在限制炮序的时候才可调用
    // *** 使用示例：
    // setNextPao(10)------将炮列表中第十门炮设置为下一门即将发射的炮
    // setNextPao(2, 8)------将炮列表中位于 (2, 8) 的炮设置为下一门即将发射的炮
    void setNextPao(int next_pao)
    {
        InsertOperation([=] { this->SetNext(next_pao); });
    }

    // *** In Queue
    // 设置即将发射的下一门炮
    // 此函数只有在限制炮序的时候才可调用
    // *** 使用示例：
    // setNextPao(10)------将炮列表中第十门炮设置为下一门即将发射的炮
    // setNextPao(2, 8)------将炮列表中位于 (2, 8) 的炮设置为下一门即将发射的炮
    void setNextPao(int row, int col)
    {
        InsertOperation([=] { this->SetNext(row, col); });
    }

    // *** In Queue
    // 跳炮函数
    // *** 使用示例：
    // skipPao(2)---跳过按照顺序即将要发射的2门炮
    void skipPao(int x)
    {
        InsertOperation([=] { this->Skip(x); });
    }

    // *** In Queue
    // 发炮函数
    // *** 使用示例：
    // pao(2,9)----------------炮击二行，九列
    // pao({ {2,9},{5,9} })-----炮击二行，九列，五行，九列
    void pao(int row, float col)
    {
        InsertOperation([=] { this->Fire(row, col); });
    }

    // *** In Queue
    // 发炮函数
    // *** 使用示例：
    // pao(2,9)----------------炮击二行，九列
    // pao({ {2,9},{5,9} })-----炮击二行，九列，五行，九列
    void pao(const std::vector<Position>& lst)
    {
        InsertOperation([=] { this->Fire(lst); });
    }

    // *** In Queue
    // 发炮函数 炮CD恢复自动发炮
    // *** 使用示例：
    // recoverPao(2,9)----------------炮击二行，九列
    // recoverPao({ {2,9},{5,9} })-----炮击二行，九列，五行，九列
    void recoverPao(int row, float col)
    {
        InsertOperation([=] { this->RecoverFire(row, col); });
    }

    // *** In Queue
    // 发炮函数 炮CD恢复自动发炮
    // *** 使用示例：
    // recoverPao(2,9)----------------炮击二行，九列
    // recoverPao({ {2,9},{5,9} })-----炮击二行，九列，五行，九列
    void recoverPao(const std::vector<Position>& lst)
    {
        InsertOperation([=] { this->RecoverFire(lst); });
    }
    // *** In Queue
    // 屋顶修正飞行时间发炮. 此函数开销较大不适合精确键控.
    // 此函数只适用于RE与ME 修正时间：387cs
    // *** 使用示例：
    // roofPao(3,7)---------------------修正飞行时间后炮击3行7列
    // roofPao({ {2,9},{5,9} })---------修正飞行时间后炮击2行9列,5行9列
    void roofPao(int row, float col)
    {
        InsertOperation([=] { this->RoofFire(row, col); });
    }

    // *** In Queue
    // 屋顶修正飞行时间发炮. 此函数开销较大不适合精确键控.
    // 此函数只适用于RE与ME 修正时间：387cs
    // *** 使用示例：
    // roofPao(3,7)---------------------修正飞行时间后炮击3行7列
    // roofPao({ {2,9},{5,9} })---------修正飞行时间后炮击2行9列,5行9列
    void roofPao(const std::vector<Position>& lst)
    {
        InsertOperation([=] { this->RoofFire(lst); });
    }

    // *** In Queue
    // 重置炮列表
    // *** 使用示例:
    // resetPaoList({{3, 1},{4, 1},{3, 3},{4, 3}})-------经典四炮
    void resetPaoList(const std::vector<Grid>& lst)
    {
        InsertOperation([=] { this->SetList(lst); });
    }

    // *** In Queue
    // 自动填充炮列表
    // *** 注意：此函数无条件将场地上的所有炮填充至此炮列表
    void autoGetPaoList()
    {
        InsertOperation([=] { this->AutoGetList(); });
    }
};
} // namespace AvZ
#endif