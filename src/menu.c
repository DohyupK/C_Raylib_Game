#include "game.h"
#include <stdio.h>

static const char *MENU_ACH_KEYS[ACHIEVE_COUNT] = {
    "ACH_FIRST", "ACH_COMBO", "ACH_BOSS", "ACH_COLLECT",
    "ACH_SURVIVE", "ACH_GRENADE", "ACH_FASHION", "ACH_SCORE"
};

void Codex_Unlock(GameState *g, int typeId) {
    if (typeId < 0 || typeId >= MONSTER_TYPE_COUNT) return;
    if (g->codexUnlocked[typeId]) return;
    g->codexUnlocked[typeId] = true;
    snprintf(g->codexPopup, sizeof(g->codexPopup), "%s",
             TextFormat(L("CODEX_POPUP"), LMonsterName(typeId)));
    g->codexPopupTimer = 3.5f;
    GameShowMessage(g, TextFormat(L("MSG_CODEX"), LMonsterName(typeId)), 2.2f);
    AudioSfx_Item();
}

static bool Btn(Rectangle r, const char *label, Color base) {
    Vector2 m = GetMousePosition();
    bool hot = CheckCollisionPointRec(m, r);
    DrawRectangleRec(r, hot ? Fade(base, 0.95f) : Fade(base, 0.75f));
    DrawRectangleLinesEx(r, 2, RAYWHITE);
    float tw = Ui_Measure(label, 20);
    Ui_Draw(label, r.x + (r.width - tw) * 0.5f, r.y + 12, 20, WHITE);
    return hot && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void Menu_Update(GameState *g) {
    if (!g->paused) return;
    if (IsKeyPressed(KEY_ONE)) g->menuPage = MENU_PAGE_PAUSE;
    if (IsKeyPressed(KEY_TWO)) {
        g->menuPage = MENU_PAGE_CODEX;
        snprintf(g->codexPopup, sizeof(g->codexPopup), "%s", L("CODEX_OPEN_NOTE"));
        g->codexPopupTimer = 2.0f;
    }
    if (IsKeyPressed(KEY_THREE)) g->menuPage = MENU_PAGE_COSTUME;
    if (IsKeyPressed(KEY_FOUR)) g->menuPage = MENU_PAGE_ACHIEVE;
    if (IsKeyPressed(KEY_FIVE)) g->menuPage = MENU_PAGE_RANKING;
    if (g->menuPage == MENU_PAGE_CODEX) {
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) g->codexIndex = (g->codexIndex + MONSTER_TYPE_COUNT - 1) % MONSTER_TYPE_COUNT;
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) g->codexIndex = (g->codexIndex + 1) % MONSTER_TYPE_COUNT;
    }
}

