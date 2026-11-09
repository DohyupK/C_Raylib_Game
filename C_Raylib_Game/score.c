#include "game.h"
#include <stdio.h>
#include <string.h>

#define RANK_FILE "save/ranking.txt"

void Score_Init(GameState *g) {
    g->score.score = 0;
    g->score.kills = 0;
    g->score.bossKills = 0;
    g->score.combo = 0;
    g->score.comboTimer = 0;
    if (g->ranking.count > 0) g->score.highScore = g->ranking.scores[0];
    else g->score.highScore = 0;
}

void Ranking_Load(GameState *g) {
    memset(&g->ranking, 0, sizeof(g->ranking));
    if (!DirectoryExists("save")) MakeDirectory("save");
    if (!FileExists(RANK_FILE)) return;
    char *text = LoadFileText(RANK_FILE);
    if (!text) return;
    char *p = text;
    while (*p && g->ranking.count < RANK_COUNT) {
        int score = 0, kills = 0;
        if (sscanf(p, "%d %d", &score, &kills) >= 1) {
            g->ranking.scores[g->ranking.count] = score;
            g->ranking.kills[g->ranking.count] = kills;
            g->ranking.count++;
        }
        while (*p && *p != '\n') p++;
        if (*p == '\n') p++;
    }
    UnloadFileText(text);
    if (g->ranking.count > 0) g->score.highScore = g->ranking.scores[0];
}

void Ranking_Save(GameState *g) {
    if (!DirectoryExists("save")) MakeDirectory("save");
    char buf[512];
    int n = 0;
    for (int i = 0; i < g->ranking.count; i++) {
        n += sprintf(buf + n, "%d %d\n", g->ranking.scores[i], g->ranking.kills[i]);
        if (n >= (int)sizeof(buf) - 32) break;
    }
    SaveFileText(RANK_FILE, buf);
}

void Ranking_Submit(GameState *g) {
    int score = g->score.score;
    int kills = g->score.kills;
    if (score <= 0) return;

    /* insert sorted descending */
    int insert = g->ranking.count;
    for (int i = 0; i < g->ranking.count; i++) {
        if (score > g->ranking.scores[i]) { insert = i; break; }
    }
    if (insert >= RANK_COUNT) return;

    int newCount = g->ranking.count < RANK_COUNT ? g->ranking.count + 1 : RANK_COUNT;
    for (int i = newCount - 1; i > insert; i--) {
        g->ranking.scores[i] = g->ranking.scores[i - 1];
        g->ranking.kills[i] = g->ranking.kills[i - 1];
    }
    g->ranking.scores[insert] = score;
    g->ranking.kills[insert] = kills;
    g->ranking.count = newCount;
    if (g->ranking.scores[0] > g->score.highScore) g->score.highScore = g->ranking.scores[0];
    Ranking_Save(g);
}

void Score_AddKill(GameState *g, int points, Vector3 at) {
    if (g->score.comboTimer > 0.0f) g->score.combo++;
    else g->score.combo = 1;
    g->score.comboTimer = 2.5f;
    int gained = points * (1 + g->score.combo / 5);
    g->score.score += gained;
    g->score.kills++;
    if (g->score.score > g->score.highScore) g->score.highScore = g->score.score;
    char buf[48];
    snprintf(buf, sizeof(buf), "+%d", gained);
    GameAddFloatText(g, (Vector3){at.x, 2.0f, at.z}, g->camera, buf, GOLD);
}

void Score_DrawHud(GameState *g) {
    if (g->score.comboTimer > 0.0f) g->score.comboTimer -= GetFrameTime();
    else g->score.combo = 0;

    Ui_Draw(TextFormat(L("SCORE"), g->score.score), SCREEN_W - 260, 20, 28, GOLD);
    Ui_Draw(TextFormat(L("HIGH"), g->score.highScore), SCREEN_W - 260, 52, 18, LIGHTGRAY);
    Ui_Draw(TextFormat(L("KILLS"), g->score.kills), SCREEN_W - 260, 74, 18, LIGHTGRAY);
    if (g->score.combo > 1) {
        Ui_Draw(TextFormat(L("COMBO"), g->score.combo), SCREEN_W - 260, 98, 22, ORANGE);
    }
}
