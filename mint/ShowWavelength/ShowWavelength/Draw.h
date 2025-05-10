/*
 * @Description：A-TAS绘制类
 */
#pragma once
#ifndef __DRAW_H__
#define __DRAW_H__
#include "avz.h"

struct ABar {
    // 控制进度条内容的增长方向
    enum Direction {
        RIGHT = 0,
        DOWN = 1,
        LEFT = 2,
        UP = 3
    };

    int posX;
    int posY;
    double denominator;
    double numerator;
    std::vector<double> separators;
    double round;
    ABar::Direction direction;
    int length;
    int width;
    uint32_t barARGB;
    uint32_t backgroundARGB;
    uint32_t frameARGB;
    int frameThickness;

    ABar() = default;

    // 绘制矩形进度条
    // [posX] 定位点的横坐标，往右或往下长的定位点在左上角，往左或往上长的定位点在右下角；
    // [posY] 定位点的纵坐标，往右或往下长的定位点在左上角，往左或往上长的定位点在右下角；
    // [denominator] 分母；
    // [numerator] 分子；
    // [seperators] 分节线的分子，默认为空；
    // [round] 取整模式， 0 为无条件舍去， 0.5 为四舍五入， 1 为无条件进位，默认为 1；
    // [direction] 内容的增长方向，参数前面需加上 ABar:: ，默认为RIGHT；
    // [length] 增长方向的長度，默认为 100；
    // [width] 垂直于增长方向的寬度，默认为 10；
    // [barARGB] 内容的ARGB，默认为不透明橙色；
    // [backgroundARGB] 背景的ARGB，默认为不透明白色；
    // [frameARGB] 框线的ARGB，默认为不透明黑色；
    // [frameThickness] 框线的粗细，默认为 1；
    explicit ABar(int posX, int posY, double denominator, double numerator, std::vector<double> separators = {}, double round = 1, ABar::Direction direction = ABar::RIGHT, int length = 100, int width = 10, uint32_t barARGB = 0xFFFFC000, uint32_t backgroundARGB = 0xFFFFFFFF, uint32_t frameARGB = 0xFF000000, int frameThickness = 1)
        : posX(posX)
        , posY(posY)
        , denominator(denominator)
        , numerator(numerator)
        , separators(std::move(separators))
        , round(round)
        , direction(direction)
        , length(length)
        , width(width)
        , barARGB(barARGB)
        , backgroundARGB(backgroundARGB)
        , frameARGB(frameARGB)
        , frameThickness(frameThickness) {
    }
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    ABar(int posX, int posY, double denominator, double numerator, std::initializer_list<T> inputSeparators = {}, double round = 1, ABar::Direction direction = ABar::RIGHT, int length = 100, int width = 10, uint32_t barARGB = 0xFFFFC000, uint32_t backgroundARGB = 0xFFFFFFFF, uint32_t frameARGB = 0xFF000000, int frameThickness = 1)
        : posX(posX)
        , posY(posY)
        , denominator(denominator)
        , numerator(numerator)
        , round(round)
        , direction(direction)
        , length(length)
        , width(width)
        , barARGB(barARGB)
        , backgroundARGB(backgroundARGB)
        , frameARGB(frameARGB)
        , frameThickness(frameThickness) {
        // 把输入的数据类型转换成double
        for (const auto& val : inputSeparators) {
            separators.push_back(static_cast<double>(val));
        }
    }
};

struct A7Seg {
    long long n;
    int posX;
    int posY;
    int length;
    int width;
    int space;

    A7Seg() = default;

    // 绘制七段码
    // [n] 欲绘制数字；
    // [posX] 定位点的横坐标；
    // [posY] 定位点的纵坐标；
    // [length] 线长，默认为1像素；
    // [width] 线宽，默认为1像素；
    // [space] 间距，默认为1像素；
    explicit A7Seg(long long n, int posX, int posY, int length = 1, int width = 1, int space = 1)
        : n(n)
        , posX(posX)
        , posY(posY)
        , length(length)
        , width(width)
        , space(space) {
    }
};

