# AvZ Testing 测试框架 220605

[![GitHub](https://badgen.net/badge/GitHub/avz_testing/cyan?icon=github)](https://github.com/Rottenham/avz-testing)

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

## 使用示例

见 examples 文件夹下，内含测试脚本（.cpp）与对应的存档文件。

示例里默认使用 AvZ Logger 记录相关数据。输出文件将优先被保存至桌面的 AZL_output 文件夹下。

> 若打开该文件夹失败，程序会试图在 C:\Software\AsmVsZombies 目录下创建 AZL_output 文件夹。

你可以自行指定输出文件夹的位置，如：
```c++
cresc::Logger Log("C:/MyFolder");
```

若使用反斜杠，请使用双反斜杠，如：
```c++
cresc::Logger Log("C:\\MyFolder");
```

> 考虑到测试样本量大的情况，示例程序默认将统计结果分10次输出，即便中途退出也能尽可能保留数据。


## 常见问题

- 脚本编译通过，但进入关卡时卡顿或闪退

先尝试重新编译脚本并注入。若仍有问题，一般是存档出现损坏，重制存档文件即可。

- 测试运行期间游戏无响应

跳帧测试期间，游戏无响应属正常现象，但每2f结束时选卡界面会正常显示。

若长时间无响应，请使用 [PVZ Tools](https://get.pvz.tools/) 或 [无炮辅助](https://tieba.baidu.com/p/6640418740) 等能够显示刷新倒计时的工具，检查是否卡在某一波次。一般这种问题的原因是测试脚本编写有问题。


## 更多说明
https://www.bilibili.com/read/cv16933298
