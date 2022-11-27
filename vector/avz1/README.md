<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-27 20:06:03
 * @Description: 
-->
# 适用于 AvZ2 的 AvZ1 接口兼容层

## 使用

```C++
// 使用这条语句可以让绝大部分 AvZ1 的脚本在 AvZ2 中运行
// 如果你使用了其他的 AvZ1 插件，一定要把这条语句写在脚本的最上面的一行
// showme 插件由于使用了 Hook，无法做到完美兼容，请使用适用于 AvZ2 的 showme 插件
#include "avz1/api.h"

void Script(){

}
```

## 注意事项

```C++
// 由于一些技术原因
// 以下的接口没有做到兼容
// 请使用此插件时注意一下
std::vector<int> GetRefreshedWave();
ShowQueue(const std::vector<int>& lst);
template <class Func>
void ScriptExitDeal(Func&& func);
void SetScriptStartTime(int time, int wave);
void SetErrorMode(); // 此接口请使用 AvZ2 提供的 ALogger 类
```