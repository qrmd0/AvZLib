# 刷新测试器

基于 AvZ 的刷新测试框架，可以测试各种操作的刷新概率并给出详细的数据报告。

## 使用

刷新测试器的核心对象是 `Task`，一个 `Task` 对应一个测试项目。

构建 `Task` 的语法如下：

```cpp
Task task = TaskBuilder()
    .output_folder("D:\\refresh") // 存储结果的目录，需要提前创建好
    .prefix("PP-225") // 测试项目的名称
    .total(1000) // 测试的选卡数（每选卡20波）
    .assume_refresh(true) // 激活为true，分离为false
    .huge(false) // 旗帜波为true，普通波为false
    .required_types({GIGA_GARGANTUAR, GARGANTUAR}) // 锁定必出的出怪种类
    .banned_types({}) // 锁定不出的出怪种类
    .operation([](int wave){
        SetTime(225 - 373); // wave已经被设置好了
        pao_operator.pao({{2, 9}, {5, 9}});
    })
    .check_time(401) // 检查刷新情况的时间；可以填多个（如{200, 300, 400}），会为每个时间单独生成一份数据
    // 以下为可选参
    .debug(false) // 关闭跳帧，默认为false
    .card_selection({STARFRUIT}) // 选卡类型，默认为寒冰菇、毁灭菇、樱桃、辣椒、窝瓜、地刺、三叶草、高坚果、大蒜、土豆
    .clear_zombies(true) // 效果为消灭本波全部僵尸才会触发刷新，且最后一个检查时间后会清除全场僵尸
                         // 若开启该选项，则以上设置会在每一波生效，否则只在w9、w19、w20生效，默认开启
                         // 含自然输出的测试可能需要关闭该选项以模拟上波僵尸影响植物索敌的情形
    .dance_cheat(true) // 开启女仆秘籍和Dance秘籍（会自动根据assume_refresh选择类型），默认为true
    .giga_count(-1) // 锁定每波红眼数，用于变速波测试。默认为-1，即不限制
;
```

刷新测试脚本的基本结构如下：

```cpp
#include "refresh/refresh.h"

vector<Task> get_tasks() {
    vector<Task> tasks;
    tasks.push_back(TaskBuilder()
        .total("foo")
        // 后略
    );
    tasks.push_back(TaskBuilder()
        .prefix("bar")
        // 后略
    );
    return tasks;
}
```

脚本需要定义 `vector<Task> get_tasks()` 函数。该函数会在第一次选卡之前运行，并返回需要进行的所有测试。
`void Script()` 由 `refresh.h` 定义，脚本无需（也不能）定义该函数。

`refresh.h` 中包含了 [AvZ Testing](https://github.com/qrmd0/AvZLib/tree/main/crescendo/avz-testing)，脚本可直接使用其中的函数，无需 `#include "avz_testing.h"`。

刷新测试器会自动开启如下修改：

- 存档只读
- 玉米炮无冷却
- 无视阳光
- 卡片无冷却
- 随意放置
- 蘑菇免唤醒（**写测试脚本时需注意**）
- 植物无敌
- 不长墓碑，不出墓碑、珊瑚、空降僵尸
- 取消延迟引信（炸水路的炮延迟仍是378）

不要将其与 PvZ 修改器一同使用。

为加速测试，可以使用 [AvZ 多实例注入器](https://github.com/alumkal/avz-multi-instance-tools/tree/main/injector)，用其替换 AvZ1 自带的 `injector.exe`。
准备好多个脚本（或者把一个脚本里的测试项目分成多份），开启与脚本数量相同的 PvZ 进程，运行每个脚本各一次即可。成功注入的 PvZ 进程的窗口标题会带上 `(1)` `(2)` 等标记。

参考脚本见 `examples/PE-activate.cpp`。

## 解读结果

刷新测试器会生成类似 `examples/output/PP-225.xlsx` 的文件，其中有三张表格。在进行简单分析时只需使用 `stats` 表。

### `stats`

`average` 为总刷新概率。

下面的内容为各选卡意外波数的直方图，如第一行 `0 145` 表示有 145 次选卡完全没有发生意外，第二行 `0-0.5 148` 表示有 148 次选卡 发生了 0-0.5 波的意外。
意外波数的上限是 20，这意味着该选卡每波都一定发生意外。
（刷新测试器的统计方法是统计每波剩余血量占比并以此确定该波意外概率，因此意外波数可以是小数而非 0 或 1）

### `data`

每行表示一次选卡，按意外波数排序。

前两列分别是意外波数和选卡序号。（选卡序号可用于在 `raw` 中查询数据）

接下来是各种僵尸（**不包含小鬼、伴舞**）的数量。以普僵为例，`普` 列表示该选卡中普僵的数量，`1` 列表示存活普僵的数量。对于巨人，其后的 `1` `2` `3` `4` 分别表示需对应次数的灰烬才能消灭的僵尸的数量（如红眼的 `4` 列表示没有受到灰烬伤害的红眼数量，`3` 列表示受到 1 次灰烬伤害的红眼数量）。
这些数量**按每波意外概率加权平均**。

### `raw`

每行表示一波。

前三列分别是选卡序号、波次序号和剩余血量比例，其余列的意义和 `data` 相同。


## 开源许可

Copyright 2023 Reisen

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

<http://www.apache.org/licenses/LICENSE-2.0>

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
