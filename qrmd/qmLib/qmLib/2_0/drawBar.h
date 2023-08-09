
#ifndef __DRAWBAR__
#define __DRAWBAR__

#include "avz.h"

struct ABar {
    enum Direction {
        UP = 0,
        LEFT = 1,
        DOWN = 2,
        RIGHT = 3
    };

    int x;
    int y;
    double rate;
    int sizeX;
    int sizeY;
    uint32_t fillARGB;
    uint32_t backgroundARGB;
    int frameThickness;
    ABar::Direction direction;
    std::vector<double> separators;

    ABar() = default;

    // 绘制按指定比例填充的信息条
    // ------------参数------------
    // x 信息条中心的横坐标
    // y 信息条中心的纵坐标
    // rate 信息条的填充比例
    // sizeX 信息条内容的水平长度
    // sizeY 信息条内容的垂直长度
    // fillARGB 填充颜色与透明度
    // backgroundARGB 背景颜色和透明度
    // frameThickness 边框粗细
    // direction 信息条的填充方向
    // separators 在指定比例处绘制与填充方向垂直的分节线
    explicit ABar(int x, int y, double rate, int sizeX = 76, int sizeY = 7, uint32_t fillARGB = 0xFFFFFC00, uint32_t backgroundARGB = 0xFFFFFFFF, int frameThickness = 1, ABar::Direction direction = ABar::RIGHT, std::vector<double> separators = {})
        : x(x)
        , y(y)
        , rate(rate)
        , sizeX(sizeX)
        , sizeY(sizeY)
        , fillARGB(fillARGB)
        , backgroundARGB(backgroundARGB)
        , frameThickness(frameThickness)
        , direction(direction)
        , separators(separators)
    {
    }
};

class APainterEx : public APainter {
    __ADeleteCopyAndMove(APainterEx);

public:
    APainterEx() = default;

    void Draw(const ARect& rect, int duration = 1)
    {
        APainter::Draw(rect, duration);
    }
    void Draw(const AText& posText, int duration = 1)
    {
        APainter::Draw(posText, duration);
    }
    void Draw(const ABar& bar, int duration = 1)
    {
        if (!_aStaticPainter.draw.IsOpen3dAcceleration()) {
            __aInternalGlobal.loggerPtr->Warning("您尚未开启 3D 加速，无法使用绘制类");
            return;
        }
        __AStaticPainter::DrawInfo info;

        int x = bar.x;
        int y = bar.y;
        double rate = bar.rate;
        int sizeX = bar.sizeX;
        int sizeY = bar.sizeY;
        int frameThickness = bar.frameThickness;
        uint32_t fillARGB = bar.fillARGB;
        uint32_t backgroundARGB = bar.backgroundARGB;
        ABar::Direction direction = bar.direction;
        std::vector<double> separators = bar.separators;

        int contentX = int(rate * sizeX);
        int contentY = int(rate * sizeY);
        uint8_t alpha = (fillARGB & (0xFF << 24)) >> 24;

        // 绘制内容
        this->SetRectColor(fillARGB);
        switch (direction) {
        case ABar::UP:
            APainter::Draw(ARect(x - sizeX / 2, y + sizeY / 2 - contentY, sizeX, contentY));
            break;
        case ABar::LEFT:
            APainter::Draw(ARect(x + sizeX / 2 - contentX + 1, y - sizeY / 2, contentX, sizeY));
            break;
        case ABar::DOWN:
            APainter::Draw(ARect(x - sizeX / 2, y - sizeY / 2, sizeX, contentY));
            break;
        case ABar::RIGHT:
            APainter::Draw(ARect(x - sizeX / 2, y - sizeY / 2, contentX, sizeY));
            break;
        default:
            break;
        }

        // 绘制背景
        this->SetRectColor(backgroundARGB);
        switch (direction) {
        case ABar::UP:
            APainter::Draw(ARect(x - sizeX / 2, y - sizeY / 2, sizeX, sizeY - contentY));
            break;
        case ABar::LEFT:
            APainter::Draw(ARect(x - sizeX / 2, y - sizeY / 2, sizeX - contentX, sizeY));
            break;
        case ABar::DOWN:
            APainter::Draw(ARect(x - sizeX / 2, y - sizeY / 2 + contentY, sizeX, sizeY - contentY));
            break;
        case ABar::RIGHT:
            APainter::Draw(ARect(x - sizeX / 2 + contentX, y - sizeY / 2, sizeX - contentX, sizeY));
            break;
        default:
            break;
        }

        // 绘制边框
        if (frameThickness > 0) {
            this->SetRectColor(AArgb(alpha, 0x00, 0x00, 0x00));
            // 绘制上边框
            APainter::Draw(ARect(x - sizeX / 2 - frameThickness, y - sizeY / 2 - frameThickness, frameThickness + sizeX, frameThickness));
            // 绘制右边框
            APainter::Draw(ARect(x + sizeX / 2, y - sizeY / 2 - frameThickness, frameThickness, frameThickness + sizeY));
            // 绘制下边框
            APainter::Draw(ARect(x - sizeX / 2, y + sizeY / 2, sizeX + frameThickness, frameThickness));
            // 绘制左边框
            APainter::Draw(ARect(x - sizeX / 2 - frameThickness, y - sizeY / 2, frameThickness, sizeY + frameThickness));
            // 绘制分节线
            for (const auto& each : separators) {
                switch (direction) {
                case ABar::UP:
                    APainter::Draw(ARect(x - sizeX / 2, y + sizeY / 2 - each * sizeY, sizeX, frameThickness));
                    break;
                case ABar::LEFT:
                    APainter::Draw(ARect(x + sizeX / 2 - each * sizeX, y - sizeY / 2, frameThickness, sizeY));
                    break;
                case ABar::DOWN:
                    APainter::Draw(ARect(x - sizeX / 2, y - sizeY / 2 + each * sizeY, sizeX, frameThickness));
                    break;
                case ABar::RIGHT:
                    APainter::Draw(ARect(x - sizeX / 2 + each * sizeX, y - sizeY / 2, frameThickness, sizeY));
                    break;
                default:
                    break;
                }
            }
        }

        info.duration = duration;
    }
};

#endif