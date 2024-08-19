//
// Created by Andres Fuentes Hernandez on 8/18/24.
//
#include <stdlib.h>
#include <stdio.h>
#include "memory.h"

uint8_t read_memory (struct memory* memory, uint16_t address){
  return memory->slots[address];
}

void write_memory (struct memory* memory, uint16_t address, uint8_t data){
  memory->slots[address] = data;
}