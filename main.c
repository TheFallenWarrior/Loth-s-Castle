/*
	Loth's Castle
	Copyright 2025 TheFallenWarrior
*/

#ifdef __NES__
	#include <conio.h>
	#include <joystick.h>
	#include <nes.h>
	#define init() joy_install(joy_static_stddrv)
	#define renderScreen() (void)0
#else
	#include "pc-port.h"
#endif
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

#include "main.h"

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
	uint8_t oldPos[3];
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
	while(joy_read(JOY_1)) renderScreen();
	while(!(mask&joy_read(JOY_1))) renderScreen();
	return joy_read(JOY_1);
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
	uint8_t roomType;

	roomType = rooms[z][y][x];
	if(!roomType){
		roomType = 1+rand()%15;
		if(roomType >= YENDORORB_ROOM) roomType = EMPTY_ROOM;
		if(roomType == MONSTER_ROOM)   roomType = 0x80 | (rand()%(2+Player.turns/10))%10;
		rooms[z][y][x] = roomType;
	}
	cclearxy(1, 22, 30);
}

void playerAttack(){
	uint8_t attackPower;
	uint8_t damage;

	attackPower = D4+Player.weap;
	damage = (Enemy.type>attackPower ? 0 : attackPower-Enemy.type);
	Enemy.hp = (damage>Enemy.hp ? 0 : Enemy.hp-damage);
	CCLEAR_AREA(21, 7);
	drawWindow(0, 20, 31, 7);
	cprintfxy(
		1, 20,
		"%s"
		"You attack the %s!",
		"MESSAGE\r\n\x0e\n",
		enemy_names[Enemy.type]
	);
	waitForInput(0);
	cclearxy(1, 22, 30);
	cprintfxy(
		1, 22,
		"%s took %d damage.",
		enemy_names[Enemy.type], damage
	);
}

uint8_t playerBribe(){
	uint16_t bribeAmount;

	bribeAmount = rand()%1000;
	bribeAmount = MIN(bribeAmount, Player.gold);
	Player.gold -= bribeAmount;
	CCLEAR_AREA(21, 7);
	drawWindow(0, 20, 31, 7);
	cprintfxy(
		1, 20,
		"%s"
		"Gave %d GP to %s.",
		"MESSAGE\r\n\x0e\n",
		bribeAmount, enemy_names[Enemy.type]
	);
	updateStats();
	waitForInput(0);
	if(Player.spi+D4 > Enemy.type+D8){
		cclearxy(1, 22, 30);
		cprintfxy(
			1, 22,
			"%s says:\r\n\x0e"
			"%s",
			enemy_names[Enemy.type],
			" `Ok, just don't tell anyone.'"
		);
		waitForInput(0);
		if(rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]] == VENDOR_ROOM)
			vendorsAngry = 0;
		rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]] = EMPTY_ROOM;
		return 1;
	} else{
		cclearxy(1, 22, 30);
		cprintfxy(
			1, 22,
			"%s says:\r\n\x0e"
			"%s",
			enemy_names[Enemy.type],
			" `All I want is your life!'"
		);
		return 0;
	}
}

