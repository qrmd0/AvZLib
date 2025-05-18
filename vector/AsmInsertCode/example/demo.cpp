#include "asm_insert_code/asm_insert_code.h"

ALogger<AConsole> logger;

void AScript() {

    // 设置共享 hook
    // 共享 hook 可以在一个地址 hook 多次
    // 使用更加灵活，但是性能较低
    // 如果一个 hook 函数每帧存在大量调用，请使用 AInsertUniqueAsmCode
    // 按键监控功能
    // 41B820 Board::KeyDown(KeyCode theKey, ecx = Board* this)
    // "关卡界面键盘按键按下，进行游戏关卡内的按键检测。关卡内的所有按键（指令、暂停键等）均在此处检测。
    AInsertSharedAsmCode(0x41B820, [](AAsmCodeContext* context) {
        // 根据函数原型，只要是参数没有指定寄存器
        // 那就是使用栈来传递的参数
        // 需要使用 esp 加偏移量获取真正的参数值
        logger.Info("你按下按键的代码为: {}", AMRef<int>(context->esp + 4));
    });

    // 获取玉米炮的落点坐标
    // 466D50 Plant::CobCannonFire(int theTargetY, int theTargetX, eax = Plant* this)
    // 玉米加农炮锁定目标位置，播放发射动画，记录目标位置坐标，并进入发射预备状态。
    AInsertSharedAsmCode(0x466D50, [](AAsmCodeContext* context) {
        int x = AMRef<int>(context->esp + 4);
        int y = AMRef<int>(context->esp + 8);
        logger.Info("玉米炮的落点为:({}, {})", x, y);
    });

    // 僵尸不掉落
    // 530170 Zombie::DropLoot(eax = Zombie* this)
    // 僵尸掉落事件函数。一般用于处理僵尸死亡时的掉落事件。
    AInsertSharedAsmCode(0x530170, [](AAsmCodeContext* context) {
        // 通过修改 eip 寄存器，使得运行完成插入的代码之后直接跳到 DropLoot 结束的地方
        // 这样就不会生成掉落物了
        context->eip = 0x5302E3;
    });

    // 设置唯一 hook
    // 唯一 hook 的类型只能为 void(__stdcall*)(AAsmCodeContext*)
    // 并且一个地址只能绑定一个唯一 hook
    // 此 hook 优点是运行效率高
    // 缺点是不灵活
    // 僵尸受到伤害时秒杀
    // 5317C0 Zombie::TakeDamage(eax = unsigned int theDamageFlags, int theDamage, esi = Zombie* this)
    // 令僵尸受伤，会根据僵尸状态及防具情况结算伤害，同时处理僵尸受伤的相关事件。
    auto handle = AInsertUniqueAsmCode(
        0x5317C0, [](AAsmCodeContext * context) __stdcall {
            AZombie* zombie = (AZombie*)context->esi;
            logger.Info("伤害为: {}", AMRef<int>(context->esp + 4));
            zombie->Hp() = 0;
            zombie->OneHp() = 0;
            zombie->TwoHp() = 0;
        });

    // 按下 E 取消此效果
    AConnect('E', [handle]() mutable {
        handle.Remove();
    });
}
