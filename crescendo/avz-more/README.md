# AvZ More 更多AvZ功能 220605

## 运行环境
本插件针对 AvZ 220213 版本开发，不确保对其它 AvZ 版本的兼容性。

## 使用方法
将 avz_more.h 置于 AvZ/inc 目录后，编写脚本并运行即可。<br>
需在Script()外添加“using namespace cresc;”<br>
各个类与函数的使用方式，可以参见头文件里的注释。

## 内含功能
-用卡相关：SmartCard, RecoverCard, SafeCard, RecoverSafeCard<br>
-智能垫材：SmartStall<br>
-快捷发炮：PaoOperatorAZM(pp, dd, pd, bd)<br>
-快捷用冰：IceFillerAZM(ice), nice, mice<br>
-平均分配僵尸行数：AverageSpawn<br>
-常用修改器功能：IgnoreSun, NoFog<br>
-杂项功能：CountReadyPao, SetDoomPlace等