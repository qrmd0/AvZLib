/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-09-28 20:37:23
 * @Description:
 */
#ifndef __SM_H__
#define __SM_H__

#include "sm_obj.h"
#include "sm_painter.h"
#include <avz.h>

class SMShowMe : public ATickRunnerWithNoStart {

public:
    // 得到画家
    SMPainter& GetPainter() { return painter; }

    // 得到植物信息显示设置
    SMShowObj<APlant>& GetPlantSettings() { return plant; }

    // 得到僵尸信息显示设置
    SMShowObj<AZombie>& GetZombieSettings() { return zombie; }

    // 得到种子/卡片信息显示设置
    SMShowObj<ASeed>& GetSeedSettings() { return seed; }

    // 得到场地物品信息显示设置
    SMShowObj<APlaceItem>& GetPlaceItemSettings() { return placeItem; }

    void Start()
    {
#if __AVZ_VERSION__ < 240113
        ATickRunner::Start([this] {
#else
        ATickRunnerWithNoStart::_Start([this] {
#endif
            auto mouseWindow = AGetPvzBase()->MouseWindow();
            int mouseX = mouseWindow->MouseAbscissa();
            int mouseY = mouseWindow->MouseOrdinate();
            int txtPos = -1;

            // Plant
            if (!plant.IsPaused()) {
                auto showInfo = plant.GetShowText(mouseX, mouseY);
                if (!showInfo.empty()) {
                    painter.Draw(AText(showInfo, mouseX, mouseY, APos(++txtPos)));
                }
            }

            // Zombie
            if (!zombie.IsPaused()) {
                auto showInfo = zombie.GetShowText(mouseX, mouseY);
                if (!showInfo.empty()) {
                    painter.Draw(AText(showInfo, mouseX, mouseY, APos(++txtPos)));
                }
            }

            // Seed
            if (!seed.IsPaused()) {
                auto showInfo = seed.GetShowText(mouseX, mouseY);
                if (!showInfo.empty()) {
                    painter.Draw(AText(showInfo, mouseX, mouseY, APos(++txtPos)));
                }
            }

            // PlaceItem
            if (!placeItem.IsPaused()) {
                auto showInfo = placeItem.GetShowText(mouseX, mouseY);
                if (!showInfo.empty()) {
                    painter.Draw(AText(showInfo, mouseX, mouseY, APos(++txtPos)));
                }
            }
        },
            false);
    }

protected:
    SMPainter painter;
    SMShowObj<APlant> plant;
    SMShowObj<AZombie> zombie;
    SMShowObj<ASeed> seed;
    SMShowObj<APlaceItem> placeItem;
};

#endif