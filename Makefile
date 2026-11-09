# Arena Breach - MinGW build (raylib 6.2)
CC      = gcc
RAYLIB  = third_party/raylib-6.2_win64_mingw-w64
CFLAGS  = -std=c99 -O2 -Wall -I$(RAYLIB)/include -IC_Raylib_Game
LDFLAGS = -L$(RAYLIB)/lib -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = \
	C_Raylib_Game/main.c \
	C_Raylib_Game/util.c \
	C_Raylib_Game/camera_ctrl.c \
	C_Raylib_Game/world.c \
	C_Raylib_Game/player.c \
	C_Raylib_Game/weapons.c \
	C_Raylib_Game/inventory.c \
	C_Raylib_Game/monster_data.c \
	C_Raylib_Game/monsters.c \
	C_Raylib_Game/waves.c \
	C_Raylib_Game/abilities.c \
	C_Raylib_Game/score.c \
	C_Raylib_Game/bosses.c \
	C_Raylib_Game/meta.c \
	C_Raylib_Game/hud.c \
	C_Raylib_Game/assets.c \
	C_Raylib_Game/audio_sfx.c \
	C_Raylib_Game/ui_font.c \
	C_Raylib_Game/menu.c \
	C_Raylib_Game/locale_kr.c

OUT = ArenaBreach.exe

.PHONY: all clean run

all: $(OUT)

$(OUT): $(SRC) C_Raylib_Game/game.h
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)
	copy /Y $(RAYLIB)\lib\raylib.dll . 2>nul || cp $(RAYLIB)/lib/raylib.dll .

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT)
