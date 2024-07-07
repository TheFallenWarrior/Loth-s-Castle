# Loth's Castle

Loth's Castle is an old-school ASCII rogue-like for NES and PC. It is a
Wizard's Castle clone written in C.

For the sake of simplicity, the entirety of the game's source code resides in
`main.c` and the PC-specific code sits in `pc-port.c`.

## Building

As is, Loth's Castle can be compiled to NES through
[cc65](https://github.com/cc65/cc65) or to many modern devices using
[Raylib](https://github.com/raysan5/raylib) (tested on Linux and Windows, but
other platforms should work too).

Assuming you have both Raylib and and cc65 installed, you can compile the NES
version with `make main.nes`, and the Linux version with `make main`. `make
all` will compile both targets at once.

Both versions should look and behave the same aside from drawscreen times.

## Credits

Game code by João F. S. Pereira (TheFallenWarrior).

Some `#define`s in `pc-port.c` were taken from the cc65 runtime library.

Game design was heavily inspired by
[Amulet of Yendor](https://www.mobygames.com/game/13440/amulet-of-yendor/) and
Leslie Bird's [Wizard's Castle](https://github.com/lesliesbird/WizardsCastle).