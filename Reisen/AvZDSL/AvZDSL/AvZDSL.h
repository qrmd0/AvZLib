#ifndef _REISEN_AVZDSL_AVZDSL_H
#define _REISEN_AVZDSL_AVZDSL_H

#include "avz.h"

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
};

class WaveLengthModifier {
private:
    std::vector<int> waves;

public:
    WaveLengthModifier(const std::vector<int>& waves_) : waves(waves_) {}

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

#if __cplusplus >= 202101L
template <typename... Ts>
ARelOp RelOpSum(Ts... args) {
    return (... + static_cast<ARelOp>(args));
}
#endif
}; // namespace _ReisenAvZDSL

#if __cplusplus < 202101L
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
    [[nodiscard("ARelOp 需要绑定到时间才会执行")]]
    auto operator[](Ts... args) const {
        return _ReisenAvZDSL::ARelOpIR{t, _ReisenAvZDSL::RelOpSum(args...)};
    }
#else
    [[nodiscard("ARelOp 需要绑定到时间才会执行")]]
    auto operator[](const ARelOp& x) const {
        return _ReisenAvZDSL::ARelOpIR{t, x};
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

    [[nodiscard("ARelOp 需要绑定到时间才会执行")]]
    auto operator+(const _ReisenAvZDSL::ARelOpIR& rhs) const {
        return _ReisenAvZDSL::ARelOpIR{t + rhs.t, rhs.o};
    }

    [[nodiscard("ARelOp 需要绑定到时间才会执行")]]
    auto operator-(const _ReisenAvZDSL::ARelOpIR& rhs) const {
        return _ReisenAvZDSL::ARelOpIR{t - rhs.t, rhs.o};
    }
};

ARelTime operator""_cs(unsigned long long x) { return x; }

namespace _ReisenAvZDSL {
class ANowT {
private:
    int t;

public:
    ANowT(int t_ = 0) : t(t_) {}

#if __cplusplus >= 202101L
    template <typename... Ts>
    void operator[](Ts... args) const {
        AConnect(ANowDelayTime(t), _ReisenAvZDSL::RelOpSum(args...));
    }
#else
    void operator[](const ARelOp& x) const {
        AConnect(ANowDelayTime(t), x);
    }
#endif

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

public:
    AWave(const std::vector<int>& waves_, int t_ = 0) : waves(waves_), t(t_) {}

    template <typename... Ts>
    AWave(Ts... args) : waves({args...}), t(0) {}

#if __cplusplus >= 202101L
    template <typename... Ts>
    auto operator[](Ts... args) {
        for(int w : waves)
            AConnect(ATime(w, t), _ReisenAvZDSL::RelOpSum(args...));
        return _ReisenAvZDSL::WaveLengthModifier(waves);
    }
#else
    auto operator[](const ARelOp& x) const {
        for(int w : waves)
            AConnect(ATime(w, t), x);
        return _ReisenAvZDSL::WaveLengthModifier(waves);
    }
#endif

    AWave operator+(const ARelTime rhs) const {
        return AWave(waves, t + int(rhs));
    }

    AWave operator-(const ARelTime rhs) const {
        return AWave(waves, t - int(rhs));
    }

    auto operator+(const _ReisenAvZDSL::ARelOpIR& rhs) const {
        for(int w : waves)
            AConnect(ATime(w, t), rhs);
        return _ReisenAvZDSL::WaveLengthModifier(waves);
    }

    auto operator-(const _ReisenAvZDSL::ARelOpIR& rhs) const {
        return operator+(-rhs);
    }
};

AWave operator""_wave(unsigned long long x) { return int(x); }

#define AMkRelOp(...) ARelOp([=]{ __VA_ARGS__; })
#endif