void enemyAttack(){
	uint8_t attackPower;
	uint8_t damage;

	CCLEAR_AREA(21, 7);
	drawWindow(0, 20, 31, 7);
	cprintfxy(
		1, 20,
		"%s"
		"%s attacks you!",
		"MESSAGE\r\n\x0e\n",
		enemy_names[Enemy.type]
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
	attackPower = (D4+Enemy.type)/2;
	damage = (Player.arm>attackPower ? 0 : attackPower-Player.arm);
	Player.hp = (damage>Player.hp ? 0 : Player.hp-damage);
	CCLEAR_AREA(21, 7);
	drawWindow(0, 20, 31, 7);
	cprintfxy(
		1, 20,
		"%s"
		"%s took %d damage.",
		"MESSAGE\r\n\x0e\n", "You", damage
	);
	updateStats();
	waitForInput(0);
}

void battle(){
	uint8_t input;
	uint8_t runInput;
	uint16_t goldDropped;

	while(1){
		CCLEAR_AREA(21, 7);
		drawWindow(0, 20, 31, 7);
		cputsxy(
			1,20,
			"COMMAND\r\n\x0e\n"
			"Up:   FIGHT\r\n\x0e"
			"Right:BRIBE\r\n\x0e"
			"Down: RUN\r\n\x0e"
			"Left: CAST"
		);
		input = waitForInput(JOY_DPAD_MASK);
		if(JOY_UP(input)) playerAttack();
		else if(JOY_RIGHT(input)){
			if(!Player.gold){
				CCLEAR_AREA(21, 7);
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
		else if(JOY_DOWN(input)){
			if(Player.dex+D4 > Enemy.type+D8){
				CCLEAR_AREA(21, 7);
				drawWindow(0, 20, 31, 7);
				cprintfxy(
					1, 20,
					"%s"
					"Run to where?\r\n\x0e"
					"(Use your D-pad.)",
					"MESSAGE\r\n\x0e\n"
				);
				runInput = waitForInput(JOY_DPAD_MASK);
				Player.oldPos[X] = Player.pos[X];
				Player.oldPos[Y] = Player.pos[Y];
				Player.oldPos[Z] = Player.pos[Z];
				if(JOY_UP(runInput))         Player.pos[Y] = (Player.pos[Y]-1)&7;
				else if(JOY_DOWN(runInput))  Player.pos[Y] = (Player.pos[Y]+1)&7;
				else if(JOY_LEFT(runInput))  Player.pos[X] = (Player.pos[X]-1)&7;
				else if(JOY_RIGHT(runInput)) Player.pos[X] = (Player.pos[X]+1)&7;
				revealRoom(Player.pos[X], Player.pos[Y], Player.pos[Z]);
				trigger();
				return;
			} else{
				CCLEAR_AREA(21, 7);
				drawWindow(0, 20, 31, 7);
				cprintfxy(
					1, 20,
					"%s"
					"Failed to run away.",
					"MESSAGE\r\n\x0e\n"
				);
			}
		}
		else if(JOY_LEFT(input)){
			CCLEAR_AREA(21, 7);
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
			CCLEAR_AREA(21, 7);
			drawWindow(0, 20, 31, 7);
			cprintfxy(
				1, 20,
				"%s"
				"You succumb to your wounds.",
				"MESSAGE\r\n\x0e\n",
				enemy_names[Enemy.type]
			);
			waitForInput(0);
			return;
		}

	}
	CCLEAR_AREA(21, 7);
	drawWindow(0, 20, 31, 7);
	cprintfxy(
		1, 20,
		"%s"
		"You killed %s.",
		"MESSAGE\r\n\x0e\n",
		enemy_names[Enemy.type]
	);
	waitForInput(0);
	cclearxy(1, 22, 30);
	goldDropped = rand()%((1+Enemy.type)*175);
	if(rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]] == VENDOR_ROOM)
		goldDropped *= 4;
	Player.gold += goldDropped;
	cprintfxy(
		1, 20,
		"%s"
		"%s dropped %d GP.",
		"MESSAGE\r\n\x0e\n",
		enemy_names[Enemy.type], goldDropped
	);
	updateStats();
	waitForInput(0);
	rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]] = EMPTY_ROOM;
	message = 0;
}

