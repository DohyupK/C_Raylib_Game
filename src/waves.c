#include "game.h"
#include <math.h>
#include <stdio.h>

void Waves_Init(GameState *g) {
    g->wave.active = false;
    g->wave.warnTimer = 0;
    g->wave.duration = 0;
}

void Waves_Update(GameState *g, float dt) {
    if (g->gameOver) return;

    if (!g->wave.active) {
        g->bossTimer += dt; /* reuse as wave cooldown accumulator */
        float need = (g->gameTime < 20.0f) ? 20.0f : 40.0f;
        if (g->bossTimer >= need) {
            g->bossTimer = 0.0f;
            g->wave.active = true;
            g->wave.warnTimer = 3.0f;
            g->wave.duration = 12.0f;
            float ang = GameRand01(g) * PI * 2.0f;
            g->wave.direction = (Vector3){cosf(ang), 0, sinf(ang)};
            g->wave.typeA = GameRandRange(g, 0, MONSTER_TYPE_COUNT - 1);
            do {
                g->wave.typeB = GameRandRange(g, 0, MONSTER_TYPE_COUNT - 1);
            } while (g->wave.typeB == g->wave.typeA);
            g->wave.spawnRate = 2.2f;
            g->wave.spawnAcc = 0;
            MonsterDef a, b;
            MonsterData_Get(g->wave.typeA, &a);
            MonsterData_Get(g->wave.typeB, &b);
            GameShowMessage(g, TextFormat(L("MSG_WARN"), a.nameKr, b.nameKr), 3.0f);
            /* clear ambient trash so the single-direction wave is obvious */
            for (int i = 0; i < MAX_MONSTERS; i++) {
                if (g->monsters[i].active && !g->monsters[i].isBoss) {
                    g->monsters[i].active = false;
                }
            }
        }
        return;
    }

    if (g->wave.warnTimer > 0.0f) {
        g->wave.warnTimer -= dt;
        return;
    }

    g->wave.duration -= dt;
    g->wave.spawnAcc += dt * g->wave.spawnRate;
    while (g->wave.spawnAcc >= 1.0f) {
        g->wave.spawnAcc -= 1.0f;
        int type = (GameRand01(g) < 0.5f) ? g->wave.typeA : g->wave.typeB;
        Monsters_SpawnDirectional(g, type, g->wave.direction, 20.0f + GameRand01(g) * 4.0f);
        /* medium speed: bump via temporary - monsters use warningActive speed mul in Monsters_Update
           Also boost this spawn's velocity feel by choosing faster archetypes often already handled */
    }

    if (g->wave.duration <= 0.0f) {
        g->wave.active = false;
        GameShowMessage(g, L("MSG_WAVE_END"), 2.0f);
    }
}

void Waves_DrawHud(GameState *g) {
    if (!g->wave.active) return;
    const char *title = (g->wave.warnTimer > 0.0f) ? L("WARN") : L("WAVE");
    float tw = Ui_Measure(title, 40);
    DrawRectangle((int)(SCREEN_W / 2 - tw / 2 - 20), 40, (int)(tw + 40), 70, Fade(MAROON, 0.75f));
    Ui_Draw(title, SCREEN_W / 2 - tw / 2, 50, 40, YELLOW);

    /* direction arrow */
    Vector2 center = {SCREEN_W * 0.5f, 130};
    Vector2 dir = {g->wave.direction.x, g->wave.direction.z};
    float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
    if (len > 0.01f) { dir.x /= len; dir.y /= len; }
    Vector2 tip = {center.x + dir.x * 40.0f, center.y + dir.y * 40.0f};
    DrawCircleV(center, 8, WHITE);
    DrawLineEx(center, tip, 4, ORANGE);
    Ui_Draw(L("THREAT_DIR"), center.x - 40, center.y + 50, 14, ORANGE);
}
