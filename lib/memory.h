//
// Created by Andres Fuentes Hernandez on 8/18/24.
//

#ifndef RETRO_EMULATOR_LIB_MEMORY_H_
#define RETRO_EMULATOR_LIB_MEMORY_H_

#include "device_manager.h"
#include <stdlib.h>

struct memory {
  uint16_t start;
  uint16_t end;
  uint8_t slots[0x10000];
};
// this should have a clock method for write

struct memory new_memory(uint16_t start, uint16_t end);
struct bus_device new_memory_bus(struct memory memory[static 1]);
uint8_t read_memory (struct memory memory[static 1], uint16_t address);
void write_memory (struct memory memory[static 1], uint16_t address, uint8_t data);

#endif //RETRO_EMULATOR_LIB_MEMORY_H_
