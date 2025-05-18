// Asm Hook 插件
// 使用此插件可以在任意函数前调用自定义的代码
// 可以用来实现很多奇奇怪怪的东西
// 本插件共提供两个函数：AInstallSharedHook 和 AInstallUniqueHook
// 使用方法见这两个函数的注释

#ifndef __ASM_INSERT_CODE_H__
#define __ASM_INSERT_CODE_H__

#define __ASM_INSERT_CODE_VESION__ 250518

#include <avz.h>
#include <minhook/MinHook.h>

struct AAsmCodeContext {
    // 用户自定义数据，一般这个值是个指针，指向一个结构体
    // 对于 UniqueHook, 此值为 AInstallUniqueHook 函数参数中的 userData
    // 对于 SharedHook, 此值被内部实现占用，用户不应访问该值
    // 因为 SharedHook 可以使用 lambda 函数，这样的函数可以通过捕获传参
    DWORD userData;

    // 寄存器信息
    DWORD efl;
    DWORD edi;
    DWORD esi;
    DWORD ebp;
    DWORD esp;
    DWORD ebx;
    DWORD edx;
    DWORD ecx;
    DWORD eax;
    DWORD eip;
};

using AUniqueCodeFunc = void(__stdcall*)(AAsmCodeContext*);
using ASharedCodeFunc = std::function<void(AAsmCodeContext*)>;

class __AAsmCode {
    __ADeleteCopyAndMove(__AAsmCode);

public:
#pragma pack(push, 1)
    struct InsertCodeAsm {
        uint8_t push0 = 0x68;               // push retAddr
        uintptr_t retAddr = 0x0;            //
        uint8_t pushad = 0x60;              // pushad
        uint8_t pushfd = 0x9c;              // pushfd
        uint8_t addEspAddC4[5] =            // add [esp+10], 4
            {0x83, 0x44, 0x24, 0x10, 0x04}; //
        uint8_t push1 = 0x68;               // push userData
        uintptr_t userData = 0x0;           //
        uint8_t pushEsp = 0x54;             // push esp
        uint8_t call = 0xe8;                // call callAddr
        uintptr_t callAddr = 0x0;           //
        uint8_t popEax = 0x58;              // pop eax  balance push userData
        uint8_t popfd = 0x9d;               // popfd
        uint8_t popad = 0x61;               // popad
        uint8_t ret = 0xc3;                 // ret
    };
#pragma pack(pop)

    __AAsmCode(HANDLE codeHandle, uintptr_t address, AUniqueCodeFunc func, uintptr_t userData) {
        if (!codeHandle || address == 0) {
            return;
        }
        _codeHandle = codeHandle;
        _heapPtr = HeapAlloc(codeHandle, 0, sizeof(InsertCodeAsm));
        if (!_heapPtr) {
            return;
        }
        auto ica = new (_heapPtr) InsertCodeAsm;
        // call 指令的机器码的实现是 相对于call后面的那条指令的地址
        // 因此这里需要 - (uintptr_t)&ica->popEax;
        ica->callAddr = (uintptr_t)func - (uintptr_t)&ica->popEax;
        ica->userData = userData;
        void* trampolineFunc = nullptr;
        if (MH_CreateHook((void*)address, ica, &trampolineFunc) != MH_OK) {
            return;
        }
        if (MH_EnableHook((void*)address) != MH_OK) {
            return;
        }
        ica->retAddr = (uint32_t)trampolineFunc;
        _address = address;
    }
    ~__AAsmCode() {
        if (_address && MH_RemoveHook((void*)_address) != MH_OK) {
            AGetInternalLogger()->Warning("MH_DisableHook Failed!, hook address: {}", _address);
        }

        if (_heapPtr) {
            ((InsertCodeAsm*)_heapPtr)->~InsertCodeAsm();
            HeapFree(_codeHandle, 0, _heapPtr);
        }
    }

    bool IsOk() {
        return _address != 0;
    }

protected:
    uintptr_t _address = 0;
    void* _heapPtr = 0;
    HANDLE _codeHandle = 0;
};

