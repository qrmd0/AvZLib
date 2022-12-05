# AvZ DSL 教程

> 使用 DSL 最明显的优点在于，一旦您获得了一种语言和转换引擎，您在 DSL 覆盖的软件开发特定方面的工作就会变得更有**效率**，因为您不必手动完成繁琐的工作。\
如果您有一种方法可以用与领域高度一致的语言表达领域重点，您的**思路将变得更加清晰**，因为您编写的代码不会被实现细节搞得混乱。 换言之，使用 DSL 允许您将基本点与复杂性分开。\
DSL 的领域、抽象和符号与领域专家（即非编程人员）如何表达自己高度一致，这就**在技术人员与领域人员之间形成良好的配合**。\
—— [JetBrains](https://www.jetbrains.com/zh-cn/mps/concepts/domain-specific-languages/)

**AvZ DSL可以在 C++20 下编译。但为了避免玄学问题，建议将 AvZ 编译命令（打开 VS Code 设置搜索 Avz Compiler Cmd）**
**中的 `c++20` 改为 `c++2b`。[^1]**

[^1]: AvZ DSL 依赖于 C++23 草案中的多维 `operator[]`。在 C++20 下，该功能以重载 `operator,(ARelOp, ARelOp)`
    的方式实现，可能导致未知的问题。示例在两个 C++ 版本下都能编译。

示例：

见 `examples/[FE]jd4.cpp`。

语法：

`:=` 左侧是 AvZ DSL 语法，右侧是 AvZ 原生语法。看不懂可以结合着示例看。

```cpp
AMkRelOp(...) := ARelOp([=]{ ...; })
123_cs := ARelTime(123)
ARelTime(time)[r1, r2, ... rn] := ARelOp(time, r1 + r2 + ... + rn)
ARelTime(a) + ARelTime(b)[...] := (ARelTime(a) + ARelTime(b))[...] // 时间运算不用加括号
123_wave := AWave(123)
AWave(w1, w2, ..., wn)[r1, r2, ..., rn]; :=
    for(int w : {w1, w2, ..., wn})
        AConnect(ATime(w, 0), r1 + r2 + ... + rn);
AWave(w1, w2, ..., wn)[r1, r2, ..., rn].AssumeWavelength(wl); :=
    AWave(w1, w2, ..., wn)[r1, r2, ..., rn];
    for(int w : {a, b, c, ..., z})
        AAssumeWavelength({ATime(w, wl)});
AWave(...) + ARelTime(t)[...]; := (AWave(...) + ARelTime(t))[...]; // 波次加时间不用加括号
ANow[r1, r2, ..., rn]; := AConnect(ANowTime(), r1 + r2 + ... + rn);
ANow + ARelTime(t)[...]; := (ANow + ARelTime(t))[...]; // ANow 加时间也不用加括号
```
