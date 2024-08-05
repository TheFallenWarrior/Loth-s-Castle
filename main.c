/*
	Loth's Castle
	Copyright 2024 TheFallenWarrior
*/

#ifdef __NES__
	#include <conio.h>
	#include <joystick.h>
	#include <nes.h>
	#define init()	joy_install(joy_static_stddrv)
#else
	#include "pc-port.c"
#endif
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

#define JOY_DPAD_MASK   0xf0

#define D4          (1+(rand()&3))
#define D8          (1+(rand()&7))
#define MIN(x, y)   ((x) > (y) ? (y) : (x))

void cprintfxy(uint8_t, uint8_t, const char*, ...);
uint8_t waitForInput(uint8_t);
void clearScreenArea(uint8_t, uint8_t);
void drawWindow(uint8_t, uint8_t, uint8_t, uint8_t);
void revealRoom(uint8_t, uint8_t, uint8_t);
void playerAttack();
uint8_t playerBribe();
void enemyAttack();
void battle();
void drinkFountain();
void vendor();
void useTorch();
void trigger();
void interact();
void charCreation();
void deathScreen();
void updateStats();
void drawScreen();
uint8_t gameLoop();

// Used to avoid confusion in Player.pos
enum coordinates{
	X,
	Y,
	Z
};

enum playerRaceTypes{
	HUMAN,
	ELF,
	DWARF,
	GNOLL
};

enum weaponTypes{
	STICK,
	DAGGER,
	MACE,
	SWORD
};

enum armorTypes{
	RAGS,
	LEATHER,
	CHAIN,
	PLATE
};

enum treasureTypes{
	RUBYRED,    // Avoid Lethargy
	NORNSTONE,  // Nothing
	PALEPEARL,  // Avoid Bleeding
	OPALEYE,    // Cure Blindness
	GREENGEM,   // Avoid Forgetting
	BLUEFLAME,  // Nothing
	PALINTIR,   // Nothing
	SYLMARYL    // Nothing
};

enum treasureMasks{
	RUBYRED_MASK    = 0x01,
	NORNSTONE_MASK  = 0x02,
	PALEPEARL_MASK  = 0x04,
	OPALEYE_MASK    = 0x08,
	GREENGEM_MASK   = 0x10,
	BLUEFLAME_MASK  = 0x20,
	PALINTIR_MASK   = 0x40,
	SYLMARYL_MASK   = 0x80
};

enum statusAilments{
	HEALTHY,
	LETHARGY,
	BLEED,
	FORGETFULNESS,
	BLINDNESS
};

enum roomContents{
	UNKNOWN,
	EMPTY,
	UPSTAIRS,
	DOWNSTAIRS,
	FOUNTAIN,
	GOLDPIECES,
	CHEST,
	MONSTER,
	TREASURE,
	WARP,
	VENDOR,
	YENDORORB
};

enum enemyTypes{
	GOBLIN,
	KOBOLD,
	WOLF,
	ORC,
	OGRE,
	LAMIA,
	TROLL,
	MINOTAUR,
	CHIMERA,
	DRAGON
};

enum buttons{
	A,
	B,
	SELECT,
	START
};

const char* const playerRaceNames[] = {
	"HUMAN",
	"ELF",
	"DWARF",
	"GNOLL"
};

const char* const sexNames[] = {
	"MALE",
	"FEMALE"
};

const char* const armorNames[] = {
	"RAGS",
	"LEATHER",
	"CHAIN",
	"PLATE"
};

const char* const weaponNames[] = {
	"STICK",
	"DAGGER",
	"MACE",
	"SWORD"
};

const char* const treasureNames[] = {
	"RUBY RED",
	"NORNSTONE",
	"PALE PEARL",
	"OPAL EYE",
	"GREEN GEM",
	"BLUE FLAME",
	"PALINTIR",
	"SYLMARYL"
};

const char* const statusNames[] = {
	"",
	"LETHARGIC",
	"BLEEDING",
	"",
	"BLIND"
};

const char* const enemyNames[] = {
	"GOBLIN",
	"KOBOLD",
	"WOLF",
	"ORC",
	"OGRE",
	"LAMIA",
	"TROLL",
	"MINOTAUR",
	"CHIMERA",
	"DRAGON"
};

const char* const attrChangeDescriptions[] = {
	"BETTER",
	"FLIMSIER",
	"NIMBLER",
	"CLUMSIER",
	"SMARTER",
	"STUPIDER"
};

const char* const messageStrings[] = {
	"",
	"Lit up a torch.",
	"Ascended the stairs.",
	"Descended the stairs.",
	"Drank from the fountain.",
	"",
	"Opened the chest.",
	"",
	"",
	"You've been teleported.",
	"",
	"You have the Orb of Power."
};

