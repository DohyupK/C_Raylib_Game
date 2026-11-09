# Arena Breach - MinGW build (raylib 6.2)
CC      = gcc
RAYLIB  = third_party/raylib-6.2_win64_mingw-w64
CFLAGS  = -std=c99 -O2 -Wall -I$(RAYLIB)/include -Isrc
LDFLAGS = -L$(RAYLIB)/lib -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = src/main.c
OUT = ArenaBreach.exe

.PHONY: all clean run

all: $(OUT)

$(OUT): $(SRC) src/game.h
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)
	copy /Y $(RAYLIB)\lib\raylib.dll . 2>nul || cp $(RAYLIB)/lib/raylib.dll .

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT)
