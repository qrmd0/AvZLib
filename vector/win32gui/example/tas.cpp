#define UNICODE
#include "showme/sm.h"
#include "win32gui/main.h"
#include <dsl/shorthand_240205.h>
#include <filesystem>
#include <iomanip>
#include <tlhelp32.h>

constexpr int MAIN_WIDTH = 700;
constexpr int MAIN_HEIGHT = 450;

// 编译时需要在编译命令中添加 -lcomdlg32

AMainWindow mainWindow("AvZ Win32Gui TAS by vector-wlc", MAIN_WIDTH, MAIN_HEIGHT);
std::shared_ptr<A7zCompressor> compressor = nullptr;
SMShowMe showMe;
ATickRunner tickRunner;
bool isInitSuccess = false;

// 得到本工具的路径
const std::string& GetToolPath()
{
    static std::string toolPath;
    if (!toolPath.empty()) {
        return toolPath;
    }
    HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (handle == INVALID_HANDLE_VALUE) {
        return toolPath;
    }

    // 枚举进程模块
    MODULEENTRY32W info;
    info.dwSize = sizeof(MODULEENTRY32W);
    Module32FirstW(handle, &info);
    while (Module32NextW(handle, &info)) {
        if (std::wstring(info.szModule).find(L"libavz") != std::wstring::npos) {
            toolPath = AWStrToStr(info.szExePath);
            for (int i = toolPath.size() - 1; i >= 0; --i) {
                if (toolPath[i] == '\\' || toolPath[i] == '/') {
                    toolPath.resize(i);
                    break;
                }
            }
            CloseHandle(handle);
            return toolPath;
        }
    }
    CloseHandle(handle);
    return toolPath;
}

std::string GetCurTimeStr()
{
    auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&t), "%Y-%m-%d-%H-%M-%S");
    return ss.str();
}

ALabel* infoLabel = nullptr;

void Info(const std::string& tip)
{
    if (infoLabel != nullptr) {
        infoLabel->SetText(tip);
    }
}

void Warning(const std::string& tip)
{
    Info(tip);
    MessageBeep(MB_ICONWARNING);
}

#define __CheckASCII(path, info, ret) \
    for (auto c : path) {             \
        if (uint8_t(c) > 127) {       \
            info;                     \
            return ret;               \
        }                             \
    }

std::string OpenFileDialog(const std::string& initPath)
{
    wchar_t szFileName[MAX_PATH] = {};
    auto initDir = AStrToWstr(initPath);
    OPENFILENAMEW openFileName = {};
    openFileName.lStructSize = sizeof(OPENFILENAMEW);
    openFileName.nMaxFile = MAX_PATH; // 这个必须设置，不设置的话不会出现打开文件对话框
    openFileName.lpstrFilter = L"回放文件 (*.7z)\0\0";
    openFileName.lpstrFile = szFileName;
    openFileName.nFilterIndex = 1;
    openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    openFileName.lpstrInitialDir = initDir.c_str();

    if (GetOpenFileNameW(&openFileName)) {
        return AWStrToStr(openFileName.lpstrFile);
    }
    return "";
}

struct {
    // Game Ctrl
    bool checkMaidCheats = false;
    float gameSpeed = 2;

    // Replay
    bool checkShowMouse = true;
    bool checkAutoRecordOnGameStart = false;
    bool checkAutoStop = true;
    bool checkInterpolate = true;
    bool checkShowTips = true;
    float recordTickInterval = 10;
    char savePath[1000];

    // Display
    bool checkHover = true;
    bool checkResidual = true;
    bool checkPumpkin = true;
    bool checkCobCannon = true;
    bool checkGloom = true;
    bool checkGarg = true;
    bool checkGiga = true;
    bool checkZomboni = true;
    bool checkFootball = true;
    bool checkJackBox = true;

    // Key
    AKey keyAdvancePause = 'Z';
    AKey keyHover = 'C';
    AKey keyRecord = 'Q';
    AKey keyPlay = 'W';
    AKey keyPause = 'E';
    AKey keyMaidCheats = 'R';
    AKey keySpeed = 'X';
    AKey keyResidualHp = 'V';
    AKey keyStop = 'S';
    AKey keyPreTick = 'A';
    AKey keyNextTick = 'D';

    // Spawn
    bool checkTypes[20] = {0};
    bool checkAverageRowSpawn = true;
    bool checkRandomType = false;
} settings;

#define FightUiCheck()                            \
    if (AGetPvzBase()->GameUi() != 3) {           \
        Warning("只有在战斗界面才能使用此功能!"); \
        return;                                   \
    }

#define FightOrCardUiCheck()                                            \
    if (AGetPvzBase()->GameUi() != 3 && AGetPvzBase()->GameUi() != 2) { \
        Warning("只有在战斗界面或者选卡界面才能使用此功能!");           \
        return;                                                         \
    }

