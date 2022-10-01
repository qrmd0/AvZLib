/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-09-28 18:13:27
 * @Description:
 */

#ifndef __DRAW_H__
#define __DRAW_H__

#include "sm_texture.h"

class SMPainter : public AvZ::GlobalVar {
private:
    __SMDraw draw;
    std::unordered_map<wchar_t, __SMTexture*> texture_dict;
    std::vector<std::vector<int>> pos_dict = {
        {0, -1},
        {0, 0},
        {-1, -1},
        {-1, 0},
    };

    DWORD str_color = SMArgb(0xff, 0, 0xff, 0xff);
    DWORD rect_color = SMArgb(0x7f, 0, 0, 0);

public:
    // 控制文本矩形框出现在鼠标的哪个方位
    enum PosType {
        RIGHT_TOP,
        RIGHT_BUTTOM,
        LEFT_TOP,
        LEFT_BOTTOM,
    };

    // 设置字体
    // 使用示例
    // setFont("黑体") ------ 将字体设置为黑体
    void setFont(const std::string& name)
    {
        draw.fontName = AvZ::StrToWstr(name);
        clearFont();
    }

    // 设置字体大小
    // 使用示例
    // setFontSize(15) ------ 将字体大小设置为 15
    // 注意此处字体大小不一定与 MS Word 中的相同
    void setFontSize(int size)
    {
        draw.fontSize = size;
        clearFont();
    }

    bool isOk()
    {
        static int record_clock = 0;
        static bool is_ok = false;
        int game_clock = AvZ::GetMainObject()->gameClock();
        if (game_clock != record_clock) { // 一帧刷新一次
            record_clock = game_clock;
            is_ok = draw.refresh();
        }
        return is_ok;
    }

    // 设置文本颜色
    // 使用示例
    // 注意每个参数范围为 [0, 255]
    // setColor(SMArgb(0xff, 0, 0, 0)) ----- 将文本的不透明度设置为 0xff, 也就是不透明, 色彩设置为 RGB(0, 0, 0), 也就是黑色
    // setColor(SMArgb(0x7f, 0xff, 0, 0)) ----- 将文本的不透明度设置为 0x7f, 色彩设置为 RGB(0xff, 0, 0), 也就是红色
    // setColor(SMArgb(0x7f, 0xff, 0, 0), SMArgb(0x7f, 0, 0, 0)) ---- 将文本的不透明度设置为 0x7f, 色彩设置为 RGB(0xff, 0, 0), 也就是红色, 将背景色的不透明度设置为 0x7f, 色彩设置为 RGB(0, 0, 0), 也就是黑色
    void setColor(DWORD str_color, DWORD bkg_color = SMArgb(0x7f, 0, 0, 0))
    {
        this->str_color = str_color;
        rect_color = bkg_color;
    }

    // 绘制文本
    // 使用示例
    // drawText("hello", 100, 100) ------ 在游戏画面(100, 100) 处绘制 hello
    // drawText("hello", 100, 100, RIGHT_TOP) ------ 在游戏画面(100, 100) 处绘制 hello, 并且文本绘制是在 (100, 100) 的右上方
    void drawText(const SMText& text,
        int x, int y, int pos = RIGHT_BUTTOM)
    {
        if (!isOk()) {
            return;
        }
        int rect_width = 0;

        // 这里是真的丑陋，以后再优化吧
        std::vector<SMText> text_draw;
        SMText line;
        for (auto&& ch : text) {
            if (ch != '\n') {
                line.push_back(ch);
            } else {
                if (rect_width < line.size()) {
                    rect_width = line.size();
                }
                text_draw.push_back(std::move(line));
                line.clear();
            }
        }

        if (rect_width < line.size()) {
            rect_width = line.size();
        }
        text_draw.push_back(std::move(line));
        line.clear();

        rect_width = rect_width * draw.fontSize * 0.45;
        int rect_height = text_draw.size() * draw.fontSize;
        x += rect_width * pos_dict[pos][0];
        y += rect_height * pos_dict[pos][1];
        drawRect(x, y, rect_width, rect_height);
        for (auto&& str : text_draw) {
            drawString(str, x, y);
            y += draw.fontSize;
        }
    }

    // 绘制矩形
    // 使用示例
    // drawText(100, 100, 200, 200) ------ 在游戏画面(100, 100) 处绘制宽高为 (200, 200) 的矩形
    void drawRect(int x, int y, int width, int height)
    {
        if (!isOk()) {
            return;
        }
        draw.drawRect(x, y, width, height, rect_color);
    }

    // 绘制字符串
    // 使用示例
    // drawString("hello", 100, 100) ------ 在游戏画面(100, 100) 处绘制 hello, 注意此处绘制的字符串没有背景色
    void drawString(const SMText& str, int x, int y)
    {
        if (!isOk()) {
            return;
        }
        auto wstr = AvZ::StrToWstr(str);
        for (auto&& ch : wstr) {
            auto iter = texture_dict.find(ch);
            if (iter == texture_dict.end()) { // 将字符串存入字典
                auto texPtr = new __SMTexture(ch, draw.getTextureNeedInfo());
                texture_dict[ch] = texPtr;
            }

            texture_dict[ch]->draw(str_color, x, y);
            x += ch > 0xff ? draw.fontSize : draw.fontSize / 2;
        }
    }

    virtual void enterFight() override
    {
        // InstallDrawHook
        *(uint16_t*)0x54C8CD = 0x5890;
        *(uint32_t*)0x667D0C = (uint32_t)&asmDraw;
        *(uint32_t*)0x671578 = (uint32_t)&asmDraw;
        *(uint32_t*)0x676968 = (uint32_t)&asmDraw;
    }

    virtual void exitFight() override
    {
        clearFont();
        // UninstallDrawHook
        *(uint16_t*)0x54C8CD = 0xD0FF;
        *(uint32_t*)0x667D0C = 0x54C650;
        *(uint32_t*)0x671578 = 0x54C650;
        *(uint32_t*)0x676968 = 0x54C650;
    }

private:
    void clearFont()
    {
        DeleteObject(draw.textNeedInfo.HFont);
        draw.textNeedInfo.HFont = nullptr;
        // free texture memory
        for (auto&& obj : texture_dict) {
            delete obj.second;
        }
        texture_dict.clear();
    }

    static bool asmDraw()
    {
        static int __x = 0;

#ifdef __MINGW32__
        __asm__ __volatile__(
            "pushal;"
            "movl 0x6A9EC0, %%ecx;"
            "movl $0x54C650, %%edx;"
            "calll *%%edx;"
            "movl %%eax, %[__x];"
            "popal;"
            :
            : [__x] "m"(__x)
            :);

        if (__x) {
            void Draw();
            Draw();
            __asm__ __volatile__(
                "pushal;"
                "pushl $0;"
                "movl 0x6A9EC0, %%ecx;"
                "movl $0X54BAE0, %%edx;"
                "calll *%%edx;"
                "popal;"
                :
                :
                :);
        }

        __asm__ __volatile__(
            "movl %[__x], %%eax;"
            :
            : [__x] "m"(__x)
            :);

        return __x;

#else
        // static int ans
        // asm mov ecx [0x6A9EC0]
        // asm call 0x54C650
        // asm mov ans eax
        // if(ans){
        //     drawfunc()
        //     asm push 0
        //     asm mov ecx [0x6A9EC0]
        //     asm call 0X54BAE0
        // }
        // asm mov eax ans
#endif
    }
};
#endif