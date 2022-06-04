# AvZ Testing 测试框架 220605

![GitHub](https://badgen.net/badge/GitHub/avz_testing/cyan?icon=github)

## 运行环境

本插件针对 AvZ 220213 版本开发，不确保对其它 AvZ 版本的兼容性。

## 使用方法
将 avz_testing.h 与 avz_logger.h 置于 AvZ/inc 目录后，编写脚本并运行即可。

需在 Script() 外添加:
```c++
#include "avz_testing.h"
#include "avz_logger.h"
using namespace cresc;
```

运行完毕后，会跳出弹窗，展示统计信息。

每次测试前，请重新注入 AvZ 脚本，否则可能会遇到问题。若上一次测试正常结束，会自动取消多次注入（见代码）。

## 更多说明
https://www.bilibili.com/read/cv16933298
