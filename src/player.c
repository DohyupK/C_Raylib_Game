#include "game.h"
#include <math.h>

void Player_Init(GameState *g) {
    Player *p = &g->player;
    p->position = (Vector3){0.0f, 0.0f, 0.0f};
    p->velocity = (Vector3){0};
    p->yaw = 0.0f;
    p->maxHp = 100.0f;
    p->hp = p->maxHp;
    p->dashTimer = 0.0f;
    p->dashCooldown = 0.0f;
    p->grounded = true;
    p->weapon = WEAPON_PISTOL;
    p->fireCooldown = 0.0f;
    p->invulnTimer = 0.0f;
    p->speedMul = 1.0f;
    p->speedTimer = 0.0f;
    p->shieldHp = 0.0f;
    p->berserkTimer = 0.0f;
    p->overchargeTimer = 0.0f;
    for (int i = 0; i < 4; i++) p->abilityCd[i] = 0.0f;
    p->costume = COSTUME_DEFAULT;
    p->costumeMonsterId = -1;
    p->costumeSpecialCd = 0.0f;
    p->grenades = 5;
}

Vector3 Player_AimPoint(GameState *g) {
    Ray ray = GetScreenToWorldRay(GetMousePosition(), g->camera);
    if (fabsf(ray.direction.y) < 0.0001f) {
        return (Vector3){g->player.position.x, 0, g->player.position.z + 1};
    }
    float t = -ray.position.y / ray.direction.y;
    if (t < 0.0f) t = 0.0f;
    return Vector3Add(ray.position, Vector3Scale(ray.direction, t));
}

