 
# AsmInsertCode: 汇编代码插入

此插件能在 pvz 汇编代码任意位置插入用户自定义代码，且最小插入间隔为 5 个字节

## 这意味着什么？

**配合CE和逆向大佬的原版函数表，使用此插件，你可以实现很多实用、有趣、奇奇怪怪的东西！**

例如

1. 植物秒杀僵尸

原理：在僵尸受伤检测函数的第一行插入将僵尸血量设置为 0 的代码，
这样僵尸只要受到伤害就会死亡
```C++
// 僵尸受到伤害时秒杀
// 5317C0 Zombie::TakeDamage(eax = unsigned int theDamageFlags, int theDamage, esi = Zombie* this)
// 令僵尸受伤，会根据僵尸状态及防具情况结算伤害，同时处理僵尸受伤的相关事件。
AInsertUniqueAsmCode(
    0x5317C0, [](AAsmCodeContext * context) __stdcall {
        AZombie* zombie = (AZombie*)context->esi;
        zombie->Hp() = 0;
        zombie->OneHp() = 0;
        zombie->TwoHp() = 0;
    });
```

2. 记录玉米炮每次发射的落点

原理：在发射玉米炮函数的第一行插入记录坐标的代码
```C++
// 获取玉米炮的落点坐标
// 466D50 Plant::CobCannonFire(int theTargetY, int theTargetX, eax = Plant* this)
// 玉米加农炮锁定目标位置，播放发射动画，记录目标位置坐标，并进入发射预备状态。
AInsertSharedAsmCode(0x466D50, [](AAsmCodeContext* context) {
    int x = AMRef<int>(context->esp + 4);
    int y = AMRef<int>(context->esp + 8);
    logger.Info("玉米炮的落点为:({}, {})", x, y);
});
```
 
3. 僵尸不掉落

原理：通过修改 eip 寄存器，使得运行完成插入的代码之后直接跳到 DropLoot 结束的地方， 这样就不会生成掉落物了
注意修改 eip 寄存器的时候要注意栈平衡，不然程序就会崩溃
```C++
// 僵尸不掉落
// 530170 Zombie::DropLoot(eax = Zombie* this)
// 僵尸掉落事件函数。一般用于处理僵尸死亡时的掉落事件。
AInsertSharedAsmCode(0x530170, [](AAsmCodeContext* context) {
    // 通过修改 eip 寄存器，使得运行完成插入的代码之后直接跳到 DropLoot 结束的地方
    // 这样就不会生成掉落物了
    // 这个地址是通过使用 CE 查出来的
    context->eip = 0x5302E3;
});
```

## 使用说明

说在前面：首先你需要了解相关的C语言和汇编知识，例如函数是怎么传参的，调用约定是啥？栈平衡是啥？除此之外，如果涉及到修改 eip 寄存器的，还需要会 CE 的基础使用，不然将会寸步难行

本插件提供了两个函数 AInsertUniqueAsmCode 和 AInsertSharedAsmCode，
其中，AInsertUniqueAsmCode 只能插入调用约定为 __stdcall 的函数，并且一个地址下同时只能插入一个函数，
但是其调用效率高。 AInsertSharedAsmCode 则相反，能够插入任意形参为 AAsmCodeContext* 的函数， 
包括 lambda 函数，但是协程函数除外，如果实在有协程的需求，请配合使用 AConnect 和 ACoLaunch(实在是懒得封装了)，
AInsertSharedAsmCode 一个地址可以同时插入多个函数，但是其调用效率比 Unique 低，
但是大多数场景下，AInsertSharedAsmCode 已经足够使用，除非你插入的代码一帧之内被调用了很多次，且代码的本身的开销很小。

接下以上面三个用例，说明此插件的详细用法。

### 第一个用例
```C++
// 僵尸受到伤害时秒杀
// 5317C0 Zombie::TakeDamage(eax = unsigned int theDamageFlags, int theDamage, esi = Zombie* this)
// 令僵尸受伤，会根据僵尸状态及防具情况结算伤害，同时处理僵尸受伤的相关事件。
AInsertUniqueAsmCode(
    0x5317C0, [](AAsmCodeContext * context) __stdcall {
        AZombie* zombie = (AZombie*)context->esi;
        zombie->Hp() = 0;
        zombie->OneHp() = 0;
        zombie->TwoHp() = 0;
    });
```

代码解析：第一步要通过原版函数表找到你要的函数，比如就秒杀这个功能的话，我首先想到得就是僵尸受伤函数，
然后通过函数表查到了 5317C0 Zombie::TakeDamage 这个函数，查到函数之后看参数，发现他的参数是 
eax = unsigned int theDamageFlags, int theDamage, esi = Zombie* this, 此时咱们看一下 AAsmCodeContext 
是个啥玩意，
```C++
struct AAsmCodeContext {
    // 用户自定义数据，一般这个值是个指针，指向一个结构体
    // 对于 UniqueHook, 此值为 AInstallUniqueHook 函数参数中的 userData
    // 对于 SharedHook, 此值被内部实现占用，用户不应访问该值
    // 因为 SharedHook 可以使用 lambda 函数，这样的函数可以通过捕获传参
    DWORD userData;

    // 寄存器信息
    DWORD efl;
    DWORD edi;
    DWORD esi;
    DWORD ebp;
    DWORD esp;
    DWORD ebx;
    DWORD edx;
    DWORD ecx;
    DWORD eax;
    DWORD eip;
};
```

