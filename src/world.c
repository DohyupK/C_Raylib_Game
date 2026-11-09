#include "game.h"
#include <math.h>

static void AddBuilding(GameState *g, float x, float z, float w, float h, float d, int modelId, const char *nameKr, Color c) {
    if (g->obstacleCount >= MAX_OBSTACLES) return;
    Obstacle *o = &g->obstacles[g->obstacleCount++];
    o->solid = true;
    o->box.min = (Vector3){x - w * 0.5f, 0.0f, z - d * 0.5f};
    o->box.max = (Vector3){x + w * 0.5f, h, z + d * 0.5f};
    o->color = c;
    o->modelId = modelId;
    o->nameKr = nameKr;
}

void World_Init(GameState *g) {
    g->obstacleCount = 0;
    AddBuilding(g, 8.0f, 6.0f, 5.0f, 4.0f, 5.0f, 0, L("B0"), (Color){90, 90, 100, 255});
    AddBuilding(g, -10.0f, 4.0f, 4.5f, 3.5f, 4.5f, 1, L("B1"), (Color){80, 95, 80, 255});
    AddBuilding(g, 0.0f, -12.0f, 6.0f, 4.2f, 5.0f, 2, L("B2"), (Color){110, 80, 70, 255});
    AddBuilding(g, 14.0f, -8.0f, 4.0f, 3.8f, 4.0f, 3, L("B3"), (Color){70, 70, 90, 255});
    AddBuilding(g, -15.0f, -10.0f, 5.0f, 3.2f, 5.5f, 4, L("B4"), (Color){95, 85, 75, 255});
    AddBuilding(g, 5.0f, 14.0f, 5.0f, 3.0f, 5.0f, 5, L("B5"), (Color){85, 100, 110, 255});
    AddBuilding(g, -6.0f, 12.0f, 3.5f, 2.8f, 3.5f, 6, L("B6"), (Color){100, 70, 70, 255});
    AddBuilding(g, 18.0f, 2.0f, 3.8f, 3.0f, 4.2f, 7, L("B7"), (Color){75, 85, 95, 255});
    AddBuilding(g, -18.0f, 8.0f, 4.0f, 3.0f, 4.0f, 1, L("B1"), (Color){88, 88, 70, 255});
    AddBuilding(g, 10.0f, -16.0f, 4.5f, 3.2f, 4.0f, 6, L("B6"), (Color){92, 92, 105, 255});
    AddBuilding(g, -8.0f, -18.0f, 4.0f, 3.0f, 4.0f, 7, L("B7"), (Color){70, 90, 70, 255});
    AddBuilding(g, 0.0f, 0.0f, 2.8f, 1.6f, 2.8f, -1, "Cover", (Color){120, 120, 60, 255});

    for (int i = -2; i <= 2; i++) {
        AddBuilding(g, (float)i * 8.0f, 24.0f, 6.0f, 3.0f, 1.8f, -1, "Wall", (Color){60, 60, 65, 255});
        AddBuilding(g, (float)i * 8.0f, -24.0f, 6.0f, 3.0f, 1.8f, -1, "Wall", (Color){60, 60, 65, 255});
        AddBuilding(g, 24.0f, (float)i * 8.0f, 1.8f, 3.0f, 6.0f, -1, "Wall", (Color){60, 60, 65, 255});
        AddBuilding(g, -24.0f, (float)i * 8.0f, 1.8f, 3.0f, 6.0f, -1, "Wall", (Color){60, 60, 65, 255});
    }
}

bool World_ResolveCircle(GameState *g, Vector3 *pos, float radius) {
    bool hit = false;
    for (int i = 0; i < g->obstacleCount; i++) {
        Obstacle *o = &g->obstacles[i];
        if (!o->solid) continue;
        float cx = Clamp(pos->x, o->box.min.x, o->box.max.x);
        float cz = Clamp(pos->z, o->box.min.z, o->box.max.z);
        float dx = pos->x - cx;
        float dz = pos->z - cz;
        float distSq = dx * dx + dz * dz;
        if (distSq < radius * radius) {
            hit = true;
            if (distSq < 0.0001f) {
                float left = fabsf(pos->x - o->box.min.x);
                float right = fabsf(o->box.max.x - pos->x);
                float nearZ = fabsf(pos->z - o->box.min.z);
                float farZ = fabsf(o->box.max.z - pos->z);
                float m = fminf(fminf(left, right), fminf(nearZ, farZ));
                if (m == left) pos->x = o->box.min.x - radius;
                else if (m == right) pos->x = o->box.max.x + radius;
                else if (m == nearZ) pos->z = o->box.min.z - radius;
                else pos->z = o->box.max.z + radius;
            } else {
                float dist = sqrtf(distSq);
                float push = (radius - dist) / dist;
                pos->x += dx * push;
                pos->z += dz * push;
            }
        }
    }
    pos->x = Clamp(pos->x, -22.0f, 22.0f);
    pos->z = Clamp(pos->z, -22.0f, 22.0f);
    return hit;
}

void World_Draw(GameState *g) {
    DrawPlane((Vector3){0, 0, 0}, (Vector2){60, 60}, g->groundColor);
    DrawGrid(60, 1.0f);
    for (int i = 0; i < g->obstacleCount; i++) {
        Assets_DrawBuilding(&g->obstacles[i]);
    }
}

void Game_UpdateBackground(GameState *g) {
    static const Color grounds[BG_THEME_COUNT] = {
        {42, 48, 42, 255}, {55, 40, 40, 255}, {35, 45, 60, 255}, {50, 45, 30, 255},
        {30, 50, 45, 255}, {60, 35, 55, 255}, {40, 40, 40, 255}, {45, 55, 35, 255}
    };
    static const Color clears[BG_THEME_COUNT] = {
        {20, 22, 28, 255}, {28, 16, 16, 255}, {12, 18, 32, 255}, {26, 22, 14, 255},
        {14, 26, 22, 255}, {28, 14, 26, 255}, {18, 18, 20, 255}, {16, 24, 16, 255}
    };
    int t = GameBgTheme(g);
    g->groundColor = grounds[t];
    g->bgClear = clears[t];
}
