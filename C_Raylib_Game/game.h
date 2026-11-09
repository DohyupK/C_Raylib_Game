#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <stdint.h>

#define SCREEN_W 1280
#define SCREEN_H 720
#define MAX_BULLETS 256
#define MAX_GRENADES 32
#define MAX_MONSTERS 120
#define MAX_PICKUPS 64
#define MAX_OBSTACLES 48
#define INV_SLOTS 16
#define HOTBAR_SLOTS 8
#define MAX_ITEM_OWNED 10
#define PICKUP_LIFETIME 60.0f
#define MONSTER_TYPE_COUNT 30
#define MAX_FLOAT_TEXTS 48
#define MAX_PARTICLES 128
#define BUILDING_MODEL_COUNT 8
#define BG_THEME_COUNT 8

typedef enum {
    MENU_PAGE_PAUSE = 0,
    MENU_PAGE_CODEX,
    MENU_PAGE_COSTUME,
    MENU_PAGE_ACHIEVE,
    MENU_PAGE_RANKING
} MenuPage;

#define RANK_COUNT 10
#define ACHIEVE_COUNT 8

typedef struct {
    int scores[RANK_COUNT];
    int kills[RANK_COUNT];
    int count;
} RankingBoard;

#define PLAYER_RADIUS 0.45f
#define PLAYER_HEIGHT 1.6f
#define GRAVITY 28.0f
#define MOVE_SPEED 9.0f
#define DASH_SPEED 22.0f
#define DASH_TIME 0.18f
#define DASH_COOLDOWN 1.4f
#define JUMP_VELOCITY 10.5f

typedef enum {
    WEAPON_PISTOL = 0,
    WEAPON_SHOTGUN,
    WEAPON_RIFLE,
    WEAPON_COUNT
} WeaponKind;

typedef enum {
    ITEM_EMPTY = 0,
    ITEM_MEDKIT,
    ITEM_AMMO,
    ITEM_SPEED_BOOST,
    ITEM_SHIELD,
    ITEM_GRENADE_PACK,
    ITEM_SCORE_GEM,
    ITEM_MONSTER_BOOK,
    ITEM_COUNT
} ItemKind;

typedef enum {
    ABILITY_NONE = 0,
    ABILITY_BERSERK,   /* Q: damage+speed burst */
    ABILITY_BARRIER,   /* E: temporary shield */
    ABILITY_OVERCHARGE /* R: rapid fire window */
} AbilityKind;

typedef enum {
    COSTUME_DEFAULT = 0,
    COSTUME_SCOUT,
    COSTUME_HEAVY,
    COSTUME_NIGHT,
    COSTUME_GOLD,
    COSTUME_COUNT
} CostumeId;

typedef enum {
    BOSS_NONE = 0,
    BOSS_HORDE_KING,
    BOSS_PLAGUE_MOTHER,
    BOSS_SHADOW_COLOSSUS
} BossKind;

typedef struct {
    Vector3 position;
    Vector3 velocity;
    float yaw;
    float hp;
    float maxHp;
    float dashTimer;
    float dashCooldown;
    bool grounded;
    int weapon;
    float fireCooldown;
    float invulnTimer;
    float speedMul;
    float speedTimer;
    float shieldHp;
    float berserkTimer;
    float overchargeTimer;
    float abilityCd[4];
    CostumeId costume;       /* visual tint fallback */
    int costumeMonsterId;    /* -1 = none, else unlocked monster form (1 only) */
    float costumeSpecialCd;
    int grenades;
} Player;

typedef struct {
    bool active;
    Vector3 position;
    Vector3 velocity;
    float life;
    float damage;
    float radius;
    int ownerWeapon;
} Bullet;

typedef struct {
    bool active;
    Vector3 position;
    Vector3 velocity;
    float fuse;
    float blastRadius;
    float damage;
} Grenade;

typedef struct {
    bool active;
    Vector3 position;
    ItemKind kind;
    float bob;
    float life;      /* despawn timer */
    int metaTypeId;  /* monster book type */
} Pickup;

typedef struct {
    bool solid;
    BoundingBox box;
    Color color;
    int modelId;     /* building GLB index, -1 = box only */
    const char *nameKr;
} Obstacle;

