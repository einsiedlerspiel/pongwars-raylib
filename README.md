title: Pong Wars

Inspired by @vnglst's [Mastodon
Post](https://hachyderm.io/@vnglst/111828811496422610) and [Java Script
Version](https://github.com/vnglst/pong-wars)

## Dependencies

[raylib](https://www.raylib.com/)

## Compiling to HTML5/WASM

The makefile includes a compilation rule for html5/wasm, but before that'll work
you'll probably need to

1. Follow raylibs
   [instructions](https://github.com/raysan5/raylib/wiki/Working-for-Web-(HTML5))
   to set up our environment for compilation with
   [emscripten](https://emscripten.org/).
2. Modify the paths to raylib and `emscripten.sh` in the makefile to fit your
   environment.

## TODOs

- prevent balls from getting stuck sometimes.
