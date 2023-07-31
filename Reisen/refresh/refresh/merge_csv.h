#pragma once

#include <Windows.h>
#include "avz.h"
#include "csv2xlsx.exe.b85"
#include "global_vars.h"
#include "to_gbk.h"

using namespace std;

void extract_exe() {
    const uint8_t tmp[] = {0, 62, 0, 63, 64, 65, 66, 0, 67, 68, 69, 70, 0, 71, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 72, 73, 74, 75, 76, 77, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 0, 0, 0, 78, 79, 80, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 81, 82, 83, 84, 0};
    auto b85_decode_table = tmp - 32;
    int n = sizeof(csv2xlsx_exe_b85) - 1;
    auto output = new uint8_t[n * 4 / 5];
    for(int i = 0; i < n / 5; i++) {
        uint32_t v = b85_decode_table[csv2xlsx_exe_b85[i * 5 + 0]] * 52200625u +
                     b85_decode_table[csv2xlsx_exe_b85[i * 5 + 1]] * 614125u +
                     b85_decode_table[csv2xlsx_exe_b85[i * 5 + 2]] * 7225u +
                     b85_decode_table[csv2xlsx_exe_b85[i * 5 + 3]] * 85u +
                     b85_decode_table[csv2xlsx_exe_b85[i * 5 + 4]] * 1u;
        for(int j = i * 4 + 3; j >= i * 4; j--) {
            output[j] = v & 0xff;
            v >>= 8;
        }
    }
    string path = string(getenv("TEMP")) + "\\csv2xlsx.exe";
    auto fp = fopen(path.c_str(), "wb");
    fwrite(output, 1, n * 4 / 5, fp);
    fclose(fp);
    delete[] output;
}

void merge_csv() {
    string cmd = to_gbk("cmd /C %TEMP%\\csv2xlsx.exe -f 等线 --fontsize 11 -c , -s stats -s data -s raw -o");
    string prefix = cur_task->output_folder + "\\" + cur_task->prefix;
    for(auto s : {".xlsx", "-stats.csv", "-data.csv", "-raw.csv"})
        cmd += string(" \"") + prefix + s + '"';
    for(auto s : {"-stats.csv", "-data.csv", "-raw.csv"})
        cmd += string(" && del \"") + prefix + s + '"';
    WinExec(cmd.c_str(), SW_HIDE);
}
