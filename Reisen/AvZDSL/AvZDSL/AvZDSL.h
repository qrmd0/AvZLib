#ifndef _REISEN_AVZDSL_AVZDSL_H
#define _REISEN_AVZDSL_AVZDSL_H

#include "avz.h"

struct _ARelOpIR {
    int t;
    ARelOp o;

    operator ARelOp() const {
        return ARelOp(t, o);
    }

    _ARelOpIR operator-() const {
        return _ARelOpIR{-t, o};
    }
};

#if __cplusplus >= 202101L
template <typename... Ts>
ARelOp _RelOpSum(Ts... args) {
    return (... + static_cast<ARelOp>(args));
}
#else
ARelOp operator,(const ARelOp& lhs, const ARelOp& rhs) {
    return lhs + rhs;
}
#endif

class ARelTime {
private:
    int t;

public:
    ARelTime(int t_) : t(t_) {}

    explicit operator int() const {
        return t;
    }

#if __cplusplus >= 202101L
    template <typename... Ts>
    _ARelOpIR operator[](Ts... args) const {
        return _ARelOpIR{t, _RelOpSum(args...)};
    }
#else
    _ARelOpIR operator[](const ARelOp& x) const {
        return _ARelOpIR{t, x};
    }
#endif

    ARelTime operator-() const {
        return ARelTime(-t);
    }

    ARelTime operator+(const ARelTime rhs) const {
        return ARelTime(t + rhs.t);
    }

    ARelTime operator-(const ARelTime rhs) const{
        return ARelTime(t - rhs.t);
    }

    _ARelOpIR operator+(const _ARelOpIR& rhs) const {
        return _ARelOpIR{t + rhs.t, rhs.o};
    }

    _ARelOpIR operator-(const _ARelOpIR& rhs) const {
        return _ARelOpIR{t - rhs.t, rhs.o};
    }
};

ARelTime operator""_cs(unsigned long long x) { return x; }

class _ANowT {
private:
    int t;

public:
    _ANowT(int t_ = 0) : t(t_) {}

#if __cplusplus >= 202101L
    template <typename... Ts>
    void operator[](Ts... args) const {
        AConnect(ANowTime(), _RelOpSum(args...));
    }
#else
    void operator[](const ARelOp& x) const {
        AConnect(ANowTime(), x);
    }
#endif

    _ANowT operator+(const ARelTime rhs) const {
        return _ANowT(t + int(rhs));
    }

    _ANowT operator-(const ARelTime rhs) const {
        return _ANowT(t - int(rhs));
    }

    void operator+(const _ARelOpIR& rhs) const {
        AConnect(ANowTime(), rhs);
    }

    void operator-(const _ARelOpIR& rhs) const {
        operator+(-rhs);
    }
} ANow;

class _AWavelengthModifier {
private:
    std::vector<int> waves;

public:
    _AWavelengthModifier(const std::vector<int>& waves_) : waves(waves_) {}

    void AssumeWaveLength(int wl) {
        for(int w : waves)
            if(!ARangeIn(w, {9, 19, 20}))
                AAssumeWavelength({ATime(w, wl)});
    }

    void SetWaveLength(int wl) {
        for(int w : waves)
            if(!ARangeIn(w, {9, 19, 20}))
                ASetWavelength({ATime(w, wl)});
    }
};

class AWave {
private:
    std::vector<int> waves;
    int t;

public:
    AWave(const std::vector<int>& waves_, int t_ = 0) : waves(waves_), t(t_) {}

    template <typename... Ts>
    AWave(Ts... args) : waves({args...}), t(0) {}

#if __cplusplus >= 202101L
    template <typename... Ts>
    _AWavelengthModifier operator[](Ts... args) {
        for(int w : waves)
            AConnect(ATime(w, t), _RelOpSum(args...));
        return waves;
    }
#else
    _AWavelengthModifier operator[](const ARelOp& x) const {
        for(int w : waves)
            AConnect(ATime(w, t), x);
        return waves;
    }
#endif

    AWave operator+(const ARelTime rhs) const {
        return AWave(waves, t + int(rhs));
    }

    AWave operator-(const ARelTime rhs) const {
        return AWave(waves, t - int(rhs));
    }

    _AWavelengthModifier operator+(const _ARelOpIR& rhs) const {
        for(int w : waves)
            AConnect(ATime(w, t), rhs);
        return waves;
    }

    _AWavelengthModifier operator-(const _ARelOpIR& rhs) const {
        return operator+(-rhs);
    }
};

AWave operator""_wave(unsigned long long x) { return int(x); }

#define AMkRelOp(...) ARelOp([=]{ __VA_ARGS__; })
#endif
