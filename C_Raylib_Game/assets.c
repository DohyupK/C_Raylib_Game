#include "game.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define ENEMY_MODEL_COUNT 30

static Model gPlayerModel;
static Model gEnemyModels[ENEMY_MODEL_COUNT];
static Model gBuildings[BUILDING_MODEL_COUNT];
static float gPlayerScale = 1.0f;
static float gEnemyScales[ENEMY_MODEL_COUNT];
static float gBuildingScales[BUILDING_MODEL_COUNT];
static Texture2D gItemIcons[ITEM_COUNT];
static Model gItemModels[ITEM_COUNT];
static float gItemScales[ITEM_COUNT];
static bool gAssetsLoaded = false;

static const char *ItemModelFiles[ITEM_COUNT] = {
    NULL,
    "assets/models/items/medkit.glb",
    "assets/models/items/ammo.glb",
    "assets/models/items/speed.glb",
    "assets/models/items/shield.glb",
    "assets/models/items/grenade.glb",
    "assets/models/items/gem.glb",
    "assets/models/items/book.glb",
};

static const char *EnemyFiles[ENEMY_MODEL_COUNT] = {
    "assets/models/m00_frog.glb", "assets/models/m01_rat.glb", "assets/models/m02_snake.glb",
    "assets/models/m03_spider.glb", "assets/models/m04_wasp.glb", "assets/models/m05_snake_angry.glb",
    "assets/models/m06_wolf.glb", "assets/models/m07_fox.glb", "assets/models/m08_husky.glb",
    "assets/models/m09_bull.glb", "assets/models/m10_raptor.glb", "assets/models/m11_trex.glb",
    "assets/models/m12_tri.glb", "assets/models/m13_stego.glb", "assets/models/m14_legs.glb",
    "assets/models/m15_legs_gun.glb", "assets/models/m16_fly.glb", "assets/models/m17_fly_gun.glb",
    "assets/models/m18_large.glb", "assets/models/m19_large_gun.glb", "assets/models/m20_mech_g.glb",
    "assets/models/m21_mech_l.glb", "assets/models/m22_mech_m.glb", "assets/models/m23_mech_s.glb",
    "assets/models/m24_bee.glb", "assets/models/m25_crab.glb", "assets/models/m26_char.glb",
    "assets/models/m27_deer.glb", "assets/models/m28_stag.glb", "assets/models/m29_para.glb",
};

static const char *BuildingFiles[BUILDING_MODEL_COUNT] = {
    "assets/models/buildings/Building1_Large.glb",
    "assets/models/buildings/Building2_Large.glb",
    "assets/models/buildings/Building3_Big.glb",
    "assets/models/buildings/Building4.glb",
    "assets/models/buildings/House1.glb",
    "assets/models/buildings/House2.glb",
    "assets/models/buildings/Building1_Small.glb",
    "assets/models/buildings/Building2_Small.glb",
};

static float FitScale(Model model, float targetHeight) {
    BoundingBox bb = GetModelBoundingBox(model);
    float h = bb.max.y - bb.min.y;
    float w = bb.max.x - bb.min.x;
    float d = bb.max.z - bb.min.z;
    float biggest = fmaxf(h, fmaxf(w, d));
    if (biggest < 0.0001f) return 1.0f;
    return targetHeight / biggest;
}

