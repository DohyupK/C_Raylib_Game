#include "game.h"
#include <stdio.h>
#include <string.h>

static Font gFont;
static bool gFontOk = false;

static void AddCodepoint(int *arr, int *count, int max, int cp) {
    if (cp < 32 || *count >= max) return;
    for (int i = 0; i < *count; i++) if (arr[i] == cp) return;
    arr[(*count)++] = cp;
}

static void AddFromFile(int *arr, int *count, int max, const char *path) {
    if (!FileExists(path)) return;
    char *text = LoadFileText(path);
    if (!text) return;
    int n = 0;
    int *cps = LoadCodepoints(text, &n);
    if (cps) {
        for (int i = 0; i < n; i++) AddCodepoint(arr, count, max, cps[i]);
        UnloadCodepoints(cps);
    }
    UnloadFileText(text);
}

void Ui_Init(void) {
    gFontOk = false;
    const char *path = "assets/fonts/NanumGothic.ttf";
    if (!FileExists(path)) {
        TraceLog(LOG_WARNING, "Korean font missing: %s", path);
        return;
    }

    int codepoints[8192];
    int count = 0;
    for (int i = 32; i < 127; i++) AddCodepoint(codepoints, &count, 8192, i);

    /* UTF-8 locale files — this is what makes Hangul render */
    AddFromFile(codepoints, &count, 8192, "assets/locale/ko_ui.txt");
    AddFromFile(codepoints, &count, 8192, "assets/locale/ko_glyphs.txt");

    /* compatibility jamo */
    for (int cp = 0x3131; cp <= 0x3163; cp++) AddCodepoint(codepoints, &count, 8192, cp);

    gFont = LoadFontEx(path, 32, codepoints, count);
    if (gFont.glyphCount > 0 && gFont.texture.id != 0) {
        SetTextureFilter(gFont.texture, TEXTURE_FILTER_BILINEAR);
        gFontOk = true;
        TraceLog(LOG_INFO, "Korean font OK: req=%d loaded=%d", count, gFont.glyphCount);
    } else {
        TraceLog(LOG_WARNING, "Korean font LoadFontEx failed");
    }
}

void Ui_Unload(void) {
    if (gFontOk) UnloadFont(gFont);
    gFontOk = false;
}

Font Ui_Font(void) { return gFontOk ? gFont : GetFontDefault(); }

void Ui_Draw(const char *text, float x, float y, float size, Color color) {
    if (!text) return;
    if (gFontOk) DrawTextEx(gFont, text, (Vector2){x, y}, size, 1.0f, color);
    else DrawText(text, (int)x, (int)y, (int)size, color);
}

float Ui_Measure(const char *text, float size) {
    if (!text) return 0;
    if (gFontOk) return MeasureTextEx(gFont, text, size, 1.0f).x;
    return (float)MeasureText(text, (int)size);
}
