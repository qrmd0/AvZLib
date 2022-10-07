/*
 * @Author: qrmd
 * @Date: 2022-10-02 10:03:02
 * @LastEditors: qrmd
 * @LastEditTime: 2022-10-07 23:09:07
 * @Description:avz_showme 进度条示例
 * 使用方法：1、前往https://gitee.com/vector-wlc/AsmVsZombies，根据教程下载并安装好AsmVsZombies
 *          2、前往游戏存档文件夹C:/ProgramData/PopCap Games/PlantsVsZombies/userdata，备份原游戏存档，然后用脚本配套的存档文件替换同名文件
 *          3、在Visul Studio Code中打开本脚本，右键点击编辑区空白处，在弹出菜单中选择“AvZ:Run Script”
 * 来自AvZScript公开脚本仓库：
 * 主库：https://github.com/qrmd0/AvZScript
 * 镜像库：https://gitee.com/qrmd/AvZScript
 * Copyright (c) 2022 by qrmd, All Rights Reserved.
 */
#include "avz.h"
#include "showme/sm.h"

int bar_width = 100; // 水平进度条的宽度（像素）
int bar_height = 6;  // 水平进度条的高度（像素）
int frame_size = 1;  // 边框粗细（像素）

int JACK_IN_THE_BOX_OPEN_BOX_TIME = 110;         // 小丑僵尸开盒时长
int BUNGEE_REMAIN_TIME = 300;                    // 蹦极僵尸停留时长
int BUNGEE_GRAB_TIME = 63;                       // 蹦极僵尸抓取时长
int SHOOT_BASKETBALL_COUNTDOWN = 300;            // 投篮僵尸投篮时长
int FREEZE_TIME = 600;                           // 寒冰菇冻结最长时间
int CANNON_LOADING_TIME = 3475;                  // 玉米加农炮装填时间
int CRATER_REMAINING_TIME = 18000;               // 弹坑消失时间
float SHOOT_BASKETBALL_CIRCULATION_RATE = 0.243; // 投篮僵尸抛出篮球时的动画循环率
float HAMMER_CIRCULATION_RATE = 0.644;           // 巨人僵尸锤击命中时的动画循环率
float THROW_IMP_CIRCULATION_RATE = 0.739;        // 巨人僵尸投出小鬼时的动画循环率
using namespace AvZ;
SMShowMe show_bar;

struct ZombieAdd : public Zombie {
    // 僵尸动画编号
    uint16_t& animationCode()
    {
        return (uint16_t&)((uint8_t*)this)[0x118];
    }
    // 受伤判定高度
    int& hurtHeight()
    {
        return (int&)((uint8_t*)this)[0x98];
    }
    // 本体血量上限
    int& hpMax()
    {
        return (int&)((uint8_t*)this)[0xCC];
    }
    // 一类饰品血量上限
    int& oneHpMax()
    {
        return (int&)((uint8_t*)this)[0xD4];
    }
};
struct PlantAdd : public Plant {
    // 生产物品倒计时
    int& produceCountdown()
    {
        return (int&)((uint8_t*)this)[0x58];
    }
};
struct AnimationAdd : public Animation {
    // 动画循环次数
    int& circulationCount()
    {
        return (int&)((uint8_t*)this)[0x5c];
    }
};
// 场地物品对象
struct GroundItem {
private:
    void operator=(GroundItem&& _) { }
    GroundItem(GroundItem&& _) { }
    uint8_t data[0xec];

public:
    GroundItem() { }
    // 是否消失
    const bool& isDisappeared()
    {
        return (bool&)((uint8_t*)this)[0x20];
    }

