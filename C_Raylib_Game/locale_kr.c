#include "game.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LOC_MAX 320
#define LOC_KEY 48
#define LOC_VAL 192

static char gKeys[LOC_MAX][LOC_KEY];
static char gVals[LOC_MAX][LOC_VAL];
static int gCount = 0;

void Locale_Init(void) {
    gCount = 0;
    char *text = LoadFileText("assets/locale/ko_ui.txt");
    if (!text) {
        TraceLog(LOG_WARNING, "locale file missing: assets/locale/ko_ui.txt");
        return;
    }
    char *p = text;
    while (*p && gCount < LOC_MAX) {
        while (*p == '\r' || *p == '\n' || *p == ' ' || *p == '\t') p++;
        if (*p == '#' || *p == '\0') {
            while (*p && *p != '\n') p++;
            continue;
        }
        char *lineStart = p;
        while (*p && *p != '\n' && *p != '\r') p++;
        char saved = *p;
        *p = '\0';

        char *eq = strchr(lineStart, '=');
        if (eq && eq > lineStart) {
            *eq = '\0';
            const char *key = lineStart;
            const char *val = eq + 1;
            strncpy(gKeys[gCount], key, LOC_KEY - 1);
            gKeys[gCount][LOC_KEY - 1] = '\0';
            strncpy(gVals[gCount], val, LOC_VAL - 1);
            gVals[gCount][LOC_VAL - 1] = '\0';
            /* unescape %% -> handled by TextFormat callers; keep as-is */
            gCount++;
        }
        *p = saved;
        if (*p) p++;
    }
    UnloadFileText(text);
    TraceLog(LOG_INFO, "Locale loaded: %d entries", gCount);
}

const char *L(const char *key) {
    for (int i = 0; i < gCount; i++) {
        if (strcmp(gKeys[i], key) == 0) return gVals[i];
    }
    return key; /* fallback shows key if missing */
}

const char *LMonsterName(int typeId) {
    return L(TextFormat("M%d", typeId));
}

const char *LMonsterAbility(int typeId) {
    return L(TextFormat("M%dA", typeId));
}
