#include "game.h"
#include <stdio.h>

typedef enum {
    SFX_SHOOT = 0,
    SFX_GRENADE,
    SFX_HIT,
    SFX_ITEM,
    SFX_BOSS,
    SFX_PAUSE,
    SFX_COUNT
} SfxId;

static Sound gSounds[SFX_COUNT];
static bool gAudioReady = false;

static Sound LoadOne(const char *path) {
    if (!FileExists(path)) {
        TraceLog(LOG_WARNING, "SFX missing: %s", path);
        Sound empty = {0};
        return empty;
    }
    return LoadSound(path);
}

void AudioSfx_Init(void) {
    InitAudioDevice();
    SetMasterVolume(0.85f);
    gSounds[SFX_SHOOT] = LoadOne("assets/sfx/shoot.ogg");       /* Kenney CC0 */
    gSounds[SFX_GRENADE] = LoadOne("assets/sfx/grenade.wav");   /* original generated */
    gSounds[SFX_HIT] = LoadOne("assets/sfx/hit.ogg");           /* Kenney CC0 */
    gSounds[SFX_ITEM] = LoadOne("assets/sfx/item.ogg");         /* Kenney CC0 */
    gSounds[SFX_BOSS] = LoadOne("assets/sfx/boss.wav");         /* original generated */
    gSounds[SFX_PAUSE] = LoadOne("assets/sfx/pause.wav");       /* original generated */
    gAudioReady = true;
    TraceLog(LOG_INFO, "Audio SFX initialized");
}

void AudioSfx_Unload(void) {
    if (!gAudioReady) return;
    for (int i = 0; i < SFX_COUNT; i++) {
        if (gSounds[i].frameCount > 0) UnloadSound(gSounds[i]);
    }
    CloseAudioDevice();
    gAudioReady = false;
}

static void PlayId(SfxId id, float pitch, float volume) {
    if (!gAudioReady) return;
    if (gSounds[id].frameCount == 0) return;
    SetSoundPitch(gSounds[id], pitch);
    SetSoundVolume(gSounds[id], volume);
    PlaySound(gSounds[id]);
}

void AudioSfx_Shoot(void) { PlayId(SFX_SHOOT, 0.95f + (float)GetRandomValue(0, 10) * 0.01f, 0.55f); }
void AudioSfx_Grenade(void) { PlayId(SFX_GRENADE, 1.0f, 0.9f); }
void AudioSfx_Hit(void) { PlayId(SFX_HIT, 0.9f + (float)GetRandomValue(0, 20) * 0.01f, 0.7f); }
void AudioSfx_Item(void) { PlayId(SFX_ITEM, 1.0f, 0.75f); }
void AudioSfx_Boss(void) { PlayId(SFX_BOSS, 1.0f, 1.0f); }
void AudioSfx_Pause(void) { PlayId(SFX_PAUSE, 1.0f, 0.6f); }
