/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-09-28 18:13:27
 * @Description:
 */

#ifndef __DRAW_H__
#define __DRAW_H__

#include <avz.h>

class SMPainter : public APainter {
public:
    // 控制进度条内容的增长方向
    enum Direction { UP = 0,
        LEFT = 1,
        DOWN = 2,
        RIGHT = 3 };

    // 绘制矩形进度条
    // [painter] 填入用于绘制进度条的 SMPainter;
    // [posX] 填入进度条中心所在的横坐标；
    // [posY] 填入进度条中心所在的纵坐标；
    // [rate] 填入进度条的进度比率，取值范围为 [0, 1]；
    // [sizeX] 填入进度条内容的横向长度，默认为 76；
    // [sizeY] 填入进度条内容的纵向长度，默认为 6；
    // [frameThickness] 填入进度条的边框粗细，默认为 1；
    // [ARGB]
    // 填入进度条内容的不透明度和RGB颜色信息，默认为不透明橙色。背景的不透明度与内容相同，颜色为白色，边框的不透明度与内容相同，颜色为黑色；
    // [direction] 填入进度条内容的增长方向，参数前面需加上 SMPainter:: ，默认为
    // RIGHT； [seperators] 填入进度条的分节线位置，其元素取值范围为 [0,
    // 1]，默认为空
    void DrawBar(int posX, int posY, double rate, int sizeX = 76,
        int sizeY = 6, int frameThickness = 1,
        uint32_t ARGB = 0xFFFFC00, Direction direction = RIGHT,
        std::vector<double> separators = {})
    {
        uint8_t alpha = (ARGB & (0xFF << 24)) >> 24;

        int content_x = int(rate * sizeX);
        int content_y = int(rate * sizeY);

        // 绘制内容
        this->SetRectColor(ARGB);
        switch (direction) {
        case UP:
            this->Draw(ARect(posX - sizeX / 2, posY + sizeY / 2 - content_y, sizeX,
                content_y));
            break;
        case LEFT:
            this->Draw(ARect(posX + sizeX / 2 - content_x + 1, posY - sizeY / 2,
                content_x, sizeY));
            break;
        case DOWN:
            this->Draw(ARect(posX - sizeX / 2, posY - sizeY / 2, sizeX, content_y));
            break;
        case RIGHT:
            this->Draw(ARect(posX - sizeX / 2, posY - sizeY / 2, content_x, sizeY));
            break;
        default:
            break;
        }

        // 绘制背景
        this->SetRectColor(AArgb(alpha, 0xFF, 0xFF, 0xFF));
        switch (direction) {
        case UP:
            this->Draw(ARect(posX - sizeX / 2, posY - sizeY / 2, sizeX,
                sizeY - content_y));
            break;
        case LEFT:
            this->Draw(ARect(posX - sizeX / 2, posY - sizeY / 2, sizeX - content_x,
                sizeY));
            break;
        case DOWN:
            this->Draw(ARect(posX - sizeX / 2, posY - sizeY / 2 + content_y, sizeX,
                sizeY - content_y));
            break;
        case RIGHT:
            this->Draw(ARect(posX - sizeX / 2 + content_x, posY - sizeY / 2,
                sizeX - content_x, sizeY));
            break;
        default:
            break;
        }

        // 绘制边框
        this->SetRectColor(AArgb(alpha, 0x00, 0x00, 0x00));
        // 绘制上边框
        this->Draw(ARect(posX - sizeX / 2 - frameThickness,
            posY - sizeY / 2 - frameThickness,
            frameThickness + sizeX, frameThickness));
        // 绘制右边框
        this->Draw(ARect(posX + sizeX / 2, posY - sizeY / 2 - frameThickness,
            frameThickness, frameThickness + sizeY));
        // 绘制下边框
        this->Draw(ARect(posX - sizeX / 2, posY + sizeY / 2,
            sizeX + frameThickness, frameThickness));
        // 绘制左边框
        this->Draw(ARect(posX - sizeX / 2 - frameThickness, posY - sizeY / 2,
            frameThickness, sizeY + frameThickness));

        // 绘制分节线
        for (auto each : separators) {
            switch (direction) {
            case UP:
                this->Draw(ARect(posX - sizeX / 2, posY + sizeY / 2 - int(each * sizeY),
                    sizeX, frameThickness));
                break;
            case LEFT:
                this->Draw(ARect(posX + sizeX / 2 - int(each * sizeX), posY - sizeY / 2,
                    frameThickness, sizeY));
                break;
            case DOWN:
                this->Draw(ARect(posX - sizeX / 2, posY - sizeY / 2 + int(each * sizeY),
                    sizeX, frameThickness));
                break;
            case RIGHT:
                this->Draw(ARect(posX - int(sizeX / 2) + each * sizeX, posY - sizeY / 2,
                    frameThickness, sizeY));
                break;
            default:
                break;
            }
        }
    }
};
#endif