void drinkFountain(){
	uint8_t fountainEffect;

	fountainEffect = rand()&7;
	cclearxy(1, 22, 30);
	cclearxy(1, 24, 30);
	cputsxy(1, 22, "Drank from the fountain.");
	waitForInput(0);
	if(fountainEffect > 5){
		Player.race = rand()&3;
		Player.sex  = rand()&1;
		cclearxy(1, 22, 30);
		cprintfxy(
			1, 22,
			"You turned into a %s\r\n\x0e"
			"%s.",
			sex_names[Player.sex],
			player_race_names[Player.race]
		);
		message = 0;
		waitForInput(0);
		return;
	} else switch(fountainEffect){
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
	cprintfxy(1, 22, "You feel %s.", attr_change_descriptions[fountainEffect]);
	message = 0;
	waitForInput(0);
	return;
}

// Vendor interaction menu
void vendor(){
	uint8_t menuInput;
	uint8_t shopInput;
	uint8_t purchased;
	uint8_t quoteIndex;

	cclearxy(1, 22, 30);
	cclearxy(1, 24, 30);
	cputsxy(
		1,20,
		"COMMAND\r\n\x0e\n"
		"Up:   TRADE\r\n\x0e"
		"Right:TALK\r\n\x0e"
		"Down: ATTACK"
	);
	menuInput = waitForInput(JOY_DPAD_MASK & ~JOY_LEFT_MASK);

	CCLEAR_AREA(21, 7);
	drawWindow(0, 20, 31, 7);
	cputsxy(1, 20, "MESSAGE\r\n\x0e\n");
	if(JOY_UP(menuInput)){
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
		if(ARM_LEATHER > Player.arm)
			cprintf("Right:%s(1000)\r\n\x0e", armor_names[ARM_LEATHER]);
		if(ARM_CHAIN > Player.arm)
			cprintf("Down: %s  (2000)\r\n\x0e", armor_names[ARM_CHAIN]);
		if(ARM_PLATE > Player.arm)
			cprintf("Left: %s  (3000)", armor_names[ARM_PLATE]);

		shopInput = waitForInput(JOY_DPAD_MASK);
		if(JOY_RIGHT(shopInput) && ARM_LEATHER > Player.arm && Player.gold > 1000){
			Player.arm = ARM_LEATHER;
			Player.gold -= 1000;
			purchased = 1;
		}
		else if(JOY_DOWN(shopInput) && ARM_CHAIN > Player.arm && Player.gold > 2000){
			Player.arm = ARM_CHAIN;
			Player.gold -= 2000;
			purchased = 1;
		}
		else if(JOY_LEFT(shopInput) && ARM_PLATE > Player.arm && Player.gold > 3000){
			Player.arm = ARM_PLATE;
			Player.gold -= 3000;
			purchased = 1;
		}
		else purchased = 0;
		updateStats();
		CCLEAR_AREA(21, 7);
		drawWindow(0, 20, 31, 7);
		cputsxy(1, 20, "MESSAGE\r\n\x0e\n");
		if(purchased) cprintf("Bought the %s.", armor_names[Player.arm]);
		else cputs("Nevermind.");
		waitForInput(0);

		CCLEAR_AREA(21, 7);
		drawWindow(0, 20, 31, 7);
		cprintfxy(
			1, 20,
			"BUY WEAPON\r\n\x0e\n"
			"%s",
			"Up:   Nothing\r\n\x0e"
		);
		if(WPN_DAGGER > Player.weap)
			cprintf("Right:%s (1000)\r\n\x0e", weapon_names[WPN_DAGGER]);
		if(WPN_MACE > Player.weap)
			cprintf("Down: %s   (2000)\r\n\x0e", weapon_names[WPN_MACE]);
		if(WPN_SWORD > Player.weap)
			cprintf("Left: %s  (3000)", weapon_names[WPN_SWORD]);

		shopInput = waitForInput(JOY_DPAD_MASK);
		if(JOY_RIGHT(shopInput) && WPN_DAGGER > Player.weap && Player.gold > 1000){
			Player.weap = WPN_DAGGER;
			Player.gold -= 1000;
			purchased = 1;
		}
		else if(JOY_DOWN(shopInput) && WPN_MACE > Player.weap && Player.gold > 2000){
			Player.weap = WPN_MACE;
			Player.gold -= 2000;
			purchased = 1;
		}
		else if(JOY_LEFT(shopInput) && WPN_SWORD > Player.weap && Player.gold > 3000){
			Player.weap = WPN_SWORD;
			Player.gold -= 3000;
			purchased = 1;
		}
		else purchased = 0;
		updateStats();
		CCLEAR_AREA(21, 7);
		drawWindow(0, 20, 31, 7);
		cputsxy(1, 20, "MESSAGE\r\n\x0e\n");
		if(purchased) cprintf("Bought the %s.", weapon_names[Player.weap]);
		else cputs("Nevermind.");
		waitForInput(0);

		CCLEAR_AREA(21, 7);
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

		shopInput = waitForInput(JOY_DPAD_MASK);
		if(JOY_RIGHT(shopInput) && Player.hp < 18 && Player.gold > 1000){
			++Player.hp;
			Player.gold -= 1000;
			purchased = 1;
		}
		else if(JOY_DOWN(shopInput) && Player.dex < 18 && Player.gold > 1000){
			++Player.dex;
			Player.gold -= 1000;
			purchased = 3;
		}
		else if(JOY_LEFT(shopInput) && Player.spi < 18 && Player.gold > 1000){
			++Player.spi;
			Player.gold -= 1000;
			purchased = 5;
		}
		else purchased = 0;
		updateStats();
		CCLEAR_AREA(21, 7);
		drawWindow(0, 20, 31, 7);
		cputsxy(1, 20, "MESSAGE\r\n\x0e\n");
		if(purchased) cprintf("You feel %s.", attr_change_descriptions[purchased-1]);
		else cputs("Nevermind.");
		waitForInput(0);

		CCLEAR_AREA(21, 7);
		drawWindow(0, 20, 31, 7);
		cputsxy(
			1, 20,
			"BUY TORCHES\r\n\x0e\n"
			"Buy 10 torches for 1000 GP?\r\n\x0e"
			"Up:   YES\r\n\x0e"
			"Down: NO"
		);

		shopInput = waitForInput(JOY_UP_MASK | JOY_DOWN_MASK);
		if(JOY_UP(shopInput) && Player.gold > 1000){
			Player.torches += 10;
			Player.gold -= 1000;
			purchased = 1;
		}
		else purchased = 0;
		updateStats();
		CCLEAR_AREA(21, 7);
		drawWindow(0, 20, 31, 7);
		cputsxy(1, 20, "MESSAGE\r\n\x0e\n");
		if(purchased) cputs("Bought the torches.");
		else cputs("Nevermind.");
	}
	else if(JOY_RIGHT(menuInput)){
		quoteIndex = rand()%6;
		cprintf(
			"%s says:\r\n\x0e"
			"%s",
			"VENDOR",
			vendor_quotes[quoteIndex]
		);
	}
	else if(JOY_DOWN(menuInput)){
		vendorsAngry = 1;
		trigger();
	}
	message = 0;
	if(Player.hp) waitForInput(0);
}

// Light up rooms adjacent to the player
void useTorch(){
	uint8_t mapX;
	uint8_t mapY;

	if(Player.torches){
		message = 1;
		--Player.torches;
		for(mapX=(Player.pos[X]+7); mapX<=(Player.pos[X]+9); ++mapX){
			for(mapY=(Player.pos[Y]+7); mapY<=(Player.pos[Y]+9); ++mapY)
				revealRoom(mapX&7, mapY&7, Player.pos[Z]);
		}
	}
	Player.oldPos[Z] = 0xff;
}

// An event that happens automatically
void trigger(){
	uint8_t roomType;
	uint8_t coord;

	roomType = rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]];
	if(roomType == WARP_ROOM){
		for(coord=0;coord<3;++coord) Player.pos[coord] = rand()&7;
		Player.oldPos[Z] = 0xff;
		revealRoom(Player.pos[X], Player.pos[Y], Player.pos[Z]);

		message = WARP_ROOM;
		drawScreen();
		waitForInput(0);
		Player.oldPos[X] = Player.pos[X];
		Player.oldPos[Y] = Player.pos[Y];
		Player.oldPos[Z] = Player.pos[Z];
		message = 0;
		trigger();
	}
	else if(roomType == VENDOR_ROOM && vendorsAngry){
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
			enemy_names[Enemy.type]
		);
		waitForInput(0);
		battle();
	}
	else if(roomType & 0x80){
		drawScreen();
		Enemy.type = roomType & 0x7f;
		Enemy.hp = D4 + Enemy.type;
		cclearxy(1, 22, 30);
		cclearxy(1, 24, 30);
		cprintfxy(
			1, 22,
			"Encounter!\r\n\x0e\n"
			"You are facing a lousy\r\n\x0e"
			"%s!",
			enemy_names[Enemy.type]
		);
		waitForInput(0);
		battle();
	}
}

