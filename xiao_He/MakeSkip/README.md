# MakeSkip

目前用于快捷制作frs，计划推广到更多的特殊阵设。

## 运行环境

针对 AvZ1 2022_10_01 版本开发。

## 功能一览

**强烈建议在修改前先备份存档，一旦修改出错对存档的打击是毁灭性的！**

```cpp
// 销毁类型为 PLANT_MAIN(植物本体动画), 编号为 0 的对象的动画
// 植物与僵尸拥有多个动画，绝大多数时候使用的都是其本体动画 PLANT_MAIN 和 ZOMBIE_MAIN
DeleteAnimation(PLANT_MAIN, 0);

// 以 PP 模式将 编号为 0 的对象的本体动画编号 复制给 编号为 1 的对象
// 模式共有4种：
// PP: 植物 本体动画ID复制给 植物
// PZ: 植物 本体动画ID复制给 僵尸
// ZP: 僵尸 本体动画ID复制给 植物
// ZZ: 僵尸 本体动画ID复制给 僵尸
CopyAnimationId(PP, 0, 1);

// 以 PP 模式将 编号为 0 的对象的本体动画编号 复制给 编号为 1和2 的对象
CopyAnimationId(PP, 0, {1,2});

// 将类型为 PLANT_HEAD_1, 编号为 0 的对象的动画编号 复制给 类型为 PLANT_MAIN, 编号为 1 的对象
// 如果只需复制本体动画，无需选择该函数
CopyAnimationId(PLANT_HEAD_1, 0, PLANT_MAIN, 1);
```