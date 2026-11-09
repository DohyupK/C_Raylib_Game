#include "game.h"
#include <stdio.h>

void Bosses_Update(GameState *g, float dt) {
    (void)dt;
    if (g->gameOver) return;

    /* spawn bosses on score milestones */
    static const int thresholds[3] = {150, 400, 800};
    static const BossKind kinds[3] = {BOSS_HORDE_KING, BOSS_PLAGUE_MOTHER, BOSS_SHADOW_COLOSSUS};
    static const int typeIds[3] = {7, 9, 5}; /* Stone Brute, Venom Crawler, Pale Wisp bases */

    for (int i = 0; i < 3; i++) {
        if (g->nextBoss != i) continue;
        if (g->score.score < thresholds[i]) return;
        /* ensure no existing boss */
        for (int m = 0; m < MAX_MONSTERS; m++) {
            if (g->monsters[m].active && g->monsters[m].isBoss) return;
        }
        Monsters_SpawnAround(g, typeIds[i], 18.0f, 20.0f, true);
        for (int m = 0; m < MAX_MONSTERS; m++) {
            if (g->monsters[m].active && g->monsters[m].isBoss && g->monsters[m].bossKind == BOSS_NONE) {
                g->monsters[m].bossKind = kinds[i];
                if (kinds[i] == BOSS_HORDE_KING) {
                    g->monsters[m].maxHp *= 1.2f;
                    g->monsters[m].hp = g->monsters[m].maxHp;
                    g->monsters[m].scale = 2.8f;
                } else if (kinds[i] == BOSS_PLAGUE_MOTHER) {
                    g->monsters[m].scale = 2.6f;
                } else {
                    g->monsters[m].scale = 3.0f;
                    g->monsters[m].phased = false;
                }
                break;
            }
        }
        const char *keys[] = {"BOSS0", "BOSS1", "BOSS2"};
        GameShowMessage(g, TextFormat(L("MSG_BOSS"), L(keys[i])), 3.5f);
        AudioSfx_Boss();
        g->nextBoss = i + 1;
        return;
    }

    /* boss unique behaviors */
    for (int i = 0; i < MAX_MONSTERS; i++) {
        Monster *m = &g->monsters[i];
        if (!m->active || !m->isBoss) continue;
        if (m->bossKind == BOSS_HORDE_KING && m->specialCd <= 0.0f) {
            for (int s = 0; s < 3; s++) Monsters_SpawnAround(g, 0, 3.0f, 6.0f, false);
            m->specialCd = 9.0f;
        } else if (m->bossKind == BOSS_PLAGUE_MOTHER && m->specialCd <= 0.0f) {
            for (int s = 0; s < 2; s++) Monsters_SpawnAround(g, 24, 4.0f, 7.0f, false); /* plague rats */
            m->specialCd = 7.0f;
            if (Vector3Distance(m->position, g->player.position) < 7.0f) {
                g->player.hp -= 3.0f;
            }
        } else if (m->bossKind == BOSS_SHADOW_COLOSSUS && m->specialCd <= 0.0f) {
            m->phased = true;
            m->specialTimer = 2.0f;
            m->specialCd = 6.0f;
        }
    }
}