const char* const roomDescriptions[] = {
	"an empty room",
	"stairs going up",
	"stairs going down",
	"a fountain of water",
	"a pile of gold pieces",
	"a wooden chest",
	"",
	"a treasure item",
	"",
	"a vendor",
	"the Orb of Power"
};

const char* const interactionPrompts[] = {
	"ascend",
	"descend",
	"drink",
	"grab",
	"open",
	"",
	"grab",
	"",
	"interact",
	"grab"
};

const char* const deathCauses[] = {
	"DEATHSPELL",
	"blood loss",
	"clumsiness",
	"stupidity"
};

const char* const vendorQuotes[] = {
	" `Why is there so much gold\r\n\x0e"
	" in the Astral Plane?'",
	" `Old Loth was a despot.'",
	" `Another earthling?\r\n\x0e"
	" MINOTAURs love flesh like\r\n\x0e"
	" yours.'",
	" `You look cute.'",
	" `Please look through my\r\n\x0e"
	" wares.'",
	" `Forget 25.807, DRAGONs are\r\n\x0e"
	" the real root of all evil.'"
};

#ifdef __NES__
	const char* const buttonNames[] = {
		"A",
		"B",
		"SELECT",
		"START"
	};
#else
	const char* const buttonNames[] = {
		"C",
		"X",
		"SPACE",
		"RETURN"
	};
#endif

const uint8_t mapIcons[] = {
	'?',
	' ',
	'U',
	'D',
	'F',
	'G',
	'C',
	'M',
	'T',
	'W',
	'V',
	'?'
};

// Used to map the treasure bits
const uint8_t bitMaskTable[] = {
	0x01,
	0x02,
	0x04,
	0x08,
	0x10,
	0x20,
	0x40,
	0x80,
};

uint8_t message;
uint8_t vendorsAngry;
uint8_t killedByDeathspell;

// Castle rooms 3D matrix
//            Z  Y  X
uint8_t rooms[8][8][8];

struct{
	uint8_t hp, dex, spi;
	uint8_t sex, race;
	uint16_t gold;
	uint8_t arm, weap;
	uint8_t status, treasures;
	uint8_t torches, orb;
	uint8_t pos[3];
	uint16_t turns;
} Player;

struct{
	uint8_t type;
	uint8_t hp;
} Enemy;

#ifdef __NES__
	#pragma bss-name (push, "ZEROPAGE")
	#pragma data-name (push, "ZEROPAGE")
#endif

// General purpose variables, put in zero-page for faster access
uint8_t i, j, k, l;
uint16_t i16;

// Equivalent to gotoxy(x, y); cprintf(str, ...);
void cprintfxy(uint8_t x, uint8_t y, const char *str, ...){
	va_list args;
	va_start(args, str);

	gotoxy(x, y);
	vcprintf(str, args);

	va_end(args);
}

// Waits for input specified by a mask and then returns it
uint8_t waitForInput(uint8_t mask){
	if(!mask) mask = 0xff;
	while(joy_read(JOY_1));
	while(!(mask&joy_read(JOY_1)));
	return joy_read(JOY_1);
}

// Clears screen from y to max_y
void clearScreenArea(uint8_t y, uint8_t max_y){
	if(y >= max_y) return;
	cclearxy(0, y, (max_y-y)*32);
}

// Draws a rectangle border
void drawWindow(uint8_t x, uint8_t y, uint8_t width, uint8_t height){
	if(!width || !height) return;
	cputcxy(x, y, '\x14');
	cputcxy(x+width, y, '\x12');
	cputcxy(x, y+height, '\x11');
	cputcxy(x+width, y+height, '\x08');

	chlinexy(x+1, y, width-1);
	chlinexy(x+1, y+height, width-1);
	cvlinexy(x, y+1, height-1);
	cvlinexy(x+width, y+1, height-1);
}

void revealRoom(uint8_t x, uint8_t y, uint8_t z){
	l = rooms[z][y][x];
	if(!l){
		l = 1+rand()%15;
		if(l >= YENDORORB) l = EMPTY;
		if(l == MONSTER)   l = 0x80 | (rand()%(2+Player.turns/10))%10;
		rooms[z][y][x] = l;
	}
	cclearxy(1, 22, 30);
}

void playerAttack(){
	l = D4+Player.weap;
	j = (Enemy.type>l ? 0 : l-Enemy.type);
	Enemy.hp = (j>Enemy.hp ? 0 : Enemy.hp-j);
	clearScreenArea(21, 28);
	drawWindow(0, 20, 31, 7);
	cprintfxy(
		1, 20,
		"%s"
		"You attack the %s!",
		"MESSAGE\r\n\x0e\n",
		enemyNames[Enemy.type]
	);
	waitForInput(0);
	cclearxy(1, 22, 30);
	cprintfxy(
		1, 22,
		"%s took %d damage.",
		enemyNames[Enemy.type], j
	);
}

