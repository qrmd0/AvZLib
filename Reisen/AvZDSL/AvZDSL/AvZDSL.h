#ifndef _REISEN_AVZDSL_AVZDSL_H
#define _REISEN_AVZDSL_AVZDSL_H

#include "avz.h"
#include <regex>
#include <string>
#include <vector>

ARelOp operator,(const ARelOp& lhs, const ARelOp& rhs) {
    return lhs + rhs;
}

namespace _ReisenAvZDSL {
struct ARelOpIR {
    int t;
    ARelOp o;

    operator ARelOp() const {
        return ARelOp(t, o);
    }

    ARelOpIR operator-() const {
        return ARelOpIR{-t, o};
    }

    friend ARelOpIR operator*(const int n, const ARelOpIR& x) {
        return ARelOpIR{n * x.t, x.o};
    }
};
}; // namespace _ReisenAvZDSL

class ARelTime {
private:
    int t;

public:
    ARelTime(int t_) : t(t_) {}

    operator int() const {
        return t;
    }

    [[nodiscard("ARelOp 需要绑定到时间才会执行")]]
    auto operator[](const ARelOp& x) const {
        return _ReisenAvZDSL::ARelOpIR{t, x};
    }

    ARelTime operator-() const {
        return ARelTime(-t);
    }

    ARelTime operator+(const ARelTime rhs) const {
        return ARelTime(t + rhs.t);
    }

    ARelTime operator-(const ARelTime rhs) const {
        return ARelTime(t - rhs.t);
    }

    [[nodiscard("ARelOp 需要绑定到时间才会执行")]]
    auto operator+(const _ReisenAvZDSL::ARelOpIR& rhs) const {
        return _ReisenAvZDSL::ARelOpIR{t + rhs.t, rhs.o};
    }

    [[nodiscard("ARelOp 需要绑定到时间才会执行")]]
    auto operator-(const _ReisenAvZDSL::ARelOpIR& rhs) const {
        return _ReisenAvZDSL::ARelOpIR{t - rhs.t, rhs.o};
    }

    friend ARelTime operator*(const int lhs, const ARelTime rhs) {
        return ARelTime(lhs * rhs.t);
    }
};

ARelTime operator""_cs(unsigned long long x) { return x; }

namespace _ReisenAvZDSL {
class ANowT {
private:
    int t;

public:
    ANowT(int t_ = 0) : t(t_) {}

    ANowT operator()(int t_) {
        return ANowT(t + t_);
    }

    void operator[](const ARelOp& x) const {
        AConnect(ANowDelayTime(t), x);
    }

    ANowT operator+(const ARelTime rhs) const {
        return ANowT(t + int(rhs));
    }

    ANowT operator-(const ARelTime rhs) const {
        return ANowT(t - int(rhs));
    }

    void operator+(const ARelOpIR& rhs) const {
        AConnect(ANowDelayTime(t), rhs);
    }

    void operator-(const ARelOpIR& rhs) const {
        operator+(-rhs);
    }
};
}; // namespace _ReisenAvZDSL

_ReisenAvZDSL::ANowT ANow;

class AWave {
private:
    std::vector<int> waves;
    int t;

    static void ParseWaveImpl(std::vector<int>& out, int x) {
        out.push_back(x);
    }

    static void ParseWaveImplSingle(std::vector<int>& out, const std::string& str) {
        std::regex expr(R"((\d+)(?:-(\d+)(?:\+(\d+))?)?)");
        std::smatch result;
        if(!std::regex_match(str, result, expr)) {
            AGetInternalLogger()->Error("# 不是合法的波数范围", str);
            return;
        }
        int l = std::stoi(result[1]);
        int r = result[2].length() ? std::stoi(result[2]) : l;
        int inc = result[3].length() ? std::stoi(result[3]) : 1;
        for(int i = l; i <= r; i += inc)
            out.push_back(i);
    }

    static void ParseWaveImpl(std::vector<int>& out, const std::string& str) {
        int cur = 0, start = -1;
        while(cur < str.size()) {
            if(ARangeIn(str[cur], {',', ' '})) {
                if(start >= 0)
                    ParseWaveImplSingle(out, str.substr(start, cur - start));
                start = -1;
            } else if(start == -1)
                start = cur;
            cur++;
        }
        if(start >= 0)
            ParseWaveImplSingle(out, str.substr(start, str.size() - start));
    }

    static std::vector<int> ParseWave(auto... args) {
        std::vector<int> out;
        (ParseWaveImpl(out, args), ...);
        return out;
    }

public:
    AWave(const std::vector<int>& waves_, int t_ = 0) : waves(waves_), t(t_) {}

    AWave(auto... args) : waves(ParseWave(args...)), t(0) {}

    AWave operator[](const ARelOp& x) const {
        for(int w : waves)
            AConnect(ATime(w, t), x);
        return *this;
    }

    AWave operator+(const ARelTime rhs) const {
        return AWave(waves, t + int(rhs));
    }

    AWave operator-(const ARelTime rhs) const {
        return AWave(waves, t - int(rhs));
    }

    AWave operator+(const _ReisenAvZDSL::ARelOpIR& rhs) const {
        for(int w : waves)
            AConnect(ATime(w, t), rhs);
        return *this;
    }

    AWave operator-(const _ReisenAvZDSL::ARelOpIR& rhs) const {
        return operator+(-rhs);
    }

    void AssumeWaveLength(int wl) const {
        for(int w : waves)
            if(!ARangeIn(w, {9, 19, 20}))
                AAssumeWavelength({ATime(w, wl)});
    }

    void SetWaveLength(int wl) const {
        for(int w : waves)
            if(!ARangeIn(w, {9, 19, 20}))
                ASetWavelength({ATime(w, wl)});
    }
};

AWave operator""_wave(unsigned long long x) { return x; }
AWave operator""_wave(const char* x, size_t _) { return x; }

#define AMkRelOp(...) ARelOp([=]() mutable { __VA_ARGS__; })
#define RFunc [[nodiscard("ARelOp 需要绑定到时间才会执行")]] ARelOp
#endif
