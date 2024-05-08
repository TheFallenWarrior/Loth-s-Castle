/*
	Loth's Castle
*/

#ifdef __NES__
	#include <conio.h>
	#include <joystick.h>
	#include <nes.h>
	#define init()	joy_install(joy_static_stddrv)
#else
	#include "pc-port.c"
#endif
#include <stdint.h>
#include <stdlib.h>

#define D4         	(1+rand()%4)
#define D8         	(1+rand()%8)
#define MIN(x, y)  	((x) > (y) ? (y) : (x))

#define bRUBYRED    0x01    // Avoid Lethargy
#define bNORNSTONE  0x02    // Nothing
#define bPALEPEARL  0x04    // Avoid Leech
#define bOPALEYE    0x08    // Cure Blindness
#define bGREENGEM   0x10    // Avoid Forgetting
#define bBLUEFLAME  0x20    // Nothing
#define bPALINTIR   0x40    // Nothing
#define bSYLMARYL   0x80    // Nothing

uint8_t waitForInput();
void clearScreenArea(uint8_t, uint8_t);
void drawWindow(uint8_t, uint8_t, uint8_t, uint8_t);
void revealRoom(uint8_t, uint8_t, uint8_t);
void battle();
void useTorch();
void trigger();
void interact();
void charCreation();
void updateStats();
void drawScreen();
void gameLoop();

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
	nRUBYRED,
	nNORNSTONE,
	nPALEPEARL,
	nOPALEYE,
	nGREENGEM,
	nBLUEFLAME,
	nPALINTIR,
	nSYLMARYL
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

const char* const playerRaceNames[] = {
	"HUMAN",
	"ELF",
	"DWARF",
	"GNOLL"
};

const char* const genderNames[] = {
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
	"SICK",
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
	"Opened the chest.",
	"",
	"",
	"You've been teleported.",
	"",
	"You have the Orb of Yendor."
};

const char* const roomDescriptions[] = {
	"an empty room",
	"stairs going up",
	"stairs going down",
	"a fountain of water",
	"a wooden chest",
	"",
	"a treasure item",
	"",
	"a vendor",
	"the Orb of Yendor"
};