void Menu_Draw(GameState *g) {
    if (!g->paused) return;
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.62f));

    float panelX = 100, panelY = 60, panelW = SCREEN_W - 200, panelH = SCREEN_H - 120;
    DrawRectangleRounded((Rectangle){panelX, panelY, panelW, panelH}, 0.04f, 8, Fade((Color){25, 30, 45, 255}, 0.95f));
    DrawRectangleRoundedLinesEx((Rectangle){panelX, panelY, panelW, panelH}, 0.04f, 8, 2, SKYBLUE);

    float tabW = 150;
    if (Btn((Rectangle){panelX + 16, panelY + 16, tabW, 40}, L("PAUSE"), g->menuPage == MENU_PAGE_PAUSE ? DARKBLUE : GRAY))
        g->menuPage = MENU_PAGE_PAUSE;
    if (Btn((Rectangle){panelX + 16 + tabW + 8, panelY + 16, tabW, 40}, L("CODEX"), g->menuPage == MENU_PAGE_CODEX ? DARKGREEN : GRAY)) {
        g->menuPage = MENU_PAGE_CODEX;
        snprintf(g->codexPopup, sizeof(g->codexPopup), "%s", L("CODEX_OPEN_NOTE"));
        g->codexPopupTimer = 2.0f;
    }
    if (Btn((Rectangle){panelX + 16 + (tabW + 8) * 2, panelY + 16, tabW, 40}, L("COSTUME"), g->menuPage == MENU_PAGE_COSTUME ? MAROON : GRAY))
        g->menuPage = MENU_PAGE_COSTUME;
    if (Btn((Rectangle){panelX + 16 + (tabW + 8) * 3, panelY + 16, tabW, 40}, L("ACHIEVE"), g->menuPage == MENU_PAGE_ACHIEVE ? ORANGE : GRAY))
        g->menuPage = MENU_PAGE_ACHIEVE;
    if (Btn((Rectangle){panelX + 16 + (tabW + 8) * 4, panelY + 16, tabW, 40}, L("RANKING"), g->menuPage == MENU_PAGE_RANKING ? PURPLE : GRAY))
        g->menuPage = MENU_PAGE_RANKING;

    if (g->menuPage == MENU_PAGE_PAUSE) {
        Ui_Draw(L("GAME_PAUSED"), panelX + 40, panelY + 100, 36, YELLOW);
        Ui_Draw(TextFormat(L("SCORE_KILL_DIFF"), g->score.score, g->score.kills, GameDiffLevel(g)),
                panelX + 40, panelY + 150, 22, LIGHTGRAY);
        if (Btn((Rectangle){panelX + 40, panelY + 220, 220, 50}, L("RESUME"), DARKGREEN)) {
            g->paused = false;
            AudioSfx_Pause();
        }
        Btn((Rectangle){panelX + 40, panelY + 290, 220, 50}, L("QUIT"), MAROON);
        Ui_Draw(L("ESC_HINT"), panelX + 40, panelY + panelH - 50, 18, GRAY);
        Ui_Draw(L("MENU_TABS_HINT"), panelX + 40, panelY + panelH - 28, 16, DARKGRAY);
    } else if (g->menuPage == MENU_PAGE_CODEX) {
        MonsterDef def;
        MonsterData_Get(g->codexIndex, &def);
        bool unlocked = g->codexUnlocked[g->codexIndex];
        Ui_Draw(TextFormat(L("CODEX_MOVE"), g->codexIndex + 1, MONSTER_TYPE_COUNT),
                panelX + 40, panelY + 90, 22, SKYBLUE);

        DrawRectangle((int)panelX + 50, (int)panelY + 140, 240, 240, Fade(def.color, unlocked ? 0.9f : 0.25f));
        if (unlocked) Ui_Draw(def.nameKr, panelX + 70, panelY + 230, 28, WHITE);
        else {
            Ui_Draw("???", panelX + 130, panelY + 230, 36, DARKGRAY);
            Ui_Draw(L("LOCKED"), panelX + 120, panelY + 280, 22, GRAY);
        }

        Ui_Draw(unlocked ? def.nameKr : L("UNKNOWN_MONSTER"), panelX + 340, panelY + 150, 30, WHITE);
        Ui_Draw(unlocked ? def.abilityKr : L("BOOK_HINT"), panelX + 340, panelY + 200, 22, LIGHTGRAY);
        if (unlocked) {
            Ui_Draw(TextFormat(L("ABILITY_TYPE"), Special_NameKr(def.special)), panelX + 340, panelY + 250, 22, GOLD);
            Ui_Draw(TextFormat(L("HP_SPEED"), def.hp, def.speed), panelX + 340, panelY + 290, 20, LIGHTGRAY);
            if (Btn((Rectangle){panelX + 340, panelY + 340, 240, 48}, L("EQUIP_THIS"), ORANGE)) {
                g->player.costumeMonsterId = g->codexIndex;
                GameShowMessage(g, TextFormat(L("MSG_EQUIP"), def.nameKr), 1.5f);
                AudioSfx_Item();
            }
        }
    } else if (g->menuPage == MENU_PAGE_COSTUME) {
        Ui_Draw(L("COSTUME_TITLE"), panelX + 40, panelY + 90, 28, YELLOW);
        Ui_Draw(L("COSTUME_DESC"), panelX + 40, panelY + 130, 20, LIGHTGRAY);

        if (Btn((Rectangle){panelX + 40, panelY + 180, 260, 48}, L("UNEQUIP"), DARKGRAY)) {
            g->player.costumeMonsterId = -1;
            GameShowMessage(g, L("MSG_UNEQUIP"), 1.2f);
        }

        int drawn = 0;
        for (int i = 0; i < MONSTER_TYPE_COUNT; i++) {
            if (!g->codexUnlocked[i]) continue;
            MonsterDef def;
            MonsterData_Get(i, &def);
            int col = drawn % 3;
            int row = drawn / 3;
            Rectangle r = {panelX + 40 + col * 280, panelY + 250 + row * 60, 260, 50};
            bool eq = (g->player.costumeMonsterId == i);
            const char *label = eq ? TextFormat("%s%s", def.nameKr, L("EQUIPPED")) : def.nameKr;
            if (Btn(r, label, eq ? ORANGE : BLUE)) {
                g->player.costumeMonsterId = i;
                GameShowMessage(g, TextFormat(L("MSG_EQUIP"), def.nameKr), 1.5f);
                AudioSfx_Item();
            }
            drawn++;
            if (drawn >= 9) break;
        }
        if (drawn == 0) Ui_Draw(L("NO_UNLOCK"), panelX + 40, panelY + 260, 20, GRAY);
    } else if (g->menuPage == MENU_PAGE_ACHIEVE) {
        Ui_Draw(L("ACHIEVE_TITLE"), panelX + 40, panelY + 90, 28, GOLD);
        Ui_Draw(L("ACHIEVE_DESC"), panelX + 40, panelY + 128, 18, LIGHTGRAY);
        int cleared = 0;
        for (int i = 0; i < ACHIEVE_COUNT; i++) {
            bool ok = g->achievements.unlocked[i];
            if (ok) cleared++;
            float y = panelY + 170 + i * 42;
            DrawRectangle((int)panelX + 40, (int)y, (int)panelW - 80, 36, Fade(ok ? DARKGREEN : DARKGRAY, 0.7f));
            DrawRectangleLines((int)panelX + 40, (int)y, (int)panelW - 80, 36, ok ? LIME : GRAY);
            Ui_Draw(ok ? L("ACH_CLEAR") : L("ACH_LOCKED"), panelX + 52, y + 8, 18, ok ? LIME : GRAY);
            Ui_Draw(L(MENU_ACH_KEYS[i]), panelX + 160, y + 8, 20, ok ? WHITE : LIGHTGRAY);
        }
        Ui_Draw(TextFormat(L("ACH_PROGRESS"), cleared, ACHIEVE_COUNT), panelX + 40, panelY + panelH - 40, 20, SKYBLUE);
    } else if (g->menuPage == MENU_PAGE_RANKING) {
        Ui_Draw(L("RANKING_TITLE"), panelX + 40, panelY + 90, 28, VIOLET);
        Ui_Draw(L("RANKING_DESC"), panelX + 40, panelY + 128, 18, LIGHTGRAY);
        Ui_Draw(L("RANK_HEADER"), panelX + 60, panelY + 170, 20, GOLD);
        if (g->ranking.count <= 0) {
            Ui_Draw(L("RANK_EMPTY"), panelX + 60, panelY + 220, 22, GRAY);
        } else {
            for (int i = 0; i < g->ranking.count; i++) {
                float y = panelY + 210 + i * 36;
                Color c = (i == 0) ? GOLD : ((i < 3) ? ORANGE : LIGHTGRAY);
                Ui_Draw(TextFormat(L("RANK_ROW"), i + 1, g->ranking.scores[i], g->ranking.kills[i]),
                        panelX + 60, y, 22, c);
            }
        }
        Ui_Draw(TextFormat(L("RANK_CURRENT"), g->score.score, g->score.kills),
                panelX + 40, panelY + panelH - 40, 20, SKYBLUE);
    }
}
