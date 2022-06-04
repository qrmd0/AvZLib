/*
 * @Author: crescendo
 * @Date: 2022-05-28 08:45:31
 * @Last Modified by: crescendo
 * @Last Modified time: 2022-05-29 17:30:26
 *  __AVZ_VERSION__ == 220213
 * AvZ 日志输出工具
 */

/*使用示例：
AZL::Logger Log("C:\MyFolder");
void Script() {
    Log.log("hello, #", "world");
    SetTime(0, 1);
    Log.logInQueue("time = #, wave = #", 0, 1);
}
*/

#pragma once
#include "libavz.h"
#include <fstream>

#ifndef __AZL_VERSION__
#define __AZL_VERSION__ 220601
#endif

namespace AZL
{
    void Utf8ToGbk(std::string &strUTF8)
    {
        int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
        wchar_t *wszGBK = new wchar_t[len + 1];
        memset(wszGBK, 0, len * 2 + 2);
        MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, wszGBK, len);
        len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
        char *szGBK = new char[len + 1];
        memset(szGBK, 0, len + 1);
        WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
        strUTF8 = szGBK;
        if (wszGBK)
        {
            delete[] wszGBK;
        }
        if (szGBK)
        {
            delete[] szGBK;
        }
    }

    void getCurrentTime(char *buffer, const char *format)
    {
        time_t t = time(0);
        struct tm *now = localtime(&t);
        strftime(buffer, 80, format, now);
    }

    class Logger
    {
    protected:
        std::string output_path = "C:\\Software\\AsmVsZombies\\AZL_output\\"; // default

    public:
        // 默认存储在桌面
        Logger()
        {
            char *buf = std::getenv("USERPROFILE");
            if (buf)
            {
                output_path = buf;
                output_path += "\\Desktop\\AZL_output\\";
                free(buf);
            }
            init();
        }

        // 指定存储路径
        Logger(const std::string &p)
        {
            output_path = p;
            std::replace(output_path.begin(), output_path.end(), '/', '\\');
            if (p.back() != '\\')
                output_path += "\\";
            output_path += "AZL_output\\";
            init();
        }

        // ***Not In Queue
        // 输出调试信息，同AvZ::ShowErrorNotInQueue
        template <typename... Args>
        void log(const std::string &content = "", Args... args)
        {
            std::ofstream ofile;
            ofile.open(output_path, std::ofstream::out | std::ofstream::app);
            if (!ofile.is_open())
            {
                AvZ::ShowErrorNotInQueue("AVZ Logger出错：打开输出文件失败\n输出文件路径：#", output_path);
            }
            else
            {
                char buffer[80];
                getCurrentTime(buffer, "%H:%M:%S - ");
                std::string _content = buffer + content;
                std::initializer_list<int>{(AvZ::string_convert(_content, args), 0)...};
                _content += "\n\n";
                Utf8ToGbk(_content);
                ofile << _content;
            }
        }

        template <typename... Args>
        void logInQueue(const std::string &content = "", Args... args)
        {
            AvZ::InsertOperation([=]()
                                 { log(content, args...); },
                                 "Logger:logInQueue");
        }

    protected:
        void init()
        {
            if (!CreateDirectoryA(output_path.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
            {
                return;
            }
            char buffer[80];
            getCurrentTime(buffer, "Log(%Y-%m-%d_%H.%M.%S).txt");
            output_path += buffer;
            std::ofstream ofile;
            ofile.open(output_path, std::ofstream::out | std::ofstream::trunc);
            if (ofile.is_open())
            {
                char greeting[80];
                getCurrentTime(greeting, "%H.%M.%S - 打开输出文件成功\n\n");
                std::string greeting_str(greeting);
                Utf8ToGbk(greeting_str);
                ofile << greeting_str;
            }
        }
    };

    class BufferedLogger : public Logger
    {
    public:
        BufferedLogger() : Logger()
        {
        }

        BufferedLogger(const std::string &p) : Logger(p)
        {
        }

        template <typename... Args>
        void log(const std::string &content = "", Args... args)
        {
            if (!ofile.is_open())
                ofile.open(output_path, std::ofstream::out | std::ofstream::app);
            if (!ofile.is_open())
            {
                AvZ::ShowErrorNotInQueue("AVZ Logger出错：打开输出文件失败\n输出文件路径：#", output_path);
            }
            else
            {
                char buffer[80];
                getCurrentTime(buffer, "%H:%M:%S - ");
                std::string _content = buffer + content;
                std::initializer_list<int>{(AvZ::string_convert(_content, args), 0)...};
                _content += "\n\n";
                Utf8ToGbk(_content);
                ofile << _content;
            }
        }

        template <typename... Args>
        void logInQueue(const std::string &content = "", Args... args)
        {
            AvZ::InsertOperation([=]()
                                 { this->log(content, args...); },
                                 "BufferedLogger:logInQueue");
        }

        void flush(int wave = -1, int time = 0)
        {
            if (wave == -1)
            {
                ofile.flush();
            }
            else
            {
                if (wave <= 0 || wave > 20)
                    wave = 20;
                if (time < 0)
                    time = 0;
                AvZ::InsertTimeOperation(
                    time, wave, [=]()
                    { ofile.flush(); },
                    "BufferedLogger::flush");
            }
        }

    protected:
        std::ofstream ofile;
    };
} // namespace AZL
