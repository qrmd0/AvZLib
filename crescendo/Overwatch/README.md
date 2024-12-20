# Overwatch: 基于原生AvZ的砸率、炮伤测试框架

说明：
- 支持 AvZ1, AvZ2
- 通过 `AvZ:Get AvZ Extension` 安装本插件
- 添加 `#include "Overwatch/overwatch.h`, 并声明一个 `Overwatch` 对象
- 使用 `start()`, `dump_stats()` 等函数进行测试

使用例见 `examples/水无16_情况1.cpp` (此示例为 AvZ1 ).