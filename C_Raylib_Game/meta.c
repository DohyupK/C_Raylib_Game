#include "game.h"
#include <stdio.h>
#include <string.h>

static const char *ACH_KEYS[8] = {
    "ACH_FIRST", "ACH_COMBO", "ACH_BOSS", "ACH_COLLECT",
    "ACH_SURVIVE", "ACH_GRENADE", "ACH_FASHION", "ACH_SCORE"
};

static int grenadesThrown = 0;

void Meta_Init(GameState *g) {
    memset(g->achievements.unlocked, 0, sizeof(g->achievements.unlocked));
    memset(g->achievements.notified, 0, sizeof(g->achievements.notified));
    memset(g->codexUnlocked, 0, sizeof(g->codexUnlocked));
    grenadesThrown = 0;
    g->player.costume = COSTUME_DEFAULT;
    g->player.costumeMonsterId = -1;
}

static void Unlock(GameState *g, int id) {
    if (id < 0 || id >= 8) return;
    if (g->achievements.unlocked[id]) return;
    g->achievements.unlocked[id] = true;
    GameShowMessage(g, TextFormat(L("ACH_FMT"), L(ACH_KEYS[id])), 2.5f);
}

Color Meta_CostumeColor(CostumeId id) {
    switch (id) {
    case COSTUME_SCOUT: return (Color){80, 180, 255, 255};
    case COSTUME_HEAVY: return (Color){180, 100, 60, 255};
    case COSTUME_NIGHT: return (Color){40, 40, 70, 255};
    case COSTUME_GOLD: return (Color){240, 200, 40, 255};
    default: return (Color){60, 140, 220, 255};
    }
}

const char *Meta_CostumeName(CostumeId id) {
    (void)id;
    return "기본";
}

void Meta_CycleCostume(GameState *g) {
    GameShowMessage(g, L("MSG_ESC_COSTUME"), 1.5f);
}

void Meta_OnKill(GameState *g, bool wasBoss) {
    Unlock(g, 0);
    if (g->score.combo >= 5) Unlock(g, 1);
    if (wasBoss) {
        g->score.bossKills++;
        Unlock(g, 2);
    }
    if (g->score.score >= 500) Unlock(g, 7);
    if (g->player.costumeMonsterId >= 0) Unlock(g, 6);

    if (g->score.kills > 0 && g->score.kills % 100 == 0) {
        GameShowMessage(g, TextFormat(L("MSG_DIFF"), GameDiffLevel(g)), 2.5f);
    } else if (g->score.kills > 0 && g->score.kills % 50 == 0) {
        GameShowMessage(g, TextFormat(L("MSG_BG"), GameBgTheme(g) + 1), 2.0f);
    }
}

void Meta_Update(GameState *g) {
    if (g->gameTime >= 60.0f) Unlock(g, 4);
    if (IsKeyPressed(KEY_Z)) {
        grenadesThrown++;
        if (grenadesThrown >= 10) Unlock(g, 5);
    }
    int kinds = 0;
    bool seen[ITEM_COUNT] = {0};
    for (int i = 0; i < INV_SLOTS; i++) {
        ItemKind k = g->inventory.slots[i].kind;
        if (k != ITEM_EMPTY && !seen[k]) { seen[k] = true; kinds++; }
    }
    if (kinds >= 4) Unlock(g, 3);
}

void Meta_DrawHud(GameState *g) {
    int unlocked = 0;
    for (int i = 0; i < 8; i++) if (g->achievements.unlocked[i]) unlocked++;
    int books = 0;
    for (int i = 0; i < MONSTER_TYPE_COUNT; i++) if (g->codexUnlocked[i]) books++;
    Ui_Draw(TextFormat(L("ACH"), unlocked, books, MONSTER_TYPE_COUNT), 20, SCREEN_H - 120, 16, SKYBLUE);
}
