#include <win32gui/main.h>

// MainWindow 必须创建为全局变量
AMainWindow mainWindow("Hello World");

AOnAfterInject({
    auto btn = mainWindow.AddPushButton("Hello World", 0, 0);
    AConnect(btn, [] {
        AMsgBox::Show("Hello World");
    });
});

void AScript()
{
}