// Interact with the object in the room
void interact(){
	uint8_t roomType;
	uint16_t goldFound;
	uint8_t treasureIndex;

	roomType = rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]];
	if(roomType <= 1) return;
	message = roomType;
	switch(roomType){
		case UPSTAIRS_ROOM:
		Player.pos[Z] = (Player.pos[Z]+1)&7;
		Player.oldPos[Z] = 0xff;
		revealRoom(Player.pos[X], Player.pos[Y], Player.pos[Z]);
		drawScreen();
		waitForInput(0);
		message = 0;
		trigger();
		Player.oldPos[X] = Player.pos[X];
		Player.oldPos[Y] = Player.pos[Y];
		Player.oldPos[Z] = Player.pos[Z];
		break;

		case DOWNSTAIRS_ROOM:
		Player.oldPos[Z] = 0xff;
		Player.pos[Z] = (Player.pos[Z]-1)&7;
		revealRoom(Player.pos[X], Player.pos[Y], Player.pos[Z]);
		drawScreen();
		waitForInput(0);
		message = 0;
		trigger();
		Player.oldPos[X] = Player.pos[X];
		Player.oldPos[Y] = Player.pos[Y];
		Player.oldPos[Z] = Player.pos[Z];
		break;

		case GOLDPIECES_ROOM:
		goldFound = 10*D8 + Player.turns/2 + rand()%(Player.turns/2);
		goldFound = MIN(goldFound, 999);
		Player.gold += goldFound;
		cclearxy(1, 22, 30);
		cclearxy(1, 24, 30);
		cprintfxy(1, 22, "You found %d GP.", goldFound);
		updateStats();
		waitForInput(0);
		rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]] = EMPTY_ROOM;
		break;

		case TREASURE_ROOM:
		treasureIndex = rand()&7;
		Player.treasures |= bitmask_table[treasureIndex];
		cclearxy(1, 22, 30);
		cclearxy(1, 24, 30);
		cprintfxy(1, 22, "You have obtained %s.", treasure_names[treasureIndex]);
		rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]] = EMPTY_ROOM;
		message = 0;
		waitForInput(0);
		break;

		case VENDOR_ROOM:
		vendor();
		break;

		case FOUNTAIN_ROOM:
		drinkFountain();
	}
}

