#include "game.h"
#include <stdio.h>

void Hud_Draw(GameState *g) {
    Player *p = &g->player;

    DrawRectangle(20, 20, 220, 18, Fade(BLACK, 0.5f));
    DrawRectangle(20, 20, (int)(220.0f * (p->hp / p->maxHp)), 18, RED);
    Ui_Draw(TextFormat(L("HP"), p->hp), 24, 18, 16, WHITE);

    if (p->shieldHp > 0.0f) Ui_Draw(TextFormat(L("SHIELD"), p->shieldHp), 20, 42, 16, SKYBLUE);

    Ui_Draw(TextFormat(L("WEAPON"), Weapons_Name(p->weapon)), 20, 64, 18, RAYWHITE);
    Ui_Draw(TextFormat(L("GRENADES"), p->grenades), 20, 86, 18, LIME);
    Ui_Draw(TextFormat(L("DASH_CD"), p->dashCooldown > 0 ? p->dashCooldown : 0), 20, 108, 16, LIGHTGRAY);

    Ui_Draw(TextFormat(L("Q_BERSERK"), p->abilityCd[ABILITY_BERSERK]), 20, 140, 16,
            p->abilityCd[ABILITY_BERSERK] > 0 ? GRAY : ORANGE);
    Ui_Draw(TextFormat(L("E_BARRIER"), p->abilityCd[ABILITY_BARRIER]), 20, 160, 16,
            p->abilityCd[ABILITY_BARRIER] > 0 ? GRAY : SKYBLUE);
    Ui_Draw(TextFormat(L("R_OVER"), p->abilityCd[ABILITY_OVERCHARGE]), 20, 180, 16,
            p->abilityCd[ABILITY_OVERCHARGE] > 0 ? GRAY : GOLD);

    Ui_Draw(TextFormat(L("DIFF_BG"), GameDiffLevel(g), GameBgTheme(g) + 1), 20, 210, 16, YELLOW);

    if (p->costumeMonsterId >= 0) {
        MonsterDef def;
        MonsterData_Get(p->costumeMonsterId, &def);
        Ui_Draw(TextFormat(L("COSTUME_FMT"), def.nameKr, Special_NameKr(def.special)), 20, 232, 16, ORANGE);
    } else {
        Ui_Draw(L("COSTUME_BASIC"), 20, 232, 16, LIGHTGRAY);
    }

    Ui_Draw(L("HELP1"), 20, SCREEN_H - 40, 15, GRAY);
    Ui_Draw(L("HELP2"), 20, SCREEN_H - 22, 15, GRAY);
    Ui_Draw(L("HELP3"), SCREEN_W / 2.0f - 200, 8, 16, LIME);

    Score_DrawHud(g);
    Waves_DrawHud(g);
    Meta_DrawHud(g);
    UtilDrawFloatTexts(g);

    if (g->messageTimer > 0.0f) {
        float w = Ui_Measure(g->message, 24);
        DrawRectangle((int)(SCREEN_W / 2 - w / 2 - 12), SCREEN_H / 2 - 60, (int)(w + 24), 40, Fade(BLACK, 0.65f));
        Ui_Draw(g->message, SCREEN_W / 2 - w / 2, SCREEN_H / 2 - 50, 24, YELLOW);
    }

    if (g->codexPopupTimer > 0.0f) {
        g->codexPopupTimer -= GetFrameTime();
        float alpha = g->codexPopupTimer < 0.5f ? g->codexPopupTimer / 0.5f : 1.0f;
        float w = Ui_Measure(g->codexPopup, 22);
        float bx = SCREEN_W / 2.0f - w / 2.0f - 16;
        float by = 70;
        DrawRectangleRounded((Rectangle){bx, by, w + 32, 48}, 0.2f, 6, Fade((Color){20, 60, 40, 255}, 0.85f * alpha));
        DrawRectangleRoundedLinesEx((Rectangle){bx, by, w + 32, 48}, 0.2f, 6, 2, Fade(LIME, alpha));
        Ui_Draw(g->codexPopup, SCREEN_W / 2.0f - w / 2.0f, by + 12, 22, Fade(RAYWHITE, alpha));
    }

    if (!g->tutorialDone) {
        const char *keys[] = {"TIP0", "TIP1", "TIP2", "TIP3", "TIP4", "TIP5"};
        if (g->tutorialStep < 6) {
            DrawRectangle(SCREEN_W / 2 - 300, SCREEN_H - 170, 600, 60, Fade(DARKBLUE, 0.85f));
            Ui_Draw(L(keys[g->tutorialStep]), SCREEN_W / 2 - 280, SCREEN_H - 155, 20, WHITE);
            Ui_Draw(L("NEXT_TIP"), SCREEN_W / 2 - 60, SCREEN_H - 125, 14, LIGHTGRAY);
        }
    }

    if (g->gameOver) {
        DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.6f));
        Ui_Draw(L("GAME_OVER"), SCREEN_W / 2 - 90, SCREEN_H / 2 - 40, 40, RED);
        Ui_Draw(TextFormat(L("FINAL_SCORE"), g->score.score), SCREEN_W / 2 - 90, SCREEN_H / 2 + 10, 24, GOLD);
        Ui_Draw(L("RESTART"), SCREEN_W / 2 - 90, SCREEN_H / 2 + 50, 20, RAYWHITE);
    }

    Inventory_Draw(g);
    Menu_Draw(g);
}