const uint8_t mapIcons[] = {
	'?',
	' ',
	'U',
	'D',
	'F',
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

// General purpose variables
uint8_t i, j, k, l;

uint8_t message;

// Castle rooms 3D matrix
//            Z  Y  X
uint8_t rooms[8][8][8];

struct{
	uint8_t hp, dex, spi;
	uint8_t gender, race;
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

// Waits for controller input and then returns it
uint8_t waitForInput(){
	while(joy_read(JOY_1));
	while(!(joy_read(JOY_1)));
	return joy_read(JOY_1);
}

// Clears screen from x to max_x
void clearScreenArea(uint8_t x, uint8_t max_x){
	for(i=x;i<max_x;++i){
		cclearxy(0, i, 32);
	}
}

// Draws a rectangle border
void drawWindow(uint8_t x, uint8_t y, uint8_t width, uint8_t height){
	if(!width || !height) return;
	cputcxy(x, y, '\24');
	cputcxy(x+width, y, '\22');
	cputcxy(x, y+height, '\21');
	cputcxy(x+width, y+height, '\10');

	for(i=x+1;i<x+width;++i){
		cputcxy(i, y, '\13');
		cputcxy(i, y+height, '\13');
	}
	for(i=y+1;i<y+height;++i){
		cputcxy(x, i, '\16');
		cputcxy(x+width, i, '\16');
	}
}

void revealRoom(uint8_t x, uint8_t y, uint8_t z){
	l = rooms[z][y][x];
	if(!l){
		l = 1+rand()%9;
		if(l == MONSTER){
			l = 0x80 | (rand()%(2+Player.turns/10))%10;
		}
		rooms[z][y][x] = l;
	}
	cclearxy(1, 21, 30);
}

void battle(){
	static uint16_t loot;
	while(1){
		clearScreenArea(21, 28);
		drawWindow(0, 20, 31, 7);
		cputsxy(1,20,
			"COMMAND\r\n\16"
			"Up:   FIGHT\r\n\16"
			"Right:BRIBE\r\n\16"
			"Down: RUN\r\n\16"
			"Left: CAST"
		);
		i = waitForInput();
		if(!(i&0xf0)) continue;
		if(JOY_UP(i)){
			l = D4+Player.weap;
			j = (Enemy.type>l ? 0 : l-Enemy.type);
			Enemy.hp = (j>Enemy.hp ? 0 : Enemy.hp-j);
			clearScreenArea(21, 28);
			drawWindow(0, 20, 31, 7);
			gotoxy(1, 20);
			cprintf(
				"%s"
				"%s took %d damage.",
				"MESSAGE\r\n\16",
				enemyNames[Enemy.type], j
			);
		}
		else if(JOY_RIGHT(i)){
			if(!Player.gold){
				clearScreenArea(21, 28);
				drawWindow(0, 20, 31, 7);
				gotoxy(1,20);
				cprintf(
					"%s"
					"You have no money.",
					"MESSAGE\r\n\16"
				);
				waitForInput();
				continue;
			}
			loot = rand()%1000;
			loot = MIN(loot, Player.gold);
			Player.gold -= loot;
			clearScreenArea(21, 28);
			drawWindow(0, 20, 31, 7);
			gotoxy(1, 20);
			cprintf(
				"%s"
				"Gave %d GP to %s.",
				"MESSAGE\r\n\16",
				loot, enemyNames[Enemy.type]
			);
			updateStats();
			waitForInput();
			if(Player.spi+D4 > Enemy.type+D8){
				cclearxy(1, 21, 30);
				gotoxy(1, 21);
				cprintf(
					"%s became amicable.",
					enemyNames[Enemy.type]
				);
				waitForInput();
				rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]] = EMPTY;
				return;
			} else{
				cclearxy(1, 21, 30);
				gotoxy(1, 21);
				cprintf(
					"%s wasn't convinced.",
					enemyNames[Enemy.type]
				);
			}
		}
		else if(JOY_DOWN(i)){
			if(Player.dex+D4 > Enemy.type+D8){
				clearScreenArea(21, 28);
				drawWindow(0, 20, 31, 7);
				gotoxy(1, 20);
				cprintf(
					"%s"
					"Run to where?\r\n\16"
					"(Use your D-pad.)",
					"MESSAGE\r\n\16"
				);
				j = waitForInput();
				if(!(j&0xf0)) continue;
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
				gotoxy(1, 20);
				cprintf("%sFailed to run away.", "MESSAGE\r\n\16");
			}
		}
		else if(JOY_LEFT(i)){
			clearScreenArea(21, 28);
			drawWindow(0, 20, 31, 7);
			gotoxy(1, 20);
			cprintf(
				"%s"
				"You can't cast spells.",
				"MESSAGE\r\n\16"
			);
			waitForInput();
			continue;
		}
		waitForInput();
		if(!Enemy.hp) break;
		if(Player.dex > 2*Enemy.type + D4) continue;
		clearScreenArea(21, 28);
		drawWindow(0, 20, 31, 7);
		gotoxy(1, 20);
		cprintf(
			"%s"
			"%s attacks you!",
			"MESSAGE\r\n\16",
			enemyNames[Enemy.type]
		);
		waitForInput();
		l = (D4+Enemy.type)/2;
		j = (Player.arm>l ? 0 : l-Player.arm);
		Player.hp = (j>Player.hp ? 0 : Player.hp-j);
		clearScreenArea(21, 28);
		drawWindow(0, 20, 31, 7);
		gotoxy(1, 20);
		cprintf(
			"%s"
			"%s took %d damage.",
			"MESSAGE\r\n\16", "You", j
		);
		updateStats();
		waitForInput();
	}
	clearScreenArea(21, 28);
	drawWindow(0, 20, 31, 7);
	gotoxy(1, 20);
	cprintf(
		"%s"
		"You killed %s.",
		"MESSAGE\r\n\16",
		enemyNames[Enemy.type]
	);
	waitForInput();
	cclearxy(1, 21, 30);
	loot = rand()%1000;
	Player.gold += loot;
	gotoxy(1, 20);
	cprintf(
		"%s"
		"%s dropped %d GP.",
		"MESSAGE\r\n\16",
		enemyNames[Enemy.type], loot
	);
	updateStats();
	waitForInput();
	rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]] = EMPTY;
	message = 0;
}

