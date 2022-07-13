# Dance秘籍

Dance秘籍可以大幅提高领带三人组的速度。具体原理见 [
让普僵加速！Dance秘籍及其原理](https://tieba.baidu.com/p/7921781826)。

用法：将 `inc` 目录复制到 AvZ 根目录下，在脚本开头加上 `#include "dance_cheat.h"`。

API：

```cpp
// In Queue
// 开启Dance秘籍。
void DanceCheat::start();

// In Queue
// 关闭Dance秘籍。
void DanceCheat::stop();
```