这个结构体包含了 9 个寄存器的信息，注意，**这 9 个寄存器的值是可读可写的**，但是一般用来读，此外还有一个 userData，
这个看注释就行了，了解了这个结构体的信息，继续看函数的参数：
eax = unsigned int theDamageFlags, int theDamage, esi = Zombie* this，这说明咱们通过读 esi 
寄存器的值就能拿到僵尸对象的指针，这样拿到指针就能对这个僵尸做坏事了，这个秒杀用例中就是将其三个 hp 值全部写为 0.
除此之外，还有一个 eax 是 theDamageFlags，还有一个 theDamage 参数，但是你发现这个参数没有寄存器标识，
这说明这个参数使用栈来传递的，那么如果读取这个参数的值呢？我们可以使用 esp 寄存器来获取，
因为 esp 寄存器就是栈顶地址。此时栈的结构是啥样的？因为咱们插入的这个函数的第一行，所以**栈顶是函数的返回地址**,
这个是 call 命令将返回地址压到栈顶的，属于汇编的基础知识，那么栈中的第二个元素就是咱们需要的 theDamage 的值了，
我们就可以使用  AMRef<int>(context->esp + 4); 获取到这个值。


### 第二个用例
```C++
// 获取玉米炮的落点坐标
// 466D50 Plant::CobCannonFire(int theTargetY, int theTargetX, eax = Plant* this)
// 玉米加农炮锁定目标位置，播放发射动画，记录目标位置坐标，并进入发射预备状态。
AInsertSharedAsmCode(0x466D50, [](AAsmCodeContext* context) {
    int x = AMRef<int>(context->esp + 4);
    int y = AMRef<int>(context->esp + 8);
    logger.Info("玉米炮的落点为:({}, {})", x, y);
});
```
有了第一个用例的基础，咱们直接看第二个用例不同的地方，就是这个函数有两个函数是用栈传的，
那么此时就有一个问题，栈中的第一个元素是 theTargetY 还是第二个元素是？
我推荐的做法是：试！

首先先尝试:
```C++
int x = AMRef<int>(context->esp + 4);
int y = AMRef<int>(context->esp + 8);
```

如果发现不对，就返过来
```C++
int y = AMRef<int>(context->esp + 4);
int x = AMRef<int>(context->esp + 8);
```

总能试出来，那么有啥窍门吗？实际上这玩意和函数调用约定有关系，你可以去研究，
**但是这前提是函数表中的参数顺序和真正的pvz原版C++源码一致才行**，那么一致吗，鬼知道！
所以还是试吧


### 第三个用例

```C++
// 僵尸不掉落
// 530170 Zombie::DropLoot(eax = Zombie* this)
// 僵尸掉落事件函数。一般用于处理僵尸死亡时的掉落事件。
AInsertSharedAsmCode(0x530170, [](AAsmCodeContext* context) {
    // 通过修改 eip 寄存器，使得运行完成插入的代码之后直接跳到 DropLoot 结束的地方
    // 这样就不会生成掉落物了
    // 这个地址是通过使用 CE 查出来的
    context->eip = 0x5302E3;
});
```

这个用例不同点是修改了 eip 寄存器，修改 eip 寄存器的值需要小心，因为一旦栈不平衡游戏就会瞬间崩溃，
那啥会导致栈不平衡呢？举个例子

例如有这样的汇编代码

```
地址   代码
1     push a
2     push b   // 你在这里 AInsertSharedAsmCode
...

11    pop b    // eip 被修改为了 11 
12    pop a
```

这样就会直接导致程序崩溃，因为地址为 2 的汇编代码被覆盖了，在计算机眼里，此时栈里面只有 a，
然鹅你将 eip 改为了 11，那么程序运行你的 insert code 之后就会到第 11 行 ，就会 pop 两次，
这显然就出问题了，游戏就会崩溃，**所以修改 eip 需要使用 CE 分析会不会导致栈不平衡!!!**

当然还有其他用法，请查看 [demo](./example/demo.cpp)

## 原理
关键实现就是

```C++
struct InsertCodeAsm {
    uint8_t push0 = 0x68;               // push retAddr
    uintptr_t retAddr = 0x0;            //
    uint8_t pushad = 0x60;              // pushad
    uint8_t pushfd = 0x9c;              // pushfd
    uint8_t addEspAddC4[5] =            // add [esp+10], 4
        {0x83, 0x44, 0x24, 0x10, 0x04}; //
    uint8_t push1 = 0x68;               // push userData
    uintptr_t userData = 0x0;           //
    uint8_t pushEsp = 0x54;             // push esp
    uint8_t call = 0xe8;                // call callAddr
    uintptr_t callAddr = 0x0;           //
    uint8_t popEax = 0x58;              // pop eax  balance push userData
    uint8_t popfd = 0x9d;               // popfd
    uint8_t popad = 0x61;               // popad
    uint8_t ret = 0xc3;                 // ret
};
```

首先通过 MinHook Jmp 到咱们的 InsertCodeAsm 代码块，然后再将各种信息压栈，最后调用咱们的 callAddr,
也就是插入的代码，其中 pushEsp 就对应了插入函数中的参数 AAsmCodeContext*, 而开始的 push retAddr 和结束的
ret 则将 eip 修改为了 retAddr, 这两处代码是这段汇编的神来之笔。

然后 AInsertSharedAsmCode 和 AInsertUniqueAsmCode 的实现，AInsertUniqueAsmCode 是直接将 callAddr 
写成了你写的插入函数，没有多余的函数跳转，所以快，而 AInsertSharedAsmCode 为了支持一个地址下多个插入，
callAddr 是 static void __stdcall __ACodeCallback(AAsmCodeContext* context)，再由这个函数遍历插入函数池，
逐个调用，所以效率稍低，多了一层函数跳转和一些判断。

好了，就这么多。