static Texture2D MakeItemIcon(Color base, Color accent, int style) {
    Image img = GenImageColor(64, 64, (Color){30, 30, 36, 255});
    ImageDrawRectangle(&img, 4, 4, 56, 56, base);
    if (style == 0) { /* cross medkit */
        ImageDrawRectangle(&img, 28, 12, 8, 40, accent);
        ImageDrawRectangle(&img, 12, 28, 40, 8, accent);
    } else if (style == 1) { /* ammo box */
        ImageDrawRectangle(&img, 16, 20, 32, 28, accent);
        ImageDrawRectangle(&img, 22, 14, 20, 10, WHITE);
    } else if (style == 2) { /* bolt */
        ImageDrawCircle(&img, 32, 32, 18, accent);
        ImageDrawCircle(&img, 32, 32, 8, YELLOW);
    } else if (style == 3) { /* shield */
        ImageDrawCircle(&img, 32, 34, 20, accent);
        ImageDrawRectangle(&img, 22, 14, 20, 16, Fade(WHITE, 0.7f));
    } else if (style == 4) { /* grenade */
        ImageDrawCircle(&img, 32, 36, 16, accent);
        ImageDrawRectangle(&img, 28, 12, 8, 14, DARKGRAY);
    } else if (style == 5) { /* gem */
        ImageDrawRectangle(&img, 20, 16, 24, 32, accent);
        ImageDrawRectangle(&img, 26, 22, 12, 20, WHITE);
    } else { /* book */
        ImageDrawRectangle(&img, 14, 12, 36, 40, accent);
        ImageDrawRectangle(&img, 18, 16, 28, 32, RAYWHITE);
        ImageDrawRectangle(&img, 22, 24, 20, 4, RED);
        ImageDrawRectangle(&img, 22, 32, 20, 4, RED);
    }
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

void Assets_Init(void) {
    memset(&gPlayerModel, 0, sizeof(gPlayerModel));
    memset(gEnemyModels, 0, sizeof(gEnemyModels));
    memset(gBuildings, 0, sizeof(gBuildings));
    memset(gItemIcons, 0, sizeof(gItemIcons));
    gAssetsLoaded = false;

    if (FileExists("assets/models/player.glb")) {
        gPlayerModel = LoadModel("assets/models/player.glb");
        gPlayerScale = FitScale(gPlayerModel, 1.8f);
    }

    int loaded = 0;
    for (int i = 0; i < ENEMY_MODEL_COUNT; i++) {
        if (!FileExists(EnemyFiles[i])) continue;
        gEnemyModels[i] = LoadModel(EnemyFiles[i]);
        float target = (i == 11 || i >= 20) ? 2.4f : 1.7f;
        gEnemyScales[i] = FitScale(gEnemyModels[i], target);
        loaded++;
    }

    for (int i = 0; i < BUILDING_MODEL_COUNT; i++) {
        if (!FileExists(BuildingFiles[i])) continue;
        gBuildings[i] = LoadModel(BuildingFiles[i]);
        gBuildingScales[i] = FitScale(gBuildings[i], 4.5f);
    }

    gItemIcons[ITEM_MEDKIT] = MakeItemIcon(RED, WHITE, 0);
    gItemIcons[ITEM_AMMO] = MakeItemIcon(GOLD, BROWN, 1);
    gItemIcons[ITEM_SPEED_BOOST] = MakeItemIcon(SKYBLUE, BLUE, 2);
    gItemIcons[ITEM_SHIELD] = MakeItemIcon(BLUE, SKYBLUE, 3);
    gItemIcons[ITEM_GRENADE_PACK] = MakeItemIcon(LIME, DARKGREEN, 4);
    gItemIcons[ITEM_SCORE_GEM] = MakeItemIcon(PURPLE, VIOLET, 5);
    gItemIcons[ITEM_MONSTER_BOOK] = MakeItemIcon(ORANGE, GOLD, 6);

    memset(gItemModels, 0, sizeof(gItemModels));
    for (int i = 1; i < ITEM_COUNT; i++) {
        if (!ItemModelFiles[i] || !FileExists(ItemModelFiles[i])) continue;
        gItemModels[i] = LoadModel(ItemModelFiles[i]);
        gItemScales[i] = FitScale(gItemModels[i], 0.85f);
    }

    gAssetsLoaded = loaded > 0;
    TraceLog(LOG_INFO, "Assets: %d enemies, buildings, item icons/models", loaded);
}

void Assets_Unload(void) {
    if (gPlayerModel.meshCount > 0) UnloadModel(gPlayerModel);
    for (int i = 0; i < ENEMY_MODEL_COUNT; i++)
        if (gEnemyModels[i].meshCount > 0) UnloadModel(gEnemyModels[i]);
    for (int i = 0; i < BUILDING_MODEL_COUNT; i++)
        if (gBuildings[i].meshCount > 0) UnloadModel(gBuildings[i]);
    for (int i = 0; i < ITEM_COUNT; i++) {
        if (gItemIcons[i].id > 0) UnloadTexture(gItemIcons[i]);
        if (gItemModels[i].meshCount > 0) UnloadModel(gItemModels[i]);
    }
    gAssetsLoaded = false;
}

bool Assets_Ready(void) { return gAssetsLoaded; }

Texture2D Assets_ItemIcon(ItemKind kind) {
    if (kind <= ITEM_EMPTY || kind >= ITEM_COUNT) return (Texture2D){0};
    return gItemIcons[kind];
}

void Assets_DrawItemIcon(ItemKind kind, Rectangle dest) {
    Texture2D t = Assets_ItemIcon(kind);
    if (t.id > 0) DrawTexturePro(t, (Rectangle){0, 0, (float)t.width, (float)t.height}, dest, (Vector2){0, 0}, 0, WHITE);
    else DrawRectangleRec(dest, Item_Color(kind));
}

void Assets_DrawPickup(ItemKind kind, Vector3 pos, float bob) {
    Color ic = Item_Color(kind);
    DrawCylinder((Vector3){pos.x, 0.02f, pos.z}, 0.45f, 0.45f, 0.05f, 12, Fade(ic, 0.55f));
    if (kind > ITEM_EMPTY && kind < ITEM_COUNT && gItemModels[kind].meshCount > 0) {
        float yaw = bob * 40.0f;
        float sc = gItemScales[kind];
        DrawModelEx(gItemModels[kind], pos, (Vector3){0, 1, 0}, yaw, (Vector3){sc, sc, sc}, WHITE);
    } else {
        DrawCube(pos, 0.4f, 0.4f, 0.4f, ic);
        DrawCubeWires(pos, 0.4f, 0.4f, 0.4f, WHITE);
    }
}

void Assets_DrawBuilding(const Obstacle *o) {
    if (o->modelId >= 0 && o->modelId < BUILDING_MODEL_COUNT && gBuildings[o->modelId].meshCount > 0) {
        Vector3 size = Vector3Subtract(o->box.max, o->box.min);
        Vector3 center = Vector3Add(o->box.min, Vector3Scale(size, 0.5f));
        center.y = 0.0f;
        float sc = gBuildingScales[o->modelId];
        /* fit roughly to footprint */
        float footprint = fmaxf(size.x, size.z);
        BoundingBox bb = GetModelBoundingBox(gBuildings[o->modelId]);
        float mw = fmaxf(bb.max.x - bb.min.x, bb.max.z - bb.min.z);
        if (mw > 0.01f) sc = footprint / mw;
        DrawModelEx(gBuildings[o->modelId], center, (Vector3){0, 1, 0}, 0,
                    (Vector3){sc, sc, sc}, WHITE);
    } else {
        Vector3 size = Vector3Subtract(o->box.max, o->box.min);
        Vector3 center = Vector3Add(o->box.min, Vector3Scale(size, 0.5f));
        DrawCube(center, size.x, size.y, size.z, o->color);
        DrawCubeWires(center, size.x, size.y, size.z, BLACK);
    }
}

void Assets_DrawPlayer(GameState *g) {
    Player *p = &g->player;
    Color tint = Meta_CostumeColor(p->costume);
    if (p->costumeMonsterId >= 0) {
        MonsterDef def;
        MonsterData_Get(p->costumeMonsterId, &def);
        tint = def.color;
    }
    if (p->invulnTimer > 0.0f && ((int)(g->gameTime * 20) % 2 == 0)) tint.a = 120;

    DrawCylinder((Vector3){p->position.x, 0.03f, p->position.z}, 0.9f, 0.9f, 0.06f, 16, Fade(LIME, 0.55f));
    DrawCircle3D((Vector3){p->position.x, 0.08f, p->position.z}, 1.1f, (Vector3){1, 0, 0}, 90, LIME);

    float yawDeg = p->yaw * RAD2DEG;
    if (p->costumeMonsterId >= 0) {
        int mid = p->costumeMonsterId % ENEMY_MODEL_COUNT;
        if (gEnemyModels[mid].meshCount > 0) {
            float sc = gEnemyScales[mid] * 0.95f;
            DrawModelEx(gEnemyModels[mid], p->position, (Vector3){0, 1, 0}, yawDeg,
                        (Vector3){sc, sc, sc}, tint);
        } else if (gPlayerModel.meshCount > 0) {
            DrawModelEx(gPlayerModel, p->position, (Vector3){0, 1, 0}, yawDeg,
                        (Vector3){gPlayerScale, gPlayerScale, gPlayerScale}, tint);
        }
    } else if (gPlayerModel.meshCount > 0) {
        DrawModelEx(gPlayerModel, p->position, (Vector3){0, 1, 0}, yawDeg,
                    (Vector3){gPlayerScale, gPlayerScale, gPlayerScale}, tint);
    } else {
        DrawCube((Vector3){p->position.x, 1.0f, p->position.z}, 0.7f, 1.4f, 0.5f, tint);
    }

    if (p->shieldHp > 0.0f) {
        DrawSphereWires((Vector3){p->position.x, 1.0f, p->position.z}, 1.2f, 10, 10, SKYBLUE);
    }
    Vector3 aim = Player_AimPoint(g);
    DrawCircle3D((Vector3){aim.x, 0.06f, aim.z}, 0.55f, (Vector3){1, 0, 0}, 90, YELLOW);
    DrawLine3D((Vector3){p->position.x, 0.2f, p->position.z}, (Vector3){aim.x, 0.2f, aim.z}, Fade(YELLOW, 0.35f));
}

void Assets_DrawMonster(GameState *g, Monster *m, const MonsterDef *def) {
    Color tint = def->color;
    if (m->phased) tint.a = 100;
    if (m->isBoss) tint = (Color){255, 70, 70, 255};
    float sMul = m->isBoss ? 1.6f : 1.0f;
    int modelId = m->typeId % ENEMY_MODEL_COUNT;
    DrawCylinder((Vector3){m->position.x, 0.03f, m->position.z},
                 0.55f * m->scale, 0.55f * m->scale, 0.05f, 12, Fade(RED, 0.45f));
    if (gEnemyModels[modelId].meshCount > 0) {
        float sc = gEnemyScales[modelId] * sMul * (0.85f + def->radius * 0.25f);
        Vector3 toP = Vector3Subtract(g->player.position, m->position);
        float yaw = atan2f(toP.x, toP.z) * RAD2DEG;
        DrawModelEx(gEnemyModels[modelId], m->position, (Vector3){0, 1, 0}, yaw,
                    (Vector3){sc, sc, sc}, tint);
    } else {
        DrawCube((Vector3){m->position.x, 0.8f * m->scale, m->position.z},
                 0.8f * m->scale, 1.2f * m->scale, 0.8f * m->scale, tint);
    }
    if (m->isBoss) {
        DrawCircle3D((Vector3){m->position.x, 0.05f, m->position.z}, 1.5f * m->scale,
                     (Vector3){1, 0, 0}, 90, ORANGE);
    }
}
