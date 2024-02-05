#include <win32gui/main.h>

static bool isAdvancedPause = true;

// 高级暂停句柄必须拿住，这样才能控制高级暂停按键
static AConnectHandle apHandle;
constexpr int INTERVAL = 10;

AMainWindow mainWindow("Multi Page");

AWindow* CreateKeyPage(int pageX, int pageY)
{
    auto window = mainWindow.AddWindow(pageX, pageY);

    int x = INTERVAL;
    int y = INTERVAL;
    auto keyLabel = window->AddLabel("高级暂停按键:", x, y);
    x += INTERVAL + keyLabel->GetWidth();
    auto keyEdit = window->AddEdit("Z", x, y);
    x += INTERVAL + keyEdit->GetWidth();
    auto keyBtn = window->AddPushButton("修改", x, y);

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
    return window;
}

AWindow* CreateSpeedPage(int pageX, int pageY)
{
    auto window = mainWindow.AddWindow(pageX, pageY);

    int x = INTERVAL;
    int y = INTERVAL;
    auto speedLabel = window->AddLabel("游戏倍速:", x, y);
    x += INTERVAL + speedLabel->GetWidth();
    auto speedBox = window->AddComboBox(x, y);
    speedBox->AddString("10", "5", "2", "1", "0.5", "0.2", "0.1");
    x += INTERVAL + speedLabel->GetWidth();
    auto speedBtn = window->AddPushButton("修改", x, y);

    // 设置游戏倍速
    AConnect(speedBtn, [=] {
        auto str = speedBox->GetString();
        ASetGameSpeed(std::stof(str));
    });

    return window;
}

AOnAfterInject({
    auto keyPageBtn = mainWindow.AddPushButton("高级暂停", 0, 0);
    auto speedPageBtn = mainWindow.AddPushButton("速度设定", keyPageBtn->GetWidth() + INTERVAL, 0);

    auto keyPage = CreateKeyPage(0, keyPageBtn->GetHeight() + INTERVAL);
    auto speedPage = CreateSpeedPage(0, keyPageBtn->GetHeight() + INTERVAL);
    speedPage->Hide();

    AConnect(keyPageBtn, [=] {
        speedPage->Hide();
        keyPage->Show();
    });
    AConnect(speedPageBtn, [=] {
        keyPage->Hide();
        speedPage->Show();
    });
});

void AScript()
{
    apHandle = AConnect('Z', [] {
        ASetAdvancedPause(isAdvancedPause);
        isAdvancedPause = !isAdvancedPause;
    });
}