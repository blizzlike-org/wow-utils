#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
    case 1: // warrior
      p = 1;
      break;
    case 2: // paladin
      p = 0;
      break;
    case 3: // hunter
      p = 0;
      break;
    case 4: // rogue
      p = 3;
      break;
    case 5: // priest
      p = 0;
      break;
    case 7: // shaman
      p = 0;
      break;
    case 8: // mage
      p = 0;
      break;
    case 9: // warlock
      p = 0;
      break;
    case 11: // druid
      p = 0;
      break;
  }

  return p;
}

int main(int argc, const char **argv) {
  uint8_t r[] = {
     1, // human
     2, // orc
     3, // dwarf
     4, // nightelf
     5, // undead
     6, // tauren
     7, // gnome
     8, // troll
    10, // bloodelf
    11  // draenei
  };
  uint8_t c[] = {
     1, // warrior
     2, // paladin
     3, // hunter
     4, // rogue
     5, // priest
     7, // shaman
     8, // mage
     9, // warlock
    11  // druid
  };
  int i = 0, j = 0;

  for (i = 0; i < 10; ++i) {
    for (j = 0; j < 9; ++j) {
      if (c[j] == 1 && r[i] != 10 ||                                                       // warrior
          c[j] == 2 && (r[i] == 1 || r[i] == 3 || r[i] == 10 || r[i] == 11) ||             // paladin
          c[j] == 3 && r[i] != 1 && r[i] != 5 && r[i] != 7 ||                              // hunter
          c[j] == 4 && r[i] != 6 && r[i] != 11 ||                                          // rogue
          c[j] == 5 && r[i] != 2 && r[i] != 6 && r[i] != 7 ||                              // priest
          c[j] == 7 && (r[i] == 2 || r[i] == 6 || r[i] == 8 || r[i] == 11) ||              // shaman
          c[j] == 8 && r[i] != 2 && r[i] != 3 && r[i] != 4 && r[i] != 6 ||                 // mage
          c[j] == 9 && (r[i] == 1 || r[i] == 2 || r[i] == 5 || r[i] == 7 || r[i] == 10) || // warlock
          c[j] == 11 && (r[i] == 4 || r[i] == 6)) {                                        // druid
	getField(r[i], c[j], 0);
	getField(r[i], c[j], 1);
      }
    }
  }

  return 0;
}
