/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-09-28 19:53:47
 * @Description:
 */

#ifndef __SM_TYPES_H__
#define __SM_TYPES_H__

#include "sm_d3d.h"
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>

struct SMFindInfo {
    float threshold;
    float offset_x;
    float offset_y;

    SMFindInfo(float _threshold, float _offset_x, float _offset_y)
        : threshold(_threshold)
        , offset_x(_offset_x)
        , offset_y(_offset_y)
    {
    }

    SMFindInfo()
        : threshold(40)
        , offset_x(0.5f)
        , offset_y(0.5f)
    {
    }
};

class SMGeoVertex {
    float __x, __y, __z, __rhw;
    DWORD __color;

public:
    SMGeoVertex()
        : __x(0.0f)
        , __y(0.0f)
        , __z(0.0f)
        , __rhw(0.0f)
        , __color(0)
    {
    }
    SMGeoVertex(float x, float y, DWORD color)
        : __x(x)
        , __y(y)
        , __z(0.0f)
        , __rhw(1.0f)
        , __color(color)
    {
    }
    SMGeoVertex(int x, int y, DWORD color)
        : __x((float)x)
        , __y((float)y)
        , __z(0.0f)
        , __rhw(1.0f)
        , __color(color)
    {
    }
};
class SMTexVertex {
    float __x, __y, __z, __rhw;
    DWORD __color, __spec;
    float __u, __v;

public:
    SMTexVertex()
        : __x(0.0f)
        , __y(0.0f)
        , __z(0.0f)
        , __rhw(0.0f)
        , __color(0)
        , __spec(0)
        , __u(0)
        , __v(0)
    {
    }
    SMTexVertex(float x, float y, DWORD color, float u, float v)
        : __x(x)
        , __y(y)
        , __z(0.0f)
        , __rhw(1.0f)
        , __color(color)
        , __spec(0)
        , __u(u)
        , __v(v)
    {
    }
    SMTexVertex(int x, int y, DWORD color, float u, float v)
        : __x((float)x)
        , __y((float)y)
        , __z(0.0f)
        , __rhw(1.0f)
        , __color(color)
        , __spec(0)
        , __u(u)
        , __v(v)
    {
    }
};

struct SMTextureNeedInfo {
    IDirect3DDevice7* device = nullptr;
    IDirectDraw7* ddraw = nullptr;
    HFONT HFont;
    int fontW;
    int fontH;
    int fontG;
};

using SMText = std::string;

#endif