#include "game.h"

/* English identifiers only in source — Korean comes from assets/locale/ko_ui.txt (UTF-8) */
static const MonsterDef DEFS[MONSTER_TYPE_COUNT] = {
    {"Shambler", "", "", {120,160,100,255}, 40, 1.6f, 0.55f, 8, 1.2f, 1.2f, SPEC_NONE, 0, 10, false},
    {"Night Fang", "", "", {180,180,200,255}, 55, 2.0f, 0.55f, 10, 1.2f, 1.0f, SPEC_LIFESTEAL, 0.25f, 18, false},
    {"Moon Howler", "", "", {140,120,100,255}, 70, 2.3f, 0.65f, 12, 1.3f, 1.1f, SPEC_RAGE_SPEED, 1.6f, 22, false},
    {"Tomb Bound", "", "", {210,200,160,255}, 80, 1.4f, 0.6f, 9, 1.4f, 1.3f, SPEC_SLOW_WRAP, 2.0f, 20, false},
    {"Bog Walker", "", "", {90,70,50,255}, 90, 1.3f, 0.7f, 11, 1.5f, 1.4f, SPEC_MUD_POOL, 2.5f, 24, false},
    {"Pale Wisp", "", "", {200,230,255,180}, 35, 2.2f, 0.45f, 7, 1.1f, 1.0f, SPEC_PHASE, 1.5f, 16, false},
    {"Split Ooze", "", "", {80,200,90,255}, 45, 1.7f, 0.5f, 8, 1.2f, 1.2f, SPEC_SPLIT, 1, 14, false},
    {"Stone Brute", "", "", {130,130,140,255},120, 1.1f, 0.85f, 16, 1.6f, 1.6f, SPEC_ARMORED, 0.45f, 30, false},
    {"Swarm Host", "", "", {100,60,120,255}, 60, 1.8f, 0.55f, 9, 1.2f, 1.3f, SPEC_SUMMON, 1, 26, false},
    {"Venom Crawler", "", "", {70,160,70,255}, 50, 2.1f, 0.5f, 8, 1.2f, 1.1f, SPEC_POISON, 3.0f, 18, false},
    {"Tusk Charger", "", "", {170,110,80,255}, 75, 1.5f, 0.7f, 14, 1.5f, 1.5f, SPEC_CHARGE, 4.0f, 28, false},
    {"Clan Caller", "", "", {160,100,100,255}, 65, 1.9f, 0.55f, 9, 1.3f, 1.4f, SPEC_HOWL, 1.4f, 22, false},
    {"Hex Shade", "", "", {90,40,120,255}, 55, 1.8f, 0.5f, 10, 1.2f, 1.2f, SPEC_CURSE, 4.0f, 24, false},
    {"Needle Fiend", "", "", {200,200,80,255}, 48, 2.0f, 0.5f, 9, 1.2f, 1.0f, SPEC_SPIKE, 6.0f, 20, false},
    {"Grove Tender", "", "", {60,140,80,255}, 70, 1.5f, 0.6f, 8, 1.3f, 1.5f, SPEC_HEAL_AURA, 4.0f, 26, false},
    {"Ash Ghoul", "", "", {80,80,80,255}, 42, 1.9f, 0.5f, 8, 1.2f, 1.1f, SPEC_NONE, 0, 12, false},
    {"Frost Lurker", "", "", {150,200,255,255}, 60, 1.6f, 0.55f, 9, 1.3f, 1.2f, SPEC_SLOW_WRAP, 1.5f, 18, false},
    {"Ember Imp", "", "", {255,120,40,255}, 38, 2.4f, 0.4f, 7, 1.0f, 0.9f, SPEC_SPIKE, 4.0f, 15, false},
    {"Bone Rattler", "", "", {230,230,210,255}, 50, 1.8f, 0.5f, 9, 1.2f, 1.1f, SPEC_NONE, 0, 14, false},
    {"Mire Knight", "", "", {70,90,70,255}, 100, 1.35f, 0.75f, 13, 1.5f, 1.4f, SPEC_ARMORED, 0.35f, 28, false},
    {"Blood Leech", "", "", {160,30,50,255}, 44, 2.0f, 0.45f, 8, 1.1f, 1.0f, SPEC_LIFESTEAL, 0.4f, 17, false},
    {"Ruin Hound", "", "", {120,90,70,255}, 58, 2.2f, 0.55f, 10, 1.2f, 1.0f, SPEC_RAGE_SPEED, 1.4f, 19, false},
    {"Sand Wraith", "", "", {210,190,140,200}, 46, 2.1f, 0.5f, 8, 1.2f, 1.1f, SPEC_PHASE, 1.2f, 16, false},
    {"Crystal Golem", "", "", {140,200,220,255},110, 1.05f, 0.9f, 15, 1.7f, 1.7f, SPEC_ARMORED, 0.5f, 32, false},
    {"Plague Rat", "", "", {130,150,90,255}, 34, 2.5f, 0.4f, 6, 1.0f, 0.8f, SPEC_POISON, 2.0f, 12, false},
    {"Iron Tick", "", "", {100,100,110,255}, 66, 1.7f, 0.55f, 10, 1.2f, 1.2f, SPEC_SPIKE, 5.0f, 20, false},
    {"Dread Chorus", "", "", {170,80,170,255}, 72, 1.6f, 0.6f, 11, 1.4f, 1.5f, SPEC_HOWL, 1.5f, 25, false},
    {"Mirror Slime", "", "", {180,255,220,255}, 40, 1.8f, 0.5f, 7, 1.1f, 1.1f, SPEC_SPLIT, 1, 15, false},
    {"Grave Herald", "", "", {100,80,120,255}, 85, 1.5f, 0.65f, 12, 1.4f, 1.4f, SPEC_CURSE, 3.5f, 27, false},
    {"Sky Leaper", "", "", {90,160,220,255}, 52, 2.3f, 0.5f, 9, 1.2f, 1.0f, SPEC_CHARGE, 3.5f, 21, false},
};

void MonsterData_Get(int typeId, MonsterDef *out) {
    if (typeId < 0 || typeId >= MONSTER_TYPE_COUNT) typeId = 0;
    *out = DEFS[typeId];
    out->nameKr = LMonsterName(typeId);
    out->abilityKr = LMonsterAbility(typeId);
}

int MonsterData_Count(void) { return MONSTER_TYPE_COUNT; }

const char *Special_NameKr(SpecialId id) {
    switch (id) {
    case SPEC_LIFESTEAL: return L("SPEC_LIFESTEAL");
    case SPEC_RAGE_SPEED: return L("SPEC_RAGE");
    case SPEC_SLOW_WRAP: return L("SPEC_SLOW");
    case SPEC_MUD_POOL: return L("SPEC_MUD");
    case SPEC_PHASE: return L("SPEC_PHASE");
    case SPEC_SPLIT: return L("SPEC_SPLIT");
    case SPEC_ARMORED: return L("SPEC_ARMOR");
    case SPEC_SUMMON: return L("SPEC_SUMMON");
    case SPEC_POISON: return L("SPEC_POISON");
    case SPEC_CHARGE: return L("SPEC_CHARGE");
    case SPEC_HOWL: return L("SPEC_HOWL");
    case SPEC_CURSE: return L("SPEC_CURSE");
    case SPEC_SPIKE: return L("SPEC_SPIKE");
    case SPEC_HEAL_AURA: return L("SPEC_HEAL");
    default: return L("SPEC_NONE");
    }
}
