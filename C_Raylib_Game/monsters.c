#include "game.h"
#include <math.h>
#include <stdio.h>

static void SpawnPickupFull(GameState *g, Vector3 pos, ItemKind kind, int meta) {
    for (int i = 0; i < MAX_PICKUPS; i++) {
        if (g->pickups[i].active) continue;
        g->pickups[i].active = true;
        g->pickups[i].position = (Vector3){pos.x, 0.4f, pos.z};
        g->pickups[i].kind = kind;
        g->pickups[i].bob = GameRand01(g) * 6.0f;
        g->pickups[i].life = PICKUP_LIFETIME;
        g->pickups[i].metaTypeId = meta;
        return;
    }
}

static void SpawnPickupAt(GameState *g, Vector3 pos) {
    ItemKind kinds[] = {ITEM_MEDKIT, ITEM_AMMO, ITEM_SPEED_BOOST, ITEM_SHIELD, ITEM_GRENADE_PACK, ITEM_SCORE_GEM};
    SpawnPickupFull(g, pos, kinds[GameRandRange(g, 0, 5)], -1);
}

void Monsters_Init(GameState *g) {
    for (int i = 0; i < MAX_MONSTERS; i++) g->monsters[i].active = false;
    for (int i = 0; i < MAX_PICKUPS; i++) g->pickups[i].active = false;
    /* a few starter pickups */
    for (int i = 0; i < 4; i++) {
        SpawnPickupAt(g, (Vector3){(GameRand01(g) - 0.5f) * 16.0f, 0, (GameRand01(g) - 0.5f) * 16.0f});
    }
}

static Monster *AllocMonster(GameState *g) {
    for (int i = 0; i < MAX_MONSTERS; i++) {
        if (!g->monsters[i].active) return &g->monsters[i];
    }
    return NULL;
}

void Monsters_SpawnAround(GameState *g, int typeId, float distMin, float distMax, bool boss) {
    Monster *m = AllocMonster(g);
    if (!m) return;
    MonsterDef def;
    MonsterData_Get(typeId, &def);
    float ang = GameRand01(g) * PI * 2.0f;
    float dist = distMin + GameRand01(g) * (distMax - distMin);
    m->active = true;
    m->typeId = typeId;
    m->position = (Vector3){
        g->player.position.x + cosf(ang) * dist,
        0.0f,
        g->player.position.z + sinf(ang) * dist
    };
    m->velocity = (Vector3){0};
    m->maxHp = def.hp * GameDiffMul(g) * (boss ? 8.0f : 1.0f);
    m->hp = m->maxHp;
    m->attackCd = 0.0f;
    m->specialCd = 1.0f + GameRand01(g);
    m->specialTimer = 0.0f;
    m->slowTimer = 0.0f;
    m->poisonTimer = 0.0f;
    m->phased = false;
    m->isBoss = boss;
    m->fromWave = false;
    m->bossKind = BOSS_NONE;
    m->scale = boss ? 2.4f : (0.85f + def.radius);
    World_ResolveCircle(g, &m->position, def.radius * m->scale);
}

void Monsters_SpawnDirectional(GameState *g, int typeId, Vector3 dir, float dist) {
    Monster *m = AllocMonster(g);
    if (!m) return;
    MonsterDef def;
    MonsterData_Get(typeId, &def);
    dir.y = 0;
    if (Vector3Length(dir) < 0.01f) dir = (Vector3){1, 0, 0};
    dir = Vector3Normalize(dir);
    m->active = true;
    m->typeId = typeId;
    m->position = Vector3Add(g->player.position, Vector3Scale(dir, dist));
    m->position.y = 0;
    m->velocity = (Vector3){0};
    m->maxHp = def.hp * GameDiffMul(g);
    m->hp = m->maxHp;
    m->attackCd = 0;
    m->specialCd = 0.5f;
    m->specialTimer = 0;
    m->slowTimer = 0;
    m->poisonTimer = 0;
    m->phased = false;
    m->isBoss = false;
    m->fromWave = true;
    m->bossKind = BOSS_NONE;
    m->scale = 0.85f + def.radius;
}

static void KillMonster(GameState *g, Monster *m) {
    MonsterDef def;
    MonsterData_Get(m->typeId, &def);
    int pts = def.scoreValue * (m->isBoss ? 10 : 1);
    Score_AddKill(g, pts, m->position);
    Meta_OnKill(g, m->isBoss);
    GameSpawnParticle(g, m->position, def.color, 1.0f * m->scale, 0.5f);
    if (GameRand01(g) < (m->isBoss ? 1.0f : 0.35f)) SpawnPickupAt(g, m->position);
    /* 1% monster book drop for codex */
    if (!m->isBoss && GameRand01(g) < 0.01f) {
        SpawnPickupFull(g, m->position, ITEM_MONSTER_BOOK, m->typeId);
        GameShowMessage(g, L("MSG_BOOK_DROP"), 1.5f);
    }
    if (def.special == SPEC_SPLIT && !m->isBoss && m->scale > 0.7f) {
        /* split into two smaller shamblers of same type with less hp */
        for (int s = 0; s < 2; s++) {
            Monster *c = AllocMonster(g);
            if (!c) break;
            *c = *m;
            c->scale *= 0.6f;
            c->maxHp *= 0.45f;
            c->hp = c->maxHp;
            c->position.x += (s == 0 ? -0.8f : 0.8f);
            c->isBoss = false;
        }
    }
    m->active = false;
}

