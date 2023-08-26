# AvZ 修改器库

将修改器的一些常用功能移植到了 AvZ 中，方便脚本使用。

目前包含以下修改：

- `AlwaysButter`：玉米投手只投黄油
- `AlwaysKernel`：玉米投手只投玉米粒
- `AshInstantExplode`：寒冰菇和灰烬放置后立即生效
- `CobFixedDelay`：禁用“引信延迟”机制，开启后炸陆地的玉米加农炮生效延迟固定373cs，水池仍为378cs
- `CobInstantRecharge`：玉米加农炮立即装填（实际仍有476cs冷却时间）
- `DisableJackExplode`：小丑僵尸与辣椒僵尸不会爆炸
- `DisableItemDrop`：不掉战利品
- `DisableSpecialAttack`：不生成墓碑、墓碑僵尸、珊瑚僵尸、空降僵尸
- `FreePlantingCheat`：卡片不消耗阳光且无冷却时间，紫卡可直接种植
- `MushroomAwake`：蘑菇免唤醒
- `PlantAnywhere`：无视所有种植限制条件（此修改允许重叠种植，慎用）
- `PlantInvincible`：植物无敌
- `PlantWeak`：植物虚弱（受到伤害直接死亡）
- `RemoveFog`：清除浓雾
- `SaveDataReadOnly`：存档只读
- `StopZombieSpawn`：暂停刷新
- `UnlimitedSun`：不消耗阳光

## 使用方法

```cpp
// AvZ1
void Script() {
    // EnableModsScoped 会开启修改，并在选卡结束或用户回到主菜单时自动关闭
    EnableModsScoped(SaveDataReadOnly, RemoveFog, UnlimitedSun);
    // EnableModsGlobal 开启的修改会一直生效，需要手动关闭
    // 这两个函数都是 Not In Queue，定时生效需要配合 InsertTimeOperation 而非 SetTime
    InsertTimeOperation(0, 1, [](){ EnableModsGlobal(DisableJackExplode); });
    InsertTimeOperation(0, 2, [](){ DisableMods(DisableJackExplode); });
}

// AvZ2
void AScript() {
    EnableModsScoped(SaveDataReadOnly, RemoveFog, UnlimitedSun);
    // 注意：SaveDataReadOnly 在选卡后启动会导致存档被删除，因此 EnableMods 最好放在选卡之前
    ASelectCards({...});
    AConnect(ATime(1, 0), [](){ EnableModsGlobal(DisableJackExplode); });
    AConnect(ATime(2, 0), [](){ DisableMods(DisableJackExplode); });
}
```

## 自定义 Mod

一个 `Mod` 的基本结构如下：

```cpp
Mod ModName{
    {内存地址1, 修改值1, 原始值1},
    {内存地址2, 修改值2, 原始值2},
    ...
};
```

内存地址可以是一个数，也可以是用花括号括起的一列数（第一个数为基址，后面的数为各级偏移）。

修改值可以是 `char`（1 字节）、`uint32_t`（4 字节）或 `vector<uint8_t>`（任意长度）。
**注意，在大多数机器上整数类型的存储是以字节为单位从低位到高位存储的，也就是说 `0xdeadbeef` 相当于 `{0xef, 0xbe, 0xad, 0xde}`。**

示例见 `mod/mod.h`。

**`Mod` 最好被定义为全局变量。**通过 `EnableModsScoped` 启动的局部变量 `Mod` 受对象生命周期的影响，可能会意外地提前关闭。