std::mutex mtx;

void SaveSettings()
{
    if (!isInitSuccess) {
        return;
    }
    // 这里可能会导致线程不安全
    std::lock_guard lk(mtx);
    std::ofstream outFile(GetToolPath() + "/settings.dat", std::ios::out | std::ios::binary);
    outFile.write((char*)&settings, sizeof(settings));
    outFile.close();
}

void LoadSettings()
{
    std::ifstream inFile(GetToolPath() + "/settings.dat", std::ios::in | std::ios::binary);
    if (!inFile) {
        return;
    }
    inFile.read((char*)&settings, sizeof(settings));
    inFile.close();
}

class EnsureSaveSettings {
public:
    ~EnsureSaveSettings() { SaveSettings(); }
} __; // 全局对象的析构函数确保调用 SaveSettings

// 游戏控制配置
struct {
    AConnectHandle advancePause;
    AConnectHandle maidCheats;
    AConnectHandle speed;

    bool isAdvancePause = true;

    ACheckBox* maidCheatsBox = nullptr;
    AComboBox* maidCheatsOptionBox = nullptr;
    int tickMs = 10;

    AOperation advancePauseCb = [this] {
        FightOrCardUiCheck();
        if (aReplay.GetState() == aReplay.PLAYING) {
            Warning("回放正在播放，无法使用高级暂停功能");
            return;
        }
        ASetAdvancedPause(isAdvancePause);
        isAdvancePause = !isAdvancePause;
    };

    std::map<std::string, AOperation> maidCheatsDicts = {
        {"跳舞", AMaidCheats::Dancing},
        {"前进", AMaidCheats::Move},
        {"召唤", AMaidCheats::CallPartner},
    };
    AOperation maidCheatsFunc = [this] {
        if (settings.checkMaidCheats) {
            auto option = maidCheatsOptionBox->GetString();
            auto iter = maidCheatsDicts.find(option);
            if (iter != maidCheatsDicts.end()) {
                iter->second();
            }
        } else {
            AMaidCheats::Stop();
        }
    };
    AOperation maidCheatsCb = [this] {
        FightUiCheck();
        auto checked = maidCheatsBox->GetCheck();
        maidCheatsBox->SetCheck(!checked);
        settings.checkMaidCheats = !checked;
        maidCheatsFunc();
    };
    AOperation speedCb = [this]() {
        if (AGetPvzBase()->TickMs() != 10) {
            AGetPvzBase()->TickMs() = 10;
        } else {
            AGetPvzBase()->TickMs() = this->tickMs;
        }
    };
} gameCtrlConfig;

// 回放配置
struct {
    AConnectHandle startRecord;
    AConnectHandle startPlay;
    AConnectHandle pause;
    AConnectHandle goOn;
    AConnectHandle stop;
    AConnectHandle nextTick;
    AConnectHandle preTick;

    AOperation startRecordCb = [] {
        FightUiCheck();
        if (aReplay.GetState() == AReplay::RECORDING) {
            Warning("正在录制");
            return;
        }
        if (settings.checkAutoStop) {
            aReplay.Stop();
        }
        compressor->SetFilePath(settings.savePath + std::string("/") + GetCurTimeStr() + ".7z");
        aReplay.StartRecord(std::round(settings.recordTickInterval));
        Info("Replay : 开始录制");
    };
    AOperation startPlayCb = [] {
        FightUiCheck();
        if (aReplay.GetState() == AReplay::PLAYING) {
            Warning("正在播放");
            return;
        }
        if (settings.checkAutoStop) {
            aReplay.Stop();
        }
        auto fileName = OpenFileDialog(settings.savePath);
        if (fileName.empty()) {
            return;
        }
        compressor->SetFilePath(fileName);
        aReplay.StartPlay();
        Info("Replay : 开始播放");
    };
    AOperation pauseCb = [] {
        FightUiCheck();
        if (aReplay.IsPaused()) {
            if (aReplay.GetState() == aReplay.RECORDING) {
                ASetAdvancedPause(false);
                gameCtrlConfig.isAdvancePause = false;
            }
            aReplay.GoOn();
        } else {
            aReplay.Pause();
        }
    };

    AOperation stopCb = [] {
        aReplay.Stop();
    };
    AOperation nextTickCb = [] {
        FightUiCheck();
        if (aReplay.GetState() == aReplay.RECORDING) {
            ASetAdvancedPause(true);
            gameCtrlConfig.isAdvancePause = true;
        }
        aReplay.Pause();
        if (aReplay.GetState() == AReplay::PLAYING) {
            aReplay.ShowOneTick(aReplay.GetPlayIdx() + 1);
        }
    };
    AOperation preTickCb = [] {
        FightUiCheck();
        if (aReplay.GetState() == aReplay.RECORDING) {
            ASetAdvancedPause(true);
            gameCtrlConfig.isAdvancePause = true;
        }
        aReplay.Pause();
        if (aReplay.GetState() == AReplay::RECORDING) {
            aReplay.ShowOneTick(aReplay.GetRecordIdx() - 1);
        } else {
            aReplay.ShowOneTick(aReplay.GetPlayIdx() - 1);
        }
    };

} replayConfig;

