/* Arena Breach — 현재 main (초록 네모 데모 아님) */
#include "game.h"
#include <string.h>

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

    GameState game;
    memset(&game, 0, sizeof(game));
    Ranking_Load(&game);
    Game_Reset(&game);
    if (Assets_Ready()) GameShowMessage(&game, L("MSG_ASSETS_OK"), 2.0f);
    else GameShowMessage(&game, L("MSG_ASSETS_FAIL"), 4.0f);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F4)) break;

        if (IsKeyPressed(KEY_ESCAPE) && !game.gameOver) {
            game.paused = !game.paused;
            if (game.paused) {
                game.inventory.open = false;
                game.inventory.dragging = false;
                game.menuPage = MENU_PAGE_PAUSE;
            }
            AudioSfx_Pause();
        }

        float dt = GetFrameTime();
        if (dt > 0.05f) dt = 0.05f;

        if (game.paused) {
            Menu_Update(&game);
        } else {
            if (!game.tutorialDone && IsKeyPressed(KEY_ENTER)) {
                game.tutorialStep++;
                if (game.tutorialStep >= 6) game.tutorialDone = true;
            }

            if (game.gameOver && IsKeyPressed(KEY_ENTER)) {
                Game_Reset(&game);
                game.tutorialDone = true;
            }

            if (!game.gameOver) {
                game.gameTime += dt;
                Game_UpdateBackground(&game);
                Inventory_Update(&game);
                Player_Update(&game, dt);
                if (!game.inventory.open) {
                    Weapons_Update(&game, dt);
                    Monsters_Update(&game, dt);
                    Waves_Update(&game, dt);
                    Bosses_Update(&game, dt);
                    Meta_Update(&game);
                }
                Camera_Update(&game);
                UtilUpdateEffects(&game, dt);
            } else {
                UtilUpdateEffects(&game, dt);
            }
        }

        BeginDrawing();
        ClearBackground(game.bgClear.a ? game.bgClear : (Color){20, 22, 28, 255});
        BeginMode3D(game.camera);
        World_Draw(&game);
        Player_Draw(&game);
        Weapons_Draw(&game);
        Monsters_Draw(&game);
        UtilDrawEffects(&game);
        EndMode3D();
        Hud_Draw(&game);
        DrawFPS(SCREEN_W - 90, SCREEN_H - 30);
        EndDrawing();
    }

    Ui_Unload();
    Assets_Unload();
    AudioSfx_Unload();
    CloseWindow();
    return 0;
}

/* main.c 하나로 나머지 소스 포함 */
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
