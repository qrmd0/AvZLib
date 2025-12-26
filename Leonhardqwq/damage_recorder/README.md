
# PVZ-Damage-Recorder

记录每场战斗中僵尸受到的所有小于1800的伤害及其来源，在到指定路径下结果输出到csv文件，字段信息如下：

| 字段名       | 解释           |
| ------------ | -------------- |
| 波次         | 伤害发生波次   |
| 时间         | 伤害发生时间   |
| 伤害         | 本次伤害数值   |
| 来源         | 伤害来源类型，包括**范围**/**子弹**/**寒冰菇**/**未知**   |
| 僵尸ID       | 受伤僵尸唯一标识   |
| 僵尸类型     | 受伤僵尸种类       |
| 僵尸路       | 受伤僵尸所在行     |
| 僵尸坐标     | 受伤僵尸横坐标(int)     |
| 僵尸波次     | 受伤僵尸所属波次   |
| 僵尸总血量   | 僵尸受伤前总血量   |
| 子弹ID       | 子弹唯一标识       |
| 植物ID       | 伤害来源植物唯一标识   |
| 植物类型     | 伤害来源植物种类       |
| 植物列       | 伤害来源植物所在列     |
| 植物路       | 伤害来源植物所在行     |

特别指出，当使用AReplay功能时，ID唯一标识可能存在重复。

## 使用示例
```cpp
#include "damage_recorder/damage_recorder.h"

DamageRecorder damageRecorder;
void AScript(){
    damageRecorder.SetPath("D:\\AAA_LH_SL");
    damageRecorder.Setup();
}
```

---
基于AvZ2 2.8.6 nightly版本，使用了AvZ扩展库AsmInsertCode插件，特别鸣谢作者：[vector-wlc](https://github.com/vector-wlc)

如有问题请联系开发者：[Leonhard_qwq(bilibili)](https://space.bilibili.com/171987343)