class ATASPainter : public APainter {
public:
    void Draw(const ABar& bar, int duration = 1) {
        // 跳帧模式下，绘制无效
        if (__aGameControllor.isSkipTick()) {
            return;
        }
        if (!_basicPainter.IsOpen3dAcceleration()) {
            AGetInternalLogger()->Warning("您尚未开启 3D 加速，无法使用绘制类");
            return;
        }
        __ABasicPainter::DrawInfo info;

        // 绘制背景
        this->SetRectColor(bar.backgroundARGB);
        switch (bar.direction) {
        case ABar::RIGHT:
            APainter::Draw(ARect(bar.posX, bar.posY, bar.length, bar.width));
            break;
        case ABar::DOWN:
            APainter::Draw(ARect(bar.posX, bar.posY, bar.width, bar.length));
            break;
        case ABar::LEFT:
            APainter::Draw(ARect(bar.posX, bar.posY, bar.length, bar.width, APos::LEFT_TOP));
            break;
        case ABar::UP:
            APainter::Draw(ARect(bar.posX, bar.posY, bar.width, bar.length, APos::LEFT_TOP));
            break;
        default:
            break;
        }

        // 绘制内容
        int contentlength = (bar.numerator / bar.denominator * bar.length) == int(bar.numerator / bar.denominator * bar.length) ? int(bar.numerator / bar.denominator * bar.length) : int(bar.numerator / bar.denominator * bar.length + bar.round);
        this->SetRectColor(bar.barARGB);
        switch (bar.direction) {
        case ABar::RIGHT:
            APainter::Draw(ARect(bar.posX, bar.posY, contentlength, bar.width));
            break;
        case ABar::DOWN:
            APainter::Draw(ARect(bar.posX, bar.posY, bar.width, contentlength));
            break;
        case ABar::LEFT:
            APainter::Draw(ARect(bar.posX, bar.posY, contentlength, bar.width, APos::LEFT_TOP));
            break;
        case ABar::UP:
            APainter::Draw(ARect(bar.posX, bar.posY, bar.width, contentlength, APos::LEFT_TOP));
            break;
        default:
            break;
        }

        // 绘制边框
        this->SetRectColor(bar.frameARGB);
        switch (bar.direction) {
        case ABar::RIGHT:
            APainter::Draw(ARect(bar.posX - bar.frameThickness, bar.posY - bar.frameThickness, bar.length + bar.frameThickness, bar.frameThickness));
            APainter::Draw(ARect(bar.posX + bar.length, bar.posY - bar.frameThickness, bar.frameThickness, bar.width + bar.frameThickness));
            APainter::Draw(ARect(bar.posX, bar.posY + bar.width, bar.length + bar.frameThickness, bar.frameThickness));
            APainter::Draw(ARect(bar.posX - bar.frameThickness, bar.posY, bar.frameThickness, bar.width + bar.frameThickness));
            break;
        case ABar::DOWN:
            APainter::Draw(ARect(bar.posX - bar.frameThickness, bar.posY - bar.frameThickness, bar.width + bar.frameThickness, bar.frameThickness));
            APainter::Draw(ARect(bar.posX + bar.width, bar.posY - bar.frameThickness, bar.frameThickness, bar.length + bar.frameThickness));
            APainter::Draw(ARect(bar.posX, bar.posY + bar.length, bar.width + bar.frameThickness, bar.frameThickness));
            APainter::Draw(ARect(bar.posX - bar.frameThickness, bar.posY, bar.frameThickness, bar.length + bar.frameThickness));
            break;
        case ABar::LEFT:
            APainter::Draw(ARect(bar.posX + bar.frameThickness, bar.posY + bar.frameThickness, bar.length + bar.frameThickness, bar.frameThickness, APos::LEFT_TOP));
            APainter::Draw(ARect(bar.posX - bar.length, bar.posY + bar.frameThickness, bar.frameThickness, bar.width + bar.frameThickness, APos::LEFT_TOP));
            APainter::Draw(ARect(bar.posX, bar.posY - bar.width, bar.length + bar.frameThickness, bar.frameThickness, APos::LEFT_TOP));
            APainter::Draw(ARect(bar.posX + bar.frameThickness, bar.posY, bar.frameThickness, bar.width + bar.frameThickness, APos::LEFT_TOP));
            break;
        case ABar::UP:
            APainter::Draw(ARect(bar.posX + bar.frameThickness, bar.posY + bar.frameThickness, bar.width + bar.frameThickness, bar.frameThickness, APos::LEFT_TOP));
            APainter::Draw(ARect(bar.posX - bar.width, bar.posY + bar.frameThickness, bar.frameThickness, bar.length + bar.frameThickness, APos::LEFT_TOP));
            APainter::Draw(ARect(bar.posX, bar.posY - bar.length, bar.width + bar.frameThickness, bar.frameThickness, APos::LEFT_TOP));
            APainter::Draw(ARect(bar.posX + bar.frameThickness, bar.posY, bar.frameThickness, bar.length + bar.frameThickness, APos::LEFT_TOP));
            break;
        default:
            break;
        }

        // 绘制分节线
        for (const auto& each : bar.separators) {
            int position = ((each / bar.denominator * bar.length) == int(each / bar.denominator * bar.length) ? int(each / bar.denominator * bar.length) : int(each / bar.denominator * bar.length + bar.round)) - (each == bar.denominator ? 0 : 1);
            switch (bar.direction) {
            case ABar::RIGHT:
                APainter::Draw(ARect(bar.posX + position, bar.posY, bar.frameThickness, bar.width));
                break;
            case ABar::DOWN:
                APainter::Draw(ARect(bar.posX, bar.posY + position, bar.width, bar.frameThickness));
                break;
            case ABar::LEFT:
                APainter::Draw(ARect(bar.posX - position, bar.posY, bar.frameThickness, bar.width, APos::LEFT_TOP));
                break;
            case ABar::UP:
                APainter::Draw(ARect(bar.posX, bar.posY - position, bar.width, bar.frameThickness, APos::LEFT_TOP));
                break;
            default:
                break;
            }
        }

        info.duration = duration;
    }

