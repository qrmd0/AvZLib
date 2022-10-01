<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-10-01 16:10:38
 * @Description: 
-->
## `ShowMe` : AvZ 内存信息显示插件 

**Need AvZ_VERSION >= 221001**


### 接口

`ShowMe` 分为两大部分：绘图 (`SMPainter`) 和 对象显示设置 (`SMShowObj`)

`SMPainter` 中的接口如下：

```C++

    // 设置字体
    // 使用示例
    // setFont("黑体") ------ 将字体设置为黑体
    void setFont(const std::string& name);

    // 设置字体大小
    // 使用示例
    // setFontSize(15) ------ 将字体大小设置为 15
    // 注意此处字体大小不一定与 MS Word 中的相同
    void setFontSize(int size);

    // 设置文本颜色
    // 使用示例
    // 注意每个参数范围为 [0, 255]
    // setColor(SMArgb(0xff, 0, 0, 0)) ----- 将文本的不透明度设置为 0xff, 也就是不透明, 色彩设置为 RGB(0, 0, 0), 也就是黑色
    // setColor(SMArgb(0x7f, 0xff, 0, 0)) ----- 将文本的不透明度设置为 0x7f, 色彩设置为 RGB(0xff, 0, 0), 也就是红色
    // setColor(SMArgb(0x7f, 0xff, 0, 0), SMArgb(0x7f, 0, 0, 0)) ---- 将文本的不透明度设置为 0x7f, 色彩设置为 RGB(0xff, 0, 0), 也就是红色, 将背景色的不透明度设置为 0x7f, 色彩设置为 RGB(0, 0, 0), 也就是黑色
    void setColor(DWORD str_color, DWORD bkg_color = SMArgb(0x7f, 0, 0, 0));

    // 绘制矩形
    // 使用示例
    // drawText(100, 100, 200, 200) ------ 在游戏画面(100, 100) 处绘制宽高为 (200, 200) 的矩形
    void drawRect(int x, int y, int width, int height);
    
    // 绘制字符串
    // 使用示例
    // drawString("hello", 100, 100) ------ 在游戏画面(100, 100) 处绘制 hello, 注意此处绘制的字符串没有背景色
    void drawString(const SMText& str, int x, int y);

    // 控制文本矩形框出现在鼠标的哪个方位
    enum PosType {
        RIGHT_TOP,
        RIGHT_BUTTOM,
        LEFT_TOP,
        LEFT_BOTTOM,
    };


    // 绘制文本
    // 使用示例
    // drawText("hello", 100, 100) ------ 在游戏画面(100, 100) 处绘制 hello
    // drawText("hello", 100, 100, RIGHT_TOP) ------ 在游戏画面(100, 100) 处绘制 hello, 并且文本绘制是在 (100, 100) 的右上方
    void drawText(const SMText& text, int x, int y, int pos = RIGHT_BUTTOM);
```

`SMShowObj` 中的接口如下：

```C++

    ////////////////////////////////////////
    // 注意下面的例子只是列举了植物相关的属性设置
    // 其他的类别设置起来相似但不相同
    ////////////////////////////////////////


    // 设置显示种类
    // 使用示例
    // setShowType({YMJNP_47, HBG_14}) ----- 仅显示玉米加农炮和寒冰菇的内存信息
    void setShowType(const std::vector<int>& type_vec);

    // 设置显示函数
    // 使用示例
    // setShowInfoFunc([=](Plant* plant) {
    //     SMText text;
    //     text += "类型:" + name_dict[plant->type()] + '\n';
    //     text += "血量:" + std::to_string(plant->hp());
    //     return text;
    // }); ------- 显示植物中的 类型 和 血量 信息
    void setShowInfoFunc(InfoFunc<Plant> func);

    // 得到鼠标寻找对象的字典信息
    // 使用示例
    // auto&& dict = getFindDict();
    // dict[Type][State].threshold = 50;  ---- 将类型为 Type、状态为 State 的对象的寻找阈值设置为 50
    // dict[Type][State].offset_x = 0.3;  ---- 将类型为 Type、状态为 State 的对象的中心偏移量设置为 0.3
    // 进一步解释
    // threshold 指的是鼠标离游戏内对象"中心"的寻找距离，即当鼠标所在位置和游戏内对象的"中心"距离小于 50 时，就会显示对象的内存信息
    // 这里所说的距离是欧氏距离
    // offset_x 指的是计算对象"中心"时用到的偏移量，由于在 PvZ 这个游戏中，没有直接的存储对象画面中心信息的内存，
    // 这个中心需要通过计算得到， 在这里，中心横坐标的计算公式为 $center_x = 对象横坐标 + 对象的判定宽度 * offset_x$,
    // 纵坐标同理
    // 注意：只能设置植物和僵尸的搜索属性
    FindInfoDict& getFindDict();

    // 得到对象的字符串名称
    // 使用示例
    // auto&& dict = getNameDict();
    // dict[YMJNP_47] = "春哥";   ----- 将玉米加农炮的名称设置为 春哥
    std::vector<std::string>& getNameDict();

```