void Monsters_DamageOne(GameState *g, int index, float damage) {
    if (index < 0 || index >= MAX_MONSTERS) return;
    Monster *m = &g->monsters[index];
    if (!m->active || m->phased) return;
    MonsterDef def;
    MonsterData_Get(m->typeId, &def);
    float dmg = damage;
    if (def.special == SPEC_ARMORED) dmg *= (1.0f - def.specialValue);
    m->hp -= dmg;
    char buf[32];
    snprintf(buf, sizeof(buf), "%.0f", dmg);
    GameAddFloatText(g, (Vector3){m->position.x, 1.5f, m->position.z}, g->camera, buf, YELLOW);
    if (m->hp <= 0.0f) KillMonster(g, m);
}

void Monsters_DamageAt(GameState *g, Vector3 pos, float radius, float damage, bool fromPlayer) {
    (void)fromPlayer;
    for (int i = 0; i < MAX_MONSTERS; i++) {
        Monster *m = &g->monsters[i];
        if (!m->active) continue;
        if (m->phased) continue;
        MonsterDef def;
        MonsterData_Get(m->typeId, &def);
        float r = def.radius * m->scale;
        if (Vector3Distance(pos, m->position) > radius + r) continue;
        Monsters_DamageOne(g, i, damage);
    }
}

static void ApplyPlayerHit(GameState *g, float dmg) {
    if (g->player.invulnTimer > 0.0f) return;
    if (g->player.shieldHp > 0.0f) {
        float absorb = fminf(g->player.shieldHp, dmg);
        g->player.shieldHp -= absorb;
        dmg -= absorb;
    }
    g->player.hp -= dmg;
    g->player.invulnTimer = 0.55f;
    if (g->player.hp <= 0.0f) {
        g->player.hp = 0.0f;
        if (!g->gameOver) {
            g->gameOver = true;
            Ranking_Submit(g);
            GameShowMessage(g, L("MSG_DEAD"), 10.0f);
        }
    }
}