    // 绘制七段码
    // [num] 欲绘制数字；
    // [edgespace] 背景矩形边界宽度；
    // [numARGB] 数字ARGB；
    // [backgroundARGB] 背景ARGB；
    // [drawbackground] 是否绘制背景；
    // [duration] 持续时间；
    void Draw(const A7Seg& num, int edgespace = 1, uint32_t numARGB = 0xFF000000, uint32_t backgroundARGB = 0xFFFFFFFF, bool drawbackground = true, int duration = 1) {
        // 跳帧模式下，绘制无效
        if (__aGameControllor.isSkipTick()) {
            return;
        }
        if (!_basicPainter.IsOpen3dAcceleration()) {
            AGetInternalLogger()->Warning("您尚未开启 3D 加速，无法使用绘制类");
            return;
        }
        __ABasicPainter::DrawInfo info;

        std::string str = std::to_string(num.n);

        if (drawbackground) {
            this->SetRectColor(backgroundARGB);
            APainter::Draw(ARect(num.posX - edgespace, num.posY - edgespace, 2 * edgespace + str.size() * (2 * num.width + num.length + num.space) - num.space, 2 * edgespace + 3 * num.width + 2 * num.length));
        }

        this->SetRectColor(numARGB);
        for (int i = 0; i < str.size(); ++i) {
            int Xoffset = i * (2 * num.width + num.length + num.space);
            switch (str[i]) {
            case '-':
                APainter::Draw(ARect(Xoffset + num.posX, num.posY + num.width + num.length, 2 * num.width + num.length, num.width));
                break;
            case '0':
                APainter::Draw(ARect(Xoffset + num.posX + num.width, num.posY, num.length, num.width));
                APainter::Draw(ARect(Xoffset + num.posX + num.width, num.posY + 2 * num.width + 2 * num.length, num.length, num.width));
                APainter::Draw(ARect(Xoffset + num.posX, num.posY, num.width, 3 * num.width + 2 * num.length));
                APainter::Draw(ARect(Xoffset + num.posX + num.width + num.length, num.posY, num.width, 3 * num.width + 2 * num.length));
                break;
            case '1':
                APainter::Draw(ARect(Xoffset + num.posX + num.length / 2, num.posY + num.width, num.width, num.width));
                APainter::Draw(ARect(Xoffset + num.posX + num.width + num.length / 2, num.posY, num.width, 3 * num.width + 2 * num.length));
                break;
            case '2':
                APainter::Draw(ARect(Xoffset + num.posX, num.posY, num.width + num.length, num.width));
                APainter::Draw(ARect(Xoffset + num.posX + num.width, num.posY + num.width + num.length, num.length, num.width));
                APainter::Draw(ARect(Xoffset + num.posX + num.width, num.posY + 2 * num.width + 2 * num.length, num.length + num.width, num.width));
                APainter::Draw(ARect(Xoffset + num.posX, num.posY + num.width + num.length, num.width, 2 * num.width + num.length));
                APainter::Draw(ARect(Xoffset + num.posX + num.width + num.length, num.posY, num.width, 2 * num.width + num.length));
                break;
            case '3':
                APainter::Draw(ARect(Xoffset + num.posX, num.posY, num.width + num.length, num.width));
                APainter::Draw(ARect(Xoffset + num.posX, num.posY + num.width + num.length, num.width + num.length, num.width));
                APainter::Draw(ARect(Xoffset + num.posX, num.posY + 2 * num.width + 2 * num.length, num.width + num.length, num.width));
                APainter::Draw(ARect(Xoffset + num.posX + num.width + num.length, num.posY, num.width, 3 * num.width + 2 * num.length));
                break;
            case '4':
                APainter::Draw(ARect(Xoffset + num.posX + num.width, num.posY + num.width + num.length, num.length, num.width));
                APainter::Draw(ARect(Xoffset + num.posX, num.posY, num.width, 2 * num.width + num.length));
                APainter::Draw(ARect(Xoffset + num.posX + num.width + num.length, num.posY, num.width, 3 * num.width + 2 * num.length));
                break;
            case '5':
                APainter::Draw(ARect(Xoffset + num.posX + num.width, num.posY, num.length + num.width, num.width));
                APainter::Draw(ARect(Xoffset + num.posX + num.width, num.posY + num.width + num.length, num.length, num.width));
                APainter::Draw(ARect(Xoffset + num.posX, num.posY + 2 * num.width + 2 * num.length, num.width + num.length, num.width));
                APainter::Draw(ARect(Xoffset + num.posX, num.posY, num.width, 2 * num.width + num.length));
                APainter::Draw(ARect(Xoffset + num.posX + num.width + num.length, num.posY + num.width + num.length, num.width, 2 * num.width + num.length));
                break;
            case '6':
                APainter::Draw(ARect(Xoffset + num.posX + num.width, num.posY, num.length + num.width, num.width));
                APainter::Draw(ARect(Xoffset + num.posX + num.width, num.posY + num.width + num.length, num.length, num.width));
                APainter::Draw(ARect(Xoffset + num.posX + num.width, num.posY + 2 * num.width + 2 * num.length, num.length, num.width));
                APainter::Draw(ARect(Xoffset + num.posX, num.posY, num.width, 3 * num.width + 2 * num.length));
                APainter::Draw(ARect(Xoffset + num.posX + num.width + num.length, num.posY + num.width + num.length, num.width, 2 * num.width + num.length));
                break;
            case '7':
                APainter::Draw(ARect(Xoffset + num.posX, num.posY, num.width + num.length, num.width));
                APainter::Draw(ARect(Xoffset + num.posX + num.width + num.length, num.posY, num.width, 3 * num.width + 2 * num.length));
                break;
            case '8':
                APainter::Draw(ARect(Xoffset + num.posX + num.width, num.posY, num.length, num.width));
                APainter::Draw(ARect(Xoffset + num.posX + num.width, num.posY + num.width + num.length, num.length, num.width));
                APainter::Draw(ARect(Xoffset + num.posX + num.width, num.posY + 2 * num.width + 2 * num.length, num.length, num.width));
                APainter::Draw(ARect(Xoffset + num.posX, num.posY, num.width, 3 * num.width + 2 * num.length));
                APainter::Draw(ARect(Xoffset + num.posX + num.width + num.length, num.posY, num.width, 3 * num.width + 2 * num.length));
                break;
            case '9':
                APainter::Draw(ARect(Xoffset + num.posX + num.width, num.posY, num.length, num.width));
                APainter::Draw(ARect(Xoffset + num.posX + num.width, num.posY + num.width + num.length, num.length, num.width));
                APainter::Draw(ARect(Xoffset + num.posX, num.posY + 2 * num.width + 2 * num.length, num.width + num.length, num.width));
                APainter::Draw(ARect(Xoffset + num.posX, num.posY, num.width, 2 * num.width + num.length));
                APainter::Draw(ARect(Xoffset + num.posX + num.width + num.length, num.posY, num.width, 3 * num.width + 2 * num.length));
                break;
            default:
                break;
            }
        }
        info.duration = duration;
    }

    void Draw(const AText& posText, uint32_t textARGB = 0xFF00FFFF, uint32_t backgroundARGB = 0x0, int duration = 1) {
        // 跳帧模式下，绘制无效
        if (__aGameControllor.isSkipTick()) {
            return;
        }
        if (!_basicPainter.IsOpen3dAcceleration()) {
            AGetInternalLogger()->Warning("您尚未开启 3D 加速，无法使用绘制类");
            return;
        }
        this->SetTextColor(textARGB);
        this->SetRectColor(backgroundARGB);
        APainter::Draw(posText, duration);
    }

    void Draw(const ARect& rect, uint32_t RectARGB = 0xAF000000, int duration = 1) {
        // 跳帧模式下，绘制无效
        if (__aGameControllor.isSkipTick()) {
            return;
        }
        if (!_basicPainter.IsOpen3dAcceleration()) {
            AGetInternalLogger()->Warning("您尚未开启 3D 加速，无法使用绘制类");
            return;
        }
        this->SetRectColor(RectARGB);
        APainter::Draw(rect, duration);
    }
};

#endif //!__DRAW_H__