#include <win32gui/main.h>

static bool isAdvancedPause = true;

// 高级暂停句柄必须拿住，这样才能控制高级暂停按键
static AConnectHandle apHandle;

AMainWindow mainWindow("Single Page");
AOnAfterInject({
    constexpr int INTERVAL = 10;
    int x = INTERVAL;
    int y = INTERVAL;
    auto keyLabel = mainWindow.AddLabel("高级暂停按键:", x, y);
    x += INTERVAL + keyLabel->GetWidth();
    auto keyEdit = mainWindow.AddEdit("Z", x, y);
    x += INTERVAL + keyEdit->GetWidth();
    auto keyBtn = mainWindow.AddPushButton("修改", x, y);
    x = INTERVAL;
    y += INTERVAL + keyLabel->GetHeight();
    auto speedlabel = mainWindow.AddLabel("游戏倍速:", x, y);
    x += INTERVAL + speedlabel->GetWidth();
    auto speedBox = mainWindow.AddComboBox(x, y);
    speedBox->AddString("10", "5", "2", "1", "0.5", "0.2", "0.1");
    x += INTERVAL + speedlabel->GetWidth();
    auto speedBtn = mainWindow.AddPushButton("修改", x, y);
    x = INTERVAL;
    y += INTERVAL + speedlabel->GetHeight();
    auto collectBox = mainWindow.AddCheckBox("自动收集", x, y);
    collectBox->SetCheck(true);

    // 注意：只有 PushButton 和 CheckBox 才能使用 AConnect

    // 设置高级暂停按键
    AConnect(keyBtn, [=] mutable {
        auto str = keyEdit->GetText();
        if (str.empty()) {
            AGetInternalLogger()->Error("你尚未输入任何按键!");
            return;
        }
        apHandle.Stop();
        apHandle = AConnect(str[0], [] {
            ASetAdvancedPause(isAdvancedPause);
            isAdvancedPause = !isAdvancedPause;
        });
    });

    // 设置游戏倍速
    AConnect(speedBtn, [=] {
        auto str = speedBox->GetString();
        ASetGameSpeed(std::stof(str));
    });

    // 设置自动收集
    AConnect(collectBox, [=] {
        if (collectBox->GetCheck()) {
            aItemCollector.GoOn();
        } else {
            aItemCollector.Pause();
        }
    });
});

void AScript()
{
    apHandle = AConnect('Z', [] {
        ASetAdvancedPause(isAdvancedPause);
        isAdvancedPause = !isAdvancedPause;
    });
}