void drinkFountain(){
	l = rand()%8;
	cclearxy(1, 21, 30);
	cclearxy(1, 23, 30);
	cputsxy(1, 21, "Drank from the fountain.");
	waitForInput();
	if(l > 5){
		Player.race = rand()%4;
		cclearxy(1, 21, 30);
		gotoxy(1, 21);
		cprintf("You turned into a %s.", playerRaceNames[Player.race]);
		message = 0;
		waitForInput();
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
	cclearxy(1, 21, 30);
	gotoxy(1, 21);
	cprintf("You feel %s.", attrChangeDescriptions[l]);
	message = 0;
	waitForInput();
	return;
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
	switch(k){
		case WARP:
		for(i=0;i<3;++i) Player.pos[i] = rand()&7;
		revealRoom(Player.pos[X], Player.pos[Y], Player.pos[Z]);
		message = WARP;
		break;


	}
	// case MONSTER:
	if(k & 0x80){
		drawScreen();
		Enemy.type = k & 0x7f;
		Enemy.hp = D4 + Enemy.type;
		cclearxy(1, 21, 30);
		cclearxy(1, 23, 30);
		gotoxy(1, 21);
		cprintf(
			"Encounter!\r\n\16"
			"A lousy %s appeared!",
			enemyNames[Enemy.type]
		);
		waitForInput();
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
		break;

		case DOWNSTAIRS:
		Player.pos[Z] = (Player.pos[Z]-1)&7;
		break;

		case TREASURE:
		l = rand()%8;
		Player.treasures |= bitMaskTable[l];
		cclearxy(1, 21, 30);
		cclearxy(1, 23, 30);
		gotoxy(1, 21);
		cprintf("You have obtained %s.", treasureNames[l]);
		rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]] = EMPTY;
		message = 0;
		waitForInput();
		break;

		case FOUNTAIN:
		drinkFountain();
	}
}

void charCreation(){
	clrscr();

	Player.hp = Player.dex = Player.spi = j = 8;
	Player.torches = 5;
	Player.gold = 60;

	drawWindow(0, 0, 31, 5);
	gotoxy(1, 0);
	cprintf(
		"SELECT YOUR RACE\r\n\16"
		"Up:   %s\r\n\16"
		"Right:%s\r\n\16"
		"Down: %s\r\n\16"
		"Left: %s",
		playerRaceNames[HUMAN],
		playerRaceNames[ELF],
		playerRaceNames[DWARF],
		playerRaceNames[GNOLL]
	);
	cputsxy(0, 27, "Press SELECT to skip creation.");
	do{
		k = waitForInput();
		if(JOY_SELECT(k)){ //Skip character creation
			Player.race = 3;
			Player.gender = 1;
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
		} else if(JOY_DOWN(k)){
			Player.race = DWARF;
			Player.hp  += 2;
			Player.dex -= 2;
		} else if(JOY_LEFT(k)) {
			Player.race = GNOLL;
			Player.hp  -= 4;
			Player.dex += 4;
		}
	} while(!(k&0xf0));
	cclearxy(0, 27, 32);
	drawWindow(0, 6, 31, 3);
	gotoxy(1, 6);
	cprintf(
		"SELECT YOUR GENDER\r\n\16"
		"Up:   %s\r\n\16"
		"Down: %s",
		genderNames[0],
		genderNames[1]
	);
	do{
		k = waitForInput();
		if(JOY_UP(k))        Player.gender = 0;
		else if(JOY_DOWN(k)) Player.gender = 1;
	} while(!(k&0xf0));
	while(j){
		cclearxy(0, 10, 155);
		drawWindow(0, 10, 31, 5);
		gotoxy(1, 10);
		cprintf(
			"DISTRIBUTE YOUR STATS\r\n\16"
			"Up:   Hit Points %d\r\n\16"
			"Right:Dexterity  %d\r\n\16"
			"Down: Spirit     %d\r\n\16"
			"Points: %d",
			Player.hp, Player.dex, Player.spi, j
		);
		k = waitForInput();
		if(JOY_UP(k)){
			++Player.hp;
			--j;
		}
		else if(JOY_RIGHT(k)){
			++Player.dex;
			--j;
		}
		else if(JOY_DOWN(k)){
			++Player.spi;
			--j;
		}
	}
	gotoxy(1, 10);
	cprintf(
		"DISTRIBUTE YOUR STATS\r\n\16"
		"Up:   Hit Points %d\r\n\16"
		"Right:Dexterity  %d\r\n\16"
		"Down: Spirit     %d\r\n\16"
		"Points: %d",
		Player.hp, Player.dex, Player.spi, j
	);
	drawWindow(0, 16, 15, 5);
	gotoxy(1, 16);
	cprintf(
		"BUY ARMOR\r\n\16"
		"Up:%s   (00)\r\n\16"
		"Ri:%s(10)\r\n\16"
		"Do:%s  (30)\r\n\16"
		"Le:%s  (50)",
		armorNames[RAGS],
		armorNames[LEATHER],
		armorNames[CHAIN],
		armorNames[PLATE]
	);
	drawWindow(0, 25, 31, 2);
	gotoxy(1, 25);
	cprintf("YOUR GOLD\r\n\16%d GP", Player.gold);
	do{
		k = waitForInput();
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
	} while(!(k&0xf0));
	drawWindow(16, 16, 15, 5);
	cputsxy(17, 16, "BUY WEAPONS");
	cputsxy(17, 17, "Up:STICK  (00)");
	cputsxy(17, 18, "Ri:DAGGER (10)");
	cputsxy(17, 19, "Do:MACE   (30)");
	cputsxy(17, 20, "Le:SWORD  (50)");
	gotoxy(1, 26);
	cprintf("%d GP", Player.gold);
	do{
		k = waitForInput();
		if(JOY_RIGHT(k)){
			Player.weap = DAGGER;
			Player.gold -= 10;
		}
		else if(JOY_DOWN(k)){
			if(Player.gold < 30){
				k = 0;
				continue;
			}
			Player.weap = MACE;
			Player.gold -= 30;
		}
		else if(JOY_LEFT(k)){
			if(Player.gold < 50){
				k = 0;
				continue;
			}
			Player.weap = SWORD;
			Player.gold -= 50;
		}
	} while(!(k&0xf0));
}

