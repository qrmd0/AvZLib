#ifndef __CONSTANT__
#define __CONSTANT__

// 按键名
const std::map<int, std::string> _keyNames = {
    {0x01, "鼠标左键"},
    {0x02, "鼠标右键"},
    {0x03, "控制中断处理"},
    {0x04, "鼠标中键 (三键鼠标)"},
    {0x05, "X1 鼠标按钮"},
    {0x06, "X2 鼠标按钮"},
    {0x08, "BACKSPACE 键"},
    {0x09, "Tab 键"},
    {0x0C, "CLEAR 键"},
    {0x0D, "Enter 键"},
    {0x10, "SHIFT 键"},
    {0x11, "CTRL 键"},
    {0x12, "Alt 键"},
    {0x13, "PAUSE 键"},
    {0x14, "CAPS LOCK 键"},
    {0x15, "IME Kana 模式"},
    {0x15, "VK_HANGUEL"},
    {0x15, "IME Hanguel 模式"},
    {0x16, "输入法打开键"},
    {0x17, "IME Junja 模式"},
    {0x18, "IME 最终模式"},
    {0x19, "IME Hanja 模式"},
    {0x19, "IME Kanji 模式"},
    {0x1A, "IME 关闭"},
    {0x1B, "ESC 键"},
    {0x1C, "IME 转换"},
    {0x1D, "IME 不转换"},
    {0x1E, "IME 接受"},
    {0x1F, "IME 模式更改请求"},
    {0x20, "空格键"},
    {0x21, "PAGE UP 键"},
    {0x22, "PAGE DOWN 键"},
    {0x23, "END 键"},
    {0x24, "HOME 键"},
    {0x25, "向左键"},
    {0x26, "向上键"},
    {0x27, "向右键"},
    {0x28, "向下键"},
    {0x29, "SELECT 键"},
    {0x2A, "PRINT 键"},
    {0x2B, "EXECUTE 键"},
    {0x2C, "打印屏幕键"},
    {0x2D, "INS 键"},
    {0x2E, "DEL 键"},
    {0x2F, "HELP 键"},
    {0x30, "0 键"},
    {0x31, "1 键"},
    {0x32, "2 键"},
    {0x33, "3 键"},
    {0x34, "4 键"},
    {0x35, "5 键"},
    {0x36, "6 键"},
    {0x37, "7 键"},
    {0x38, "8 键"},
    {0x39, "9 键"},
    {0x42, "B 键"},
    {0x43, "C 键"},
    {0x44, "D 键"},
    {0x45, "E 键"},
    {0x46, "F 键"},
    {0x47, "G 键"},
    {0x48, "H 键"},
    {0x49, "I 键"},
    {0x4A, "J 键"},
    {0x4B, "K 键"},
    {0x4C, "L 键"},
    {0x4D, "M 键"},
    {0x4E, "N 键"},
    {0x4F, "O 键"},
    {0x50, "P 键"},
    {0x51, "Q 键"},
    {0x52, "R 键"},
    {0x53, "S 键"},
    {0x54, "T 键"},
    {0x55, "U 键"},
    {0x56, "V 键"},
    {0x57, "W 键"},
    {0x58, "X 键"},
    {0x59, "Y 键"},
    {0x5A, "Z 键"},
    {0x5B, "左 Windows 键(自然键盘)"},
    {0x5C, "右 Windows 键 (自然键盘)"},
    {0x5D, "应用程序键 (自然键盘)"},
    {0x5F, "计算机休眠键"},
    {0x60, "数字键盘 0 键"},
    {0x61, "数字键盘 1 键"},
    {0x62, "数字键盘 2 键"},
    {0x63, "数字键盘 3 键"},
    {0x64, "数字键盘 4 键"},
    {0x65, "数字键盘 5 键"},
    {0x66, "数字键盘 6 键"},
    {0x67, "数字键盘 7 键"},
    {0x68, "数字键盘 8 键"},
    {0x69, "数字键盘 9 键"},
    {0x6A, "乘键"},
    {0x6B, "添加键"},
    {0x6C, "分隔符键"},
    {0x6D, "减去键"},
    {0x6E, "十进制键"},
    {0x6F, "除号键"},
    {0x70, "F1 键"},
    {0x71, "F2 键"},
    {0x72, "F3 键"},
    {0x73, "F4 键"},
    {0x74, "F5 键"},
    {0x75, "F6 键"},
    {0x76, "F7 键"},
    {0x77, "F8 键"},
    {0x78, "F9 键"},
    {0x79, "F10 键"},
    {0x7A, "F11 键"},
    {0x7B, "F12 键"},
    {0x7C, "F13 键"},
    {0x7D, "F14 键"},
    {0x7E, "F15 键"},
    {0x7F, "F16 键"},
    {0x80, "F17 键"},
    {0x81, "F18 键"},
    {0x82, "F19 键"},
    {0x83, "F20 键"},
    {0x84, "F21 键"},
    {0x85, "F22 键"},
    {0x86, "F23 键"},
    {0x87, "F24 键"},
    {0x90, "NUM LOCK 键"},
    {0x91, "SCROLL LOCK 键"},
    {0xA0, "左 SHIFT 键"},
    {0xA1, "右 SHIFT 键"},
    {0xA2, "左 Ctrl 键"},
    {0xA3, "右 Ctrl 键"},
    {0xA4, "左 ALT 键"},
    {0xA5, "右 ALT 键"},
    {0xA6, "浏览器后退键"},
    {0xA7, "浏览器前进键"},
    {0xA8, "浏览器刷新键"},
    {0xA9, "浏览器停止键"},
    {0xAA, "浏览器搜索键"},
    {0xAB, "浏览器收藏键"},
    {0xAC, "浏览器开始”和主页 键"},
    {0xAD, "静音键"},
    {0xAE, "音量减小键"},
    {0xAF, "音量增加键"},
    {0xB0, "下一曲目键"},
    {0xB1, "上一曲目键"},
    {0xB2, "停止媒体键"},
    {0xB3, "播放/暂停媒体键"},
    {0xB4, "启动邮件键"},
    {0xB5, "选择媒体键"},
    {0xB6, "启动应用程序 1 键"},
    {0xB7, "启动应用程序 2 键"},
    {0xBA, ";：键"},
    {0xBB, "+ 键"},
    {0xBC, "， 键"},
    {0xBD, "- 键"},
    {0xBE, ". 键"},
    {0xBF, "/？键"},
    {0xC0, "”~键"},
    {0xDB, "[{ 键"},
    {0xDC, "\\| 键"},
    {0xDD, "]} 键"},
    {0xDE, "单引号/双引号 键"},
    {0xDF, "VK_OEM_8"},
    {0xE2, "VK_OEM_102"},
    {0xE5, "输入法进程键"},
    {0xE7, "VK_PACKET"},
    {0xF6, "Attn 键"},
    {0xF7, "CrSel 键"},
    {0xF8, "ExSel 键"},
    {0xF9, "擦除 EOF 键"},
    {0xFA, "播放键"},
    {0xFB, "缩放键"},
    {0xFD, "PA1 键"},
    {0xFE, "清除键"},
};

