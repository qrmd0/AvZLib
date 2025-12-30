//#pragma once
#ifndef __DAMAGE_RECORDER_H__
#define __DAMAGE_RECORDER_H__

#include "asm_insert_code/asm_insert_code.h"


class BasicInfo{
public:
    struct ZombieInfo{
        uint32_t id;
        int type;
        int wave;
        int row;
        int xint;
        int hp;

        ZombieInfo(): id(0), type(-1), wave(-1), row(-1), xint(-1), hp(-1) {}
        ZombieInfo(AZombie* zombie){
            id = zombie->Id();
            type = zombie->Type();
            wave = zombie->AtWave();
            row = zombie->Row();
            xint = zombie->MRef<int>(0x8);
            hp = zombie->Hp() + zombie->OneHp() + zombie->TwoHp();
        }
    };

    struct PlantInfo{
        uint32_t id;
        int type;
        int row;
        int col;

        PlantInfo(): id(0), type(-1), row(-1), col(-1) {}
        PlantInfo(APlant* plant){
            id = plant->Id();
            type = plant->Type();
            row = plant->Row();
            col = plant->Col();
        }
    };

    struct ProjectileInfo{
        uint32_t id;
        int time_exist;

        ProjectileInfo(): id(0), time_exist(-1) {}
        ProjectileInfo(AProjectile* projectile){id = projectile->Id();}
    };

    ZombieInfo z;
    PlantInfo p;
    ProjectileInfo proj;
    BasicInfo(): z(), p(), proj() {}

    void GetZombieInfo(AZombie* zombie){z = ZombieInfo(zombie);}
    void GetPlantInfo(APlant* plant){p = PlantInfo(plant);}
    void GetProjectileInfo(AProjectile* projectile){proj = ProjectileInfo(projectile);}
    void GetProjectileExistTime(AProjectile* projectile){
        proj.time_exist = projectile->ExistTime();
    }
};

class DamageMark : public BasicInfo{
public:
    enum Source{
        DEFAULT,
        AREA,
        PROJECTILE,
        ICE,
    };
    Source source;
    DamageMark(): source(DEFAULT) {}
};

class ProjectileMark : public BasicInfo{
public:
    bool valid;
    ProjectileMark(): valid(false) {}
};

class DamageInfo : public DamageMark {
public:
    int damage;
    ATime time;
    int time_clock; 
    DamageInfo():damage(-1), time(), time_clock(-1) {}
};


class DamageRecorder : public AStateHook {
protected:
    char damage_record_path[1000] = "C:\\ProgramData\\PopCap Games\\PlantsVsZombies";
    ATickRunner trace_record_runner;
    static DamageMark damage_mark;
    static ProjectileMark projectile_mark;
    static std::vector<DamageInfo> damage_records;
    static std::vector<DamageInfo> projectile_records;


    static void trace_record(){
        // 回溯弹出记录
        ATime now_time = ANowTime();
        while(!damage_records.empty()){
            auto& dmg = damage_records.back();
            if( now_time.wave > dmg.time.wave 
            ||  (now_time.wave == dmg.time.wave && now_time.time >= dmg.time.time) )
                break;
            damage_records.pop_back();
        }
        while(!projectile_records.empty()){
            auto& dmg = projectile_records.back();
            if( now_time.wave > dmg.time.wave 
            ||  (now_time.wave == dmg.time.wave && now_time.time >= dmg.time.time) )
                break;
            projectile_records.pop_back();
        }
    }

    virtual void _EnterFight() override {
        damage_records.clear();
        projectile_records.clear();
        damage_mark = DamageMark();
        projectile_mark = ProjectileMark();
        trace_record_runner.Start(trace_record, ATickRunner::GLOBAL);
    }

