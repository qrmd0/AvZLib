/*
 * @Author: qrmd
 * @Date: 2023-07-31 13:13:32
 * @LastEditors: qrmd
 * @LastEditTime: 2023-08-14 15:52:08
 * @Description:更好的自动修补植物类，支持紫卡修补等更多功能
 */

#ifndef __PLANTFIXERPLUS__
#define __PLANTFIXERPLUS__

#include "avz.h"

#if __AVZ_VERSION__ <= 221001
#include "PlantFixerPlus_1_0.h"
#else
#include "PlantFixerPlus_2_0.h"
#endif

#endif