uint8_t playerBribe(){
	i16 = MIN(rand()%1000, Player.gold);
	Player.gold -= i16;
	clearScreenArea(21, 28);
	drawWindow(0, 20, 31, 7);
	cprintfxy(
		1, 20,
		"%s"
		"Gave %d GP to %s.",
		"MESSAGE\r\n\x0e\n",
		i16, enemyNames[Enemy.type]
	);
	updateStats();
	waitForInput(0);
	if(Player.spi+D4 > Enemy.type+D8){
		cclearxy(1, 22, 30);
		cprintfxy(
			1, 22,
			"%s says:\r\n\x0e"
			"%s",
			enemyNames[Enemy.type],
			" `Ok, just don't tell anyone.'"
		);
		waitForInput(0);
		if(rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]] == VENDOR)
			vendorsAngry = 0; 
		rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]] = EMPTY;
		return 1;
	} else{
		cclearxy(1, 22, 30);
		cprintfxy(
			1, 22,
			"%s says:\r\n\x0e"
			"%s",
			enemyNames[Enemy.type],
			" `All I want is your life!'"
		);
		return 0;
	}
}

void enemyAttack(){
	clearScreenArea(21, 28);
	drawWindow(0, 20, 31, 7);
	cprintfxy(
		1, 20,
		"%s"
		"%s attacks you!",
		"MESSAGE\r\n\x0e\n",
		enemyNames[Enemy.type]
	);
	waitForInput(0);
	if(Player.dex > 2*Enemy.type + D4){
		cputsxy(
			1, 22,
			"But you swiftly evade the\r\n\x0e"
			"blow."
		);
		waitForInput(0);
		return;
	}
	l = (D4+Enemy.type)/2;
	j = (Player.arm>l ? 0 : l-Player.arm);
	Player.hp = (j>Player.hp ? 0 : Player.hp-j);
	clearScreenArea(21, 28);
	drawWindow(0, 20, 31, 7);
	cprintfxy(
		1, 20,
		"%s"
		"%s took %d damage.",
		"MESSAGE\r\n\x0e\n", "You", j
	);
	updateStats();
	waitForInput(0);
}

void battle(){
	while(1){
		clearScreenArea(21, 28);
		drawWindow(0, 20, 31, 7);
		cputsxy(
			1,20,
			"COMMAND\r\n\x0e\n"
			"Up:   FIGHT\r\n\x0e"
			"Right:BRIBE\r\n\x0e"
			"Down: RUN\r\n\x0e"
			"Left: CAST"
		);
		i = waitForInput(JOY_DPAD_MASK);
		if(JOY_UP(i)) playerAttack();
		else if(JOY_RIGHT(i)){
			if(!Player.gold){
				clearScreenArea(21, 28);
				drawWindow(0, 20, 31, 7);
				cprintfxy(
					1, 20,
					"%s"
					"You have no money.",
					"MESSAGE\r\n\x0e\n"
				);
				waitForInput(0);
				continue;
			}
			if(playerBribe()) return;

		}
		else if(JOY_DOWN(i)){
			if(Player.dex+D4 > Enemy.type+D8){
				clearScreenArea(21, 28);
				drawWindow(0, 20, 31, 7);
				cprintfxy(
					1, 20,
					"%s"
					"Run to where?\r\n\x0e"
					"(Use your D-pad.)",
					"MESSAGE\r\n\x0e\n"
				);
				j = waitForInput(JOY_DPAD_MASK);
				if(JOY_UP(j))         Player.pos[Y] = (Player.pos[Y]-1)&7;
				else if(JOY_DOWN(j))  Player.pos[Y] = (Player.pos[Y]+1)&7;
				else if(JOY_LEFT(j))  Player.pos[X] = (Player.pos[X]-1)&7;
				else if(JOY_RIGHT(j)) Player.pos[X] = (Player.pos[X]+1)&7;
				revealRoom(Player.pos[X], Player.pos[Y], Player.pos[Z]);
				trigger();
				return;
			} else{
				clearScreenArea(21, 28);
				drawWindow(0, 20, 31, 7);
				cprintfxy(1, 20, "%sFailed to run away.", "MESSAGE\r\n\x0e\n");
			}
		}
		else if(JOY_LEFT(i)){
			clearScreenArea(21, 28);
			drawWindow(0, 20, 31, 7);
			cprintfxy(
				1, 20,
				"%s"
				"You can't cast spells.",
				"MESSAGE\r\n\x0e\n"
			);
			waitForInput(0);
			continue;
		}
		waitForInput(0);
		if(!Enemy.hp) break;

		enemyAttack();
		if(!Player.hp){
			clearScreenArea(21, 28);
			drawWindow(0, 20, 31, 7);
			cprintfxy(
				1, 20,
				"%s"
				"You succumb to your wounds.",
				"MESSAGE\r\n\x0e\n",
				enemyNames[Enemy.type]
			);
			waitForInput(0);
			return;
		}

	}
	clearScreenArea(21, 28);
	drawWindow(0, 20, 31, 7);
	cprintfxy(
		1, 20,
		"%s"
		"You killed %s.",
		"MESSAGE\r\n\x0e\n",
		enemyNames[Enemy.type]
	);
	waitForInput(0);
	cclearxy(1, 22, 30);
	i16 = rand()%((1+Enemy.type)*175);
	if(rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]] == VENDOR)
		i16 *= 4;
	Player.gold += i16;
	cprintfxy(
		1, 20,
		"%s"
		"%s dropped %d GP.",
		"MESSAGE\r\n\x0e\n",
		enemyNames[Enemy.type], i16
	);
	updateStats();
	waitForInput(0);
	rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]] = EMPTY;
	message = 0;
}

