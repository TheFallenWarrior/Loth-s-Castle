# Loth's Castle

Loth's Castle is an old-school ASCII rogue-like for NES, PC, and Web. It is a
Wizard's Castle clone written in C.

## Building

As is, Loth's Castle can be compiled to NES through
[cc65](https://github.com/cc65/cc65) or to modern devices using
[Raylib](https://github.com/raysan5/raylib).

For the sake of simplicity, the entirety of the game's source code resides in
`main.c` and the Raylib port sits in `pc-port.c`.

`make all` will try to compile all targets at once.

### NES and Linux targets

Assuming you have both Raylib and and cc65 installed, you can compile the NES
version with `make main.nes`, and the Linux version with `make main`.

### Web

To build for web, you'll need to set up [Emscripten](https://emscripten.org).
Before compiling, make sure to add the Emscripten SDK to your path using
`source /path/to/emsdk/emsdk_env.sh`. Then use `make web/main.html` to build
the game.

> NOTE: The Makefile assumes Raylib is located at `$HOME/raylib`. If you have
it in a different location, you'll need to edit the Makefile accordingly.

## Credits

Game code by João F. S. Pereira (TheFallenWarrior).

Some `#define`s in `pc-port.c` were borrowed from the cc65 runtime library.

Game design was heavily inspired by
[Amulet of Yendor](https://www.mobygames.com/game/13440/amulet-of-yendor/) and
Leslie Bird's [Wizard's Castle](https://github.com/lesliesbird/WizardsCastle).