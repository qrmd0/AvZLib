本插件修正了 AvZ1 的以下两个 bug：

- 使用 [AvZ 多实例注入器](https://github.com/alumkal/avz-multi-instance-tools/tree/main/injector)时，`KeyConnect` 无效
- `MouseRow()` 和 `MouseCol()` 返回的位置有误

用法：

1. 在 VS Code 中按下 Ctrl+Shift+P，搜索 `AvZ: Get AvZ Extension`，点击后选择 `Reisen/MousePosPatch`
2. 在脚本开头加入一行 `#include "MousePosPatch/patch.h"`
3. 以上 bug 会被自动修复。无需改变已有的脚本代码。