void drinkFountain(){
	l = rand()&7;
	cclearxy(1, 22, 30);
	cclearxy(1, 24, 30);
	cputsxy(1, 22, "Drank from the fountain.");
	waitForInput(0);
	if(l > 5){
		Player.race = rand()&3;
		cclearxy(1, 22, 30);
		cprintfxy(
			1, 22,
			"You turned into a %s.",
			playerRaceNames[Player.race]
		);
		message = 0;
		waitForInput(0);
		return;
	} else switch(l){
		case 0:
		++Player.hp;
		break;

		case 1:
		--Player.hp;
		break;

		case 2:
		++Player.dex;
		break;

		case 3:
		--Player.dex;
		break;

		case 4:
		++Player.spi;
		break;

		case 5:
		--Player.spi;
		break;
	}
	updateStats();
	cclearxy(1, 22, 30);
	cprintfxy(1, 22, "You feel %s.", attrChangeDescriptions[l]);
	message = 0;
	waitForInput(0);
	return;
}

void vendor(){
	cclearxy(1, 22, 30);
	cclearxy(1, 24, 30);
	cputsxy(
		1,20,
		"COMMAND\r\n\x0e\n"
		"Up:   TRADE\r\n\x0e"
		"Right:TALK\r\n\x0e"
		"Down: ATTACK"
	);
	i = waitForInput(JOY_DPAD_MASK & ~JOY_LEFT_MASK);

	clearScreenArea(21, 28);
	drawWindow(0, 20, 31, 7);
	cputsxy(1, 20, "MESSAGE\r\n\x0e\n");
	if(JOY_UP(i)){
		if(Player.gold < 1000){
			cprintf(
				"%s says:\r\n\x0e"
				"%s",
				"VENDOR",
				" `You don't have enough\r\n\x0e money.'"
			);
			message = 0;
			waitForInput(0);
			return;
		}
		cprintfxy(
			1, 20,
			"BUY ARMOR\r\n\x0e\n"
			"%s",
			"Up:   Nothing\r\n\x0e"
		);
		if(LEATHER > Player.arm)
			cprintf("Right:%s(1000)\r\n\x0e", armorNames[LEATHER]);
		if(CHAIN > Player.arm)
			cprintf("Down: %s  (2000)\r\n\x0e", armorNames[CHAIN]);
		if(PLATE > Player.arm)
			cprintf("Left: %s  (3000)", armorNames[PLATE]);

		j = waitForInput(JOY_DPAD_MASK);
		if(JOY_RIGHT(j) && LEATHER > Player.arm && Player.gold > 1000){
			Player.arm = LEATHER;
			Player.gold -= 1000;
			l = 1;
		}
		else if(JOY_DOWN(j) && CHAIN > Player.arm && Player.gold > 2000){
			Player.arm = CHAIN;
			Player.gold -= 2000;
			l = 1;
		}
		else if(JOY_LEFT(j) && PLATE > Player.arm && Player.gold > 3000){
			Player.arm = PLATE;
			Player.gold -= 3000;
			l = 1;
		}
		else l = 0;
		updateStats();
		clearScreenArea(21, 28);
		drawWindow(0, 20, 31, 7);
		cputsxy(1, 20, "MESSAGE\r\n\x0e\n");
		if(l) cprintf("Bought the %s.", armorNames[Player.arm]);
		else cputs("Nevermind.");
		waitForInput(0);

		clearScreenArea(21, 28);
		drawWindow(0, 20, 31, 7);
		cprintfxy(
			1, 20,
			"BUY WEAPON\r\n\x0e\n"
			"%s",
			"Up:   Nothing\r\n\x0e"
		);
		if(DAGGER > Player.weap)
			cprintf("Right:%s (1000)\r\n\x0e", weaponNames[DAGGER]);
		if(MACE > Player.weap)
			cprintf("Down: %s   (2000)\r\n\x0e", weaponNames[MACE]);
		if(SWORD > Player.weap)
			cprintf("Left: %s  (3000)", weaponNames[SWORD]);

		j = waitForInput(JOY_DPAD_MASK);
		if(JOY_RIGHT(j) && DAGGER > Player.weap && Player.gold > 1000){
			Player.weap = DAGGER;
			Player.gold -= 1000;
			l = 1;
		}
		else if(JOY_DOWN(j) && MACE > Player.weap && Player.gold > 2000){
			Player.weap = MACE;
			Player.gold -= 2000;
			l = 1;
		}
		else if(JOY_LEFT(j) && SWORD > Player.weap && Player.gold > 3000){
			Player.weap = SWORD;
			Player.gold -= 3000;
			l = 1;
		}
		else l = 0;
		updateStats();
		clearScreenArea(21, 28);
		drawWindow(0, 20, 31, 7);
		cputsxy(1, 20, "MESSAGE\r\n\x0e\n");
		if(l) cprintf("Bought the %s.", weaponNames[Player.weap]);
		else cputs("Nevermind.");
		waitForInput(0);

		clearScreenArea(21, 28);
		drawWindow(0, 20, 31, 7);
		cprintfxy(
			1, 20,
			"BUY POTION\r\n\x0e\n"
			"%s",
			"Up:   Nothing\r\n\x0e"
		);
		if(Player.hp < 18)  cputs("Right:Hit Points (1000)\r\n\x0e");
		if(Player.dex < 18) cputs("Down: Dexterity  (1000)\r\n\x0e");
		if(Player.spi < 18) cputs("Left: Spirit     (1000)");

		j = waitForInput(JOY_DPAD_MASK);
		if(JOY_RIGHT(j) && Player.hp < 18 && Player.gold > 1000){
			++Player.hp;
			Player.gold -= 1000;
			l = 1;
		}
		else if(JOY_DOWN(j) && Player.dex < 18 && Player.gold > 1000){
			++Player.dex;
			Player.gold -= 1000;
			l = 3;
		}
		else if(JOY_LEFT(j) && Player.spi < 18 && Player.gold > 1000){
			++Player.spi;
			Player.gold -= 1000;
			l = 5;
		}
		else l = 0;
		updateStats();
		clearScreenArea(21, 28);
		drawWindow(0, 20, 31, 7);
		cputsxy(1, 20, "MESSAGE\r\n\x0e\n");
		if(l) cprintf("You feel %s.", attrChangeDescriptions[l-1]);
		else cputs("Nevermind.");
		waitForInput(0);

		clearScreenArea(21, 28);
		drawWindow(0, 20, 31, 7);
		cputsxy(
			1, 20,
			"BUY TORCHES\r\n\x0e\n"
			"Buy 10 torches for 1000 GP?\r\n\x0e"
			"Up:   YES\r\n\x0e"
			"Down: NO"
		);

		j = waitForInput(JOY_UP_MASK | JOY_DOWN_MASK);
		if(JOY_UP(j) && Player.gold > 1000){
			Player.torches += 10;
			Player.gold -= 1000;
			l = 1;
		}
		else l = 0;
		updateStats();
		clearScreenArea(21, 28);
		drawWindow(0, 20, 31, 7);
		cputsxy(1, 20, "MESSAGE\r\n\x0e\n");
		if(l) cputs("Bought the torches.");
		else cputs("Nevermind.");
	}
	else if(JOY_RIGHT(i)){
		j = rand()%6;
		cprintf(
			"%s says:\r\n\x0e"
			"%s",
			"VENDOR",
			vendorQuotes[j]
		);
	}
	else if(JOY_DOWN(i)){
		vendorsAngry = 1;
		trigger();
	}
	message = 0;
	if(Player.hp) waitForInput(0);
}