    virtual void _ExitFight() override {
        for (auto& dmg : damage_records){
            // 合并子弹表到伤害表
            if (dmg.proj.id != 0){
                // 反向遍历子弹表
                for(int i=projectile_records.size()-1;i>=0;i--){
                    auto& record = projectile_records[i];
                    if (dmg.proj.id == record.proj.id           // 匹配子弹ID
                    // &&  dmg.time_clock >= record.time_clock     // 伤害发生在生成子弹之后
                    &&  record.time_clock + dmg.proj.time_exist == dmg.time_clock + 1    // 匹配时间
                    ) {
                        dmg.p = record.p;
                        break;
                    }
                }
            }
            // 处理冰冻伤害
            if (dmg.source == DamageInfo::ICE)
                dmg.p.type = int(AHBG_14);
        }

        // 写入文件

        // 获取当前时间字符串
        auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::stringstream ss;
        ss << std::put_time(std::localtime(&t), "%Y-%m-%d-%H-%M-%S");
        auto cur_time_str = ss.str();

        // 构造文件路径
        std::string file_path = damage_record_path + std::string("/") + "Records-" + cur_time_str + ".csv";

        // 打开文件
        std::ofstream ofs(file_path);

        // 僵尸名字
        std::map<AZombieType, std::string> zombie_type_names = {
            {APJ_0, "普僵"},
            {AQZ_1, "旗帜"},
            {ALZ_2, "路障"},
            {ACG_3, "撑杆"},
            {ATT_4, "铁桶"},
            {ADB_5, "读报"},
            {ATM_6, "铁门"},
            {AGL_7, "橄榄"},
            {AWW_8, "舞王"},
            {ABW_9, "伴舞"},
            {AYZ_10, "鸭子"},
            {AQS_11, "潜水"},
            {ABC_12, "冰车"},
            {AXQ_13, "雪橇"},
            {AHT_14, "海豚"},
            {AXC_15, "小丑"},
            {AQQ_16, "气球"},
            {AKG_17, "矿工"},
            {ATT_18, "跳跳"},
            {AXR_19, "雪人"},
            {ABJ_20, "蹦极"},
            {AFT_21, "扶梯"},
            {ATL_22, "投篮"},
            {ABY_23, "白眼"},
            {AXG_24, "小鬼"},
            {AJB_25, "僵博"},
            {AHY_32, "红眼"},
        };

        // 植物名字
        std::map<APlantType, std::string> plant_type_names = {
            {AWDSS_0, "豌豆射手"},
            {AXRK_1, "向日葵"},
            {AYTZD_2, "樱桃炸弹"},
            {AJG_3, "坚果"},
            {ATDDL_4, "土豆地雷"},
            {AHBSS_5, "寒冰射手"},
            {ADZH_6, "大嘴花"},
            {ASCSS_7, "双重射手"},
            {AXPG_8, "小喷菇"},
            {AYGG_9, "阳光菇"},
            {ADPG_10, "大喷菇"},    
            {AMBTSZ_11, "墓碑吞噬者"},
            {AMHG_12, "魅惑菇"},
            {ADXG_13, "胆小菇"},
            {AHBG_14, "寒冰菇"},
            {AHMG_15, "毁灭菇"},
            {AHY_16, "荷叶"},
            {AWG_17, "倭瓜"},
            {ASFSS_18, "三发射手"},
            {ACRHZ_19, "缠绕海藻"},
            {AHBLJ_20, "火爆辣椒"},
            {ADC_21, "地刺"},
            {AHJSZ_22, "火炬树桩"},
            {AGJG_23, "高坚果"},
            {ASBG_24, "水兵菇"},
            {ALDH_25, "路灯花"},
            {AXRZ_26, "仙人掌"},
            {ASYC_27, "三叶草"},
            {ALJSS_28, "裂荚射手"},
            {AYT_29, "杨桃"},
            {ANGT_30, "南瓜头"},
            {ACLG_31, "磁力菇"},
            {AJXCTS_32, "卷心菜投手"},
            {AHP_33, "花盆"},
            {AYMTS_34, "玉米投手"},
            {AKFD_35, "咖啡豆"},
            {ADS_36, "大蒜"},
            {AYZBHS_37, "叶子保护伞"},
            {AJZH_38, "金盏花"},
            {AXGTS_39, "西瓜投手"},
            {AJQSS_40, "机枪射手"},
            {ASZXRK_41, "双子向日葵"},
            {AYYG_42, "忧郁菇"},
            {AXP_43, "香蒲"},
            {ABXGTS_44, "冰西瓜投手"},
            {AXJC_45, "吸金磁"},
            {ADCW_46, "地刺王"},
            {AYMJNP_47, "玉米加农炮"},
        };

        // 来源名字
        std::map<DamageInfo::Source, std::string> source_names = {
            {DamageInfo::DEFAULT, "未知"},
            {DamageInfo::AREA, "范围"},
            {DamageInfo::PROJECTILE, "子弹"},
            {DamageInfo::ICE, "寒冰菇"},
        };

        // 写入表头
        ofs << "波次,时间,伤害,"
            << "来源,"
            << "僵尸ID,僵尸类型,僵尸路,僵尸坐标,僵尸波次,僵尸总血量,"
            << "子弹ID,"
            << "植物ID,植物类型,植物路,植物列\n";

        // 写入数据
        for (const auto& dmg : damage_records){
            ofs << dmg.time.wave << ","
                << dmg.time.time << ","
                << dmg.damage << ",";

            auto it = source_names.find(dmg.source);
            if (it != source_names.end())   ofs << it->second;
            ofs << ",";
            // 僵尸信息
            ofs << dmg.z.id << ",";
            auto zt_it = zombie_type_names.find(static_cast<AZombieType>(dmg.z.type));
            if (zt_it != zombie_type_names.end())    ofs << zt_it->second;
            ofs << ",";
            ofs << dmg.z.row+1 << ","
                << dmg.z.xint << ","
                << dmg.z.wave+1 << ","
                << dmg.z.hp << ",";
            // 子弹信息
            ofs << dmg.proj.id << ",";
            // 植物信息
            ofs << dmg.p.id << ",";
            auto pt_it = plant_type_names.find(static_cast<APlantType>(dmg.p.type));
            if (pt_it != plant_type_names.end())    ofs << pt_it->second;
            ofs << ",";
            ofs << dmg.p.row+1 << ","
                << dmg.p.col+1 << "\n";   
        }
        ofs.close();

        damage_records.clear();
        projectile_records.clear();
    }

