/*
	Loth's Castle
	Copyright 2025 TheFallenWarrior
*/

#ifndef LC_MAIN_H
#define LC_MAIN_H

#include <stdint.h>

#define JOY_DPAD_MASK   0xf0

#define D4          (1+(rand()&3))
#define D8          (1+(rand()&7))
#define MIN(x, y)   ((x) > (y) ? (y) : (x))
#define CCLEAR_AREA(y, k) cclearxy(0, (y), (k)*32);

void cprintfxy(uint8_t, uint8_t, const char*, ...);
uint8_t waitForInput(uint8_t);
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
	RAC_HUMAN,
	RAC_ELF,
	RAC_DWARF,
	RAC_GNOLL
};

enum weaponTypes{
	WPN_STICK,
	WPN_DAGGER,
	WPN_MACE,
	WPN_SWORD
};

enum armorTypes{
	ARM_RAGS,
	ARM_LEATHER,
	ARM_CHAIN,
	ARM_PLATE
};

enum treasureTypes{
	TRS_RUBYRED,    // Avoid Lethargy
	TRS_NORNSTONE,  // Nothing
	TRS_PALEPEARL,  // Avoid Bleeding
	TRS_OPALEYE,    // Cure Blindness
	TRS_GREENGEM,   // Avoid Forgetting
	TRS_BLUEFLAME,  // Nothing
	TRS_PALINTIR,   // Nothing
	TRS_SYLMARYL    // Nothing
};

enum treasureMasks{
	TRS_RUBYRED_MASK    = 0x01,
	TRS_NORNSTONE_MASK  = 0x02,
	TRS_PALEPEARL_MASK  = 0x04,
	TRS_OPALEYE_MASK    = 0x08,
	TRS_GREENGEM_MASK   = 0x10,
	TRS_BLUEFLAME_MASK  = 0x20,
	TRS_PALINTIR_MASK   = 0x40,
	TRS_SYLMARYL_MASK   = 0x80
};

enum statusAilments{
	STA_HEALTHY,
	STA_LETHARGY,
	STA_BLEED,
	STA_FORGETFULNESS,
	STA_BLINDNESS
};

enum roomContents{
	UNKNOWN_ROOM,
	EMPTY_ROOM,
	UPSTAIRS_ROOM,
	DOWNSTAIRS_ROOM,
	FOUNTAIN_ROOM,
	GOLDPIECES_ROOM,
	CHEST_ROOM,
	MONSTER_ROOM,
	TREASURE_ROOM,
	WARP_ROOM,
	VENDOR_ROOM,
	YENDORORB_ROOM
};

enum enemyTypes{
	ENM_GOBLIN,
	ENM_KOBOLD,
	ENM_WOLF,
	ENM_ORC,
	ENM_OGRE,
	ENM_LAMIA,
	ENM_TROLL,
	ENM_MINOTAUR,
	ENM_CHIMERA,
	ENM_DRAGON
};

enum buttons{
	BTN_A,
	BTN_B,
	BTN_SELECT,
	BTN_START
};

const char* const player_race_names[] = {
	"HUMAN",
	"ELF",
	"DWARF",
	"GNOLL"
};

const char* const sex_names[] = {
	"MALE",
	"FEMALE"
};

const char* const armor_names[] = {
	"RAGS",
	"LEATHER",
	"CHAIN",
	"PLATE"
};

const char* const weapon_names[] = {
	"STICK",
	"DAGGER",
	"MACE",
	"SWORD"
};

const char* const treasure_names[] = {
	"RUBY RED",
	"NORNSTONE",
	"PALE PEARL",
	"OPAL EYE",
	"GREEN GEM",
	"BLUE FLAME",
	"PALINTIR",
	"SYLMARYL"
};

const char* const status_names[] = {
	"",
	"LETHARGIC",
	"BLEEDING",
	"",
	"BLIND"
};

const char* const enemy_names[] = {
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

const char* const attr_change_descriptions[] = {
	"BETTER",
	"FLIMSIER",
	"NIMBLER",
	"CLUMSIER",
	"SMARTER",
	"STUPIDER"
};

const char* const message_strings[] = {
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

const char* const room_descriptions[] = {
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

const char* const interaction_prompts[] = {
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

const char* const death_causes[] = {
	"DEATHSPELL",
	"blood loss",
	"clumsiness",
	"stupidity"
};

const char* const vendor_quotes[] = {
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
	const char* const button_names[] = {
		"A",
		"B",
		"SELECT",
		"START"
	};
#else
	const char* const button_names[] = {
		"C",
		"X",
		"SPACE",
		"RETURN"
	};
#endif

const uint8_t map_icons[] = {
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
const uint8_t bitmask_table[] = {
	0x01,
	0x02,
	0x04,
	0x08,
	0x10,
	0x20,
	0x40,
	0x80,
};

#endif