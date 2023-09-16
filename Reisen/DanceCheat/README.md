# Dance 秘籍

Dance 秘籍可以大幅提高或降低领带三人组的速度。具体原理见 [让普僵加速！Dance秘籍及其原理](https://tieba.baidu.com/p/7921781826)。

## 接口

AvZ2：

```cpp
// 将当前普僵运动模式设为 state（true = dance，false = 正常）
void SetDance(bool state);

// mode = DanceCheatMode::FAST: 开启加速模式
// mode = DanceCheatMode::SLOW: 开启减速模式
// mode = DanceCheatMode::STOP: 关闭 Dance 秘籍，恢复正常运动模式
void DanceCheat(DanceCheatMode mode);
ARelOp DanceCheatR(DanceCheatMode mode);
```

AvZ1：

```cpp
// Not In Queue
void SetDance(bool state);

void DanceCheat(DanceCheatMode mode);
void DanceCheatNotInQueue(DanceCheatMode mode);
```
