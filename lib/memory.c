//
// Created by Andres Fuentes Hernandez on 8/18/24.
//
#include "memory.h"

struct memory new_memory(uint16_t start, uint16_t end){
  struct memory memory = {
      .start = start,
      .end = end
  };
  for (size_t i = start; i <= end; i++) {
    memory.slots[i] = 0;
  }
  return memory;
}

struct bus_device new_memory_bus(struct memory memory[static 1]){
  struct bus_device memory_bus = {
      .hardware = (void *) memory,
      .read = (uint8_t (*)(void *, uint16_t)) read_memory,
      .write = (void (*)(void *, uint16_t, uint8_t)) write_memory,
  };
  return memory_bus;
}

uint8_t read_memory (struct memory memory[static 1], uint16_t address){
  return memory->slots[address];
}

void write_memory (struct memory memory[static 1], uint16_t address, uint8_t data){
  memory->slots[address] = data;
}