class __AAsmCodeManager : public AOrderedBeforeScriptHook<-1>,
                          public AOrderedExitFightHook<-1> {
public:
    struct CodeInfo {
        // asm hook 结构体
        // 当使用 InsertUniqueCode 时，插入的代码就在此对象中
        // 当使用 InsertSharedCode 时，真正插入的代码会放在下面的 pool 中
        std::shared_ptr<__AAsmCode> asmCode;

        // 当使用 InsertSharedCode 时，真正插入的代码会放在这个 pool 中
        // 使用这个结构体的原因是，此结构体插入和删除都很快
        // 并且能够在遍历的过程中删除和添加元素
        // 缺点是如果删除了大量元素，此池会增加很多无效遍历
        // 但是这种情况很少
        __AObjectPool<ASharedCodeFunc> codePool;

        // 用于区分插入的类型
        bool isUnique = false;
    };

    using CodeMap = std::map<uintptr_t, CodeInfo>;

    struct RetInfo {
        CodeMap::iterator iter;
        std::size_t idx;
    };

    std::optional<RetInfo> InsertUniqueCode(
        uintptr_t address, AUniqueCodeFunc codeFunc, uintptr_t userData) {
        if (_codeMap.contains(address)) {
            AGetInternalLogger()->Warning("已存在地址为 {} 的代码插入", address);
            return std::nullopt;
        }
        std::shared_ptr<__AAsmCode> ptr
            = std::make_shared<__AAsmCode>(_codeHeap, address, codeFunc, userData);
        if (!(ptr->IsOk())) {
            AGetInternalLogger()->Warning("地址为 {} 的唯一代码插入未成功", address);
            return std::nullopt;
        }
        auto res = _codeMap.emplace(address, CodeInfo());
        if (!res.second) {
            AGetInternalLogger()->Warning("地址为 {} 的唯一代码插入未成功", address);
            return std::nullopt;
        }
        auto&& codeInfo = res.first->second;
        codeInfo.asmCode = ptr;
        codeInfo.isUnique = true;
        return RetInfo {
            .iter = res.first,
        };
    }

    template <typename CodeFunc>
    std::optional<RetInfo> InsertSharedCode(uintptr_t address, CodeFunc&& codeFunc) {
        auto iter = _codeMap.find(address);
        if (iter != _codeMap.end()) {
            if (iter->second.isUnique == true) {
                // 如果此地址已经被唯一 code 插入了
                // 则无法插入新代码
                AGetInternalLogger()->Warning(
                    "地址为 {} 的共享代码插入未成功，原因是此地址已被唯一代码插入", address);
                return std::nullopt;
            }
        } else {
            // 共享 Hook 第一次设置的时候需要申请 MinHook
            auto res = _codeMap.emplace(address, CodeInfo());
            if (!res.second) {
                AGetInternalLogger()->Warning("地址为 {} 的 共享代码插入未成功", address);
                return std::nullopt;
            }
            iter = res.first;
            auto&& codeInfo = iter->second;
            codeInfo.asmCode = std::make_shared<__AAsmCode>(
                _codeHeap, address, __ACodeCallback, (uintptr_t)&codeInfo.codePool);

            if (!(codeInfo.asmCode->IsOk())) {
                AGetInternalLogger()->Warning("地址为 {} 的 共享代码插入未成功", address);
                return std::nullopt;
            }
            codeInfo.isUnique = false;
        }

        auto&& objInfo = iter->second.codePool.Insert(std::forward<CodeFunc>(codeFunc));
        return RetInfo {
            .iter = iter,
            .idx = objInfo.idx,
        };
    }

    void RemoveHook(const RetInfo& retInfo) {
        // 如果是唯一插入直接删除节点即可
        if (retInfo.iter->second.isUnique) {
            _codeMap.erase(retInfo.iter);
            return;
        }
        // 共享插入需要先删除当前插入
        auto&& pool = retInfo.iter->second.codePool;
        pool.Remove(retInfo.idx);

        // 检查池中是否存在存活的 插入
        for (std::size_t idx = 0; idx < pool.Size(); ++idx) {
            if (pool.IsAlive(idx)) {
                return;
            }
        }

        // 如果没有存活的插入, 直接删除节点
        _codeMap.erase(retInfo.iter);
    }

protected:
    CodeMap _codeMap;
    HANDLE _codeHeap = nullptr;

    virtual void _BeforeScript() override {
        _codeHeap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 0, 0);
        if (_codeHeap == nullptr) {
            return;
        }
        // Initialize MinHook.
        if (MH_Initialize() != MH_OK) {
            return;
        }
    }
    virtual void _ExitFight() override {
        _codeMap.clear();
        // Uninitialize MinHook.
        if (MH_Uninitialize() != MH_OK) {
            return;
        }
        if (_codeHeap) {
            HeapDestroy(_codeHeap);
        }
    }

    static void __stdcall __ACodeCallback(AAsmCodeContext* context) {
        __AObjectPool<ASharedCodeFunc>* codePool = ((__AObjectPool<ASharedCodeFunc>*)context->userData);
        for (std::size_t idx = 0; idx < codePool->Size(); ++idx) {
            if (codePool->IsAlive(idx)) {
                (*codePool)[idx](context);
            }
        }
    }
};

