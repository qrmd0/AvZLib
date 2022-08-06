# Dance秘籍

Dance秘籍可以大幅提高领带三人组的速度。具体原理见 [让普僵加速！Dance秘籍及其原理](https://tieba.baidu.com/p/7921781826)。

用法：将 `inc` 目录复制到 AvZ 根目录下，在脚本开头加上 `#include "dance_cheat.h"`。

API：

```cpp
// Not In Queue
// 将当前dance状态设为state（true = dance，false = 正常）
void SetDance(bool state);

// In Queue
// mode = FAST: 加速模式
// mode = SLOW: 减速模式
// mode = STOP: 关闭dance秘籍，恢复正常运动模式
void DanceCheat(DanceCheatMode mode);
```
