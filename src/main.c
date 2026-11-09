/* Arena Breach entry — compile this file only (includes other .c below) */
#include "game.h"
#include <string.h>
#include <stdlib.h>

static void Game_Reset(GameState *g) {
    RankingBoard keepRank = g->ranking;
    memset(g, 0, sizeof(*g));
    g->ranking = keepRank;
    g->rng = 0xC0FFEEu;
    g->nextBoss = 0;
    g->tutorialDone = false;
    g->tutorialStep = 0;
    g->paused = false;
    g->menuPage = MENU_PAGE_PAUSE;
    g->codexIndex = 0;
    g->codexPopupTimer = 0;
    g->player.costumeMonsterId = -1;
    Camera_Init(g);
    Player_Init(g);
    Weapons_Init(g);
    World_Init(g);
    Inventory_Init(g);
    Monsters_Init(g);
    Waves_Init(g);
    Score_Init(g);
    Meta_Init(g);
    Game_UpdateBackground(g);
    GameShowMessage(g, L("MSG_SURVIVE"), 2.0f);
}

int main(void) {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_W, SCREEN_H, "Arena Breach - Raylib 6.2");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);
    EnableCursor();

    AudioSfx_Init();
    Assets_Init();
    Locale_Init();
    Ui_Init();

    /* Heap: GameState is too large for default stack (MSVC C6262) */
    GameState *g = (GameState *)calloc(1, sizeof(GameState));
    if (!g) {
        CloseWindow();
        return 1;
    }

    Ranking_Load(g);
    Game_Reset(g);
    if (Assets_Ready()) GameShowMessage(g, L("MSG_ASSETS_OK"), 2.0f);
    else GameShowMessage(g, L("MSG_ASSETS_FAIL"), 4.0f);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F4)) break;

        if (IsKeyPressed(KEY_ESCAPE) && !g->gameOver) {
            g->paused = !g->paused;
            if (g->paused) {
                g->inventory.open = false;
                g->inventory.dragging = false;
                g->menuPage = MENU_PAGE_PAUSE;
            }
            AudioSfx_Pause();
        }

        float dt = GetFrameTime();
        if (dt > 0.05f) dt = 0.05f;

        if (g->paused) {
            Menu_Update(g);
        } else {
            if (!g->tutorialDone && IsKeyPressed(KEY_ENTER)) {
                g->tutorialStep++;
                if (g->tutorialStep >= 6) g->tutorialDone = true;
            }

            if (g->gameOver && IsKeyPressed(KEY_ENTER)) {
                Game_Reset(g);
                g->tutorialDone = true;
            }

            if (!g->gameOver) {
                g->gameTime += dt;
                Game_UpdateBackground(g);
                Inventory_Update(g);
                Player_Update(g, dt);
                if (!g->inventory.open) {
                    Weapons_Update(g, dt);
                    Monsters_Update(g, dt);
                    Waves_Update(g, dt);
                    Bosses_Update(g, dt);
                    Meta_Update(g);
                }
                Camera_Update(g);
                UtilUpdateEffects(g, dt);
            } else {
                UtilUpdateEffects(g, dt);
            }
        }

        BeginDrawing();
        ClearBackground(g->bgClear.a ? g->bgClear : (Color){20, 22, 28, 255});
        BeginMode3D(g->camera);
        World_Draw(g);
        Player_Draw(g);
        Weapons_Draw(g);
        Monsters_Draw(g);
        UtilDrawEffects(g);
        EndMode3D();
        Hud_Draw(g);
        DrawFPS(SCREEN_W - 90, SCREEN_H - 30);
        EndDrawing();
    }

    free(g);
    Ui_Unload();
    Assets_Unload();
    AudioSfx_Unload();
    CloseWindow();
    return 0;
}

/* Include other sources through main.c */
#include "util.c"
#include "camera_ctrl.c"
#include "world.c"
#include "player.c"
#include "weapons.c"
#include "inventory.c"
#include "monster_data.c"
#include "monsters.c"
#include "waves.c"
#include "abilities.c"
#include "score.c"
#include "bosses.c"
#include "meta.c"
#include "hud.c"
#include "assets.c"
#include "audio_sfx.c"
#include "ui_font.c"
#include "menu.c"
#include "locale_kr.c"
