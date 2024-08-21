//
// Created by Andres Fuentes Hernandez on 8/18/24.
//

#ifndef RETRO_EMULATOR_LIB_EMBEDDED_SYSTEM_H_
#define RETRO_EMULATOR_LIB_EMBEDDED_SYSTEM_H_
#include "register.h"
#include "device_manager.h"

enum cpu_state {
  RESET = 0,
  FETCH,
  EXECUTE,
  HALT,
};

enum micro_state {
  S0 = 0,
  S1,
  S2,
  S3,
  S4,
  S5,
  S6,
  S7
};

struct cpu_internals {
  struct register_u8 accumulator;
  struct register_u8 x_register;
  struct register_u8 y_register;
  struct register_u8 status_register;
  struct register_u8 stack_pointer;
  struct register_u8 instruction_register;
  struct register_u16 program_counter;
  struct register_u16 address_register;
  struct register_u8 data_register;
  struct register_u8 temp_register;
  enum cpu_state state;
  enum micro_state micro_step;
};



void tick(struct cpu_internals cpu[1], struct device_manager device_manager[1]);
struct cpu_internals new_cpu();

#endif //RETRO_EMULATOR_LIB_EMBEDDED_SYSTEM_H_
