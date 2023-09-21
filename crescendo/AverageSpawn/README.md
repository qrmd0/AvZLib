# 平均分配僵尸所在行数

平均分配指定种类僵尸所在行数（默认只平均分配红眼）。

## 使用示例
```cpp
#include "AverageSpawn/AverageSpawn.h"

AverageSpawn(); // 不填参数，平均分配每波红眼
AverageSpawn({POGO_ZOMBIE, ZOMBONI}); // 平均分配每波跳跳和冰车
AverageSpawn({GIGA_GARGANTUAR}, true); // 仅在收尾波(w9, w19)平均分配红眼
AverageSpawn({GIGA_GARGANTUAR}, true, 1); // 仅在收尾波平均分配红眼，从1路起顺次分配
```

注意：只能读取DE、NE、PE、FE、RE五大场景，无法辨认的场景默认是ME.