typedef struct {
    ItemKind kind;
    int count;
} InvSlot;

typedef struct {
    bool open;
    InvSlot slots[INV_SLOTS];
    int dragFrom;
    bool dragging;
} Inventory;

typedef enum {
    SPEC_NONE = 0,
    SPEC_LIFESTEAL,
    SPEC_RAGE_SPEED,
    SPEC_SLOW_WRAP,
    SPEC_MUD_POOL,
    SPEC_PHASE,
    SPEC_SPLIT,
    SPEC_ARMORED,
    SPEC_SUMMON,
    SPEC_POISON,
    SPEC_CHARGE,
    SPEC_HOWL,
    SPEC_CURSE,
    SPEC_SPIKE,
    SPEC_HEAL_AURA
} SpecialId;

typedef struct {
    const char *name;
    const char *nameKr;
    const char *abilityKr;
    Color color;
    float hp;
    float speed;
    float radius;
    float damage;
    float attackRange;
    float attackCooldown;
    SpecialId special;
    float specialValue;
    int scoreValue;
    bool boss;
} MonsterDef;

typedef struct {
    bool active;
    int typeId;
    Vector3 position;
    Vector3 velocity;
    float hp;
    float maxHp;
    float attackCd;
    float specialCd;
    float specialTimer;
    float slowTimer;
    float poisonTimer;
    bool phased;
    bool isBoss;
    bool fromWave;
    BossKind bossKind;
    float scale;
} Monster;

typedef struct {
    bool active;
    float warnTimer;
    float duration;
    Vector3 direction;
    int typeA;
    int typeB;
    float spawnRate;
    float spawnAcc;
} WarningWave;

typedef struct {
    int score;
    int kills;
    int bossKills;
    int combo;
    float comboTimer;
    int highScore;
} ScoreState;

typedef struct {
    bool unlocked[32];
    bool notified[32];
} AchievementState;

typedef struct {
    bool active;
    Vector3 position;
    Color color;
    float life;
    float maxLife;
    float size;
} Particle;

typedef struct {
    bool active;
    Vector2 screenPos;
    char text[48];
    Color color;
    float life;
} FloatText;

typedef struct GameState {
    Player player;
    Camera3D camera;
    Bullet bullets[MAX_BULLETS];
    Grenade grenades[MAX_GRENADES];
    Pickup pickups[MAX_PICKUPS];
    Obstacle obstacles[MAX_OBSTACLES];
    int obstacleCount;
    Inventory inventory;
    Monster monsters[MAX_MONSTERS];
    WarningWave wave;
    ScoreState score;
    AchievementState achievements;
    Particle particles[MAX_PARTICLES];
    FloatText floatTexts[MAX_FLOAT_TEXTS];
    float spawnAcc;
    float gameTime;
    float bossTimer;
    int nextBoss;
    bool paused;
    MenuPage menuPage;
    int codexIndex;
    bool codexUnlocked[MONSTER_TYPE_COUNT];
    RankingBoard ranking;
    float codexPopupTimer;
    char codexPopup[96];
    bool gameOver;
    bool tutorialDone;
    int tutorialStep;
    float messageTimer;
    char message[128];
    unsigned int rng;
    Color bgClear;
    Color groundColor;
} GameState;

static inline int GameDiffLevel(const GameState *g) { return g->score.kills / 100; }
static inline float GameDiffMul(const GameState *g) { return 1.0f + 0.22f * (float)GameDiffLevel(g); }
static inline int GameBgTheme(const GameState *g) { return (g->score.kills / 50) % BG_THEME_COUNT; }

static inline float GameRand01(GameState *g) {
    g->rng = g->rng * 1664525u + 1013904223u;
    return (g->rng >> 8) * (1.0f / 16777216.0f);
}

static inline int GameRandRange(GameState *g, int lo, int hiInclusive) {
    if (hiInclusive <= lo) return lo;
    int span = hiInclusive - lo + 1;
    int v = (int)(GameRand01(g) * (float)span);
    if (v >= span) v = span - 1;
    return lo + v;
}

void GameShowMessage(GameState *g, const char *msg, float seconds);
void GameAddFloatText(GameState *g, Vector3 worldPos, Camera3D cam, const char *text, Color color);
void GameSpawnParticle(GameState *g, Vector3 pos, Color color, float size, float life);
void UtilUpdateEffects(GameState *g, float dt);
void UtilDrawEffects(GameState *g);
void UtilDrawFloatTexts(GameState *g);

