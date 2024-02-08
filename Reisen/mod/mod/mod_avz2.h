#ifndef _REISEN_MOD_MOD_AVZ2_H
#define _REISEN_MOD_MOD_AVZ2_H

#include "avz.h"

namespace _ModInternal {
class ModEntry {
private:
    template <typename T>
    static std::vector<uint8_t> ToBytes(T x) {
        std::vector<uint8_t> result(sizeof(T));
        memcpy(result.data(), &x, sizeof(T));
        return result;
    }

public:
    std::vector<uintptr_t> addr;
    std::vector<uint8_t> data, orig;

    template <typename T>
    ModEntry(uintptr_t addr, T data, T orig)
        : addr{addr}, data(ToBytes(data)), orig(ToBytes(orig)) {}

    template <typename T>
    ModEntry(uintptr_t addr, T data)
        : addr{addr}, data(ToBytes(data)) {}

    ModEntry(uintptr_t addr, const std::vector<uint8_t>& data, const std::vector<uint8_t>& orig)
        : addr{addr}, data(data), orig(orig) {}

    ModEntry(uintptr_t addr, const std::vector<uint8_t>& data)
        : addr{addr}, data(data) {}

    template <typename T>
    ModEntry(const std::vector<uintptr_t>& addr, T data, T orig)
        : addr(addr), data(ToBytes(data)), orig(ToBytes(orig)) {}

    template <typename T>
    ModEntry(const std::vector<uintptr_t>& addr, T data)
        : addr(addr), data(ToBytes(data)) {}

    ModEntry(const std::vector<uintptr_t>& addr, const std::vector<uint8_t>& data, const std::vector<uint8_t>& orig)
        : addr(addr), data(data), orig(orig) {}

    ModEntry(const std::vector<uintptr_t>& addr, const std::vector<uint8_t>& data)
        : addr(addr), data(data) {}
};

enum class ModState {
    OFF,
    SCOPED_ON,
    GLOBAL_ON
};

class Mod : protected AStateHook {
protected:
    std::vector<ModEntry> entries;
    ModState state = ModState::OFF;

    void* ResolveAddress(const std::vector<uintptr_t>& addr) {
        uintptr_t result = 0;
        for(int i = 0; i + 1 < addr.size(); i++)
            result = *(uintptr_t*)(result + addr[i]);
        return (void*)(result + addr[addr.size() - 1]);
    }

    void _ExitFight() override {
        if(state == ModState::SCOPED_ON)
            operator()(ModState::OFF);
    }

public:
    Mod(const std::vector<ModEntry>& entries) : entries(entries) {}
    Mod(std::initializer_list<ModEntry> entries) : entries(entries) {}

    Mod(const Mod&) = delete;
    Mod& operator=(const Mod&) = delete;

    ~Mod() {
        if(state == ModState::SCOPED_ON)
            operator()(ModState::OFF);
    }

    void operator()(ModState state_) {
        ModState oldState = state;
        state = state_;
        if((state == ModState::OFF) == (oldState == ModState::OFF))
            return;
        auto data = (state == ModState::OFF) ? &ModEntry::orig : &ModEntry::data;
        for(const auto& entry : entries)
            if(!(entry.*data).empty())
                memcpy(ResolveAddress(entry.addr), (entry.*data).data(), (entry.*data).size());
    }

    ModState State() const {
        return state;
    }
};

template <ModState state>
void ApplyMod(Mod& mod) {
    mod(state);
}

template <ModState state>
void ApplyMod(Mod&& mod) {
    mod(state);
}

template <typename... Args>
void EnableModsScoped(Args&&... args) {
    (ApplyMod<ModState::SCOPED_ON>(std::forward<Args>(args)), ...);
}

template <typename... Args>
void EnableModsGlobal(Args&&... args) {
    (ApplyMod<ModState::GLOBAL_ON>(std::forward<Args>(args)), ...);
}

template <typename... Args>
void DisableMods(Args&&... args) {
    (ApplyMod<ModState::OFF>(std::forward<Args>(args)), ...);
}
};

using _ModInternal::Mod;
using _ModInternal::ModState;
using _ModInternal::EnableModsScoped;
using _ModInternal::EnableModsGlobal;
using _ModInternal::DisableMods;

void KillAllZombies() {
    for(auto& zombie : aAliveZombieFilter)
        zombie.State() = 3;
}
#endif
