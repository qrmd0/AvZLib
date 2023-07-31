#include "refresh/refresh.h"

using operation_t = function<void(int)>;

string base_path = "C:\\ProgramData\\PopCap Games\\PlantsVsZombies\\refresh\\PE";

auto default_conf = TaskBuilder().total(1000);
auto norm_act = default_conf.huge(false).assume_refresh(true);
auto norm_sep = default_conf.huge(false).assume_refresh(false);
auto huge_act = default_conf.huge(true).assume_refresh(true);
auto huge_sep = default_conf.huge(true).assume_refresh(false);
auto hb_n_a = norm_act
    .output_folder(base_path + "\\红白\\激活")
    .required_types({GARGANTUAR, GIGA_GARGANTUAR})
    .banned_types({});
auto h_n_a = norm_act
    .output_folder(base_path + "\\红\\激活")
    .required_types({GIGA_GARGANTUAR})
    .banned_types({GARGANTUAR});
auto hb_n_s = norm_sep
    .output_folder(base_path + "\\红白\\分离")
    .required_types({GARGANTUAR, GIGA_GARGANTUAR, ZOMBONI})
    .banned_types({});
auto h_n_s = norm_sep
    .output_folder(base_path + "\\红\\分离")
    .required_types({GIGA_GARGANTUAR, ZOMBONI})
    .banned_types({GARGANTUAR});
auto b_n_s = norm_sep
    .output_folder(base_path + "\\白\\分离")
    .required_types({GARGANTUAR, ZOMBONI})
    .banned_types({GIGA_GARGANTUAR});
auto h_h_a = huge_act
    .output_folder(base_path + "\\红\\大波激活")
    .required_types({GIGA_GARGANTUAR})
    .banned_types({});
auto f_h_a = huge_act
    .output_folder(base_path + "\\快速\\大波激活")
    .required_types({})
    .banned_types({GARGANTUAR, GIGA_GARGANTUAR});

/*
请确保测试前建立以下的文件夹结构：
C:\ProgramData\PopCap Games\PlantsVsZombies\refresh
└─PE
   ├─红
   │  └─激活
   └─红白
       └─激活
测试用阵型的布阵码：LI43NHRYSFpudKQY0xA4dpBUVBNeXi9W
直接运行该脚本需要50-60h，应考虑拆分各项测试，并行执行以节约时间
*/
vector<Task> get_tasks() {
    vector<Task> tasks;
    // 测试红白出怪和单红出怪
    for(auto& base : {hb_n_a, h_n_a}) {
        // 加速波 PP
        for(int t : {225, 260, 295, 318, 341, 359, 401, 500})
            tasks.push_back(base
                .prefix("PP-" + to_string(t))
                .check_time(max(t, 401))
                .operation([=](int wave){
                    float col = (t >= 401) ? 8.75 : 9;
                    SetTime(t - 373);
                    pao_operator.pao({{2, col}, {5, col}});
                })
            );

        // 加速波 PPDD
        for(int t : {225, 260, 295})
            tasks.push_back(base
                .prefix("PPDD-" + to_string(t))
                .check_time(401)
                .operation([=](int wave){
                    SetTime(t - 373);
                    pao_operator.pao({{2, 9}, {5, 9}});
                    Delay(107);
                    pao_operator.pao({{1, 8}, {5, 8}});
                })
            );

        // 加速波 PP + 灰烬
        for(int t : {225, 260, 296, 318}) {
            tasks.push_back(base
                .prefix("PPA'-fixed-" + to_string(t))
                .check_time(401)
                .operation([=](int wave){
                    SetTime(t - 373); pao_operator.pao({{2, 9}, {5, 9}});
                    SetTime(t - 99); Card(JALAPENO, 1, 1);
                })
            );
            if(t > 260)
                continue;
            tasks.push_back(base
                .prefix("PPA'-smart-" + to_string(t))
                .check_time(401)
                .operation([=](int wave){
                    SetTime(t - 373); pao_operator.pao({{2, 9}, {5, 9}});
                    InsertTimeOperation(t - 99, wave, [](){
                        SetNowTime();
                        int garg_count[6] = {0};
                        for(auto& z : alive_zombie_filter)
                            if(z.type() == GARGANTUAR || z.type() == GIGA_GARGANTUAR)
                                garg_count[z.row()]++;
                        int row = max_element(garg_count, garg_count + 6) - garg_count;
                        Card(JALAPENO, row + 1, 1);
                    });
                })
            );
            tasks.push_back(base
                .prefix("PPA-fixed-" + to_string(t))
                .check_time(401)
                .operation([=](int wave){
                    SetTime(t - 373); pao_operator.pao({{2, 9}, {5, 9}});
                    SetTime(t - 99); Card(CHERRY_BOMB, 1, 9);
                })
            );
            tasks.push_back(base
                .prefix("PPA-smart-" + to_string(t))
                .check_time(401)
                .operation([=](int wave){
                    SetTime(t - 373); pao_operator.pao({{2, 9}, {5, 9}});
                    InsertTimeOperation(t - 99, wave, [](){
                        SetNowTime();
                        int garg_count[6] = {0};
                        for(auto& z : alive_zombie_filter)
                            if(z.type() == GARGANTUAR || z.type() == GIGA_GARGANTUAR)
                                garg_count[z.row()]++;
                        int row = garg_count[0] + garg_count[1] > garg_count[4] + garg_count[5] ? 2 : 5;
                        Card(CHERRY_BOMB, row, 9);
                    });
                })
            );
        }

        // 冰波 PP (1 冰)
        for(int t = 1050; t <= 1550; t += 100) {
            tasks.push_back(base
                .prefix("I-PP-" + to_string(t))
                .check_time(t)
                .operation([=](int wave){
                    SetTime(1 - 100); Card(ICE_SHROOM, 1, 1);
                    SetTime(t - 373); pao_operator.pao({{2, 8.75}, {5, 8.75}});
                })
            );
            tasks.push_back(base
                .prefix("IPP-PP-" + to_string(t))
                .check_time(t)
                .operation([=](int wave){
                    SetTime(1 - 100); Card(ICE_SHROOM, 1, 1);
                    SetTime(301 - 373); pao_operator.pao({{1, 9}, {5, 9}});
                    SetTime(t - 373); pao_operator.pao({{2, 8.75}, {5, 8.75}});
                })
            );
        }

        // 冰波 PP (100 冰)
        for(int t = 950; t <= 1550; t += 100) {
            float col;
            switch(t) {
                case 1450: col = 8.625; break; // 长冰波落点需要适当左移
                case 1550: col = 8.4; break;
                default: col = 8.75;
            }
            tasks.push_back(base
                .prefix("100I-PP-" + to_string(t))
                .check_time(t)
                .operation([=](int wave){
                    SetTime(100 - 100); Card(ICE_SHROOM, 1, 1);
                    SetTime(t - 373); pao_operator.pao({{2, col}, {5, col}});
                })
            );
            tasks.push_back(base
                .prefix("100IPP-PP-" + to_string(t))
                .check_time(t)
                .operation([=](int wave){
                    SetTime(100 - 100); Card(ICE_SHROOM, 1, 1);
                    SetTime(173 - 373); pao_operator.pao({{1, 8.6375}, {5, 8.6375}});
                    SetTime(t - 373); pao_operator.pao({{2, 8.75}, {5, 8.75}});
                })
            );
        }
    }
    return tasks;
}