/* modules */
void Camera_Init(GameState *g);
void Camera_Update(GameState *g);
void World_Init(GameState *g);
void World_Draw(GameState *g);
bool World_ResolveCircle(GameState *g, Vector3 *pos, float radius);
void Player_Init(GameState *g);
void Player_Update(GameState *g, float dt);
void Player_Draw(GameState *g);
Vector3 Player_AimPoint(GameState *g);
void Weapons_Init(GameState *g);
void Weapons_Update(GameState *g, float dt);
void Weapons_Draw(GameState *g);
void Weapons_TryFire(GameState *g);
void Weapons_ThrowGrenade(GameState *g);
void Weapons_Cycle(GameState *g);
const char *Weapons_Name(int kind);
void Inventory_Init(GameState *g);
void Inventory_Update(GameState *g);
void Inventory_Draw(GameState *g);
bool Inventory_Add(GameState *g, ItemKind kind, int count);
void Inventory_UseHotbar(GameState *g, int index);
const char *Item_Name(ItemKind kind);
Color Item_Color(ItemKind kind);
void MonsterData_Get(int typeId, MonsterDef *out);
int MonsterData_Count(void);
void Monsters_Init(GameState *g);
void Monsters_Update(GameState *g, float dt);
void Monsters_Draw(GameState *g);
void Monsters_SpawnAround(GameState *g, int typeId, float distMin, float distMax, bool boss);
void Monsters_SpawnDirectional(GameState *g, int typeId, Vector3 dir, float dist);
void Monsters_DamageAt(GameState *g, Vector3 pos, float radius, float damage, bool fromPlayer);
void Monsters_DamageOne(GameState *g, int index, float damage);
void Waves_Init(GameState *g);
void Waves_Update(GameState *g, float dt);
void Waves_DrawHud(GameState *g);
void Abilities_Update(GameState *g, float dt);
void Abilities_Try(GameState *g, AbilityKind kind);
void Score_Init(GameState *g);
void Score_AddKill(GameState *g, int points, Vector3 at);
void Score_DrawHud(GameState *g);
void Ranking_Load(GameState *g);
void Ranking_Save(GameState *g);
void Ranking_Submit(GameState *g);
void Bosses_Update(GameState *g, float dt);
void Meta_Init(GameState *g);
void Meta_Update(GameState *g);
void Meta_OnKill(GameState *g, bool wasBoss);
void Meta_DrawHud(GameState *g);
void Meta_CycleCostume(GameState *g);
Color Meta_CostumeColor(CostumeId id);
const char *Meta_CostumeName(CostumeId id);
void Hud_Draw(GameState *g);
void Game_UpdateBackground(GameState *g);

void Assets_Init(void);
void Assets_Unload(void);
bool Assets_Ready(void);
void Assets_DrawPlayer(GameState *g);
void Assets_DrawMonster(GameState *g, Monster *m, const MonsterDef *def);
void Assets_DrawBuilding(const Obstacle *o);
Texture2D Assets_ItemIcon(ItemKind kind);
void Assets_DrawItemIcon(ItemKind kind, Rectangle dest);
void Assets_DrawPickup(ItemKind kind, Vector3 pos, float bob);

void AudioSfx_Init(void);
void AudioSfx_Unload(void);
void AudioSfx_Shoot(void);
void AudioSfx_Grenade(void);
void AudioSfx_Hit(void);
void AudioSfx_Item(void);
void AudioSfx_Boss(void);
void AudioSfx_Pause(void);

void Ui_Init(void);
void Ui_Unload(void);
Font Ui_Font(void);
void Ui_Draw(const char *text, float x, float y, float size, Color color);
float Ui_Measure(const char *text, float size);

void Locale_Init(void);
const char *L(const char *key);
const char *LMonsterName(int typeId);
const char *LMonsterAbility(int typeId);

void Menu_Update(GameState *g);
void Menu_Draw(GameState *g);
void Codex_Unlock(GameState *g, int typeId);
const char *Special_NameKr(SpecialId id);
int Inventory_TotalCount(GameState *g);

#endif
