#pragma once

#include "task.h"

using namespace std;

struct RefreshData {
    double wave_prob[21], hp_ratio[21];
    map<int, int> left_count[21];
    double result, sum;
    int hist[41];
    vector<tuple<double, int, string>> ranking;
};

vector<Task>::iterator cur_task;
int progress;
vector<int> type_list;
vector<RefreshData> refresh_data;