void charCreation(){
	uint8_t pointsToAssign;
	uint8_t input;

	clrscr();

	Player.pos[X] = Player.pos[Y] = Player.pos[Z] = 0;
	Player.orb = Player.treasures = 0;
	Player.hp = Player.dex = Player.spi = 8;
	pointsToAssign = 8;
	Player.gold = 60;
	Player.turns = 0;
	Player.oldPos[Z] = 0xff;

	vendorsAngry = killedByDeathspell = 0;

	drawWindow(0, 0, 31, 5);
	cprintfxy(
		1, 0,
		"SELECT YOUR RACE\r\n\x0e"
		"Up:   %s\r\n\x0e"
		"Right:%s\r\n\x0e"
		"Down: %s\r\n\x0e"
		"Left: %s",
		player_race_names[RAC_HUMAN],
		player_race_names[RAC_ELF],
		player_race_names[RAC_DWARF],
		player_race_names[RAC_GNOLL]
	);
	cprintfxy(
		0, 27,
		"Press %s to skip creation.",
		button_names[BTN_SELECT]
	);

	input = waitForInput(JOY_DPAD_MASK | JOY_SELECT_MASK);
	if(JOY_SELECT(input)){ //Skip character creation
		Player.race = 3;
		Player.sex = 1;
		Player.dex += 8;
		Player.arm  = ARM_CHAIN;
		Player.weap = WPN_MACE;
		Player.gold = 0;
		return;
	}
	if(JOY_UP(input)) Player.race = RAC_HUMAN;
	else if(JOY_RIGHT(input)){
		Player.race = RAC_ELF;
		Player.hp  -= 2;
		Player.dex += 2;
	}
	else if(JOY_DOWN(input)){
		Player.race = RAC_DWARF;
		Player.hp  += 2;
		Player.dex -= 2;
	}
	else if(JOY_LEFT(input)){
		Player.race = RAC_GNOLL;
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
		sex_names[0],
		sex_names[1]
	);

	input = waitForInput(JOY_UP_MASK | JOY_DOWN_MASK);
	if(JOY_UP(input))        Player.sex = 0;
	else if(JOY_DOWN(input)) Player.sex = 1;

	while(pointsToAssign){
		cclearxy(0, 10, 155);
		drawWindow(0, 10, 31, 5);
		cprintfxy(
			1, 10,
			"DISTRIBUTE YOUR STATS\r\n\x0e"
			"Up:   Hit Points %d\r\n\x0e"
			"Right:Dexterity  %d\r\n\x0e"
			"Down: Spirit     %d\r\n\x0e"
			"Points: %d",
			Player.hp, Player.dex, Player.spi, pointsToAssign
		);
		input = waitForInput(JOY_DPAD_MASK & ~JOY_LEFT_MASK);
		if(JOY_UP(input)){
			++Player.hp;
			--pointsToAssign;
		}
		else if(JOY_RIGHT(input) && Player.dex < 18){
			++Player.dex;
			--pointsToAssign;
		}
		else if(JOY_DOWN(input)){
			++Player.spi;
			--pointsToAssign;
		}
	}
	cprintfxy(
		1, 10,
		"DISTRIBUTE YOUR STATS\r\n\x0e"
		"Up:   Hit Points %d\r\n\x0e"
		"Right:Dexterity  %d\r\n\x0e"
		"Down: Spirit     %d\r\n\x0e"
		"Points: %d",
		Player.hp, Player.dex, Player.spi, pointsToAssign
	);
	drawWindow(0, 16, 15, 5);
	cprintfxy(
		1, 16,
		"BUY ARMOR\r\n\x0e"
		"Up:%s   (00)\r\n\x0e"
		"Ri:%s(10)\r\n\x0e"
		"Do:%s  (30)\r\n\x0e"
		"Le:%s  (50)",
		armor_names[ARM_RAGS],
		armor_names[ARM_LEATHER],
		armor_names[ARM_CHAIN],
		armor_names[ARM_PLATE]
	);
	drawWindow(0, 22, 31, 2);
	cprintfxy(1, 22, "YOUR GOLD\r\n\x0e%d GP", Player.gold);

	input = waitForInput(JOY_DPAD_MASK);
	if(JOY_RIGHT(input)){
		Player.arm = ARM_LEATHER;
		Player.gold -= 10;
	}
	else if(JOY_DOWN(input)){
		Player.arm = ARM_CHAIN;
		Player.gold -= 30;
	}
	else if(JOY_LEFT(input)){
		Player.arm = ARM_PLATE;
		Player.gold -= 50;
	}

	drawWindow(16, 16, 15, 5);
	cputsxy(17, 16, "BUY WEAPON");
	cprintfxy(17, 17, "Up:%s  (00)", weapon_names[WPN_STICK]);
	cprintfxy(17, 18, "Ri:%s (10)", weapon_names[WPN_DAGGER]);
	cprintfxy(17, 19, "Do:%s   (30)", weapon_names[WPN_MACE]);
	cprintfxy(17, 20, "Le:%s  (50)", weapon_names[WPN_SWORD]);
	cprintfxy(1, 23, "%02d GP", Player.gold);
	while(1){
		input = waitForInput(JOY_DPAD_MASK);
		if(JOY_UP(input)) break;
		if(JOY_RIGHT(input)){
			Player.weap = WPN_DAGGER;
			Player.gold -= 10;
			break;
		}
		else if(JOY_DOWN(input)){
			if(Player.gold < 30)
				continue;
			Player.weap = WPN_MACE;
			Player.gold -= 30;
			break;
		}
		else if(JOY_LEFT(input)){
			if(Player.gold < 50)
				continue;
			Player.weap = WPN_SWORD;
			Player.gold -= 50;
			break;
		}
	}

	drawWindow(0, 25, 31, 2);
	cprintfxy(8, 26, "Get ready, %s!", player_race_names[Player.race]);
	cprintfxy(1, 23, "%02d GP", Player.gold);
	waitForInput(0);

	// Automatically buy torches
	Player.torches = Player.gold/2;
	Player.gold = 0;
}

