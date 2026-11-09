#include "game.h"
#include <stdio.h>

const char *Item_Name(ItemKind kind) {
    switch (kind) {
    case ITEM_MEDKIT: return L("MEDKIT");
    case ITEM_AMMO: return L("AMMO");
    case ITEM_SPEED_BOOST: return L("SPEED");
    case ITEM_SHIELD: return L("SHIELD_ITEM");
    case ITEM_GRENADE_PACK: return L("GRENADE_PACK");
    case ITEM_SCORE_GEM: return L("SCORE_GEM");
    case ITEM_MONSTER_BOOK: return L("MONSTER_BOOK");
    default: return L("EMPTY");
    }
}

Color Item_Color(ItemKind kind) {
    switch (kind) {
    case ITEM_MEDKIT: return RED;
    case ITEM_AMMO: return GOLD;
    case ITEM_SPEED_BOOST: return SKYBLUE;
    case ITEM_SHIELD: return BLUE;
    case ITEM_GRENADE_PACK: return LIME;
    case ITEM_SCORE_GEM: return PURPLE;
    case ITEM_MONSTER_BOOK: return ORANGE;
    default: return GRAY;
    }
}

int Inventory_TotalCount(GameState *g) {
    int n = 0;
    for (int i = 0; i < INV_SLOTS; i++) n += g->inventory.slots[i].count;
    return n;
}

void Inventory_Init(GameState *g) {
    g->inventory.open = false;
    g->inventory.dragging = false;
    g->inventory.dragFrom = -1;
    for (int i = 0; i < INV_SLOTS; i++) {
        g->inventory.slots[i].kind = ITEM_EMPTY;
        g->inventory.slots[i].count = 0;
    }
    g->inventory.slots[0] = (InvSlot){ITEM_MEDKIT, 2};
    g->inventory.slots[1] = (InvSlot){ITEM_AMMO, 1};
    g->inventory.slots[2] = (InvSlot){ITEM_GRENADE_PACK, 1};
    g->inventory.slots[3] = (InvSlot){ITEM_SPEED_BOOST, 1};
}

bool Inventory_Add(GameState *g, ItemKind kind, int count) {
    if (kind == ITEM_EMPTY || count <= 0) return false;
    if (Inventory_TotalCount(g) + count > MAX_ITEM_OWNED) {
        GameShowMessage(g, L("MSG_FULL"), 1.2f);
        return false;
    }
    for (int i = 0; i < INV_SLOTS; i++) {
        if (g->inventory.slots[i].kind == kind) {
            g->inventory.slots[i].count += count;
            return true;
        }
    }
    for (int i = 0; i < INV_SLOTS; i++) {
        if (g->inventory.slots[i].kind == ITEM_EMPTY) {
            g->inventory.slots[i].kind = kind;
            g->inventory.slots[i].count = count;
            return true;
        }
    }
    GameShowMessage(g, L("MSG_BAG_FULL"), 1.0f);
    return false;
}

static void UseItem(GameState *g, int slot) {
    InvSlot *s = &g->inventory.slots[slot];
    if (s->kind == ITEM_EMPTY || s->count <= 0) return;
    Player *p = &g->player;
    switch (s->kind) {
    case ITEM_MEDKIT:
        p->hp = Clamp(p->hp + 35.0f, 0, p->maxHp);
        GameShowMessage(g, L("MSG_HP"), 0.8f);
        break;
    case ITEM_AMMO:
        GameShowMessage(g, L("MSG_AMMO"), 0.8f);
        p->fireCooldown = 0.0f;
        break;
    case ITEM_SPEED_BOOST:
        p->speedMul = 1.45f;
        p->speedTimer = 6.0f;
        GameShowMessage(g, L("MSG_SPEED"), 0.8f);
        break;
    case ITEM_SHIELD:
        p->shieldHp = 40.0f;
        GameShowMessage(g, L("MSG_SHIELD"), 0.8f);
        break;
    case ITEM_GRENADE_PACK:
        p->grenades += 3;
        GameShowMessage(g, L("MSG_GRENADE"), 0.8f);
        break;
    case ITEM_SCORE_GEM:
        Score_AddKill(g, 50, p->position);
        GameShowMessage(g, L("MSG_GEM"), 0.8f);
        break;
    case ITEM_MONSTER_BOOK:
        GameShowMessage(g, L("MSG_BOOK_HAVE"), 0.8f);
        break;
    default: break;
    }
    s->count--;
    if (s->count <= 0) { s->kind = ITEM_EMPTY; s->count = 0; }
    AudioSfx_Item();
}