// 植物攻击周期
const std::vector<int> _plantAttackCycle = {150, 0, 0, 0, 0, 150, 0, 150, 150, 0, 150, 0, 0, 150, 0, 0, 0, 0, 150, 0, 0, 101, 0, 0, 150, 0, 150, 0, 150, 150, 0, 0, 300, 0, 300, 0, 0, 0, 0, 300, 150, 0, 200, 150, 300, 0, 101, 0};
// 植物攻击时机
const std::vector<const std::vector<int>> _plantShootTime = {{34}, {}, {}, {}, {}, {34}, {}, {25, 50}, {28}, {}, {49}, {}, {}, {25}, {}, {}, {}, {}, {34}, {}, {}, {}, {}, {}, {28}, {}, {34}, {}, {25, 50}, {39}, {}, {}, {31}, {}, {29}, {}, {}, {}, {}, {35}, {32, 49, 65, 82}, {}, {74, 102, 130, 158}, {31, 81}, {35}, {}, {}, {}};
// 僵尸血量临界点
const std::vector<int> _zombieHpCriticalPoint = {89, 89, 89, 165, 89, 89, 89, 89, 165, 89, 89, 89, 199, 89, 165, 165, 89, 89, 165, 449, 0, 165, 199, 0, 89, 0, 89, 89, 89, 89, 89, 89, 0};
// 土豆地雷准备就绪时长
const int _potatoMineReadyTime = 1500;
// 向日葵、双子向日葵、阳光菇和金盏花生产间隔
const int _sunFLowerProduceTime = 2500;
// 樱桃炸弹、寒冰菇、毁灭菇和火爆辣椒生效时长
const int _explodeTime = 100;
// 大嘴花咀嚼时长
const int _chomperChewTime = 4000;
// 磁力菇复用时长
const int _magnetShroomReadyTime = 1500;
// 玉米加农炮复用时长
const int _cobCannonLoadTime = 3475;
// 三叶草存活时长
const int _bloverLiveTime = 250;
// 墓碑吞噬者存活时长
const int _graveBuster = 400;
// 模仿者变身时长
const int _imitatorLiveTime = 320;
// 小丑僵尸最大开盒倒计时
const int _jackInTheBoxMaxOpenBoxTime = 2268;
// 蹦极僵尸停留时长
const int _bungeeRemainTime = 300;
// 蹦极僵尸抓取时长
const int _bungeeGrabTime = 63;
// 投篮僵尸投篮周期
const int _catapultShootCycle = 300;
// 投篮僵尸投篮时机
const int _catapultShootTime = 73;
// 巨人僵尸锤击周期
const int _gargantuarAttackCycle = 208;
// 巨人僵尸锤击生效时机
const int _gargantuarAttackTime = 134;
// 巨人僵尸投掷小鬼周期
const int _gargantuarThrowCycle = 142;
// 巨人僵尸投掷小鬼生效时机
const int _gargantuarThrowTime = 105;

#endif