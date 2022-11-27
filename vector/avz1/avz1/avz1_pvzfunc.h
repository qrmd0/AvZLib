/*
 * @coding: utf-8
 * @Author: yuchexi0_0
 * @Date: 2020-02-06 10:22:46
 * @Description: pvz func click_scene
 */
#ifndef __AVZ1_PVZFUNC_H__
#define __AVZ1_PVZFUNC_H__

#include <avz.h>

// 使用汇编代码调用 PvZ 游戏内部的函数
class Asm : public AAsm {

public:
    // key 1: left click
    // key -1: right click
    // key 3: middle click
    static void clickScene(AMainObject* level, int x, int y, int key)
    {
        AAsm::ClickScene(level, x, y, key);
    }

    static void click(AMouseWindow* mw, int x, int y, int key)
    {
        AAsm::Click(mw, x, y, key);
    }

    static void saveData()
    {
        AAsm::SaveData();
    }

    static void loadData()
    {
        AAsm::LoadData();
    }

    // 进入战斗页面，开始游戏
    static void rock()
    {
        AAsm::Rock();
    }

    static void mouseClick(int x, int y, int key)
    {
        AAsm::MouseClick(x, y, key);
    }

    // 发射炮
    static void shootPao(int x, int y, int rank)
    {
        AAsm::Fire(x, y, rank);
    }

    // 种卡函数
    static void plantCard(int x, int y, int index)
    {
        AAsm::PlantCard(x, y, index);
    }

    // 铲除函数
    static void shovelPlant(int x, int y)
    {
        AAsm::ShovelPlant(x, y);
    }

    // 选择卡片
    static void chooseCard(int card_type)
    {
        AAsm::ChooseCard(card_type);
    }

    // 选择模仿者卡片
    static void chooseImitatorCard(int card_type)
    {
        AAsm::ChooseImitatorCard(card_type);
    }

    // 是否可种卡片
    // 返回值意义查看 `enum PlantReject`
    // *** 使用示例:
    // if(getPlantRejectType(XRK_1, 1, 1) == Asm::NIL) { // NIL 表示可以种植物， 检查一下 2 行 2 列是否可以种植物
    //
    // }
    static int getPlantRejectType(int card_type, int row, int col)
    {
        return AAsm::GetPlantRejectType(card_type, row, col);
    }

    // 战斗界面刷新函数
    static void gameFightLoop()
    {
        AAsm::GameFightLoop();
    }

    // 游戏整体刷新函数
    static void gameTotalLoop()
    {
        AAsm::GameTotalLoop();
    }

    // 游戏最完整刷新函数，带帧率控制
    static void gameSleepLoop()
    {
        AAsm::GameSleepLoop();
    }

    static void clearObjectMemory()
    {
        AAsm::ClearObjectMemory();
    }

    static void gameExit()
    {
        AAsm::GameExit();
    }

    static void killZombiesPreview()
    {
        AAsm::KillZombiesPreview();
    }

    static void setImprovePerformance(bool is_improve_performance)
    {
        AAsm::SetImprovePerformance(is_improve_performance);
    }

    static void releaseMouse()
    {
        AAsm::ReleaseMouse();
    }

    static int gridToAbscissa(int row, int col)
    {
        return AAsm::GridToAbscissa(row, col);
    }

    static int gridToOrdinate(int row, int col)
    {
        return AAsm::GridToOrdinate(row, col);
    }
};

#endif
