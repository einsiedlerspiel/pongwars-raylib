CC = gcc
BIN = pong

SHELL := /bin/bash
LIBS = -lraylib -lm
OBJS = $(BIN).o

CFLAGS = -Wall -Wextra

# Only used for WASM compilation
RAYLIB_PATH = ~/Programming/raylib
EMCC_FLAGS = -Wall -DPLATFORM_WEB -s GL_ENABLE_GET_PROC_ADDRESS -s USE_GLFW=3
EMCC_SHELL_FILE = --shell-file $(RAYLIB_PATH)/src/minshell.html

.PHONY: all
all: $(BIN) $(BIN).html

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.html: %.c
	emcc -o $@ $< $(EMCC_FLAGS) lib/libraylib.a $(EMCC_SHELL_FILE)

%.js: %.c
	emcc -o $@ $< $(EMCC_FLAGS) lib/libraylib.a 