void Monsters_Update(GameState *g, float dt) {
    /* pickups bob + despawn after ~1 minute */
    for (int i = 0; i < MAX_PICKUPS; i++) {
        if (!g->pickups[i].active) continue;
        g->pickups[i].bob += dt * 3.0f;
        g->pickups[i].life -= dt;
        if (g->pickups[i].life <= 0.0f) {
            GameSpawnParticle(g, g->pickups[i].position, Item_Color(g->pickups[i].kind), 0.8f, 0.45f);
            GameSpawnParticle(g, g->pickups[i].position, WHITE, 0.4f, 0.3f);
            g->pickups[i].active = false;
        }
    }

    /* During WARNING/WAVE: no omnidirectional spawns — only Waves_Update directional spawns */
    if (!g->wave.active) {
        g->spawnAcc += dt * 0.35f;
        while (g->spawnAcc >= 1.0f) {
            g->spawnAcc -= 1.0f;
            int alive = 0;
            for (int i = 0; i < MAX_MONSTERS; i++) if (g->monsters[i].active) alive++;
            if (alive < 28) {
                int typeId = GameRandRange(g, 0, MONSTER_TYPE_COUNT - 1);
                Monsters_SpawnAround(g, typeId, 16.0f, 24.0f, false);
            }
        }
    } else {
        g->spawnAcc = 0.0f;
    }

    for (int i = 0; i < MAX_MONSTERS; i++) {
        Monster *m = &g->monsters[i];
        if (!m->active) continue;
        MonsterDef def;
        MonsterData_Get(m->typeId, &def);

        if (m->slowTimer > 0.0f) m->slowTimer -= dt;
        if (m->poisonTimer > 0.0f) {
            m->poisonTimer -= dt;
            m->hp -= 4.0f * dt;
            if (m->hp <= 0) { KillMonster(g, m); continue; }
        }
        if (m->specialTimer > 0.0f) {
            m->specialTimer -= dt;
            if (m->specialTimer <= 0.0f) m->phased = false;
        }
        if (m->attackCd > 0.0f) m->attackCd -= dt;
        if (m->specialCd > 0.0f) m->specialCd -= dt;

        float speed = def.speed * GameDiffMul(g);
        if (m->fromWave) speed *= 1.65f;
        if (m->slowTimer > 0.0f) speed *= 0.55f;
        if (def.special == SPEC_RAGE_SPEED && Vector3Distance(m->position, g->player.position) < 8.0f)
            speed *= def.specialValue;

        Vector3 toPlayer = Vector3Subtract(g->player.position, m->position);
        toPlayer.y = 0;
        float dist = Vector3Length(toPlayer);
        if (dist > 0.01f) {
            Vector3 dir = Vector3Scale(Vector3Normalize(toPlayer), speed);
            if (def.special == SPEC_CHARGE && m->specialCd <= 0.0f && dist < 10.0f && dist > 3.0f) {
                dir = Vector3Scale(Vector3Normalize(toPlayer), speed * def.specialValue);
                m->specialCd = 4.0f;
            }
            m->velocity = dir;
        }
        m->position.x += m->velocity.x * dt;
        m->position.z += m->velocity.z * dt;
        World_ResolveCircle(g, &m->position, def.radius * m->scale * 0.7f);

        /* specials */
        if (m->specialCd <= 0.0f) {
            switch (def.special) {
            case SPEC_PHASE:
                m->phased = true;
                m->specialTimer = def.specialValue;
                m->specialCd = 5.0f;
                break;
            case SPEC_SUMMON:
                Monsters_SpawnAround(g, 0, 2.0f, 4.0f, false);
                m->specialCd = 8.0f;
                break;
            case SPEC_HOWL:
                for (int j = 0; j < MAX_MONSTERS; j++) {
                    if (!g->monsters[j].active) continue;
                    if (Vector3Distance(m->position, g->monsters[j].position) < 8.0f)
                        g->monsters[j].slowTimer = 0; /* clear slow / hype: bump by reducing attack cd */
                    g->monsters[j].attackCd *= 0.5f;
                }
                m->specialCd = 7.0f;
                break;
            case SPEC_HEAL_AURA:
                for (int j = 0; j < MAX_MONSTERS; j++) {
                    if (!g->monsters[j].active) continue;
                    if (Vector3Distance(m->position, g->monsters[j].position) < 5.0f)
                        g->monsters[j].hp = fminf(g->monsters[j].maxHp, g->monsters[j].hp + def.specialValue);
                }
                m->specialCd = 5.0f;
                break;
            case SPEC_SLOW_WRAP:
                if (dist < 5.0f) {
                    g->player.speedMul = 0.55f;
                    g->player.speedTimer = def.specialValue;
                    m->specialCd = 6.0f;
                }
                break;
            case SPEC_MUD_POOL:
                if (dist < 4.0f) {
                    g->player.speedMul = 0.45f;
                    g->player.speedTimer = def.specialValue;
                    m->specialCd = 7.0f;
                }
                break;
            case SPEC_CURSE:
                if (dist < 6.0f) {
                    ApplyPlayerHit(g, 6.0f);
                    m->specialCd = def.specialValue;
                }
                break;
            default: break;
            }
        }

        if (dist < def.attackRange + PLAYER_RADIUS && m->attackCd <= 0.0f && !m->phased) {
            float dmg = def.damage * GameDiffMul(g) * (m->isBoss ? 1.8f : 1.0f);
            ApplyPlayerHit(g, dmg);
            if (def.special == SPEC_LIFESTEAL) m->hp = fminf(m->maxHp, m->hp + dmg * def.specialValue);
            if (def.special == SPEC_POISON) {
                /* reuse player speed timer as poison feel via damage over time-ish: small DoT pulses */
                g->player.hp -= 2.0f;
            }
            if (def.special == SPEC_SPIKE) ApplyPlayerHit(g, def.specialValue * 0.15f);
            m->attackCd = def.attackCooldown;
        }
    }
}

void Monsters_Draw(GameState *g) {
    for (int i = 0; i < MAX_PICKUPS; i++) {
        Pickup *p = &g->pickups[i];
        if (!p->active) continue;
        float y = 0.55f + sinf(p->bob) * 0.2f;
        Vector3 pos = (Vector3){p->position.x, y, p->position.z};
        Assets_DrawPickup(p->kind, pos, p->bob);
        /* blink when about to despawn */
        if (p->life < 10.0f && ((int)(p->life * 6) % 2 == 0)) {
            DrawCubeWires(pos, 0.55f, 0.55f, 0.55f, RED);
        }
    }
    for (int i = 0; i < MAX_MONSTERS; i++) {
        Monster *m = &g->monsters[i];
        if (!m->active) continue;
        MonsterDef def;
        MonsterData_Get(m->typeId, &def);
        Assets_DrawMonster(g, m, &def);

        float h = 1.8f * m->scale;
        float t = m->hp / m->maxHp;
        Vector3 bar = (Vector3){m->position.x, h + 0.2f, m->position.z};
        DrawCube(bar, 0.9f * m->scale, 0.08f, 0.08f, DARKGRAY);
        DrawCube((Vector3){bar.x - 0.45f * m->scale * (1.0f - t), bar.y, bar.z},
                 0.9f * m->scale * t, 0.08f, 0.08f, LIME);

        float dist = Vector3Distance(m->position, g->player.position);
        if (m->isBoss || dist < 12.0f) {
            Vector2 sp = GetWorldToScreen((Vector3){m->position.x, h + 0.55f, m->position.z}, g->camera);
            const char *label = m->isBoss ? TextFormat(L("BOSS_LABEL"), def.nameKr) : def.nameKr;
            float tw = Ui_Measure(label, 14);
            Ui_Draw(label, sp.x - tw * 0.5f, sp.y, 14, m->isBoss ? RED : RAYWHITE);
        }
    }
}