void updateStats(){
	Player.hp  = MIN(Player.hp, 18);
	Player.dex = MIN(Player.dex, 18);
	Player.spi = MIN(Player.spi, 18);
	// Draw player stats window
	clearScreenArea(0, 4);
	drawWindow(0, 0, 31, 4);

	gotoxy(1, 0);
	cprintf(
		"%s %s\r\n\16"
		"Hits:%2d  Attr: %2d/%2d  GP:%u\r\n\16"
		"AC:  %2d  Torch:%2d     XY:%1d%1d\r\n\16"
		"Weapon:%6s         Lv:%1d",
		(Player.status ? statusNames[Player.status] : genderNames[Player.gender]),
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
	cputsxy(1, 20, "MESSAGE\r\n\16");

	if(message){
		cputs(messageStrings[message]);
	} else{
		k = rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]];
		if(!(k & 0x80)){
			cprintf("You see %s.", roomDescriptions[k-1]);
			if(k > EMPTY)
				cputsxy(1, 23, "Press A to interact.");
		}
	}

	// Draw map screen
	gotoxy(0, 5);
	for(i=0;i<8;++i){
		for(j=0;j<8;++j){
			if(Player.pos[X] == j && Player.pos[Y] == i) cputs("[\300] ");
			else if(rooms[Player.pos[Z]][i][j] & 0x80)
				cprintf("[%c] ", mapIcons[MONSTER]);
			else cprintf("[%c] ", mapIcons[rooms[Player.pos[Z]][i][j]]);
		}
		cputc('\n');
	}
}

void gameLoop(){
	++Player.turns;
	revealRoom(Player.pos[X], Player.pos[Y], Player.pos[Z]);
	trigger();

	drawScreen();

	k = waitForInput();
	if(k&0xf0) message = 0;
	if(JOY_UP(k))         Player.pos[Y] = (Player.pos[Y]-1)&7;
	else if(JOY_DOWN(k))  Player.pos[Y] = (Player.pos[Y]+1)&7;
	else if(JOY_LEFT(k))  Player.pos[X] = (Player.pos[X]-1)&7;
	else if(JOY_RIGHT(k)) Player.pos[X] = (Player.pos[X]+1)&7;
	else if(JOY_BTN_A(k)){
		if(message) message = 0;
		else interact();
	}
	else if(JOY_BTN_B(k)) useTorch();
}

int main(){
	init();

	// Title Screen
	clrscr();
	cputs("\nLOTH'S CASTLE\r\n\n\nPRESS START");
	while(!JOY_START(joy_read(JOY_1))) ++j;
	srand(j);

	rooms[1+rand()%7][1+rand()%7][1+rand()%7] = YENDORORB;
	rooms[0][0][0] = EMPTY;

	charCreation();

	clrscr();
	while(1) gameLoop();
	joy_uninstall();

	return 0;
}
