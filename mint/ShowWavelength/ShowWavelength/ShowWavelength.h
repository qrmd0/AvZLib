/*
 * @Description：A-TAS显示波长信息
 */
#pragma once
#ifndef __SHOWWAVELENGTH_H__
#define __SHOWWAVELENGTH_H__
#include "Draw.h"

// 绘制进度条的painter
ATASPainter barPainter;
// 绘制战场信息的painter
ATASPainter fightInfoPainter;
// 用于绘制的tickRunner
ATickRunner tickPainter;

int RealCountdown() {
    if (AGetMainObject()->Wave() == 20)
        return AGetMainObject()->LevelEndCountdown();
    if (AGetMainObject()->RefreshCountdown() > 200)
        return 0;
    if (AGetMainObject()->Wave() == 9 || AGetMainObject()->Wave() == 19) {
        if (AGetMainObject()->RefreshCountdown() <= 5)
            return AGetMainObject()->HugeWaveCountdown();
        return AGetMainObject()->RefreshCountdown() + 745;
    }
    return AGetMainObject()->RefreshCountdown();
}

// 显示波长信息 (高级档位, 显示波数, 显示倍速)
void ShowWavelength(bool Advanced, int WaveCount = 5, bool ShowSpeed = true) {
    fightInfoPainter.SetFontSize(17); // 波数时间样式
    tickPainter.Start([=] {
        // 本波总血条
        if (AGetMainObject()->Wave() < 1)
            barPainter.Draw(ABar(58, 574, 1, 0, {}, 1, ABar::RIGHT, 127, 24, 0xFF9868BC, 0xC0FFFFFF));
        else if (ARangeIn(AGetMainObject()->Wave(), {9, 19, 20}) || Advanced)
            barPainter.Draw(ABar(58, 574, AGetMainObject()->MRef<int>(0x5598), AAsm::ZombieTotalHp(ANowWave() - 1), {AGetMainObject()->ZombieRefreshHp()}, 1, ABar::RIGHT, 127, 24, RealCountdown() ? 0xFF6D706C : 0xFF9868BC, 0xC0FFFFFF));
        else
            barPainter.Draw(ABar(58, 574, AGetMainObject()->MRef<int>(0x5598), AAsm::ZombieTotalHp(ANowWave() - 1), {AGetMainObject()->MRef<int>(0x5598) * 13 / 20, AGetMainObject()->MRef<int>(0x5598) / 2}, 1, ABar::RIGHT, 127, 24, 0xFF9868BC, 0xC0FFFFFF));
        // 波数时间
        fightInfoPainter.Draw(AText((AGetMainObject()->Wave() < 10 ? "0" : "") + std::to_string(AGetMainObject()->Wave() ?: 1) + ",", 59, 575), 0xFF0000FF);
        fightInfoPainter.Draw(AText(std::to_string(ANowTime(ANowWave()) == -2147483648 ? (AGetMainObject()->CompletedRounds() ? -600 : -1800) : ANowTime(ANowWave())), 82, 575), 0xFF0000FF);
        fightInfoPainter.Draw(AText(RealCountdown() && (ARangeIn(AGetMainObject()->Wave(), {9, 19, 20}) || Advanced) ? std::to_string(-RealCountdown()) : "", 145, 575), 0xFFFF0000);
        // 波长记录
        for (int i = 0; i < WaveCount; ++i) {
            if (AGetMainObject()->Wave() - i > 0 && ANowTime(ANowWave() - i) > 0 && RealCountdown() && (ARangeIn(AGetMainObject()->Wave(), {9, 19, 20}) || Advanced)) {
                barPainter.Draw(ABar(191 + 71 * i, 574, 1, 0, {}, 1, ABar::RIGHT, 65, 24, 0xFFFFC000, 0xC0FFFFFF));
                fightInfoPainter.Draw(AText((AGetMainObject()->Wave() - i < 10 ? "0" : "") + std::to_string(AGetMainObject()->Wave() - i ?: 1) + ",", 193 + 71 * i, 575), 0xFF0000FF);
                fightInfoPainter.Draw(AText(std::to_string(i ? ANowTime(ANowWave() - i) - ANowTime(ANowWave() + 1 - i) : ANowTime(ANowWave()) + RealCountdown()), 216 + 71 * i, 575), 0xFF0000FF);
            } else if (AGetMainObject()->Wave() - i > 1 && ANowTime(ANowWave() - 1 - i) > 0) {
                barPainter.Draw(ABar(191 + 71 * i, 574, 1, 0, {}, 1, ABar::RIGHT, 65, 24, 0xFFFFC000, 0xC0FFFFFF));
                fightInfoPainter.Draw(AText((AGetMainObject()->Wave() - 1 - i < 10 ? "0" : "") + std::to_string(AGetMainObject()->Wave() - 1 - i ?: 1) + ",", 193 + 71 * i, 575), 0xFF0000FF);
                fightInfoPainter.Draw(AText(std::to_string(ANowTime(ANowWave() - 1 - i) - ANowTime(ANowWave() - i)), 216 + 71 * i, 575), 0xFF0000FF);
            }
        }
        // 显示倍速
        if (AGetPvzBase()->TickMs() != 10 && ShowSpeed) {
            barPainter.Draw(ABar(6, 574, 1, 1, {}, 1, ABar::RIGHT, 46, 24, AGetPvzBase()->TickMs() > 10 ? 0xFFFF0000 : 0xFF00FF00));
            fightInfoPainter.Draw(AText(std::to_string(10 / AGetPvzBase()->TickMs()), 8, 575), 0xFF000000);
            if (AGetPvzBase()->TickMs() == 1) {
                fightInfoPainter.Draw(AText(".", 26, 575), 0xFF000000);
                fightInfoPainter.Draw(AText("0", 30, 575), 0xFF000000);
            } else {
                fightInfoPainter.Draw(AText(".", 17, 575), 0xFF000000);
                fightInfoPainter.Draw(AText(1000 / AGetPvzBase()->TickMs() % 100 ? std::to_string(1000 / AGetPvzBase()->TickMs() % 100) : "00", 21, 575), 0xFF000000);
            }
            fightInfoPainter.Draw(AText("x", 39, 575), 0xFF000000);
        }
    },
        ATickRunner::PAINT);
}

#endif //!__SHOWWAVELENGTH_H__