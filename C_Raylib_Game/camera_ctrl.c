#include "game.h"

void Camera_Init(GameState *g) {
    g->camera.position = (Vector3){0.0f, 18.0f, 14.0f};
    g->camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    g->camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    g->camera.fovy = 45.0f;
    g->camera.projection = CAMERA_PERSPECTIVE;
}

void Camera_Update(GameState *g) {
    Vector3 p = g->player.position;
    Vector3 desired = (Vector3){p.x, p.y + 16.0f, p.z + 12.0f};
    g->camera.position = Vector3Lerp(g->camera.position, desired, 0.12f);
    g->camera.target = Vector3Lerp(g->camera.target, (Vector3){p.x, 0.5f, p.z}, 0.15f);
}