struct {
    AConnectHandle hover;      // 悬停
    AConnectHandle residualHp; // 残血

    ACheckBox* hoverBox = nullptr;
    ACheckBox* residualHpBox = nullptr;

    AOperation hoverCb = [this] {
        FightUiCheck();
        auto checked = hoverBox->GetCheck();
        hoverBox->SetCheck(!checked);
        settings.checkHover = !checked;
        if (settings.checkHover) {
            showMe.Start();
        } else {
            showMe.Stop();
        }
    };

    AOperation residualHpCb = [this] {
        auto checked = residualHpBox->GetCheck();
        residualHpBox->SetCheck(!checked);
        settings.checkResidual = !checked;
    };
} displayConfig;

constexpr int SPACE = 10;
constexpr int WIDTH = 100;
constexpr int HEIGHT = 30;

std::pair<AEdit*, APushButton*> CreateEditButton(AWindow* window, const std::string& labelText,
    const std::string& editText, const std::string& btnText, int x, int y)
{
    auto label = window->AddLabel(labelText, x, y);
    x += SPACE + label->GetWidth();
    auto edit = window->AddEdit(editText, x, y, 100, 30);
    x += SPACE + edit->GetWidth();
    auto btn = window->AddPushButton(btnText, x, y);
    return {edit, btn};
}

template <typename Func>
void CreateKeyButton(AWindow* window, const std::string& labelText,
    AKey& key, const std::string& btnText, int x, int y, AConnectHandle& handle, Func&& func)
{
    std::string str;
    str.push_back(key);
    auto [edit, btn] = CreateEditButton(window, labelText, str, btnText, x, y);
    AConnect(btn, [&key, &handle, edit = edit, func = std::forward<Func>(func)] mutable {
        FightOrCardUiCheck();
        auto str = edit->GetText();
        if ((str[0] > 'Z' || str[0] < 'A') && (str[0] > '9' || str[0] < '0')) {
            Warning("绑定的按键只能为大写字母或数字");
            return;
        }
        handle.Stop();
        key = str[0];
        handle = AConnect(str[0], std::move(func));
        Info(std::string("按键") + str[0] + "绑定成功");
    });
}

template <typename Func>
void CreateNumberButton(AWindow* window, const std::string& labelText,
    float& number, const std::string& btnText, int x, int y, Func&& func)
{
    std::stringstream ss;
    ss << number;
    auto [edit, btn] = CreateEditButton(window, labelText, ss.str(), btnText, x, y);
    AConnect(btn, [&number, edit = edit, func = std::forward<Func>(func)] mutable {
        try {
            auto val = std::stof(edit->GetText());
            number = val;
            func(val);
        } catch (std::exception& exce) {
            Warning(std::string("捕获到 C++ 标准异常: ") + exce.what());
        }
    });
}

void CreateGameCtrlGroup(AWindow* window, int x, int y)
{
    window->AddLabel("游戏控制", x, y, 4 * (SPACE + WIDTH), (SPACE + HEIGHT) * 2);
    gameCtrlConfig.maidCheatsBox = window->AddCheckBox("女仆秘籍", 2 * (SPACE + WIDTH) + SPACE, y);
    gameCtrlConfig.maidCheatsBox->SetCheck(settings.checkMaidCheats);
    gameCtrlConfig.maidCheatsOptionBox = window->AddComboBox(3 * (SPACE + WIDTH) + SPACE, y);
    for (auto&& [option, _] : gameCtrlConfig.maidCheatsDicts) {
        gameCtrlConfig.maidCheatsOptionBox->AddString(option);
    }
    y += SPACE + HEIGHT;
    auto advancePause = window->AddPushButton("高级暂停", x, y);
    x += SPACE + WIDTH;
    CreateNumberButton(window, "游戏倍速:", settings.gameSpeed, "设置", x, y, [](float speed) {
        ASetGameSpeed(speed);
        if (speed >= 0.05 && speed <= 10) {
            settings.gameSpeed = speed;
            gameCtrlConfig.tickMs = int(10 / speed + 0.5);
            Info("游戏倍速设置成功");
        }
    });
    AConnect(advancePause, gameCtrlConfig.advancePauseCb);
    AConnect(gameCtrlConfig.maidCheatsBox, [] {
        if (AGetPvzBase()->GameUi() != 3) {
            gameCtrlConfig.maidCheatsBox->SetCheck(!gameCtrlConfig.maidCheatsBox->GetCheck());
            Warning("只有在战斗界面才能使用此功能!");
            return;
        }
        settings.checkMaidCheats = gameCtrlConfig.maidCheatsBox->GetCheck();
        gameCtrlConfig.maidCheatsFunc();
    });
}

