#include "game.h"
#include <math.h>
#include <stdio.h>

typedef struct {
    const char *name;
    float cooldown;
    float damage;
    float speed;
    float life;
    int pellets;
    float spread;
} WeaponStats;

static WeaponStats Stats(int kind) {
    switch (kind) {
    case WEAPON_SHOTGUN: return (WeaponStats){L("SHOTGUN"), 0.55f, 12.0f, 28.0f, 0.45f, 6, 0.22f};
    case WEAPON_RIFLE: return (WeaponStats){L("RIFLE"), 0.12f, 14.0f, 45.0f, 0.9f, 1, 0.02f};
    default: return (WeaponStats){L("PISTOL"), 0.22f, 18.0f, 38.0f, 0.8f, 1, 0.04f};
    }
}

const char *Weapons_Name(int kind) { return Stats(kind).name; }

void Weapons_Init(GameState *g) {
    for (int i = 0; i < MAX_BULLETS; i++) g->bullets[i].active = false;
    for (int i = 0; i < MAX_GRENADES; i++) g->grenades[i].active = false;
}

static void SpawnBullet(GameState *g, Vector3 pos, Vector3 vel, float dmg, float life, float radius, int weapon) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (g->bullets[i].active) continue;
        g->bullets[i].active = true;
        g->bullets[i].position = pos;
        g->bullets[i].velocity = vel;
        g->bullets[i].damage = dmg;
        g->bullets[i].life = life;
        g->bullets[i].radius = radius;
        g->bullets[i].ownerWeapon = weapon;
        return;
    }
}

void Weapons_Cycle(GameState *g) {
    g->player.weapon = (g->player.weapon + 1) % WEAPON_COUNT;
    GameShowMessage(g, TextFormat(L("MSG_WEAPON"), Weapons_Name(g->player.weapon)), 1.0f);
}

void Weapons_TryFire(GameState *g) {
    Player *p = &g->player;
    if (p->fireCooldown > 0.0f) return;
    WeaponStats s = Stats(p->weapon);
    float cd = s.cooldown;
    if (p->overchargeTimer > 0.0f) cd *= 0.45f;
    if (p->costume == COSTUME_HEAVY) cd *= 1.1f;
    p->fireCooldown = cd;

    Vector3 aim = Player_AimPoint(g);
    Vector3 origin = (Vector3){p->position.x, p->position.y + 1.0f, p->position.z};
    Vector3 dir = Vector3Subtract(aim, origin);
    dir.y = 0;
    if (Vector3Length(dir) < 0.01f) dir = (Vector3){sinf(p->yaw), 0, cosf(p->yaw)};
    dir = Vector3Normalize(dir);

    float dmgMul = (p->berserkTimer > 0.0f) ? 1.5f : 1.0f;
    if (p->costume == COSTUME_GOLD) dmgMul *= 1.1f;

    for (int i = 0; i < s.pellets; i++) {
        float ang = (GameRand01(g) - 0.5f) * s.spread * 2.0f;
        float c = cosf(ang), sn = sinf(ang);
        Vector3 d = (Vector3){dir.x * c - dir.z * sn, 0, dir.x * sn + dir.z * c};
        SpawnBullet(g, origin, Vector3Scale(d, s.speed), s.damage * dmgMul, s.life, 0.18f, p->weapon);
    }
    AudioSfx_Shoot();
}

void Weapons_ThrowGrenade(GameState *g) {
    if (g->player.grenades <= 0) {
        GameShowMessage(g, L("MSG_NO_GRENADE"), 0.8f);
        return;
    }
    for (int i = 0; i < MAX_GRENADES; i++) {
        if (g->grenades[i].active) continue;
        Vector3 aim = Player_AimPoint(g);
        Vector3 origin = (Vector3){g->player.position.x, g->player.position.y + 1.2f, g->player.position.z};
        Vector3 dir = Vector3Subtract(aim, origin);
        float dist = Vector3Length(dir);
        if (dist < 0.1f) dir = (Vector3){sinf(g->player.yaw) * 8, 4, cosf(g->player.yaw) * 8};
        else {
            dir = Vector3Normalize(dir);
            dir = Vector3Scale(dir, Clamp(dist * 1.2f, 8.0f, 18.0f));
            dir.y = 6.0f + Clamp(dist * 0.15f, 0.0f, 4.0f);
        }
        g->grenades[i].active = true;
        g->grenades[i].position = origin;
        g->grenades[i].velocity = dir;
        g->grenades[i].fuse = 1.6f;
        g->grenades[i].blastRadius = 3.5f;
        g->grenades[i].damage = 55.0f;
        g->player.grenades--;
        return;
    }
}

