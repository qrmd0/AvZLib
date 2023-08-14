/*
 * @Author: qrmd
 * @Date: 2023-07-31 13:13:32
 * @LastEditors: qrmd
 * @LastEditTime: 2023-08-14 01:05:39
 * @Description:计算僵尸横坐标
 */

#ifndef __GETZOMBIEABSCISSAS__
#define __GETZOMBIEABSCISSAS__

#include "avz.h"

#if __AVZ_VERSION__ <= 221001
#include "GetZombieAbscissas_1_0.h"
#else
#include "GetZombieAbscissas_2_0.h"
#endif

#endif