void CreateReplayGroup(AWindow* window, int x, int y)
{
    window->AddLabel("回放", x, y, 4 * (SPACE + WIDTH), (SPACE + HEIGHT) * 5);

    auto showMouseBox = window->AddCheckBox("显示鼠标", SPACE + WIDTH + SPACE, y);
    showMouseBox->SetCheck(settings.checkShowMouse);
    auto autoRecordBox = window->AddCheckBox("游戏开始时自动录制", 2 * (SPACE + WIDTH) + SPACE, y, WIDTH * 2);
    autoRecordBox->SetCheck(settings.checkAutoRecordOnGameStart);
    y += SPACE + HEIGHT;
    auto startRecordBtn = window->AddPushButton("录制", x, y);
    x += SPACE + WIDTH;
    auto startPlayBtn = window->AddPushButton("播放", x, y);
    x += SPACE + WIDTH;
    auto pauseBtn = window->AddPushButton("暂停/继续", x, y);
    x += SPACE + WIDTH;
    auto stopBtn = window->AddPushButton("停止", x, y);

    AConnect(showMouseBox, [showMouseBox] {
        settings.checkShowMouse = showMouseBox->GetCheck();
        aReplay.SetMouseVisible(settings.checkShowMouse);
    });
    AConnect(autoRecordBox, [autoRecordBox] {
        settings.checkAutoRecordOnGameStart = autoRecordBox->GetCheck();
    });
    AConnect(startRecordBtn, replayConfig.startRecordCb);
    AConnect(startPlayBtn, replayConfig.startPlayCb);
    AConnect(pauseBtn, replayConfig.pauseCb);
    AConnect(stopBtn, replayConfig.stopCb);

    // 下一行
    y += SPACE + HEIGHT;
    x = SPACE;
    auto preTickBtn = window->AddPushButton("上一帧", x, y);
    x += SPACE + WIDTH;
    auto autoStopBox = window->AddCheckBox("切换时停止", x, y);
    autoStopBox->SetCheck(settings.checkAutoStop);
    x += SPACE + WIDTH;
    auto interpolateBox = window->AddCheckBox("播放时补帧", x, y);
    interpolateBox->SetCheck(settings.checkInterpolate);
    x += SPACE + WIDTH;
    auto tipBox = window->AddCheckBox("提示信息", x, y);
    tipBox->SetCheck(settings.checkShowTips);
    AConnect(autoStopBox, [autoStopBox] {
        settings.checkAutoStop = autoStopBox->GetCheck();
    });
    AConnect(preTickBtn, replayConfig.preTickCb);
    AConnect(interpolateBox, [=] {
        settings.checkInterpolate = interpolateBox->GetCheck();
        aReplay.SetInterpolate(settings.checkInterpolate);
    });
    AConnect(tipBox, [=] {
        settings.checkShowTips = tipBox->GetCheck();
        aReplay.SetShowInfo(settings.checkShowTips);
    });

    // 下一行
    y += SPACE + HEIGHT;
    x = SPACE;
    auto nextTickBtn = window->AddPushButton("下一帧", x, y);
    x += SPACE + WIDTH;
    CreateNumberButton(window, "录制帧间隔:", settings.recordTickInterval, "设置", x, y, [](float tickInterval) {
        if (aReplay.GetState() != aReplay.REST) {
            Warning("回放正在工作，无法设置帧间隔");
            return;
        };
        if (tickInterval < 1) {
            Warning("帧间隔最小为 1");
            tickInterval = 1;
        }
        settings.recordTickInterval = int(tickInterval + 0.5);
        Info("设置录制帧间隔成功");
    });
    AConnect(nextTickBtn, replayConfig.nextTickCb);

    // 下一行
    y += SPACE + HEIGHT;
    x = SPACE;
    window->AddLabel("保存路径:", x, y);
    x += SPACE + WIDTH;
    auto savePathEdit = window->AddEdit(settings.savePath, x, y, WIDTH * 2 + SPACE, 30, ES_AUTOHSCROLL);
    savePathEdit->SetText(settings.savePath);
    x += (SPACE + WIDTH) * 2;
    auto btn = window->AddPushButton("设置", x, y);
    AConnect(btn, [=] {
        auto path = savePathEdit->GetText();
        __CheckASCII(path, Warning("您设置的保存路径: [" + path + "] 中含有非 ASCII 字符, 请将其设置为纯英文路径再次尝试");
                     savePathEdit->SetText(settings.savePath), );
        if (!std::filesystem::exists(path)) {
            Warning("设置的路径: [" + path + "] 不存在");
            savePathEdit->SetText(settings.savePath);
        } else {
            std::strcpy(settings.savePath, path.c_str());
            Info("设置路径: [" + path + "] 成功");
        }
    });
}

