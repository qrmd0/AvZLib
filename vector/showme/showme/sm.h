/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-09-28 20:37:23
 * @Description:
 */
#ifndef __SM_H__
#define __SM_H__

#include "avz.h"
#include "sm_obj.h"
#include "sm_painter.h"
#include <functional>

class SMShowMe {

public:
    // 得到画家
    SMPainter& getPainter()
    {
        return painter;
    }

    // 得到植物信息显示设置
    SMShowObj<Plant>& getPlantSettings()
    {
        return plant;
    }

    // 得到僵尸信息显示设置
    SMShowObj<Zombie>& getZombieSettings()
    {
        return zombie;
    }

    // 得到种子/卡片信息显示设置
    SMShowObj<Seed>& getSeedSettings()
    {
        return seed;
    }

    // 得到场地物品信息显示设置
    SMShowObj<PlaceItem>& getPlaceItemSettings()
    {
        return place_item;
    }

    bool draw()
    {
        if (!painter.isOk()) {
            return false;
        }

        if (AvZ::GetPvzBase()->gameUi() != 3) {
            return false;
        }
        auto mouse_window = AvZ::GetPvzBase()->mouseWindow();
        int mouse_x = mouse_window->mouseAbscissa();
        int mouse_y = mouse_window->mouseOrdinate();
        int txt_pos = -1;

        // Plant
        if (plant.getTypeDict().back()) {
            auto show_info = plant.getShowText(mouse_x, mouse_y);
            if (!show_info.empty()) {
                painter.drawText(show_info, mouse_x, mouse_y, ++txt_pos);
            }
        }

        // Zombie
        if (zombie.getTypeDict().back()) {
            auto show_info = zombie.getShowText(mouse_x, mouse_y);
            if (!show_info.empty()) {
                painter.drawText(show_info, mouse_x, mouse_y, ++txt_pos);
            }
        }

        // Seed
        if (seed.getTypeDict().back()) {
            auto show_info = seed.getShowText(mouse_x, mouse_y);
            if (!show_info.empty()) {
                painter.drawText(show_info, mouse_x, mouse_y, ++txt_pos);
            }
        }

        // PlaceItem
        if (place_item.getTypeDict().back()) {
            auto show_info = place_item.getShowText(mouse_x, mouse_y);
            if (!show_info.empty()) {
                painter.drawText(show_info, mouse_x, mouse_y, ++txt_pos);
            }
        }
        return true;
    }

protected:
    SMPainter painter;
    SMShowObj<Plant> plant;
    SMShowObj<Zombie> zombie;
    SMShowObj<Seed> seed;
    SMShowObj<PlaceItem> place_item;
};

#endif