// Display player achievements and reset game state
void deathScreen(){
	uint8_t treasureCount;
	uint8_t deathCause;
	uint8_t floor;
	uint8_t row;
	uint8_t column;

	clrscr();
	drawWindow(0, 0, 31, 27);
	drawWindow(12, 0, 6, 2);
	cputsxy(13, 1, "DEATH");

	// Count player treasures
	treasureCount = 0;
	if(Player.treasures & TRS_RUBYRED_MASK)   ++treasureCount;
	if(Player.treasures & TRS_NORNSTONE_MASK) ++treasureCount;
	if(Player.treasures & TRS_PALEPEARL_MASK) ++treasureCount;
	if(Player.treasures & TRS_OPALEYE_MASK)   ++treasureCount;
	if(Player.treasures & TRS_GREENGEM_MASK)  ++treasureCount;
	if(Player.treasures & TRS_BLUEFLAME_MASK) ++treasureCount;
	if(Player.treasures & TRS_PALINTIR_MASK)  ++treasureCount;
	if(Player.treasures & TRS_SYLMARYL_MASK)  ++treasureCount;

	// Assess death cause
	if(!killedByDeathspell){
		if(!Player.hp)  deathCause = 1;
		if(!Player.dex) deathCause = 2;
		if(!Player.spi) deathCause = 3;
	} else deathCause = 0;

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
		sex_names[Player.sex], player_race_names[Player.race],
		death_causes[deathCause], 14+(rand()&15), Player.turns,
		Player.gold, treasureCount
	);
	cprintfxy(10, 23, "PRESS %s", button_names[BTN_START]);
	waitForInput(JOY_START_MASK);

	// Clear game state
	vendorsAngry = 0;
	for(floor=0;floor<8;++floor){
		for(row=0;row<8;++row){
			for(column=0;column<8;++column)
				rooms[floor][row][column] = 0;
		}
	}
}

