#ifndef _REISEN_COB_MARKER_COB_MARKER_H_
#define _REISEN_COB_MARKER_COB_MARKER_H_

#include "avz.h"

class CobMarker : public ATickRunnerWithNoStart, public AOrderedEnterFightHook<-1> {
protected:
    std::unordered_set<AProjectile*> projectiles;
    int textDuration;
    std::queue<int> displayQueue[6];

    size_t _InsertToQueue(int idx) {
        int time = AGetPvzBase()->MjClock();
        while (!displayQueue[idx].empty() && time - displayQueue[idx].front() > textDuration) {
            displayQueue[idx].pop();
        }
        displayQueue[idx].push(time);
        return displayQueue[idx].size();
    }

    void _Run() {
        std::unordered_set<AProjectile*> currentProjectiles;
        for (auto& p : AObjSelector(&AProjectile::Type, 11)) {
            currentProjectiles.insert(&p);
        }
        for (auto p : projectiles) {
            if (!currentProjectiles.contains(p)) {
                int row = p->CobTargetRow();
                float col = p->CobTargetAbscissa() / 80.0f;
                std::string msg = std::format("{}\n{:.02f}", ANowTime(false).time, col);

                int position = _InsertToQueue(row) - 1;
                int textX = 756 - 50 * (position / 2);
                int textY = AAsm::GridToOrdinate(row, 8) + 15 + (position % 2) * 40;
                aPainter.SetFontSize(20);
                aPainter.Draw(AText(msg, textX, textY), 300);
            }
        }
        projectiles = std::move(currentProjectiles);
    }

    virtual void _EnterFight() override {
        projectiles.clear();
        for (int i = 0; i < 6; ++i) {
            while (!displayQueue[i].empty())
                displayQueue[i].pop();
        }
    }

public:
    CobMarker(int textDuration = 300) : textDuration(textDuration) {}

    void Start() {
        ATickRunnerWithNoStart::_Start([this]() { _Run(); }, ONLY_FIGHT);
    }
};

#endif