void CreateInfoDisplayGroup(AWindow* window, int x, int y)
{
    // 这里根据视觉效果调整的
    constexpr int HEIGHT = 38;
    window->AddLabel("信息显示", x, y, (WIDTH + SPACE) * 2, (HEIGHT + SPACE) * 6);
    y += SPACE + HEIGHT;
    int tmpY = y;
    displayConfig.hoverBox = window->AddCheckBox("悬停显示", x, y);
    y += SPACE + HEIGHT;
    auto pumpkinHpBox = window->AddCheckBox("南瓜血量", x, y);
    y += SPACE + HEIGHT;
    auto cobCannonHpBox = window->AddCheckBox("春哥血量", x, y);
    y += SPACE + HEIGHT;
    auto gloomHpBox = window->AddCheckBox("曾哥血量", x, y);
    y += SPACE + HEIGHT;
    auto gargHpBox = window->AddCheckBox("白眼血量", x, y);

    // 下一列
    x += SPACE + WIDTH;
    y = tmpY;
    displayConfig.residualHpBox = window->AddCheckBox("残血时显示", x, y);
    y += SPACE + HEIGHT;
    auto gigaHpBox = window->AddCheckBox("红眼血量", x, y);
    y += SPACE + HEIGHT;
    auto zomboniHpBox = window->AddCheckBox("冰车血量", x, y);
    y += SPACE + HEIGHT;
    auto footballHpBox = window->AddCheckBox("橄榄血量", x, y);
    y += SPACE + HEIGHT;
    auto jackBoxHpBox = window->AddCheckBox("小丑血量", x, y);

    displayConfig.hoverBox->SetCheck(settings.checkHover);
    pumpkinHpBox->SetCheck(settings.checkPumpkin);
    cobCannonHpBox->SetCheck(settings.checkCobCannon);
    gloomHpBox->SetCheck(settings.checkGloom);
    gargHpBox->SetCheck(settings.checkGarg);
    displayConfig.residualHpBox->SetCheck(settings.checkResidual);
    zomboniHpBox->SetCheck(settings.checkZomboni);
    footballHpBox->SetCheck(settings.checkFootball);
    gigaHpBox->SetCheck(settings.checkGiga);
    jackBoxHpBox->SetCheck(settings.checkJackBox);

    AConnect(displayConfig.hoverBox, [] {
        settings.checkHover = displayConfig.hoverBox->GetCheck();
        if (settings.checkHover) {
            showMe.Start();
        } else {
            showMe.Stop();
        }
    });

    AConnect(displayConfig.residualHpBox, [] {
        settings.checkResidual = displayConfig.residualHpBox->GetCheck();
    });

    AConnect(pumpkinHpBox, [pumpkinHpBox] { settings.checkPumpkin = pumpkinHpBox->GetCheck(); });
    AConnect(cobCannonHpBox, [cobCannonHpBox] { settings.checkCobCannon = cobCannonHpBox->GetCheck(); });
    AConnect(gloomHpBox, [gloomHpBox] { settings.checkGloom = gloomHpBox->GetCheck(); });
    AConnect(gargHpBox, [gargHpBox] { settings.checkGarg = gargHpBox->GetCheck(); });
    AConnect(zomboniHpBox, [zomboniHpBox] { settings.checkZomboni = zomboniHpBox->GetCheck(); });
    AConnect(footballHpBox, [footballHpBox] { settings.checkFootball = footballHpBox->GetCheck(); });
    AConnect(gigaHpBox, [gigaHpBox] { settings.checkGiga = gigaHpBox->GetCheck(); });
    AConnect(jackBoxHpBox, [jackBoxHpBox] { settings.checkJackBox = jackBoxHpBox->GetCheck(); });
}

// 基本页面
AWindow* CreateBasicPage(AWindow* parent, int pageX, int pageY)
{
    auto window = parent->AddWindow(pageX, pageY);
    CreateGameCtrlGroup(window, SPACE, SPACE);
    CreateReplayGroup(window, SPACE, (SPACE * 2 + HEIGHT) * 2);
    CreateInfoDisplayGroup(window, (SPACE + WIDTH) * 4 + SPACE * 2, SPACE);
    return window;
}