// Limit player's attributes and display them
void updateStats(){
	Player.hp  = MIN(Player.hp, 18);
	Player.dex = MIN(Player.dex, 18);
	Player.spi = MIN(Player.spi, 18);
	// Draw player stats window
	CCLEAR_AREA(0, 4);
	drawWindow(0, 0, 31, 4);

	cprintfxy(
		1, 0,
		"%s %s\r\n\x0e"
		"Hits: %-2d Attr: %-2d/%-2d  GP:%u\r\n\x0e"
		"AC:   %-2d Torch:%-2d     XY:%1d%1d\r\n\x0e"
		"Weapon:%-6s         Lv:%1d",
		(
			Player.status ?
			status_names[Player.status] :
			sex_names[Player.sex]
		),
		player_race_names[Player.race],
		Player.hp, Player.dex, Player.spi, Player.gold,
		10 - (Player.arm*3), Player.torches,
		Player.pos[X]+1, Player.pos[Y]+1,
		weapon_names[Player.weap], Player.pos[Z]+1
	);
}

void drawScreen(){
	uint8_t roomType;
	uint8_t icon;
	uint8_t row;
	uint8_t column;

	// Draw map screen, if the player is the same level they were before, only
	// update the tiles that changed
	if(Player.pos[Z] == Player.oldPos[Z]){
		roomType = rooms[Player.pos[Z]][Player.oldPos[Y]][Player.oldPos[X]];
		icon = map_icons[roomType];
		if(roomType&0x80) icon = map_icons[MONSTER_ROOM];
		cprintfxy(4*Player.oldPos[X], 5+2*Player.oldPos[Y], "[%c]", icon);

		roomType = rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]];
		icon = map_icons[roomType];
		if(roomType&0x80) icon = map_icons[MONSTER_ROOM];
		else if(roomType == EMPTY_ROOM) icon = '@';
		revers(1);
		cprintfxy(4*Player.pos[X], 5+2*Player.pos[Y], "[%c]", icon);
		revers(0);
	}
	else{
		clrscr();
		gotoxy(0, 5);
		for(row=0;row<8;++row){
			for(column=0;column<8;++column){
				roomType = rooms[Player.pos[Z]][row][column];
				icon = map_icons[roomType];
				if(column == Player.pos[X] && row == Player.pos[Y]){
					revers(1);
					if(roomType == EMPTY_ROOM) icon = '@';
				}
				if(roomType&0x80) icon = map_icons[MONSTER_ROOM];
				cprintf("[%c]", icon);
				revers(0);
				cputc(' ');
			}
			cputc('\n');
		}
	}

	updateStats();

	// Draw message window
	CCLEAR_AREA(20, 7);
	drawWindow(0, 20, 31, 7);
	cputsxy(1, 20, "MESSAGE\r\n\x0e\n");

	if(message) cputs(message_strings[message]);
	else{
		roomType = rooms[Player.pos[Z]][Player.pos[Y]][Player.pos[X]];
		if(!(roomType & 0x80)){
			cprintf("You see %s.", room_descriptions[roomType-1]);
			if(roomType > EMPTY_ROOM){
				cprintfxy(
					1, 24,
					"Press %s to %s.",
					button_names[BTN_A],
					interaction_prompts[roomType-2]
				);
			}
		}
	}
}

