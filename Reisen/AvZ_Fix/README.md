# AvZ Fix

增加了一项新功能，修复了 AvZ 中的几个 **feature**。具体如下：

- 增加了可以方便遍历僵尸数组的 `ZombieIterator`
- `SetZombies` 在不输入蹦极的情况下不会刷蹦极
- `SetWaveLength` 取消了使用波次和波长的限制

用法：

```cpp
for(auto z : ZombieIterator()) // ZombieIterator 可以用于 range-based for loop
    if(z->type() == GIGA_GARGANTUAR) { // 自动考虑了 !z->isDead() && !z->isDisappeared()
        //...
    }

// ZombieIterator 是一个迭代器
bool have_giga = std::find_if(ZombieIterator::begin(), ZombieIterator::end(), [](auto z){return z->type() == GIGA_GARGANTUAR}) != ZombieIterator::end();

// 其余两项修改自动生效，不需要修改代码
SetWaveLength({{2, 2501}, {9, 4500}});

/*
std::vector<ZombieType> t = {ZOMBIE, POLE_VAULTING_ZOMBIE};
SetZombies(t); // 传入 vector 不起作用；需要把 vector 换成 initializer_list
*/
```

该项目使用了 [vector-wlc/AsmVsZombies](https://github.com/vector-wlc/AsmVsZombies) 中的代码，遵守 GPLv3 License。