AWindow* CreateKeyPage(AWindow* parent, int pageX, int pageY)
{
    auto window = parent->AddWindow(pageX, pageY);
    constexpr int HEIGHT = 40;
    window->AddLabel("", SPACE, 0, (SPACE + WIDTH) * 6 + SPACE, (SPACE + HEIGHT) * 6);
    int x = SPACE;
    int y = SPACE;
    CreateKeyButton(window, "高级暂停:", settings.keyAdvancePause, "设置", x, y,
        gameCtrlConfig.advancePause, gameCtrlConfig.advancePauseCb);
    y += SPACE + HEIGHT;
    CreateKeyButton(window, "悬停显示:", settings.keyHover, "设置", x, y,
        displayConfig.hover, displayConfig.hoverCb);
    y += SPACE + HEIGHT;
    CreateKeyButton(window, "录制:", settings.keyRecord, "设置", x, y,
        replayConfig.startRecord, replayConfig.startRecordCb);
    y += SPACE + HEIGHT;
    CreateKeyButton(window, "播放:", settings.keyPlay, "设置", x, y,
        replayConfig.startPlay, replayConfig.startPlayCb);
    y += SPACE + HEIGHT;
    CreateKeyButton(window, "暂停/继续:", settings.keyPause, "设置", x, y,
        replayConfig.pause, replayConfig.pauseCb);
    y += SPACE + HEIGHT;
    CreateKeyButton(window, "女仆秘籍:", settings.keyMaidCheats, "设置", x, y,
        gameCtrlConfig.maidCheats, gameCtrlConfig.maidCheatsCb);

    // 下一列
    x = (SPACE + WIDTH) * 3 + SPACE;
    y = SPACE;
    CreateKeyButton(window, "倍速切换:", settings.keySpeed, "设置", x, y,
        replayConfig.startRecord, replayConfig.startRecordCb);
    y += SPACE + HEIGHT;
    CreateKeyButton(window, "残血时显示:", settings.keyResidualHp, "设置", x, y,
        displayConfig.residualHp, displayConfig.residualHpCb);
    y += SPACE + HEIGHT;
    CreateKeyButton(window, "停止:", settings.keyStop, "设置", x, y,
        replayConfig.stop, replayConfig.stopCb);
    y += SPACE + HEIGHT;
    CreateKeyButton(window, "上一帧:", settings.keyPreTick, "设置", x, y,
        replayConfig.preTick, replayConfig.preTickCb);
    y += SPACE + HEIGHT;
    CreateKeyButton(window, "下一帧:", settings.keyNextTick, "设置", x, y,
        replayConfig.nextTick, replayConfig.nextTickCb);
    y += SPACE + HEIGHT;
    return window;
}

AWindow* CreateSpawnPage(AWindow* parent, int pageX, int pageY)
{
    auto window = parent->AddWindow(pageX, pageY);
    struct Info {
        std::string name;
        int type;
        ACheckBox* box;
    };
    std::vector<Info> infos = {
        {"路障", ALZ_2},
        {"撑杆", ACG_3},
        {"铁桶", ATT_4},
        {"读报", ADB_5},
        {"铁门", ATM_6},
        {"橄榄", AGL_7},
        {"舞王", AWW_8},
        {"潜水", AQS_11},
        {"冰车", ABC_12},
        {"海豚", AHT_14},
        {"小丑", AXC_15},
        {"气球", AQQ_16},
        {"矿工", AKG_17},
        {"跳跳", ATT_18},
        {"雪人", AXR_19},
        {"蹦极", ABJ_20},
        {"扶梯", AFT_21},
        {"投篮", ATL_22},
        {"白眼", ABY_23},
        {"红眼", AHY_32},
    };

    constexpr int ROW_CNT = 4;
    constexpr int COL_CNT = 5;
    constexpr int WIDTH = 125;
    constexpr int HEIGHT = 45;

    window->AddLabel("", SPACE, SPACE, (WIDTH + SPACE) * 5, (HEIGHT + SPACE) * 5);

    for (int row = 0; row < ROW_CNT; ++row) {
        for (int col = 0; col < COL_CNT; ++col) {
            int x = col * (WIDTH + SPACE) + SPACE;
            int y = row * (HEIGHT + SPACE) + SPACE * 2;
            int idx = row * COL_CNT + col;
            infos[idx].box = window->AddCheckBox(infos[idx].name, x, y);
            infos[idx].box->SetCheck(settings.checkTypes[idx]);
        }
    }

    int x = (WIDTH + SPACE) * 4 + SPACE;
    int y = (HEIGHT + SPACE) * 4 + SPACE * 2;
    auto averageBtn = window->AddPushButton("极限出怪", x, y);
    x -= SPACE + WIDTH;
    auto internalBtn = window->AddPushButton("自然出怪", x, y);
    x -= SPACE + WIDTH;
    auto randomTypeBox = window->AddCheckBox("随机添加种类", x, y, 120);
    randomTypeBox->SetCheck(settings.checkRandomType);
    x -= SPACE + WIDTH;
    auto averageSpawnBox = window->AddCheckBox("每行均匀分配", x, y, 120);
    averageSpawnBox->SetCheck(settings.checkAverageRowSpawn);

    auto spawnFunc = [infos = std::move(infos)](ASetZombieMode mode) {
        std::vector<int> types;
        types.push_back(APJ_0);
        for (int i = 0; i < infos.size(); ++i) {
            settings.checkTypes[i] = infos[i].box->GetCheck();
            if (settings.checkTypes[i]) {
                types.push_back(infos[i].type);
            }
        }
        FightOrCardUiCheck();
        if (settings.checkRandomType) {
            try {
                types = ACreateRandomTypeList(types);
            } catch (AException& exce) {
                Warning(std::string("捕获到 AException:") + exce.what());
                return;
            }
        }
        ASetZombies(types, mode);
        ::Info("出怪设置成功");
    };

    AConnect(averageBtn, [=] {
        spawnFunc(ASetZombieMode::AVERAGE);
    });
    AConnect(internalBtn, [spawnFunc = std::move(spawnFunc)] {
        spawnFunc(ASetZombieMode::INTERNAL);
    });
    AConnect(randomTypeBox, [randomTypeBox] {
        settings.checkRandomType = randomTypeBox->GetCheck();
    });
    AConnect(averageSpawnBox, [averageSpawnBox] {
        settings.checkAverageRowSpawn = averageSpawnBox->GetCheck();
    });
    return window;
}

