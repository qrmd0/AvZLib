#include "KeyAssistant.h"
#include "avz.h"

using namespace AvZ;

void Script()
{
    SetErrorMode(CONSOLE);
    // 启动KeyAssistant
    key_assistant.enable();
}