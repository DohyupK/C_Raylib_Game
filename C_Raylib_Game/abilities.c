#include "game.h"

void Abilities_Update(GameState *g, float dt) {
    (void)dt;
    (void)g;
}

void Abilities_Try(GameState *g, AbilityKind kind) {
    Player *p = &g->player;
    int idx = (int)kind;
    if (idx < 0 || idx >= 4) return;
    if (p->abilityCd[idx] > 0.0f) {
        GameShowMessage(g, L("MSG_CD"), 0.6f);
        return;
    }
    switch (kind) {
    case ABILITY_BERSERK:
        p->berserkTimer = 5.0f;
        p->abilityCd[idx] = 14.0f;
        GameShowMessage(g, L("MSG_BERSERK"), 1.0f);
        break;
    case ABILITY_BARRIER:
        p->shieldHp = 55.0f;
        p->abilityCd[idx] = 12.0f;
        GameShowMessage(g, L("MSG_BARRIER"), 1.0f);
        break;
    case ABILITY_OVERCHARGE:
        p->overchargeTimer = 4.5f;
        p->abilityCd[idx] = 16.0f;
        GameShowMessage(g, L("MSG_OVER"), 1.0f);
        break;
    default: break;
    }
}
