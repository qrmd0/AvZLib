# 落点显示器

在 PvZ 画面右侧展示每门炮生效的时机和落点。

为方便展示，落点保留小数点后两位；这不会损失任何信息。

用法：

```cpp
#include <cob_marker/cob_marker.h>

CobMarker cobMarker;

void AScript() {
    cobMarker.Start();
}
```
