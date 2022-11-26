/*
 * @Author: qrmd
 * @Date: 2022-10-02 10:03:02
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2022-11-26 10:54:03
 * @Description:avz_showme 进度条示例
 * 使用方法：1、前往https://gitee.com/vector-wlc/AsmVsZombies，根据教程下载并安装好AsmVsZombies
 *          2、前往游戏存档文件夹C:/ProgramData/PopCap
 * Games/PlantsVsZombies/userdata，备份原游戏存档，然后用脚本配套的存档文件替换同名文件
 *          3、在Visul Studio
 * Code中打开本脚本，右键点击编辑区空白处，在弹出菜单中选择“AvZ:Run Script”
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
SMShowMe show_bar;

struct ZombieAdd : public AZombie {
    // 僵尸动画编号
    uint16_t& AnimationCode() { return (uint16_t&)((uint8_t*)this)[0x118]; }
    // 受伤判定高度
    int& hurtHeight() { return (int&)((uint8_t*)this)[0x98]; }
    // 本体血量上限
    int& hpMax() { return (int&)((uint8_t*)this)[0xCC]; }
    // 一类饰品血量上限
    int& oneHpMax() { return (int&)((uint8_t*)this)[0xD4]; }
};
struct PlantAdd : public APlant {
    // 生产物品倒计时
    int& produceCountdown() { return (int&)((uint8_t*)this)[0x58]; }
};
struct AnimationAdd : public AAnimation {
    // 动画循环次数
    int& circulationCount() { return (int&)((uint8_t*)this)[0x5c]; }
};
// 场地物品对象
struct GroundItem {
protected:
    void operator=(GroundItem&& _) { }
    GroundItem(GroundItem&& _) { }
    uint8_t data[0xec];

public:
    GroundItem() { }
    // 是否消失
    const bool& isDisappeared() { return (bool&)((uint8_t*)this)[0x20]; }

    // 类型
    // 1 - 墓碑
    // 2 - 弹坑
    // 3 - 梯子
    // 11 - 钉耙
    // 12 - 脑子
    int& type() { return (int&)((uint8_t*)this)[0x8]; }
    // 行数
    // 范围 : [0, 5]
    int& row() { return (int&)((uint8_t*)this)[0x14]; }
    // 列数
    // 范围 : [0, 8]
    int& col() { return (int&)((uint8_t*)this)[0x10]; }
    // 属性倒计时（墓碑冒出的数量、弹坑消失倒计时、脑子血量、钉钯消失倒计时）
    int& countdown() { return (int&)((uint8_t*)this)[0x18]; }
};
struct MainObjectAdd : public AMainObject {
    // 场地物品内存数组
    GroundItem* groundItemArray()
    {
        return *(GroundItem**)((uint8_t*)this + 0x11c);
    }
    // 场地物品内存数组大小
    int& groundItemTotal() { return (int&)((uint8_t*)this)[0x120]; }
};

void Draw()
{
    // 游戏暂停时不显示信息
    if (AGetMainObject()->MRef<bool>(0x164))
        return;

    auto&& bar_painter = show_bar.GetPainter();
    auto animations = (AnimationAdd*)AGetPvzBase()
                          ->AnimationMain()
                          ->AnimationOffset()
                          ->AnimationArray();
    auto zombies = (ZombieAdd*)(AGetMainObject()->ZombieArray());
    auto plants = (PlantAdd*)AGetMainObject()->PlantArray();
    auto main_object_add = (MainObjectAdd*)AGetMainObject();
    auto groundItems = (GroundItem*)(main_object_add->groundItemArray());
    int index_animation = 0;
    float circulation_rate = 0.0;
    int bar_pos_x = 0;
    int bar_pos_y = 0;
    for (int index = 0; index < AGetMainObject()->ZombieTotal(); ++index) {
        if (!zombies[index].IsDisappeared() && !zombies[index].IsDead()) {
            bar_pos_x = int((2 * zombies[index].Abscissa() + zombies[index].BulletAbscissa() + zombies[index].HurtWidth()) / 2);
            // 僵尸动画进度
            bar_pos_y = int(zombies[index].Ordinate() + zombies[index].BulletOrdinate() - (bar_height * 3 + 5 * frame_size));
            int zombieType = zombies[index].Type();
            int zombieState = zombies[index].State();
            // 撑杆僵尸跳跃
            if (zombieType == APOLE_VAULTING_ZOMBIE && zombieState == 12) {
                index_animation = zombies[index].AnimationCode();
                circulation_rate = animations[index_animation].CirculationRate();

                bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width,
                    bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                    SMPainter::RIGHT, {});
            }
            // 读报僵尸失去报纸
            if (zombieType == ANEWSPAPER_ZOMBIE && zombieState == 30) {
                index_animation = zombies[index].AnimationCode();
                circulation_rate = animations[index_animation].CirculationRate();

                bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width,
                    bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                    SMPainter::RIGHT, {});
            }
            // // 舞王僵尸和伴舞僵尸跳舞
            // if ((zombieType == DANCING_ZOMBIE || zombies[index].Type()
            // == BACKUP_DANCER) && zombies[index].State() >= 44 &&
            // zombies[index].State() <= 49) {
            //     index_animation = zombies[index].AnimationCode();
            //     circulation_rate = animations[index_animation].CirculationRate();

            //     bar_painter.DrawBar(bar_pos_x, bar_pos_y,circulation_rate,
            //     bar_width, bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
            //     SMPainter::RIGHT, {});
            // }
            // 潜水僵尸入水
            if (zombieType == ASNORKEL_ZOMBIE && zombieState == 58) {
                index_animation = zombies[index].AnimationCode();
                circulation_rate = animations[index_animation].CirculationRate();

                bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width,
                    bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                    SMPainter::RIGHT, {});
            }
            // 海豚僵尸入水和跳跃
            if (zombieType == ADOLPHIN_RIDER_ZOMBIE && (zombieState == 52 || zombieState == 54)) {
                index_animation = zombies[index].AnimationCode();
                circulation_rate = animations[index_animation].CirculationRate();

                bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width,
                    bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                    SMPainter::RIGHT, {});
            }
            // 玩偶匣僵尸开盒
            if (zombieType == AJACK_IN_THE_BOX_ZOMBIE && zombieState == 16) {
                circulation_rate = 1 - zombies[index].StateCountdown() / float(JACK_IN_THE_BOX_OPEN_BOX_TIME);

                bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width,
                    bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                    SMPainter::RIGHT, {});
            }
            // 气球僵尸落地
            if (zombieType == ABALLOON_ZOMBIE && zombieState == 74) {
                index_animation = zombies[index].AnimationCode();
                circulation_rate = animations[index_animation].CirculationRate();

                bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width,
                    bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                    SMPainter::RIGHT, {});
            }
            // 矿工僵尸出土后眩晕
            if (zombieType == ADIGGER_ZOMBIE && zombieState == 36) {
                index_animation = zombies[index].AnimationCode();
                circulation_rate = (animations[index_animation].CirculationRate() + animations[index_animation].circulationCount()) / 2.0;

                bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width,
                    bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                    SMPainter::RIGHT, {});
            }
            // 蹦极僵尸停留
            if (zombieType == ABUNGEE_ZOMBIE && (zombieState == 6 || zombieState == 7)) {
                if (zombieState == 6) {
                    circulation_rate = 1 - (zombies[index].StateCountdown() + BUNGEE_GRAB_TIME) / float(BUNGEE_REMAIN_TIME + BUNGEE_GRAB_TIME);
                } else if (zombieState == 7) {
                    index_animation = zombies[index].AnimationCode();
                    circulation_rate = (BUNGEE_REMAIN_TIME + animations[index_animation].CirculationRate() * BUNGEE_GRAB_TIME) / float(BUNGEE_REMAIN_TIME + BUNGEE_GRAB_TIME);
                }

                bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width,
                    bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                    SMPainter::RIGHT, {300.0 / (300 + 63)});
            }
            // 投篮僵尸投篮
            if (zombieType == ACATAPULT_ZOMBIE && (zombieState == 67 || zombieState == 68)) {
                circulation_rate = 1 - zombies[index].StateCountdown() / float(SHOOT_BASKETBALL_COUNTDOWN);

                bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width,
                    bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                    SMPainter::RIGHT,
                    {SHOOT_BASKETBALL_CIRCULATION_RATE});
            }
            // 巨人僵尸锤击
            if ((zombieType == AGIGA_GARGANTUAR || zombieType == AGARGANTUAR) && zombies[index].IsHammering()) {
                index_animation = zombies[index].AnimationCode();
                circulation_rate = animations[index_animation].CirculationRate();

                bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width,
                    bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                    SMPainter::RIGHT, {HAMMER_CIRCULATION_RATE});
            }
            // 巨人僵尸投掷小鬼
            if ((zombieType == AGIGA_GARGANTUAR || zombieType == AGARGANTUAR) && zombieState == 69) {
                index_animation = zombies[index].AnimationCode();
                circulation_rate = animations[index_animation].CirculationRate();

                bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width,
                    bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                    SMPainter::RIGHT, {THROW_IMP_CIRCULATION_RATE});
            }
            // 僵尸冰冻倒计时
            bar_pos_y = int(zombies[index].Ordinate() + zombies[index].BulletOrdinate() - (2 * bar_height + 4 * frame_size));
            int freeze_countdown = zombies[index].FreezeCountdown();
            if (freeze_countdown > 0) {
                circulation_rate = freeze_countdown / float(FREEZE_TIME);

                bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width,
                    bar_height, 1, AArgb(0xFF, 0x0E, 0xA0, 0xE0),
                    SMPainter::RIGHT, {});
            }

            // 僵尸血量（不考虑二类饰品）
            bar_pos_y = int(zombies[index].Ordinate() + zombies[index].BulletOrdinate() - (bar_height + 2 * frame_size));
            circulation_rate = (zombies[index].Hp() + zombies[index].OneHp()) / float(zombies[index].hpMax() + zombies[index].oneHpMax());
            if (zombieType == AGARGANTUAR) {
                bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width,
                    bar_height, 1, AArgb(0xFF, 0x00, 0xFF, 0x00),
                    SMPainter::RIGHT, {1800 / 3000.0});
            } else if (zombieType == AGIGA_GARGANTUAR) {
                bar_painter.DrawBar(
                    bar_pos_x, bar_pos_y, circulation_rate, bar_width, bar_height, 1,
                    AArgb(0xFF, 0x00, 0xFF, 0x00), SMPainter::RIGHT,
                    {1800 / 6000.0, 1800 * 2 / 6000.0, 1800 * 3 / 6000.0});
            } else {
                bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, bar_width,
                    bar_height, 1, AArgb(0xFF, 0x00, 0xFF, 0x00),
                    SMPainter::RIGHT, {});
            }
        }
    }

    // 植物进度条
    for (int index = 0; index < AGetMainObject()->PlantTotal(); ++index) {
        bar_pos_x = plants[index].Xi() + 40;
        bar_pos_y = plants[index].Yi() - (bar_height + 2 * frame_size);
        if (!plants[index].IsDisappeared() && !plants[index].IsCrushed()) {
            // 樱桃炸弹、寒冰菇、毁灭菇和火爆辣椒生效进度条
            int plantType = plants[index].Type();
            if (plantType == ACHERRY_BOMB || plantType == AICE_SHROOM || plantType == ADOOM_SHROOM || plantType == AJALAPENO) {
                if (plants[index].ExplodeCountdown() > 0) {
                    circulation_rate = plants[index].ExplodeCountdown() / 100.0;

                    bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, 72,
                        bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                        SMPainter::RIGHT, {});
                }
            }
            // 咖啡豆生效进度条（咖啡豆消失后进度条不显示，待完善）
            if (plantType == ACOFFEE_BEAN) {
                if (plants[index].ExplodeCountdown() > 0) {
                    circulation_rate = (plants[index].ExplodeCountdown() + plants[index].BloverCountdown()) / 299.0;

                    bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, 72,
                        bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                        SMPainter::RIGHT, {});
                }
            }
            // 向日葵、双子向日葵、阳光菇和金盏花生产进度条
            if (plantType == ASUNFLOWER || plantType == ATWIN_SUNFLOWER || plantType == ASUN_SHROOM || plantType == AMARIGOLD) {
                if (plants[index].produceCountdown() > 0) {
                    circulation_rate = plants[index].produceCountdown() / 2500.0;

                    bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, 72,
                        bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                        SMPainter::RIGHT, {});
                }
            }
            // 土豆地雷准备进度条
            if (plantType == APOTATO_MINE && plants[index].State() != 16) {
                circulation_rate = plants[index].StateCountdown() / 1500.0;

                bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, 72,
                    bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                    SMPainter::RIGHT, {});
            }
            // 大嘴花消化进度条
            if (plantType == ACHOMPER && plants[index].State() == 13) {
                circulation_rate = plants[index].StateCountdown() / 4000.0;

                bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, 72,
                    bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                    SMPainter::RIGHT, {});
            }
            // 倭瓜砸击进度条
            if (plantType == ASQUASH) {
                if (plants[index].State() >= 3 && plants[index].State() <= 6) {
                    if (plants[index].State() == 3) {
                        circulation_rate = (plants[index].StateCountdown() + 45 + 50 + 7) / float(80 + 45 + 50 + 7);
                    } else if (plants[index].State() == 4) {
                        circulation_rate = (plants[index].StateCountdown() + 50 + 7) / float(80 + 45 + 50 + 7);
                    } else if (plants[index].State() == 5) {
                        circulation_rate = (plants[index].StateCountdown() + 7) / float(80 + 45 + 50 + 7);
                    } else if (plants[index].State() == 6) {
                        circulation_rate = (plants[index].StateCountdown()) / float(80 + 45 + 50 + 7);
                    }
                    bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, 72,
                        bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                        SMPainter::RIGHT, {});
                }
            }
            // 墓碑吞噬者消失进度条
            if (plantType == AGRAVE_BUSTER) {
                if (plants[index].StateCountdown() > 0) {
                    circulation_rate = plants[index].StateCountdown() / 400.0;

                    bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, 72,
                        bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                        SMPainter::RIGHT, {});
                }
            }
            // 三叶草生效进度条
            if (plantType == ABLOVER) {
                if (plants[index].BloverCountdown() > 0) {
                    circulation_rate = (plants[index].ExplodeCountdown() + plants[index].BloverCountdown()) / 250.0;

                    bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, 72,
                        bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                        SMPainter::RIGHT, {(250 - 50) / 250.0});
                }
            }
            // 磁力菇复用进度条
            if (plantType == AMAGNET_SHROOM) {
                if (plants[index].StateCountdown() > 0) {
                    circulation_rate = plants[index].StateCountdown() / 1500.0;

                    bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, 72,
                        bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                        SMPainter::RIGHT, {});
                }
            }
            // 玉米加农炮复用进度条
            if (plantType == ACOB_CANNON && plants[index].State() != 37) {
                index_animation = plants[index].AnimationCode();
                switch (plants[index].State()) {
                case 35:
                    circulation_rate = (125 + plants[index].StateCountdown()) / float(CANNON_LOADING_TIME);
                    break;
                case 36:
                    circulation_rate = (int(125 * (1 - animations[index_animation].CirculationRate()) + 0.5) + 1) / float(CANNON_LOADING_TIME);
                    break;
                case 38:
                    circulation_rate = (3125 + int(350 * (1 - animations[index_animation].CirculationRate()) + 0.5)) / float(CANNON_LOADING_TIME);
                    break;
                default:
                    circulation_rate = 0;
                    break;
                }
                bar_painter.DrawBar(bar_pos_x + 40, bar_pos_y, circulation_rate, 2 * 72,
                    bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                    SMPainter::RIGHT, {});
            }
            // 植物血量
            bar_pos_y = plants[index].Yi();
            circulation_rate = plants[index].Hp() / float(plants[index].HpMax());
            if (plantType == ACOB_CANNON) {
                bar_painter.DrawBar(bar_pos_x + 40, bar_pos_y, circulation_rate, 2 * 72,
                    bar_height, 1, AArgb(0xFF, 0x00, 0xFF, 0x00),
                    SMPainter::RIGHT, {});
            } else {
                if (plantType == ALILY_PAD || plantType == AFLOWER_POT) {
                    bar_pos_y = bar_pos_y = plants[index].Yi() + (2 * bar_height + 4 * frame_size);
                } else if (plantType == APUMPKIN || plantType == ACOFFEE_BEAN) {
                    bar_pos_y = bar_pos_y = plants[index].Yi() + (4 * bar_height + 8 * frame_size);
                }
                bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, 72,
                    bar_height, 1, AArgb(0xFF, 0x00, 0xFF, 0x00),
                    SMPainter::RIGHT, {});
            }
        }
    }
    int x_diff = 80; // 列间距
    int y_diff = (AGetMainObject()->Scene() == 2 || AGetMainObject()->Scene() == 3)
        ? 85
        : 100; // 行间距
    // 场地物品进度条
    for (int index = 0; index < main_object_add->groundItemTotal(); ++index) {
        if (!groundItems[index].isDisappeared()) {
            if (groundItems[index].type() == 2) {
                bar_pos_x = x_diff * (groundItems[index].col() + 1);
                bar_pos_y = y_diff * (groundItems[index].row() + 1) + 10;
                circulation_rate = groundItems[index].countdown() / CRATER_REMAINING_TIME;
                bar_painter.DrawBar(bar_pos_x, bar_pos_y, circulation_rate, 72,
                    bar_height, 1, AArgb(0xFF, 0xFF, 0xC0, 0x00),
                    SMPainter::RIGHT, {});
            }
        }
    }
}

ATickRunner tickRunner;

void AScript()
{
    tickRunner.Start(Draw);
}