    void record_damage(){
        // 伤害表
        AInsertUniqueAsmCode(
            // void Zombie::TakeDamage(int theDamage, unsigned int theDamageFlags)
            0x5317C0, [](AAsmCodeContext * context) __stdcall {
                auto dmg = AMRef<int>(context->esp + 4);
                AZombie* zombie = (AZombie*)context->esi;

                if (dmg >= 1800)    return;
                if (damage_mark.source == DamageMark::DEFAULT)  return;

                // 回溯
                trace_record();

                DamageInfo dmg_info;
                // 记录来源
                dmg_info.source = damage_mark.source;
                // 记录伤害与时间
                dmg_info.damage = dmg;
                dmg_info.time = ANowTime();
                dmg_info.time_clock = AGetMainObject()->GameClock();
                // 记录僵尸信息
                dmg_info.GetZombieInfo(zombie);

                if (damage_mark.source == DamageMark::AREA) 
                    // 范围伤害，记录植物信息
                    dmg_info.p = damage_mark.p;
                else if (damage_mark.source == DamageMark::PROJECTILE) 
                    // 子弹伤害，记录子弹信息
                    dmg_info.proj = damage_mark.proj;

                damage_records.push_back(dmg_info);
        });
    }
    void mark_valid_damage(){
        // 有效case
        // 范围伤害 喷/曾/地刺/地刺王 车底刺先结算死亡再结算伤害因此会没有植物信息
        AInsertUniqueAsmCode(
            // void Plant::DoRowAreaDamage(int theDamage, unsigned int theDamageFlags)
            0x45ED00,[](AAsmCodeContext * context) __stdcall {
                // esp + ?
                APlant* plant = (APlant*)context->edi;

                damage_mark.source = DamageMark::AREA;
                damage_mark.GetPlantInfo(plant);
        });

        // 溅射伤害
        AInsertUniqueAsmCode(
            // void Projectile::DoSplashDamage(Zombie* theZombie)
            0x46D390,[](AAsmCodeContext * context) __stdcall {
                AProjectile* projectile = (AProjectile*)context->eax;

                damage_mark.source = DamageMark::PROJECTILE;
                damage_mark.GetProjectileInfo(projectile);
                damage_mark.GetProjectileExistTime(projectile);
        });

        // 直接子弹伤害
        AInsertUniqueAsmCode(
            // void Projectile::DoImpact(Zombie* theZombie)
            0x46E000,[](AAsmCodeContext * context) __stdcall {
                AProjectile* projectile = (AProjectile*)context->ecx;

                damage_mark.source = DamageMark::PROJECTILE;
                damage_mark.GetProjectileInfo(projectile);
                damage_mark.GetProjectileExistTime(projectile);
        });

        // 点冰
        AInsertUniqueAsmCode(
            // void Zombie::HitIceTrap()
            0x5323C0,[](AAsmCodeContext * context) __stdcall {
                damage_mark.source = DamageMark::ICE;
        });    
    }
    void mark_invalid_damage(){
        // 无效case
        // 排除大嘴
        AInsertUniqueAsmCode(
            // void Plant::UpdateChomper()
            0x461320, [](AAsmCodeContext * context) __stdcall {
                damage_mark.source = DamageMark::DEFAULT;
        });
        // 排除巨人行为相关
        AInsertUniqueAsmCode(
            // void Zombie::UpdateZombieGargantuar()
            0x526D10, [](AAsmCodeContext * context) __stdcall {
                damage_mark.source = DamageMark::DEFAULT;
        });
        // 排除雪橇
        AInsertUniqueAsmCode(
            // void Zombie::UpdateZombieBobsled()
            0x528050, [](AAsmCodeContext * context) __stdcall {
                damage_mark.source = DamageMark::DEFAULT;
        });
        // 排除水族馆
        AInsertUniqueAsmCode(
            // void Zombie::UpdateZombiquarium()
            0x5291E0, [](AAsmCodeContext * context) __stdcall {
                damage_mark.source = DamageMark::DEFAULT;
        });
        // 排除自伤
        AInsertUniqueAsmCode(
            // void Zombie::UpdatePlaying()
            0x52B340, [](AAsmCodeContext * context) __stdcall {
                damage_mark.source = DamageMark::DEFAULT;
        });
        // 排除自相
        AInsertUniqueAsmCode(
            // void Zombie::EatZombie(Zombie* theZombie)
            0x52FE10, [](AAsmCodeContext * context) __stdcall {
                damage_mark.source = DamageMark::DEFAULT;
        });
    }
    void record_projectile(){
        // 子弹表
        AInsertUniqueAsmCode(
            // void Projectile::ProjectileInitialize(int theX, int theY, int theRenderOrder, int theRow, ProjectileType theProjectileType)
            0x46C730, [](AAsmCodeContext * context) __stdcall {
                // esp + ?
                AProjectile* projectile = (AProjectile*)context->esi;

                if (!projectile_mark.valid) return;

                // 回溯
                trace_record();

                DamageInfo proj_info;
                // 记录时间
                proj_info.time = ANowTime();
                proj_info.time_clock = AGetMainObject()->GameClock();
                // 记录子弹
                proj_info.GetProjectileInfo(projectile);
                // 关联植物来源
                proj_info.p = projectile_mark.p;

                projectile_records.push_back(proj_info);
        });
    }
    void mark_valid_projectile(){
        // 有效case
        // 正常开火
        AInsertUniqueAsmCode(
            // void Plant::Fire(Zombie* theTargetZombie, int theRow, PlantWeapon thePlantWeapon)
            0x466E00, [](AAsmCodeContext * context) __stdcall {
                // esp + ?
                APlant* plant = (APlant*)context->edi;

                projectile_mark.valid = true;
                projectile_mark.GetPlantInfo(plant);
        });
        // 杨桃开火
        AInsertUniqueAsmCode(
            // void Plant::StarFruitFire()
            0x45F720, [](AAsmCodeContext * context) __stdcall {
                APlant* plant = (APlant*)context->esi;

                projectile_mark.valid = true;
                projectile_mark.GetPlantInfo(plant);
        });
    }
    void mark_invalid_projectile(){
        // 无效case
        // 投篮
        AInsertUniqueAsmCode(
            // void Zombie::ZombieCatapultFire(Plant* thePlant)
            0x525730, [](AAsmCodeContext * context) __stdcall {
                projectile_mark.valid = false;
        });
        // 豌豆僵尸
        AInsertUniqueAsmCode(
            // void Zombie::UpdateZombiePeaHead()
            0x5273D0, [](AAsmCodeContext * context) __stdcall {
                projectile_mark.valid = false;
        });
        // 机枪僵尸
        AInsertUniqueAsmCode(
            // void Zombie::UpdateZombieGatlingHead()
            0x527750, [](AAsmCodeContext * context) __stdcall {
                projectile_mark.valid = false;
        });
    }

public:
    void SetPath(const char* path){std::strcpy(damage_record_path, path);}
    void Setup(){
        record_damage();
        mark_valid_damage();
        mark_invalid_damage();
        record_projectile();
        mark_valid_projectile();
        mark_invalid_projectile();
    }
};

DamageMark DamageRecorder::damage_mark;
ProjectileMark DamageRecorder::projectile_mark;
std::vector<DamageInfo> DamageRecorder::damage_records;
std::vector<DamageInfo> DamageRecorder::projectile_records;

#endif