inline __AAsmCodeManager __aAsmCodeManager;

class AAsmCodeHandle {
public:
    AAsmCodeHandle(std::optional<__AAsmCodeManager::RetInfo> retInfo)
        : _retInfo(retInfo) {
    }
    void Remove() {
        if (_retInfo) {
            __aAsmCodeManager.RemoveHook(_retInfo.value());
            _retInfo = std::nullopt;
        }
    }

protected:
    std::optional<__AAsmCodeManager::RetInfo> _retInfo;
};

// 插入共享代码
// 共享代码可以在一个地址插入多次
// 使用更加灵活，但是性能较低，因为其每次运行需要使用哈希查表
// 但是唯一插入不需要
// 所以如果一个插入代码每帧存在大量调用，请使用 AInsertUniqueAsmCode
// 使用示例：按键监控功能
// 41B820 Board::KeyDown(KeyCode theKey, ecx = Board* this)
// "关卡界面键盘按键按下，进行游戏关卡内的按键检测。关卡内的所有按键（指令、暂停键等）均在此处检测。
// AInsertSharedAsmCode(0x41B820, [](AAsmCodeContext* context) {
//     // 根据函数原型，只要是参数没有指定寄存器
//     // 那就是使用栈来传递的参数
//     // 需要使用 esp 加偏移量获取真正的参数值
//     logger.Info("你按下按键的代码为: {}", AMRef<int>(context->esp + 4));
// });
// 当不需要使用此代码插入时
// 请使用下面的代码
// auto handle = AInsertSharedAsmCode(...);
// handle.Remove();
template <typename Callback>
AAsmCodeHandle AInsertSharedAsmCode(uintptr_t address, Callback&& callback) {
    return __aAsmCodeManager.InsertSharedCode(address, std::forward<Callback>(callback));
}

// 插入唯一代码
// 唯一代码的类型只能为 void(__stdcall*)(AAsmCodeContext*)
// 并且一个地址只能绑定一个唯一代码插入
// 此方法优点是运行效率高
// 缺点是不灵活
// 使用示例：僵尸受到伤害时秒杀
// 5317C0 Zombie::TakeDamage(eax = unsigned int theDamageFlags, int theDamage, esi = Zombie* this)
// 令僵尸受伤，会根据僵尸状态及防具情况结算伤害，同时处理僵尸受伤的相关事件。
// AInsertUniqueAsmCode(
//     0x5317C0, [](AAsmCodeContext * context) __stdcall {
//         AZombie* zombie = (AZombie*)context->esi;
//         zombie->Hp() = 0;
//         zombie->OneHp() = 0;
//         zombie->TwoHp() = 0;
//     });
// 当不需要使用此代码插入时
// 请使用下面的代码
// auto handle = AInsertUniqueAsmCode(...);
// handle.Remove();
inline AAsmCodeHandle AInsertUniqueAsmCode(
    uintptr_t address, AUniqueCodeFunc callback, uintptr_t userData = 0) {
    return __aAsmCodeManager.InsertUniqueCode(address, callback, userData);
}

#endif