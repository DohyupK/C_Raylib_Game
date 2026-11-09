#include "game.h"
#include <stdio.h>
#include <string.h>

void GameShowMessage(GameState *g, const char *msg, float seconds) {
    strncpy(g->message, msg, sizeof(g->message) - 1);
    g->message[sizeof(g->message) - 1] = '\0';
    g->messageTimer = seconds;
}

void GameAddFloatText(GameState *g, Vector3 worldPos, Camera3D cam, const char *text, Color color) {
    Vector2 sp = GetWorldToScreen(worldPos, cam);
    for (int i = 0; i < MAX_FLOAT_TEXTS; i++) {
        if (!g->floatTexts[i].active) {
            g->floatTexts[i].active = true;
            g->floatTexts[i].screenPos = sp;
            g->floatTexts[i].color = color;
            g->floatTexts[i].life = 1.0f;
            strncpy(g->floatTexts[i].text, text, sizeof(g->floatTexts[i].text) - 1);
            g->floatTexts[i].text[sizeof(g->floatTexts[i].text) - 1] = '\0';
            return;
        }
    }
}

void GameSpawnParticle(GameState *g, Vector3 pos, Color color, float size, float life) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!g->particles[i].active) {
            g->particles[i].active = true;
            g->particles[i].position = pos;
            g->particles[i].color = color;
            g->particles[i].size = size;
            g->particles[i].life = life;
            g->particles[i].maxLife = life;
            return;
        }
    }
}

void UtilUpdateEffects(GameState *g, float dt) {
    if (g->messageTimer > 0.0f) g->messageTimer -= dt;
    for (int i = 0; i < MAX_FLOAT_TEXTS; i++) {
        if (!g->floatTexts[i].active) continue;
        g->floatTexts[i].life -= dt;
        g->floatTexts[i].screenPos.y -= 40.0f * dt;
        if (g->floatTexts[i].life <= 0.0f) g->floatTexts[i].active = false;
    }
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!g->particles[i].active) continue;
        g->particles[i].life -= dt;
        g->particles[i].position.y += 1.5f * dt;
        if (g->particles[i].life <= 0.0f) g->particles[i].active = false;
    }
}

void UtilDrawEffects(GameState *g) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!g->particles[i].active) continue;
        float a = g->particles[i].life / g->particles[i].maxLife;
        Color c = g->particles[i].color;
        c.a = (unsigned char)(a * 255);
        DrawSphere(g->particles[i].position, g->particles[i].size * a, c);
    }
}

void UtilDrawFloatTexts(GameState *g) {
    for (int i = 0; i < MAX_FLOAT_TEXTS; i++) {
        if (!g->floatTexts[i].active) continue;
        Color c = g->floatTexts[i].color;
        c.a = (unsigned char)(g->floatTexts[i].life * 255);
        Ui_Draw(g->floatTexts[i].text, g->floatTexts[i].screenPos.x,
                g->floatTexts[i].screenPos.y, 18, c);
    }
}