AOnAfterInject({
    __CheckASCII(GetToolPath(),
                 AMsgBox::Show("本工具只能在纯英文路径下才能正常运行, 你放置的路径: [" //
                     + GetToolPath() + "] 中含有非 ASCII 字符, 请将本工具的所有文件放置在纯英文路径下再次尝试运行");
                 ATerminate(), );
    isInitSuccess = true;

    strcpy(settings.savePath, GetToolPath().c_str());
    compressor = std::make_shared<A7zCompressor>(GetToolPath() + "/7z.exe");
    aReplay.SetCompressor(*compressor);

    LoadSettings();
    aReplay.SetMouseVisible(settings.checkShowMouse);
    aReplay.SetSaveDirPath(settings.savePath);
    gameCtrlConfig.tickMs = int(10 / settings.gameSpeed + 0.5);
    int x = SPACE;
    int y = SPACE;
    auto basicBtn = mainWindow.AddPushButton("基础", x, y);
    x += SPACE + WIDTH;
    auto keyBtn = mainWindow.AddPushButton("快捷键", x, y);
    x += SPACE + WIDTH;
    auto spawnBtn = mainWindow.AddPushButton("出怪", x, y);
    auto basicPage = CreateBasicPage(&mainWindow, 0, y + HEIGHT + SPACE);
    auto keyPage = CreateKeyPage(&mainWindow, 0, y + HEIGHT + SPACE);
    auto spawnPage = CreateSpawnPage(&mainWindow, 0, y + HEIGHT + SPACE);
    mainWindow.AddLabel("信息:", SPACE, MAIN_HEIGHT - SPACE * 2 - HEIGHT * 2, 100, 45);
    infoLabel = mainWindow.AddLabel("", SPACE + WIDTH, MAIN_HEIGHT - SPACE * 2 - HEIGHT * 2, (WIDTH + SPACE) * 5, 45);

    AConnect(basicBtn, [=] {
        keyPage->Hide();
        spawnPage->Hide();
        basicPage->Show();
    });
    AConnect(keyBtn, [=] {
        basicPage->Hide();
        spawnPage->Hide();
        keyPage->Show();
    });
    AConnect(spawnBtn, [=] {
        spawnPage->Show();
        basicPage->Hide();
        keyPage->Hide();
    });
    keyPage->Hide();
    spawnPage->Hide();
    basicPage->Show();
});

AOnBeforeExit({
    SaveSettings();
});

