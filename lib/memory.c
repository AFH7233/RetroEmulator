//
// Created by Andres Fuentes Hernandez on 8/18/24.
//
#include "memory.h"

struct memory new_memory(void){
  struct memory memory = {
      .start = 0x0000,
      .end = 0xffff
  };
  for (size_t i = 0; i < 0xFFFF; i++) {
    memory.slots[i] = 0;
  }
  return memory;
}

struct bus_device new_memory_bus(struct memory* memory){
  struct bus_device memory_bus = {
      .hardware = (void *) memory,
      .read = (uint8_t (*)(void *, uint16_t)) read_memory,
      .write = (void (*)(void *, uint16_t, uint8_t)) write_memory,
  };
  return memory_bus;
}

uint8_t read_memory (struct memory* memory, uint16_t address){
  return memory->slots[address];
}

void write_memory (struct memory* memory, uint16_t address, uint8_t data){
  memory->slots[address] = data;
}