uint8_t gameLoop(){
	uint8_t input;

	++Player.turns;
	revealRoom(Player.pos[X], Player.pos[Y], Player.pos[Z]);
	trigger();
	if(!Player.hp || !Player.dex || !Player.spi) return 0;

	drawScreen();

	message = 0;
	input = waitForInput(0);
	if(input&JOY_DPAD_MASK){
		Player.oldPos[X] = Player.pos[X];
		Player.oldPos[Y] = Player.pos[Y];
		Player.oldPos[Z] = Player.pos[Z];
		if(JOY_UP(input))         Player.pos[Y] = (Player.pos[Y]-1)&7;
		else if(JOY_DOWN(input))  Player.pos[Y] = (Player.pos[Y]+1)&7;
		else if(JOY_LEFT(input))  Player.pos[X] = (Player.pos[X]-1)&7;
		else if(JOY_RIGHT(input)) Player.pos[X] = (Player.pos[X]+1)&7;
	}
	else if(JOY_BTN_A(input)) interact();
	else if(JOY_BTN_B(input)) useTorch();

	if(!Player.hp || !Player.dex || !Player.spi) return 0;
	else return 1;
}

int main(){
	uint16_t randomSeed;

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

	cprintfxy(10, 23, "PRESS %s", button_names[BTN_START]);
	cputsxy(0, 27, "(c) 2025 TheFallenWarrior");

	randomSeed = 0;
	while(!JOY_START(joy_read(JOY_1))){
		renderScreen();
		++randomSeed;
	}
	srand(randomSeed);

	while(1){
		rooms[1+rand()%7][1+rand()%7][1+rand()%7] = YENDORORB_ROOM;
		rooms[0][0][0] = EMPTY_ROOM;

		charCreation();

		clrscr();
		while(gameLoop());
		deathScreen();
	}
	return 0;
}