// Light up rooms adjacent to the player
void useTorch(){
	if(rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]] == WARP)
		return;
	if(Player.torches){
		message = 1;
		--Player.torches;
		for(i=(Player.pos[X]+7); i<=(Player.pos[X]+9); ++i){
			for(j=(Player.pos[Y]+7); j<=(Player.pos[Y]+9); ++j)
				revealRoom(i&7, j&7, Player.pos[Z]);
		}
	}
}

// An event that happens automatically
void trigger(){
	k = rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]];
	if(k == WARP){
		for(i=0;i<3;++i) Player.pos[i] = rand()&7;
		revealRoom(Player.pos[X], Player.pos[Y], Player.pos[Z]);

		message = WARP;
		drawScreen();
		waitForInput(0);
		message = 0;
		trigger();
	}
	else if(k == VENDOR && vendorsAngry){
		drawScreen();
		Enemy.type = D4 + 5;
		Enemy.hp = D4 + Enemy.type;
		cclearxy(1, 22, 30);
		cclearxy(1, 24, 30);
		cprintfxy(
			1, 22,
			"Encounter!\r\n\x0e\n"
			"VENDOR shapeshifted into\r\n\x0e"
			"%s!",
			enemyNames[Enemy.type]
		);
		waitForInput(0);
		battle();
	}
	else if(k & 0x80){
		drawScreen();
		Enemy.type = k & 0x7f;
		Enemy.hp = D4 + Enemy.type;
		cclearxy(1, 22, 30);
		cclearxy(1, 24, 30);
		cprintfxy(
			1, 22,
			"Encounter!\r\n\x0e\n"
			"You are facing a lousy\r\n\x0e"
			"%s!",
			enemyNames[Enemy.type]
		);
		waitForInput(0);
		battle();
	}
}