    // 类型
    // 1 - 墓碑
    // 2 - 弹坑
    // 3 - 梯子
    // 11 - 钉耙
    // 12 - 脑子
    int& type()
    {
        return (int&)((uint8_t*)this)[0x8];
    }
    // 行数
    // 范围 : [0, 5]
    int& row()
    {
        return (int&)((uint8_t*)this)[0x14];
    }
    // 列数
    // 范围 : [0, 8]
    int& col()
    {
        return (int&)((uint8_t*)this)[0x10];
    }
    // 属性倒计时（墓碑冒出的数量、弹坑消失倒计时、脑子血量、钉钯消失倒计时）
    int& countdown()
    {
        return (int&)((uint8_t*)this)[0x18];
    }
};
struct MainObjectAdd : public MainObject {
    // 场地物品内存数组
    GroundItem* groundItemArray()
    {
        return *(GroundItem**)((uint8_t*)this + 0x11c);
    }
    // 场地物品内存数组大小
    int& groundItemTotal()
    {
        return (int&)((uint8_t*)this)[0x120];
    }
};
void Draw()
{
    // 游戏暂停时不显示信息
    if (ReadMemory<bool>(0x6A9EC0, 0x768, 0x164))
        return;

    auto&& bar_painter = show_bar.getPainter();
    auto animations = (AnimationAdd*)GetPvzBase()->animationMain()->animationOffset()->animationArray();
    auto zombies = (ZombieAdd*)(GetMainObject()->zombieArray());
    auto plants = (PlantAdd*)GetMainObject()->plantArray();
    auto main_object_add = (MainObjectAdd*)GetMainObject();
    auto groundItems = (GroundItem*)(main_object_add->groundItemArray());
    int index_animation = 0;
    float circulation_rate = 0.0;
    int bar_pos_x = 0;
    int bar_pos_y = 0;
    int freeze_countdown = 0;
    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index) {
        if (!zombies[index].isDisappeared() && !zombies[index].isDead()) {
            bar_pos_x = int((2 * zombies[index].abscissa() + zombies[index].bulletAbscissa() + zombies[index].hurtWidth()) / 2);
            // 僵尸动画进度
            bar_pos_y = int(zombies[index].ordinate() + zombies[index].bulletOrdinate() - (bar_height * 3 + 5 * frame_size));
            // 撑杆僵尸跳跃
            if (zombies[index].type() == POLE_VAULTING_ZOMBIE && zombies[index].state() == 12) {
                index_animation = zombies[index].animationCode();
                circulation_rate = animations[index_animation].circulationRate();

                bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {});
            }
            // 读报僵尸失去报纸
            if (zombies[index].type() == NEWSPAPER_ZOMBIE && zombies[index].state() == 30) {
                index_animation = zombies[index].animationCode();
                circulation_rate = animations[index_animation].circulationRate();

                bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {});
            }
            // // 舞王僵尸和伴舞僵尸跳舞
            // if ((zombies[index].type() == DANCING_ZOMBIE || zombies[index].type() == BACKUP_DANCER) && zombies[index].state() >= 44 && zombies[index].state() <= 49) {
            //     index_animation = zombies[index].animationCode();
            //     circulation_rate = animations[index_animation].circulationRate();

            //     bar_painter.drawBar(bar_pos_x, bar_pos_y,circulation_rate, bar_width, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {});
            // }
            // 潜水僵尸入水
            if (zombies[index].type() == SNORKEL_ZOMBIE && zombies[index].state() == 58) {
                index_animation = zombies[index].animationCode();
                circulation_rate = animations[index_animation].circulationRate();

                bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {});
            }
            // 海豚僵尸入水和跳跃
            if (zombies[index].type() == DOLPHIN_RIDER_ZOMBIE && (zombies[index].state() == 52 || zombies[index].state() == 54)) {
                index_animation = zombies[index].animationCode();
                circulation_rate = animations[index_animation].circulationRate();

                bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {});
            }
            // 玩偶匣僵尸开盒
            if (zombies[index].type() == JACK_IN_THE_BOX_ZOMBIE && zombies[index].state() == 16) {
                circulation_rate = 1 - zombies[index].stateCountdown() / float(JACK_IN_THE_BOX_OPEN_BOX_TIME);

                bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {});
            }
            // 气球僵尸落地
            if (zombies[index].type() == BALLOON_ZOMBIE && zombies[index].state() == 74) {
                index_animation = zombies[index].animationCode();
                circulation_rate = animations[index_animation].circulationRate();

                bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {});
            }
            // 矿工僵尸出土后眩晕
            if (zombies[index].type() == DIGGER_ZOMBIE && zombies[index].state() == 36) {
                index_animation = zombies[index].animationCode();
                circulation_rate = (animations[index_animation].circulationRate() + animations[index_animation].circulationCount()) / 2.0;

                bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {});
            }
            // 蹦极僵尸停留
            if (zombies[index].type() == BUNGEE_ZOMBIE && (zombies[index].state() == 6 || zombies[index].state() == 7)) {
                if (zombies[index].state() == 6) {
                    circulation_rate = 1 - (zombies[index].stateCountdown() + BUNGEE_GRAB_TIME) / float(BUNGEE_REMAIN_TIME + BUNGEE_GRAB_TIME);
                } else if (zombies[index].state() == 7) {
                    index_animation = zombies[index].animationCode();
                    circulation_rate = (BUNGEE_REMAIN_TIME + animations[index_animation].circulationRate() * BUNGEE_GRAB_TIME) / float(BUNGEE_REMAIN_TIME + BUNGEE_GRAB_TIME);
                }

                bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {300.0 / (300 + 63)});
            }
            // 投篮僵尸投篮
            if (zombies[index].type() == CATAPULT_ZOMBIE && (zombies[index].state() == 67 || zombies[index].state() == 68)) {
                circulation_rate = 1 - zombies[index].stateCountdown() / float(SHOOT_BASKETBALL_COUNTDOWN);

                bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {SHOOT_BASKETBALL_CIRCULATION_RATE});
            }
            // 巨人僵尸锤击
            if ((zombies[index].type() == GIGA_GARGANTUAR || zombies[index].type() == GARGANTUAR) && zombies[index].isHammering()) {
                index_animation = zombies[index].animationCode();
                circulation_rate = animations[index_animation].circulationRate();

                bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {HAMMER_CIRCULATION_RATE});
            }
            // 巨人僵尸投掷小鬼
            if ((zombies[index].type() == GIGA_GARGANTUAR || zombies[index].type() == GARGANTUAR) && zombies[index].state() == 69) {
                index_animation = zombies[index].animationCode();
                circulation_rate = animations[index_animation].circulationRate();

                bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {THROW_IMP_CIRCULATION_RATE});
            }
            // 僵尸冰冻倒计时
            bar_pos_y = int(zombies[index].ordinate() + zombies[index].bulletOrdinate() - (2 * bar_height + 4 * frame_size));
            freeze_countdown = zombies[index].freezeCountdown();
            if (zombies[index].freezeCountdown() > 0) {
                circulation_rate = zombies[index].freezeCountdown() / float(FREEZE_TIME);

                bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width, bar_height, 1, SMArgb(0xFF, 0x0E, 0xA0, 0xE0), SMPainter::RIGHT, {});
            }

            // 僵尸血量（不考虑二类饰品）
            bar_pos_y = int(zombies[index].ordinate() + zombies[index].bulletOrdinate() - (bar_height + 2 * frame_size));
            circulation_rate = (zombies[index].hp() + zombies[index].oneHp()) / float(zombies[index].hpMax() + zombies[index].oneHpMax());
            if (zombies[index].type() == GARGANTUAR) {
                bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width, bar_height, 1, SMArgb(0xFF, 0x00, 0xFF, 0x00), SMPainter::RIGHT, {1800 / 3000.0});
            } else if (zombies[index].type() == GIGA_GARGANTUAR) {
                bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width, bar_height, 1, SMArgb(0xFF, 0x00, 0xFF, 0x00), SMPainter::RIGHT, {1800 / 6000.0, 1800 * 2 / 6000.0, 1800 * 3 / 6000.0});
            } else {
                bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width, bar_height, 1, SMArgb(0xFF, 0x00, 0xFF, 0x00), SMPainter::RIGHT, {});
            }
        }
    }

    // 植物进度条
    for (int index = 0; index < GetMainObject()->plantTotal(); ++index) {
        bar_pos_x = plants[index].xi() + 40;
        bar_pos_y = plants[index].yi() - (bar_height + 2 * frame_size);
        if (!plants[index].isDisappeared() && !plants[index].isCrushed()) {
            // 樱桃炸弹、寒冰菇、毁灭菇和火爆辣椒生效进度条
            if (plants[index].type() == CHERRY_BOMB || plants[index].type() == ICE_SHROOM || plants[index].type() == DOOM_SHROOM || plants[index].type() == JALAPENO) {
                if (plants[index].explodeCountdown() > 0) {
                    circulation_rate = plants[index].explodeCountdown() / 100.0;

                    bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, 72, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {});
                }
            }
            // 咖啡豆生效进度条（咖啡豆消失后进度条不显示，待完善）
            if (plants[index].type() == COFFEE_BEAN) {
                if (plants[index].explodeCountdown() > 0) {
                    circulation_rate = (plants[index].explodeCountdown() + plants[index].bloverCountdown()) / 299.0;

                    bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, 72, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {});
                }
            }
            // 向日葵、双子向日葵、阳光菇和金盏花生产进度条
            if (plants[index].type() == SUNFLOWER || plants[index].type() == TWIN_SUNFLOWER || plants[index].type() == SUN_SHROOM || plants[index].type() == MARIGOLD) {
                if (plants[index].produceCountdown() > 0) {
                    circulation_rate = plants[index].produceCountdown() / 2500.0;

                    bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, 72, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {});
                }
            }
            // 土豆地雷准备进度条
            if (plants[index].type() == POTATO_MINE && plants[index].state() != 16) {
                circulation_rate = plants[index].stateCountdown() / 1500.0;

                bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, 72, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {});
            }
            // 大嘴花消化进度条
            if (plants[index].type() == CHOMPER && plants[index].state() == 13) {
                circulation_rate = plants[index].stateCountdown() / 4000.0;

                bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, 72, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {});
            }
            // 倭瓜砸击进度条
            if (plants[index].type() == SQUASH) {
                if (plants[index].state() >= 3 && plants[index].state() <= 6) {
                    if (plants[index].state() == 3) {
                        circulation_rate = (plants[index].stateCountdown() + 45 + 50 + 7) / float(80 + 45 + 50 + 7);
                    } else if (plants[index].state() == 4) {
                        circulation_rate = (plants[index].stateCountdown() + 50 + 7) / float(80 + 45 + 50 + 7);
                    } else if (plants[index].state() == 5) {
                        circulation_rate = (plants[index].stateCountdown() + 7) / float(80 + 45 + 50 + 7);
                    } else if (plants[index].state() == 6) {
                        circulation_rate = (plants[index].stateCountdown()) / float(80 + 45 + 50 + 7);
                    }
                    bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, 72, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {});
                }
            }
            // 墓碑吞噬者消失进度条
            if (plants[index].type() == GRAVE_BUSTER) {
                if (plants[index].stateCountdown() > 0) {
                    circulation_rate = plants[index].stateCountdown() / 400.0;

                    bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, 72, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {});
                }
            }
            // 三叶草生效进度条
            if (plants[index].type() == BLOVER) {
                if (plants[index].bloverCountdown() > 0) {
                    circulation_rate = (plants[index].explodeCountdown() + plants[index].bloverCountdown()) / 250.0;

                    bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, 72, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {(250 - 50) / 250.0});
                }
            }
            // 磁力菇复用进度条
            if (plants[index].type() == MAGNET_SHROOM) {
                if (plants[index].stateCountdown() > 0) {
                    circulation_rate = plants[index].stateCountdown() / 1500.0;

                    bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, 72, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {});
                }
            }
            // 玉米加农炮复用进度条
            if (plants[index].type() == COB_CANNON && plants[index].state() != 37) {
                index_animation = plants[index].animationCode();
                switch (plants[index].state()) {
                case 35:
                    circulation_rate = (125 + plants[index].stateCountdown()) / float(CANNON_LOADING_TIME);
                    break;
                case 36:
                    circulation_rate = (int(125 * (1 - animations[index_animation].circulationRate()) + 0.5) + 1) / float(CANNON_LOADING_TIME);
                    break;
                case 38:
                    circulation_rate = (3125 + int(350 * (1 - animations[index_animation].circulationRate()) + 0.5)) / float(CANNON_LOADING_TIME);
                    break;
                default:
                    circulation_rate = 0;
                    break;
                }
                bar_painter.drawBar(bar_pos_x + 40, bar_pos_y, circulation_rate, 2 * 72, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {});
            }
            // 植物血量
            bar_pos_y = plants[index].yi();
            circulation_rate = plants[index].hp() / float(plants[index].hpMax());
            if (plants[index].type() == COB_CANNON) {
                bar_painter.drawBar(bar_pos_x + 40, bar_pos_y, circulation_rate, 2 * 72, bar_height, 1, SMArgb(0xFF, 0x00, 0xFF, 0x00), SMPainter::RIGHT, {});
            } else {
                if (plants[index].type() == LILY_PAD || plants[index].type() == FLOWER_POT) {
                    bar_pos_y = bar_pos_y = plants[index].yi() + (2 * bar_height + 4 * frame_size);
                } else if (plants[index].type() == PUMPKIN || plants[index].type() == COFFEE_BEAN) {
                    bar_pos_y = bar_pos_y = plants[index].yi() + (4 * bar_height + 8 * frame_size);
                }
                bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, 72, bar_height, 1, SMArgb(0xFF, 0x00, 0xFF, 0x00), SMPainter::RIGHT, {});
            }
        }
    }
    int x_diff = 80;                                                                          // 列间距
    int y_diff = (GetMainObject()->scene() == 2 || GetMainObject()->scene() == 3) ? 85 : 100; // 行间距
    // 场地物品进度条
    for (int index = 0; index < main_object_add->groundItemTotal(); ++index) {
        if (!groundItems[index].isDisappeared()) {
            if (groundItems[index].type() == 2) {
                bar_pos_x = x_diff * (groundItems[index].col() + 1);
                bar_pos_y = y_diff * (groundItems[index].row() + 1) + 10;
                circulation_rate = groundItems[index].countdown() / CRATER_REMAINING_TIME;
                bar_painter.drawBar(bar_pos_x, bar_pos_y, circulation_rate, 72, bar_height, 1, SMArgb(0xFF, 0xFF, 0xC0, 0x00), SMPainter::RIGHT, {});
            }
        }
    }
}

void Script()
{
}
