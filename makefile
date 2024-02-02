CC = gcc

SHELL := /bin/bash

LIBS = -lraylib -lm
OBJS = pong.o
CFLAGS = -Wall -Wextra 

BIN = pong

# Only used for WASM compilation
RAYLIB_PATH = ~/Programming/raylib/
EMSCRIPTEN_PATH = /etc/profile.d/emscripten.sh

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: web
web:
	source $(EMSCRIPTEN_PATH) && emcc -o $(BIN).html pong.c -Wall -Os $(RAYLIB_PATH)/src/libraylib.a -I. -I $(RAYLIB_PATH)/src -I $(RAYLIB_PATH)/src/external -L. -L $(RAYLIB_PATH)/src -s USE_GLFW=3 --shell-file $(RAYLIB_PATH)/src/minshell.html -DPLATFORM_WEB -s GL_ENABLE_GET_PROC_ADDRESS
