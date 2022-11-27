/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-10-30 21:03:04
 * @Description:
 */
#ifndef __LIBAVZ1_H__
#define __LIBAVZ1_H__

#include "avz1_cannon.h"
#include "avz1_card.h"
#include "avz1_click.h"
#include "avz1_debug.h"
#include "avz1_execption.h"
#include "avz1_iterator.h"
#include "avz1_memory.h"
#include "avz1_pvzfunc.h"
#include "avz1_pvzstruct.h"
#include "avz1_tick.h"

namespace AvZ {
inline IceFiller ice_filler;
inline PlantFixer plant_fixer;
inline PaoOperator pao_operator;
inline AliveFilter<Zombie> alive_zombie_filter;
inline AliveFilter<Plant> alive_plant_filter;
inline AliveFilter<Seed> alive_seed_filter;
inline AliveFilter<PlaceItem> alive_place_item_filter;
} // namespace AvZ

#define StartMaidCheats AvZ::MaidCheats::dancing
#define StopMaidCheats AvZ::MaidCheats::stop

#endif