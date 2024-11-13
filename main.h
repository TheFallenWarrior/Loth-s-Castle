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

#endif