void Inventory_UseHotbar(GameState *g, int index) {
    if (index < 0 || index >= HOTBAR_SLOTS) return;
    UseItem(g, index);
}

static Rectangle SlotRect(int index) {
    int cols = 8;
    int row = index / cols;
    int col = index % cols;
    float startX = SCREEN_W * 0.5f - (cols * 56.0f) * 0.5f;
    float startY = SCREEN_H * 0.5f - 80.0f;
    return (Rectangle){startX + col * 56.0f, startY + row * 56.0f, 48, 48};
}

void Inventory_Update(GameState *g) {
    if (!g->inventory.open) return;
    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        for (int i = 0; i < INV_SLOTS; i++) {
            if (CheckCollisionPointRec(mouse, SlotRect(i)) && g->inventory.slots[i].kind != ITEM_EMPTY) {
                g->inventory.dragging = true;
                g->inventory.dragFrom = i;
                break;
            }
        }
    }
    if (g->inventory.dragging && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        int to = -1;
        for (int i = 0; i < INV_SLOTS; i++) {
            if (CheckCollisionPointRec(mouse, SlotRect(i))) { to = i; break; }
        }
        if (to >= 0 && g->inventory.dragFrom >= 0 && to != g->inventory.dragFrom) {
            InvSlot tmp = g->inventory.slots[to];
            g->inventory.slots[to] = g->inventory.slots[g->inventory.dragFrom];
            g->inventory.slots[g->inventory.dragFrom] = tmp;
        }
        g->inventory.dragging = false;
        g->inventory.dragFrom = -1;
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        for (int i = 0; i < INV_SLOTS; i++) {
            if (CheckCollisionPointRec(mouse, SlotRect(i))) {
                UseItem(g, i);
                break;
            }
        }
    }
}

void Inventory_Draw(GameState *g) {
    for (int i = 0; i < HOTBAR_SLOTS; i++) {
        Rectangle r = (Rectangle){20.0f + i * 54.0f, SCREEN_H - 70.0f, 48, 48};
        DrawRectangleRec(r, Fade(DARKGRAY, 0.75f));
        DrawRectangleLinesEx(r, 2, LIGHTGRAY);
        InvSlot *s = &g->inventory.slots[i];
        if (s->kind != ITEM_EMPTY) {
            Assets_DrawItemIcon(s->kind, (Rectangle){r.x + 6, r.y + 6, 36, 36});
            Ui_Draw(TextFormat("%d", s->count), r.x + 4, r.y + 30, 14, WHITE);
        }
        Ui_Draw(TextFormat("%d", i + 1), r.x + 2, r.y + 2, 12, YELLOW);
    }
    Ui_Draw(TextFormat(L("OWNED"), Inventory_TotalCount(g), MAX_ITEM_OWNED), 20, SCREEN_H - 90, 16, LIGHTGRAY);

    if (!g->inventory.open) return;
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.55f));
    Ui_Draw(L("BAG_HELP"), SCREEN_W * 0.5f - 220, SCREEN_H * 0.5f - 130, 22, RAYWHITE);
    for (int i = 0; i < INV_SLOTS; i++) {
        Rectangle r = SlotRect(i);
        DrawRectangleRec(r, Fade(DARKBLUE, 0.85f));
        DrawRectangleLinesEx(r, 2, SKYBLUE);
        InvSlot *s = &g->inventory.slots[i];
        if (s->kind != ITEM_EMPTY && !(g->inventory.dragging && g->inventory.dragFrom == i)) {
            Assets_DrawItemIcon(s->kind, (Rectangle){r.x + 6, r.y + 6, 36, 36});
            Ui_Draw(TextFormat("%d", s->count), r.x + 4, r.y + 32, 14, WHITE);
        }
        if (i < HOTBAR_SLOTS) Ui_Draw(TextFormat("%d", i + 1), r.x + 2, r.y + 2, 12, GOLD);
    }
    if (g->inventory.dragging && g->inventory.dragFrom >= 0) {
        InvSlot *s = &g->inventory.slots[g->inventory.dragFrom];
        Vector2 m = GetMousePosition();
        Assets_DrawItemIcon(s->kind, (Rectangle){m.x - 16, m.y - 16, 32, 32});
        Ui_Draw(Item_Name(s->kind), m.x + 20, m.y - 8, 16, WHITE);
    }
}
