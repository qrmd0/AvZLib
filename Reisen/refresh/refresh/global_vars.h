#pragma once

#include "task.h"

using namespace std;

vector<Task>::iterator cur_task;
int progress;
vector<int> type_list;
double wave_prob[21], hp_ratio[21];
map<int, int> left_count[21];
double result, sum;
int hist[41];
vector<tuple<double, int, string>> ranking;