void Weapons_Update(GameState *g, float dt) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        Bullet *b = &g->bullets[i];
        if (!b->active) continue;
        b->life -= dt;
        b->position = Vector3Add(b->position, Vector3Scale(b->velocity, dt));
        if (b->life <= 0.0f || b->position.y < -1.0f) { b->active = false; continue; }
        /* obstacle hit */
        bool blocked = false;
        for (int o = 0; o < g->obstacleCount; o++) {
            if (CheckCollisionBoxSphere(g->obstacles[o].box, b->position, b->radius)) {
                blocked = true; break;
            }
        }
        if (blocked) { b->active = false; continue; }
        /* XZ hit only — bullets fly at chest height, monster pivot is on ground */
        int hit = -1;
        float hitDist = 9999.0f;
        for (int m = 0; m < MAX_MONSTERS; m++) {
            if (!g->monsters[m].active || g->monsters[m].phased) continue;
            MonsterDef def;
            MonsterData_Get(g->monsters[m].typeId, &def);
            float r = fmaxf(0.9f, def.radius * g->monsters[m].scale + 0.35f);
            float dx = b->position.x - g->monsters[m].position.x;
            float dz = b->position.z - g->monsters[m].position.z;
            float d = sqrtf(dx * dx + dz * dz);
            if (d < b->radius + r && d < hitDist) {
                hitDist = d;
                hit = m;
            }
        }
        if (hit >= 0) {
            Monsters_DamageOne(g, hit, b->damage);
            AudioSfx_Hit();
            if (g->player.costumeMonsterId >= 0) {
                MonsterDef cdef;
                MonsterData_Get(g->player.costumeMonsterId, &cdef);
                if (cdef.special == SPEC_LIFESTEAL) {
                    g->player.hp = Clamp(g->player.hp + b->damage * cdef.specialValue, 0, g->player.maxHp);
                } else if (cdef.special == SPEC_POISON) {
                    g->monsters[hit].poisonTimer = fmaxf(g->monsters[hit].poisonTimer, 2.5f);
                } else if (cdef.special == SPEC_SPIKE) {
                    Monsters_DamageOne(g, hit, cdef.specialValue * 0.5f);
                }
            }
            b->active = false;
        }
    }

    for (int i = 0; i < MAX_GRENADES; i++) {
        Grenade *gr = &g->grenades[i];
        if (!gr->active) continue;
        gr->velocity.y -= GRAVITY * dt;
        gr->position = Vector3Add(gr->position, Vector3Scale(gr->velocity, dt));
        if (gr->position.y < 0.2f) {
            gr->position.y = 0.2f;
            gr->velocity.y *= -0.35f;
            gr->velocity.x *= 0.7f;
            gr->velocity.z *= 0.7f;
        }
        World_ResolveCircle(g, &gr->position, 0.2f);
        gr->fuse -= dt;
        if (gr->fuse <= 0.0f) {
            GameSpawnParticle(g, gr->position, ORANGE, 2.0f, 0.4f);
            GameSpawnParticle(g, gr->position, YELLOW, 1.2f, 0.3f);
            AudioSfx_Grenade();
            Monsters_DamageAt(g, gr->position, gr->blastRadius, gr->damage, true);
            /* player friendly-fire light knock */
            if (Vector3Distance(g->player.position, gr->position) < gr->blastRadius) {
                float falloff = 1.0f - Vector3Distance(g->player.position, gr->position) / gr->blastRadius;
                float dmg = gr->damage * 0.25f * falloff;
                if (g->player.shieldHp > 0) {
                    float absorb = fminf(g->player.shieldHp, dmg);
                    g->player.shieldHp -= absorb;
                    dmg -= absorb;
                }
                g->player.hp -= dmg;
            }
            gr->active = false;
        }
    }
}

void Weapons_Draw(GameState *g) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!g->bullets[i].active) continue;
        DrawSphere(g->bullets[i].position, g->bullets[i].radius, GOLD);
    }
    for (int i = 0; i < MAX_GRENADES; i++) {
        if (!g->grenades[i].active) continue;
        DrawSphere(g->grenades[i].position, 0.22f, DARKGREEN);
        DrawSphereWires(g->grenades[i].position, 0.22f, 6, 6, LIME);
    }
}
