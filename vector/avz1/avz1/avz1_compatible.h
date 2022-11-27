/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-10-30 22:37:55
 * @Description: #define some api name for old avz version
 */

#ifndef __AVZ1_COMPATIBLE_H__
#define __AVZ1_COMPATIBLE_H__

#include "libavz1.h"

// avz_card.h
#define selectCards SelectCards
#define cardNotInQueue CardNotInQueue
#define card Card

// avz_click.h
#define gridToCoordinate GridToCoordinate
#define clickGrid ClickGrid
#define clickSeed ClickSeed
#define safeClick SafeClick
#define leftClick LeftClick
#define shovelNotInQueue ShovelNotInQueue
#define shovel Shovel

// avz_debug.h
#define setErrorMode SetErrorMode
#define showErrorNotInQueue ShowErrorNotInQueue
#define showError ShowError

// avz_global.h
#define rangeIn RangeIn
#define findSameEle FindSameEle
#define readMemory ReadMemory
#define writeMemory WriteMemory

// avz_memory.h
#define pvzBase GetPvzBase
#define mouseRow MouseRow
#define mouseCol MouseCol
#define getSeedIndex GetSeedIndex
#define getPlantIndex GetPlantIndex
#define getPlantIndices GetPlantIndices
#define isZombieExist IsZombieExist
#define setWaveLength SetWavelength
#define setZombies SetZombies
#define setWaveZombies SetWaveZombies
#define Ice3(delay_time) SetPlantActiveTime(ICE_SHROOM, delay_time)
#define ice3 Ice3
#define setGameSpeed SetGameSpeed

// avz_time_operation.h
#define openMultipleEffective OpenMultipleEffective
#define setTime SetTime
#define delay Delay
#define setNowTime SetNowTime
#define waitUntil WaitUntil
#define nowTime NowTime
#define getRefreshedWave GetRefreshedWave
#define insertOperation InsertOperation
#define insertTimeOperation InsertTimeOperation
#define setInsertOperation SetInsertOperation
#define showQueue ShowQueue

#endif