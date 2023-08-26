#ifndef _REISEN_MOD_MOD_AVZ1_H
#define _REISEN_MOD_MOD_AVZ1_H

#include "avz.h"

namespace _ModInternal {
class ModEntry {
private:
    std::vector<uint8_t> U32ToBytes(uint32_t x) {
        std::vector<uint8_t> result(4);
        memcpy(result.data(), &x, 4);
        return result;
    }

public:
    std::vector<uintptr_t> addr;
    std::vector<uint8_t> data, orig;

    ModEntry(std::vector<uintptr_t> addr_, std::vector<uint8_t> data_, std::vector<uint8_t> orig_)
        : addr(std::move(addr_)), data(std::move(data_)), orig(std::move(orig_)) {}

    ModEntry(std::vector<uintptr_t> addr_, char data_, char orig_)
        : addr(std::move(addr_)), data{(unsigned char)data_}, orig{(unsigned char)orig_} {}

    ModEntry(std::vector<uintptr_t> addr_, uint32_t data_, uint32_t orig_)
        : addr(std::move(addr_)), data(U32ToBytes(data_)), orig(U32ToBytes(orig_)) {}

    ModEntry(uintptr_t addr_, std::vector<uint8_t> data_, std::vector<uint8_t> orig_)
        : addr{addr_}, data(std::move(data_)), orig(std::move(orig_)) {}

    ModEntry(uintptr_t addr_, char data_, char orig_)
        : addr{addr_}, data{(unsigned char)data_}, orig{(unsigned char)orig_} {}

    ModEntry(uintptr_t addr_, uint32_t data_, uint32_t orig_)
        : addr{addr_}, data(U32ToBytes(data_)), orig(U32ToBytes(orig_)) {}
};

enum class ModState {
    OFF,
    SCOPED_ON,
    GLOBAL_ON
};

class Mod : protected AvZ::GlobalVar {
protected:
    std::vector<ModEntry> entries;
    ModState state = ModState::OFF;

    void* ResolveAddress(const std::vector<uintptr_t>& addr) {
        uintptr_t result = 0;
        for(int i = 0; i + 1 < addr.size(); i++)
            result = *(uintptr_t*)(result + addr[i]);
        return (void*)(result + addr[addr.size() - 1]);
    }

    void exitFight() override {
        if(state == ModState::SCOPED_ON)
            operator()(ModState::OFF);
    }

public:
    Mod(std::vector<ModEntry> entries_) : entries(std::move(entries_)) {}
    Mod(std::initializer_list<ModEntry> entries_) : entries(entries_) {}

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

template <typename T>
void EnableModsScoped(T&& arg) {
    ApplyMod<ModState::SCOPED_ON>(std::forward<T>(arg));
}

template <typename T, typename... Args>
void EnableModsScoped(T&& arg, Args&&... args) {
    EnableModsScoped(arg);
    EnableModsScoped(args...);
}

template <typename T>
void EnableModsGlobal(T&& arg) {
    ApplyMod<ModState::GLOBAL_ON>(std::forward<T>(arg));
}

template <typename T, typename... Args>
void EnableModsGlobal(T&& arg, Args&&... args) {
    EnableModsGlobal(arg);
    EnableModsGlobal(args...);
}

template <typename T>
void DisableMods(T&& arg) {
    ApplyMod<ModState::OFF>(std::forward<T>(arg));
}

template <typename T, typename... Args>
void DisableMods(T&& arg, Args&&... args) {
    DisableMods(arg);
    DisableMods(args...);
}
};

using _ModInternal::Mod;
using _ModInternal::ModState;
using _ModInternal::EnableModsScoped;
using _ModInternal::EnableModsGlobal;
using _ModInternal::DisableMods;

void KillAllZombies() {
    for(auto& zombie : AvZ::alive_zombie_filter)
        zombie.state() = 3;
}
#endif