void Player_Update(GameState *g, float dt) {
    Player *p = &g->player;
    if (g->gameOver || g->inventory.open) {
        /* still apply gravity while bag open */
    }

    Vector3 aim = Player_AimPoint(g);
    Vector3 toAim = Vector3Subtract(aim, p->position);
    toAim.y = 0;
    if (Vector3Length(toAim) > 0.1f) {
        p->yaw = atan2f(toAim.x, toAim.z);
    }

    float inputX = 0.0f, inputZ = 0.0f;
    if (!g->inventory.open && !g->gameOver) {
        if (IsKeyDown(KEY_D)) inputX += 1.0f;
        if (IsKeyDown(KEY_A)) inputX -= 1.0f;
        if (IsKeyDown(KEY_W)) inputZ += 1.0f;
        if (IsKeyDown(KEY_S)) inputZ -= 1.0f;
    }
    /* camera-relative WASD so W always moves "up" on screen */
    Vector3 camForward = Vector3Subtract(g->camera.target, g->camera.position);
    camForward.y = 0.0f;
    if (Vector3Length(camForward) < 0.001f) camForward = (Vector3){0, 0, -1};
    camForward = Vector3Normalize(camForward);
    Vector3 camRight = (Vector3){-camForward.z, 0.0f, camForward.x};
    Vector3 wish = Vector3Add(Vector3Scale(camForward, inputZ), Vector3Scale(camRight, inputX));
    if (Vector3Length(wish) > 0.0f) wish = Vector3Normalize(wish);

    float speed = MOVE_SPEED * p->speedMul;
    if (p->berserkTimer > 0.0f) speed *= 1.35f;
    if (p->costumeMonsterId >= 0) {
        MonsterDef cdef;
        MonsterData_Get(p->costumeMonsterId, &cdef);
        if (cdef.special == SPEC_RAGE_SPEED) speed *= 1.25f;
        if (cdef.special == SPEC_ARMORED) speed *= 0.9f;
    }

    if (p->dashTimer > 0.0f) {
        p->dashTimer -= dt;
        p->velocity.x = wish.x * DASH_SPEED;
        p->velocity.z = wish.z * DASH_SPEED;
        if (Vector3Length(wish) < 0.01f) {
            Vector3 f = (Vector3){sinf(p->yaw), 0, cosf(p->yaw)};
            p->velocity.x = f.x * DASH_SPEED;
            p->velocity.z = f.z * DASH_SPEED;
        }
    } else {
        p->velocity.x = wish.x * speed;
        p->velocity.z = wish.z * speed;
    }

    if (p->dashCooldown > 0.0f) p->dashCooldown -= dt;
    if (!g->inventory.open && !g->gameOver && IsKeyPressed(KEY_LEFT_SHIFT) && p->dashCooldown <= 0.0f) {
        p->dashTimer = DASH_TIME;
        p->dashCooldown = DASH_COOLDOWN;
        if (p->costume == COSTUME_SCOUT) p->dashCooldown *= 0.7f;
    }

    if (!g->inventory.open && !g->gameOver && IsKeyPressed(KEY_SPACE) && p->grounded) {
        p->velocity.y = JUMP_VELOCITY;
        p->grounded = false;
    }

    p->velocity.y -= GRAVITY * dt;
    p->position.x += p->velocity.x * dt;
    p->position.z += p->velocity.z * dt;
    p->position.y += p->velocity.y * dt;
    if (p->position.y <= 0.0f) {
        p->position.y = 0.0f;
        p->velocity.y = 0.0f;
        p->grounded = true;
    }

    World_ResolveCircle(g, &p->position, PLAYER_RADIUS);

    if (p->fireCooldown > 0.0f) p->fireCooldown -= dt;
    if (p->invulnTimer > 0.0f) p->invulnTimer -= dt;
    if (p->speedTimer > 0.0f) {
        p->speedTimer -= dt;
        if (p->speedTimer <= 0.0f) p->speedMul = 1.0f;
    }
    if (p->berserkTimer > 0.0f) p->berserkTimer -= dt;
    if (p->overchargeTimer > 0.0f) p->overchargeTimer -= dt;
    for (int i = 0; i < 4; i++) {
        if (p->abilityCd[i] > 0.0f) p->abilityCd[i] -= dt;
    }

    Abilities_Update(g, dt);

    /* costume monster special passive */
    if (p->costumeSpecialCd > 0.0f) p->costumeSpecialCd -= dt;
    if (p->costumeMonsterId >= 0 && p->costumeSpecialCd <= 0.0f) {
        MonsterDef cdef;
        MonsterData_Get(p->costumeMonsterId, &cdef);
        if (cdef.special == SPEC_HEAL_AURA) {
            p->hp = Clamp(p->hp + 2.0f, 0, p->maxHp);
            p->costumeSpecialCd = 2.0f;
        } else if (cdef.special == SPEC_SLOW_WRAP || cdef.special == SPEC_MUD_POOL) {
            for (int i = 0; i < MAX_MONSTERS; i++) {
                if (!g->monsters[i].active) continue;
                if (Vector3Distance(p->position, g->monsters[i].position) < 5.0f)
                    g->monsters[i].slowTimer = 1.2f;
            }
            p->costumeSpecialCd = 3.5f;
        } else if (cdef.special == SPEC_PHASE && IsKeyPressed(KEY_LEFT_CONTROL)) {
            p->invulnTimer = cdef.specialValue;
            p->costumeSpecialCd = 8.0f;
            GameShowMessage(g, L("MSG_PHASE"), 0.8f);
        } else if (cdef.special == SPEC_SUMMON && IsKeyPressed(KEY_LEFT_CONTROL)) {
            p->shieldHp = fmaxf(p->shieldHp, 20.0f);
            p->costumeSpecialCd = 10.0f;
            GameShowMessage(g, L("MSG_SUMMON"), 0.8f);
        }
    }

    if (!g->inventory.open && !g->gameOver && !g->paused) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) Weapons_TryFire(g);
        if (IsKeyPressed(KEY_Z)) Weapons_ThrowGrenade(g);
        if (IsKeyPressed(KEY_X)) Weapons_Cycle(g);
        if (IsKeyPressed(KEY_C)) {
            float best = 2.2f;
            int bestI = -1;
            for (int i = 0; i < MAX_PICKUPS; i++) {
                if (!g->pickups[i].active) continue;
                float d = Vector3Distance(p->position, g->pickups[i].position);
                if (d < best) { best = d; bestI = i; }
            }
            if (bestI >= 0) {
                Pickup *pk = &g->pickups[bestI];
                if (pk->kind == ITEM_MONSTER_BOOK) {
                    Codex_Unlock(g, pk->metaTypeId);
                    AudioSfx_Item();
                    pk->active = false;
                } else if (Inventory_Add(g, pk->kind, 1)) {
                    GameShowMessage(g, TextFormat(L("MSG_GOT"), Item_Name(pk->kind)), 1.2f);
                    pk->active = false;
                }
            }
        }
        if (IsKeyPressed(KEY_B)) g->inventory.open = !g->inventory.open;
        if (IsKeyPressed(KEY_Q)) Abilities_Try(g, ABILITY_BERSERK);
        if (IsKeyPressed(KEY_E)) Abilities_Try(g, ABILITY_BARRIER);
        if (IsKeyPressed(KEY_R)) Abilities_Try(g, ABILITY_OVERCHARGE);
        for (int k = 0; k < HOTBAR_SLOTS; k++) {
            if (IsKeyPressed(KEY_ONE + k)) Inventory_UseHotbar(g, k);
        }
    } else if (!g->gameOver && !g->paused && IsKeyPressed(KEY_B)) {
        g->inventory.open = false;
        g->inventory.dragging = false;
        g->inventory.dragFrom = -1;
    }
}

void Player_Draw(GameState *g) {
    Assets_DrawPlayer(g);
}