// Interact with the object in the room
void interact(){
	k = rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]];
	if(k <= 1) return;
	message = k;
	switch(k){
		case UPSTAIRS:
		Player.pos[Z] = (Player.pos[Z]+1)&7;
		revealRoom(Player.pos[X], Player.pos[Y], Player.pos[Z]);
		drawScreen();
		waitForInput(0);
		message = 0;
		trigger();
		break;

		case DOWNSTAIRS:
		Player.pos[Z] = (Player.pos[Z]-1)&7;
		revealRoom(Player.pos[X], Player.pos[Y], Player.pos[Z]);
		drawScreen();
		waitForInput(0);
		message = 0;
		trigger();
		break;

		case GOLDPIECES:
		i16 = 10*D8 + Player.turns/2 + rand()%(Player.turns/2);
		i16 = MIN(i16, 999);
		Player.gold += i16;
		cclearxy(1, 22, 30);
		cclearxy(1, 24, 30);
		cprintfxy(1, 22, "You found %d GP.", i16);
		updateStats();
		waitForInput(0);
		rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]] = EMPTY;
		break;

		case TREASURE:
		l = rand()&7;
		Player.treasures |= bitMaskTable[l];
		cclearxy(1, 22, 30);
		cclearxy(1, 24, 30);
		cprintfxy(1, 22, "You have obtained %s.", treasureNames[l]);
		rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]] = EMPTY;
		message = 0;
		waitForInput(0);
		break;

		case VENDOR:
		vendor();
		break;

		case FOUNTAIN:
		drinkFountain();
	}
}

