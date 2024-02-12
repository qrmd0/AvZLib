#pragma once

#include "error.h"
#include "global.h"
#include "libavz.h"
#include "util.h"

namespace _SimpleAvZInternal {

class WavesValidator : AvZ::GlobalVar {
public:
    std::vector<int> waves = {};

    void virtual beforeScript() override
    {
        waves = {};
    }

    void virtual afterScript() override
    {
        int count[21] = {};

        for (int w : waves) {
            count[w]++;
        }

        std::vector<int> duplicate_waves;
        for (int i = 1; i <= 20; i++) {
            if (count[i] > 1) {
                duplicate_waves.push_back(i);
            }
        }
        if (!duplicate_waves.empty()) {
            error("waves", "waves调用的波次不可重复.\n重复的波次: " + concat(duplicate_waves, ","));
        }
    }
};

WavesValidator waves_validator;

class Wave {
public:
    int value;
    Wave(int v)
        : value(v)
    {
    }
    
    operator int() const
    {
        return value;
    }
};

void init(Wave& wave)
{
    _SimpleAvZInternal::global.reset_last_effect_time();
    _SimpleAvZInternal::global.last_effect_wave = wave.value;
    AvZ::SetTime(3000, wave.value); // 如果使用AvZ原生IQ函数且不SetTime, 就会有报错提醒, 而非静默出错
}

class WaveIterator : public std::iterator<std::input_iterator_tag, Wave> {
    std::vector<Wave>::iterator it;

public:
    WaveIterator(std::vector<Wave>::iterator i)
        : it(i)
    {
    }
    Wave operator*()
    {
        init(*it);
        return *it;
    }
    WaveIterator& operator++()
    {
        ++it;
        return *this;
    }
    bool operator!=(const WaveIterator& other)
    {
        return it != other.it;
    }
};

class Waves {
    std::vector<Wave> waves;

public:
    Waves(std::vector<Wave> w)
        : waves(w)
    {
    }
    WaveIterator begin()
    {
        return WaveIterator(waves.begin());
    }
    WaveIterator end()
    {
        return WaveIterator(waves.end());
    }
    ~Waves()
    {
        global.reset_last_effect_wave();
    }
};

std::vector<Wave> wave_range_to_waves_vec(const std::array<int, 2>& wave_range, int step, const std::string& func_name)
{
    if (wave_range[0] > wave_range[1]) {
        _SimpleAvZInternal::error(func_name, "起始波数应≤终止波数\n起始波数: #\n终止波数: #", wave_range[0], wave_range[1]);
    }
    std::vector<_SimpleAvZInternal::Wave> waves_vec;
    for (int w = wave_range[0]; w <= wave_range[1]; w += step) {
        if (w < 1 || w > 20) {
            _SimpleAvZInternal::error(func_name, "波数应在1~20内\n当前为: #", w);
        }
        _SimpleAvZInternal::waves_validator.waves.push_back(w);
        waves_vec.push_back(
            _SimpleAvZInternal::Wave(w));
    }
    return waves_vec;
}

}

// 设定波次.
// 基础用法: for (auto w : waves(...)) {
//              // 具体操作
//          }
// *** 使用例:
// waves(1, 2, 3)-----------------w1, w2, w3(可指定任意多个)
// waves({1, 9}, 4)---------------w1-w9 每4波一循环
// waves({1, 9}, {11, 19}, 4)-----w1-w9, w11-w19 每4波一循环
template <typename... Args>
_SimpleAvZInternal::Waves waves(Args... args)
{
    std::vector<_SimpleAvZInternal::Wave> waves_vec = {_SimpleAvZInternal::Wave(args)...};
    for (const auto& w : waves_vec) {
        if (w.value < 1 || w.value > 20) {
            _SimpleAvZInternal::error("waves", "波数应在1~20内\n波数: #", w.value);
        }
        _SimpleAvZInternal::waves_validator.waves.push_back(w.value);
    }
    return _SimpleAvZInternal::Waves(waves_vec);
}

_SimpleAvZInternal::Waves waves(const std::array<int, 2>& wave_range_1, const std::array<int, 2>& wave_range_2, int step)
{
    if (step <= 0) {
        _SimpleAvZInternal::error("waves", "循环长度应>0\n循环长度: #", step);
    }

    auto waves_vec_1 = _SimpleAvZInternal::wave_range_to_waves_vec(wave_range_1, step, "waves");
    auto waves_vec_2 = _SimpleAvZInternal::wave_range_to_waves_vec(wave_range_2, step, "waves");
    waves_vec_1.insert(waves_vec_1.end(), waves_vec_2.begin(), waves_vec_2.end());
    return _SimpleAvZInternal::Waves(waves_vec_1);
}

_SimpleAvZInternal::Waves waves(const std::array<int, 2>& wave_range, int step)
{
    if (step <= 0) {
        _SimpleAvZInternal::error("waves", "循环长度应>0\n循环长度: #", step);
    }

    return _SimpleAvZInternal::Waves(_SimpleAvZInternal::wave_range_to_waves_vec(wave_range, step, "waves"));
}