### 使用示例

1. 运行 ShowMe 

```C++
#include "avz.h"
#include "showme/sm.h" // 包含 ShowMe 头文件

SMShowMe show_me; // 创建 ShowMe 对象

// 注意此函数每帧都会调用
// 画面绘制相关的代码要写在此函数中
void Draw()
{
    show_me.draw();
}

void Script()
{
}

```

2. 设置显示种类

```C++
#include "avz.h"
#include "showme/sm.h" // 包含 ShowMe 头文件

SMShowMe show_me; // 创建 ShowMe 对象

// 注意此函数每帧都会调用
// 画面绘制相关的代码要写在此函数中
void Draw()
{
    show_me.draw();
}

void Script()
{
    // 一般设置属性的代码都放在 Script 里面，因为 Draw 函数每帧都执行，
    // 如果放在 Draw 里面会造成不必要的性能开销
    // 设置植物只显示 玉米加农炮和叶子保护伞 的信息
    show_me.getPlantSettings().setShowType({YMJNP_47, YZBHS_37});
}

```

3. 设置显示属性

```C++
#include "avz.h"
#include "showme/sm.h" // 包含 ShowMe 头文件

SMShowMe show_me; // 创建 ShowMe 对象

// 注意此函数每帧都会调用
// 画面绘制相关的代码要写在此函数中
void Draw()
{
    show_me.draw();
}

void Script()
{
    // 一般设置属性的代码都放在 Script 里面，因为 Draw 函数每帧都执行，
    // 如果放在 Draw 里面会造成不必要的性能开销
    // 设置显示植物血量和状态的属性
    show_me.getPlantSettings().setShowInfoFunc([](Plant* plant) {
        std::string str = "血量:" + std::to_string(plant->hp()) + "\n"; // 换行使用 "\n" 即可
        str += "状态:" + std::to_string(plant->state());
        return str;
    });
}
```

4. 对象搜索设置

```C++
#include "avz.h"
#include "showme/sm.h" // 包含 ShowMe 头文件

SMShowMe show_me; // 创建 ShowMe 对象

// 注意此函数每帧都会调用
// 画面绘制相关的代码要写在此函数中
void Draw()
{
    show_me.draw();
}

void Script()
{
    // 一般设置属性的代码都放在 Script 里面，因为 Draw 函数每帧都执行，
    // 如果放在 Draw 里面会造成不必要的性能开销
    // 在不理解这是什么意思时，请不要修改

    // 注意：只能设置植物和僵尸的搜索属性
    // 设置玉米加农炮的搜索阈值为 100
    show_me.getPlantSettings().getFindDict()[YMJNP_47][37].threshold = 100;

    // 设置玉米加农炮的中心偏移为 0.5
    show_me.getPlantSettings().getFindDict()[YMJNP_47][37].offset_x = 0.5;
}
```

5. 使用 `SMPainter` 显示全部春哥的血量

```C++
#include "avz.h"
#include "showme/sm.h" // 包含 ShowMe 头文件

SMShowMe show_me; // 创建 ShowMe 对象

// 注意此函数每帧都会调用
// 画面绘制相关的代码要写在此函数中
void Draw()
{
    // 获取绘制对象
    auto&& painter = show_me.getPainter();

    // 显示所有玉米加农炮的血量信息
    // 就是将信息显示在对象的坐标位置上
    for (auto&& plant : AvZ::alive_plant_filter) {
        if (plant.type() == YMJNP_47) {
            std::string str = "血量:" + std::to_string(plant.hp());
            painter.drawText(str, plant.abscissa(), plant.ordinate());
        }
    }
}

void Script()
{
}
```

6. 使用 `SMPainter` 显示某个固定位置春哥的血量

```C++
#include "avz.h"
#include "showme/sm.h" // 包含 ShowMe 头文件

SMShowMe show_me; // 创建 ShowMe 对象

// 注意此函数每帧都会调用
// 画面绘制相关的代码要写在此函数中
void Draw()
{
    // 获取绘制对象
    auto&& painter = show_me.getPainter();

    // 由于是 static 对象，这条代码只会执行一次
    static int ymjnp_idx = AvZ::GetPlantIndex(1, 1, YMJNP_47);

    // 显示所有玉米加农炮的血量信息
    // 就是将信息显示在对象的坐标位置上
    auto plant = AvZ::GetMainObject()->plantArray() + ymjnp_idx;
    std::string str = "血量:" + std::to_string(plant->hp());
    painter.drawText(str, plant->abscissa(), plant->ordinate());
}

void Script()
{
}
```

