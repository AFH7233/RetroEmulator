//
// Created by Andres Fuentes Hernandez on 8/18/24.
//

#ifndef RETRO_EMULATOR_LIB_MEMORY_H_
#define RETRO_EMULATOR_LIB_MEMORY_H_

#include <stdint.h>

struct memory {
  uint16_t start;
  uint16_t end;
  uint8_t slots[0xFFFF]; // Allways 256 even though some memory wont be used
};
// this should have a clock method for write
uint8_t read_memory (struct memory*, uint16_t);
void write_memory (struct memory*, uint16_t, uint8_t);

#endif //RETRO_EMULATOR_LIB_MEMORY_H_