void ShowInfo()
{
    if (AGetPvzBase()->GameUi() != 3) {
        return;
    }
    auto&& painter = showMe.GetPainter();

    // 显示植物僵尸的血量
    // 因为种类较少
    // 直接暴力枚举了
    int frameSize = 1;  // 边框粗细（像素）
    int barWidth = 100; // 水平进度条的宽度（像素）
    int barHeight = 6;  // 水平进度条的高度（像素）

    for (auto&& plant : aAlivePlantFilter) {
        int curHp = plant.Hp();
        int maxHp = plant.HpMax();
        if (curHp == maxHp && settings.checkResidual) {
            continue;
        }
        float rate = float(curHp) / maxHp;
        int plantType = plant.Type();
        int barX = plant.Abscissa() + 40;
        int barY = plant.Ordinate() + (barHeight + 2 * frameSize) + 20;
        if (plantType == ACOB_CANNON) {
            if (!settings.checkCobCannon) {
                continue;
            }
            painter.DrawBar(barX + 40, barY, rate, 2 * 72,
                barHeight, 1, AArgb(0xFF, 0x00, 0xFF, 0x00),
                SMPainter::RIGHT, {});
        } else if (plantType == APUMPKIN) {
            if (!settings.checkPumpkin) {
                continue;
            }
            painter.DrawBar(barX, barY + 30, rate, 72,
                barHeight, 1, AArgb(0xFF, 0x00, 0xFF, 0x00),
                SMPainter::RIGHT, {});
        } else if (plantType == AGLOOM_SHROOM) {
            if (!settings.checkGloom) {
                continue;
            }
            painter.DrawBar(barX, barY, rate, 72,
                barHeight, 1, AArgb(0xFF, 0x00, 0xFF, 0x00),
                SMPainter::RIGHT, {});
        }
    }

    barWidth = 10;  // 水平进度条的宽度（像素）
    barHeight = 80; // 水平进度条的高度（像素）
    for (auto&& zombie : aAliveZombieFilter) {
        int curHp = zombie.Hp() + zombie.OneHp();
        int maxHp = zombie.MRef<int>(0xcc) + zombie.MRef<int>(0xd4);
        if (curHp == maxHp && settings.checkResidual) {
            continue;
        }
        float rate = float(curHp) / maxHp;
        int barX = int((2 * zombie.Abscissa() + zombie.BulletAbscissa() + zombie.HurtWidth()) / 2);
        int barY = int(zombie.Ordinate() + zombie.BulletOrdinate() + (barHeight + 2 * frameSize));
        int zombieType = zombie.Type();
        if (zombieType == AGARGANTUAR) {
            if (!settings.checkGarg) {
                continue;
            }
            painter.DrawBar(barX, barY, rate, barWidth,
                barHeight, 1, AArgb(0xFF, 0xFF, 0, 0x00),
                SMPainter::UP, {1800 / 3000.0});
        } else if (zombieType == AGIGA_GARGANTUAR) {
            if (!settings.checkGiga) {
                continue;
            }
            painter.DrawBar(
                barX, barY, rate, barWidth, barHeight, 1,
                AArgb(0xFF, 0xFF, 0, 0x00), SMPainter::UP,
                {1800 / 6000.0, 1800 * 2 / 6000.0, 1800 * 3 / 6000.0});
        } else if ((zombieType == AGL_7 && settings.checkFootball)
            || (zombieType == AXC_15 && settings.checkJackBox)
            || (zombieType == ABC_12 && settings.checkZomboni)) {
            painter.DrawBar(barX, barY, rate, barWidth,
                barHeight, 1, AArgb(0xFF, 0xFF, 0, 0x00),
                SMPainter::UP, {});
        }
    }
}

// ALogger<AConsole> logger;

void AScript()
{
    ASetReloadMode(AReloadMode::MAIN_UI_OR_FIGHT_UI);
    // ASetInternalLogger(logger);
    // 按键初始化
    gameCtrlConfig.advancePause = AConnect(settings.keyAdvancePause, gameCtrlConfig.advancePauseCb);
    displayConfig.hover = AConnect(settings.keyHover, displayConfig.hoverCb);
    replayConfig.startRecord = AConnect(settings.keyRecord, replayConfig.startRecordCb);
    replayConfig.startPlay = AConnect(settings.keyPlay, replayConfig.startPlayCb);
    replayConfig.pause = AConnect(settings.keyPause, replayConfig.pauseCb);
    gameCtrlConfig.maidCheats = AConnect(settings.keyMaidCheats, gameCtrlConfig.maidCheatsCb);
    gameCtrlConfig.speed = AConnect(settings.keySpeed, gameCtrlConfig.speedCb);
    displayConfig.residualHp = AConnect(settings.keyResidualHp, displayConfig.residualHpCb);
    replayConfig.stop = AConnect(settings.keyStop, replayConfig.stopCb);
    replayConfig.preTick = AConnect(settings.keyPreTick, replayConfig.preTickCb);
    replayConfig.nextTick = AConnect(settings.keyNextTick, replayConfig.nextTickCb);

    // 信息显示
    AWaitForFight();
    aItemCollector.Stop();
    aReplay.SetMaxSaveCnt(INT_MAX);
    if (settings.checkHover) {
        showMe.Start();
    } else {
        showMe.Stop();
    }
    if (settings.checkAutoRecordOnGameStart) {
        replayConfig.startRecordCb();
    }

    if (settings.checkAverageRowSpawn) {
        AAverageSpawn();
    }

    tickRunner.Start(ShowInfo, ATickRunner::GLOBAL);
}