void charCreation(){
	clrscr();

	Player.pos[X] = Player.pos[Y] = Player.pos[Z] = 0;
	Player.orb = Player.treasures = 0;
	Player.hp = Player.dex = Player.spi = j = 8;
	Player.gold = 60;

	drawWindow(0, 0, 31, 5);
	cprintfxy(
		1, 0,
		"SELECT YOUR RACE\r\n\x0e"
		"Up:   %s\r\n\x0e"
		"Right:%s\r\n\x0e"
		"Down: %s\r\n\x0e"
		"Left: %s",
		playerRaceNames[HUMAN],
		playerRaceNames[ELF],
		playerRaceNames[DWARF],
		playerRaceNames[GNOLL]
	);
	cprintfxy(
		0, 27,
		"Press %s to skip creation.",
		buttonNames[SELECT]
	);

	k = waitForInput(JOY_DPAD_MASK | JOY_SELECT_MASK);
	if(JOY_SELECT(k)){ //Skip character creation
		Player.race = 3;
		Player.sex = 1;
		Player.dex += 8;
		Player.arm  = CHAIN;
		Player.weap = MACE;
		Player.gold = 0;
		return;
	}
	if(JOY_UP(k)) Player.race = HUMAN;
	else if(JOY_RIGHT(k)){
		Player.race = ELF;
		Player.hp  -= 2;
		Player.dex += 2;
	}
	else if(JOY_DOWN(k)){
		Player.race = DWARF;
		Player.hp  += 2;
		Player.dex -= 2;
	}
	else if(JOY_LEFT(k)){
		Player.race = GNOLL;
		Player.hp  -= 4;
		Player.dex += 4;
	}

	cclearxy(0, 27, 32);
	drawWindow(0, 6, 31, 3);
	cprintfxy(
		1, 6,
		"SELECT YOUR SEX\r\n\x0e"
		"Up:   %s\r\n\x0e"
		"Down: %s",
		sexNames[0],
		sexNames[1]
	);

	k = waitForInput(JOY_UP_MASK | JOY_DOWN_MASK);
	if(JOY_UP(k))        Player.sex = 0;
	else if(JOY_DOWN(k)) Player.sex = 1;

	while(j){
		cclearxy(0, 10, 155);
		drawWindow(0, 10, 31, 5);
		cprintfxy(
			1, 10,
			"DISTRIBUTE YOUR STATS\r\n\x0e"
			"Up:   Hit Points %d\r\n\x0e"
			"Right:Dexterity  %d\r\n\x0e"
			"Down: Spirit     %d\r\n\x0e"
			"Points: %d",
			Player.hp, Player.dex, Player.spi, j
		);
		k = waitForInput(JOY_DPAD_MASK & ~JOY_LEFT_MASK);
		if(JOY_UP(k)){
			++Player.hp;
			--j;
		}
		else if(JOY_RIGHT(k) && Player.dex < 18){
			++Player.dex;
			--j;
		}
		else if(JOY_DOWN(k)){
			++Player.spi;
			--j;
		}
	}
	cprintfxy(
		1, 10,
		"DISTRIBUTE YOUR STATS\r\n\x0e"
		"Up:   Hit Points %d\r\n\x0e"
		"Right:Dexterity  %d\r\n\x0e"
		"Down: Spirit     %d\r\n\x0e"
		"Points: %d",
		Player.hp, Player.dex, Player.spi, j
	);
	drawWindow(0, 16, 15, 5);
	cprintfxy(
		1, 16,
		"BUY ARMOR\r\n\x0e"
		"Up:%s   (00)\r\n\x0e"
		"Ri:%s(10)\r\n\x0e"
		"Do:%s  (30)\r\n\x0e"
		"Le:%s  (50)",
		armorNames[RAGS],
		armorNames[LEATHER],
		armorNames[CHAIN],
		armorNames[PLATE]
	);
	drawWindow(0, 22, 31, 2);
	cprintfxy(1, 22, "YOUR GOLD\r\n\x0e%d GP", Player.gold);

	k = waitForInput(JOY_DPAD_MASK);
	if(JOY_RIGHT(k)){
		Player.arm = LEATHER;
		Player.gold -= 10;
	}
	else if(JOY_DOWN(k)){
		Player.arm = CHAIN;
		Player.gold -= 30;
	}
	else if(JOY_LEFT(k)){
		Player.arm = PLATE;
		Player.gold -= 50;
	}

	drawWindow(16, 16, 15, 5);
	cputsxy(17, 16, "BUY WEAPON");
	cprintfxy(17, 17, "Up:%s  (00)", weaponNames[STICK]);
	cprintfxy(17, 18, "Ri:%s (10)", weaponNames[DAGGER]);
	cprintfxy(17, 19, "Do:%s   (30)", weaponNames[MACE]);
	cprintfxy(17, 20, "Le:%s  (50)", weaponNames[SWORD]);
	cprintfxy(1, 23, "%02d GP", Player.gold);
	while(1){
		k = waitForInput(JOY_DPAD_MASK);
		if(JOY_UP(k)) break;
		if(JOY_RIGHT(k)){
			Player.weap = DAGGER;
			Player.gold -= 10;
			break;
		}
		else if(JOY_DOWN(k)){
			if(Player.gold < 30)
				continue;
			Player.weap = MACE;
			Player.gold -= 30;
			break;
		}
		else if(JOY_LEFT(k)){
			if(Player.gold < 50)
				continue;
			Player.weap = SWORD;
			Player.gold -= 50;
			break;
		}
	}

	drawWindow(0, 25, 31, 2);
	cprintfxy(8, 26, "Get ready, %s!", playerRaceNames[Player.race]);
	cprintfxy(1, 23, "%02d GP", Player.gold);
	waitForInput(0);

	// Automatically buy torches
	Player.torches = Player.gold/2;
	Player.gold = 0;
}

void deathScreen(){
	clrscr();
	drawWindow(0, 0, 31, 26);
	drawWindow(12, 0, 6, 2);
	cputsxy(13, 1, "DEATH");

	// Count player treasures
	i = 0;
	if(Player.treasures & RUBYRED_MASK)   ++i;
	if(Player.treasures & NORNSTONE_MASK) ++i;
	if(Player.treasures & PALEPEARL_MASK) ++i;
	if(Player.treasures & OPALEYE_MASK)   ++i;
	if(Player.treasures & GREENGEM_MASK)  ++i;
	if(Player.treasures & BLUEFLAME_MASK) ++i;
	if(Player.treasures & PALINTIR_MASK)  ++i;
	if(Player.treasures & SYLMARYL_MASK)  ++i;

	// Assess death cause
	if(!killedByDeathspell){
		if(!Player.hp)  j = 1;
		if(!Player.dex) j = 2;
		if(!Player.spi) j = 3;
	} else j = 0;

	cputsxy(
		1, 5,
		"And like  that, your  life has\r\n\x0e"
		"come  to  a  bitter  end. Your\r\n\x0e"
		"gravestone    reveals     what\r\n\x0e"
		"little is known about you:"
	);

	cprintfxy(
		1, 11,
		" `REST IN PEACE\r\n\x0e\n"
		"  Here lies a %s %s,\r\n\x0e"
		"  who died to %s\r\n\x0e"
		"  at the age of %d,\r\n\x0e"
		"  survived for %u turns,\r\n\x0e"
		"  had %u worth of gold,\r\n\x0e"
		"  had %d/8 treasure types.'",
		sexNames[Player.sex], playerRaceNames[Player.race],
		deathCauses[j], 14+(rand()&15), Player.turns,
		Player.gold, i
	);
	cprintfxy(10, 23, "PRESS %s", buttonNames[START]);
	waitForInput(JOY_START_MASK);

	// Clear game state
	killedByDeathspell = 0;
	vendorsAngry = 0;
	for(i=0;i<8;++i){
		for(j=0;j<8;++j){
			for(k=0;k<8;++k)
				rooms[i][j][k] = 0;
		}
	}
}

