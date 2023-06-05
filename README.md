<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-10-04 18:40:25
 * @Description: 
-->

# AvZLib

AsmVsZombies 的公开插件仓库

# 镜像库

如果主库访问受限，请访问镜像库。如果要上传代码，请传入主库，主库的更新会被定期推送至镜像库，而镜像库的变更不会影响到主库。
镜像库1：https://gitee.com/qrmd/AvZLib
镜像库2：https://gitlab.com/avzlib/AvZLib

# 上传说明

**由于该项目为多人协作，提交之前请先`git pull`。**（使用网页版提交忽略此条）

## 文件夹结构

请按照如下规范上传插件，以便 AvZ VSCode 扩展为用户进行一键式下载部署插件。

```
仓库根目录
+--作者
|  +--插件名
|  |  +--插件名
|  |  |   +--information.txt
|  |  |   +--*.h
|  |  +--release
|  |  |   +--*.zip
|  |  +--README.md
|  |  +--version.txt  
+--extension_list.txt
```

文件说明

1. `information.txt` : 
* 第一行为插件打包的版本号
* 第二行为依赖的 AvZ 版本号
* 接下来的每行写上该插件所依赖的其他插件的名称和版本号

例如：A 插件此次打包的版本为 `2022_10_01`，依赖 AvZ 版本为 `2022_10_01`，并依赖版本为 `1.0.0`  的 B 插件，那么 `information.txt` 内容为：

```
version 2022_10_01
avz 2022_10_01
B 1.0.0
```

2. `*.h`

为了方便编译使用, AvZ 插件尽量使用纯头文件的形式发布, 同时一定记得在所有头文件中添加如下防止重复编译的指令

```C++
#ifndef __DEMO // 这里写成插件的名称就行 
#define __DEMO

// code

#endif
```

3. `*.zip`

这个是插件的打包文件, 该文件的名称是 `版本名.zip`, 注意压缩包里的内容为 

```
插件名称
+--*.h
+--information.txt
```

也就是该插件的所有头文件以及很重要的 `information.txt` 

4. `README.md`

用于描述插件功能和使用方法

5. `version.txt`

该文件内容为插件的所有发布版本, 例如插件发布了三个版本 : `22_10_01` `21_12_20` `21_08_20`,
那么该文件的内容为

```
22_10_01
21_12_20
21_08_20
```

6. `extension_list.txt`
   该文件内容为所有插件的列表, 当发布一个新插件的时候, 将 `作者/插件名` 追加到文件的最后一行即可

`release/` 和 `version.txt` 可以被自动管理。当**一次 push 的最近一次提交**（先 commit 再 merge 会使自动打包失效）为 `作者/插件名: release 版本` 的格式（**注意英文冒号**）时，GitHub Actions 会自动打包版本。`information.txt` 和 `extension_list.txt` **仍需手动处理**。

最后, 希望大家能够依照上述规定上传打包自己的插件, 这会对使用 AvZ 插件的用户带来很多便利。

## 开源声明

如果插件中包含了 AvZ 的部分代码，那么必须以 GPL 发布，其他情况下可以使用兼容 GPL 的常规许可证，不强制使用 GPL。
