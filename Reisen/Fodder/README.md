# AvZ 智能垫材

智能垫材插件，支持自动检测卡片CD，多个垫材集合混合使用，智能根据快速僵尸数量调整顺序等功能，且使用方便。

用法：将 `inc` 目录复制到 AvZ 根目录下，在脚本开头加上 `#include "fodder.h"`。

API：

```cpp
// 构造垫材类。
// cards: 垫材卡片类型，优先级高的（比如有100cs免疫啃食的花盆）在前。
Fodder(const std::vector<PlantType>& cards);

// In Queue
// 放垫。
// rows: 用垫的行（{}代表所有陆地行）
// removal_delay: 铲除垫材的延迟。默认值266为减速巨人锤击生效时间。
// col: 垫材放置列数。
// smart: 是否根据快速僵尸数量调整顺序（为false则严格按顺序放置）
void Fodder::operator()(std::vector<int> rows = {}, int removal_delay = 266, int col = 9, bool smart = true)
```

示例：见 `fodder-example.cpp`。