void updateStats(){
	Player.hp  = MIN(Player.hp, 18);
	Player.dex = MIN(Player.dex, 18);
	Player.spi = MIN(Player.spi, 18);
	// Draw player stats window
	clearScreenArea(0, 4);
	drawWindow(0, 0, 31, 4);

	cprintfxy(
		1, 0,
		"%s %s\r\n\x0e"
		"Hits:%2d  Attr: %2d/%2d  GP:%u\r\n\x0e"
		"AC:  %2d  Torch:%2d     XY:%1d%1d\r\n\x0e"
		"Weapon:%6s         Lv:%1d",
		(
			Player.status ?
			statusNames[Player.status] :
			sexNames[Player.sex]
		),
		playerRaceNames[Player.race],
		Player.hp, Player.dex, Player.spi, Player.gold,
		10 - (Player.arm*3), Player.torches,
		Player.pos[X]+1, Player.pos[Y]+1,
		weaponNames[Player.weap], Player.pos[Z]+1
	);
}

void drawScreen(){
	clrscr();
	updateStats();

	// Draw message window
	drawWindow(0, 20, 31, 7);
	cputsxy(1, 20, "MESSAGE\r\n\x0e\n");

	if(message) cputs(messageStrings[message]);
	else{
		k = rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]];
		if(!(k & 0x80)){
			cprintf("You see %s.", roomDescriptions[k-1]);
			if(k > EMPTY){
				cprintfxy(
					1, 24,
					"Press %s to %s.",
					buttonNames[A],
					interactionPrompts[k-2]
				);
			}
		}
	}

	// Draw map screen
	gotoxy(0, 5);
	for(i=0;i<8;++i){
		for(j=0;j<8;++j){
			l = rooms[Player.pos[Z]][i][j];
			k = mapIcons[l];
			if(j == Player.pos[X] && i == Player.pos[Y]){
				revers(1);
				if(l == EMPTY) k = '@';
			}
			if(l & 0x80) k = mapIcons[MONSTER];
			cprintf("[%c]", k);
			revers(0);
			cputc(' ');
		}
		cputc('\n');
	}

	// This shouldn't be here.
	k = rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]];
}

uint8_t gameLoop(){
	++Player.turns;
	revealRoom(Player.pos[X], Player.pos[Y], Player.pos[Z]);
	trigger();
	if(!Player.hp || !Player.dex || !Player.spi) return 0;

	drawScreen();

	message = 0;
	k = waitForInput(0);
	if(JOY_UP(k))         Player.pos[Y] = (Player.pos[Y]-1)&7;
	else if(JOY_DOWN(k))  Player.pos[Y] = (Player.pos[Y]+1)&7;
	else if(JOY_LEFT(k))  Player.pos[X] = (Player.pos[X]-1)&7;
	else if(JOY_RIGHT(k)) Player.pos[X] = (Player.pos[X]+1)&7;
	else if(JOY_BTN_A(k)) interact();
	else if(JOY_BTN_B(k)) useTorch();

	if(!Player.hp || !Player.dex || !Player.spi) return 0;
	else return 1;
}

int main(){
	init();

	// Title Screen
	clrscr();
	drawWindow(0, 0, 31, 26);
	drawWindow(8, 0, 14, 2);
	cputsxy(9, 1, "LOTH'S CASTLE");

	cputsxy(
		1, 5,
		"1000  years  ago, the  demonic\r\n\x0e"
		"priest    Loth     transported\r\n\x0e"
		"himself into the  Astral Plane\r\n\x0e"
		"to  forge  what  would  become\r\n\x0e"
		"known as the `Orb of Power'.\r\n\x0e\n"
		"Although the priest himself is\r\n\x0e"
		"long  gone,  the Orb  and  his\r\n\x0e"
		"many    riches    still    lie\r\n\x0e"
		"somewhere in his castle.\r\n\x0e\n"
		"Because  of that, many  delved\r\n\x0e"
		"deep into the  Astral Plane in\r\n\x0e"
		"search  of  the Orb of  Power,\r\n\x0e"
		"only to never return..."
	);

	cprintfxy(10, 23, "PRESS %s", buttonNames[START]);
	cputsxy(0, 27, "(c) 2024 TheFallenWarrior");

	while(!JOY_START(joy_read(JOY_1))) ++j;
	srand(j);

	while(1){
		rooms[1+rand()%7][1+rand()%7][1+rand()%7] = YENDORORB;
		rooms[0][0][0] = EMPTY;

		charCreation();

		clrscr();
		while(gameLoop());
		deathScreen();
	}

	joy_uninstall();
	return 0;
}