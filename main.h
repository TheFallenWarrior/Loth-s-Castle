/*
	Loth's Castle
	Copyright 2024 TheFallenWarrior
*/

#ifndef LC_MAIN_H
#define LC_MAIN_H

#include <stdint.h>

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

#endif