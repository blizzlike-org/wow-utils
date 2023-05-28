#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum classes {
  WARRIOR = 1,
  PALADIN = 2,
  HUNTER = 3,
  ROGUE = 4,
  PRIEST = 5,
  SHAMAN = 7,
  MAGE = 8,
  WARLOCK = 9,
  DRUID = 11
};

enum gender {
  MALE = 0,
  FEMALE = 1
};

enum powertype {
  MANA = 0,
  RAGE = 1,
  ENERGY = 3
};

enum races {
  HUMAN = 1,
  ORC = 2,
  DWARF = 3,
  NIGHTELF = 4,
  UNDEAD = 5,
  TAUREN = 6,
  GNOME = 7,
  TROLL = 8,
  BLOODELF = 10,
  DRAENEI = 11
};

int getpowertype(uint8_t c);

void getField(uint8_t r, uint8_t c, uint8_t g) {
  uint8_t p = getpowertype(c);
  uint32_t rcg = (r) | (c << 8) | (g << 16);
  uint32_t rcgp = (rcg | (p << 24));

  printf("Race: %d Class: %d Gender: %d UnitFieldByte0: %d\n", r, c, g, rcgp);
}

int getpowertype(uint8_t c) {
  uint8_t p;

  switch(c) {
    case WARRIOR:
      p = RAGE;
      break;
    case PALADIN:
      p = MANA;
      break;
    case HUNTER:
      p = MANA;
      break;
    case ROGUE:
      p = ENERGY;
      break;
    case PRIEST:
      p = MANA;
      break;
    case SHAMAN:
      p = MANA;
      break;
    case MAGE:
      p = MANA;
      break;
    case WARLOCK:
      p = MANA;
      break;
    case DRUID:
      p = MANA;
      break;
  }

  return p;
}

int main(int argc, const char **argv) {
  uint8_t r[] = {
    HUMAN,
    ORC,
    DWARF,
    NIGHTELF,
    UNDEAD,
    TAUREN,
    GNOME,
    TROLL,
    BLOODELF,
    DRAENEI
  };
  uint8_t c[] = {
    WARRIOR,
    PALADIN,
    HUNTER,
    ROGUE,
    PRIEST,
    SHAMAN,
    MAGE,
    WARLOCK,
    DRUID
  };
  int i = 0, j = 0;

  for (i = 0; i < 10; ++i) {
    for (j = 0; j < 9; ++j) {
      if (c[j] == WARRIOR && r[i] != BLOODELF ||
          c[j] == PALADIN && (r[i] == HUMAN || r[i] == DWARF || r[i] == BLOODELF || r[i] == DRAENEI) ||
          c[j] == HUNTER  && r[i] != HUMAN && r[i] != UNDEAD && r[i] != GNOME ||
          c[j] == ROGUE && r[i] != TAUREN && r[i] != DRAENEI ||
          c[j] == PRIEST && r[i] != ORC && r[i] != TAUREN && r[i] != GNOME ||
          c[j] == SHAMAN && (r[i] == ORC || r[i] == TAUREN || r[i] == TROLL || r[i] == DRAENEI) ||
          c[j] == MAGE && r[i] != ORC && r[i] != DWARF && r[i] != NIGHTELF && r[i] != TAUREN ||
          c[j] == WARLOCK && (r[i] == HUMAN || r[i] == ORC || r[i] == UNDEAD || r[i] == GNOME || r[i] == BLOODELF) ||
          c[j] == DRUID && (r[i] == NIGHTELF || r[i] == TAUREN)) {
	getField(r[i], c[j], MALE);
	getField(r[i], c[j], FEMALE);
      }
    }
  }

  return 0;
}
