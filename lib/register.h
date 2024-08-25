//
// Created by Andres Fuentes Hernandez on 8/18/24.
//

#ifndef RETRO_EMULATOR_LIB_REGISTER_H_
#define RETRO_EMULATOR_LIB_REGISTER_H_
#include <stdint.h>
#include <stdbool.h>

#define N_FLAG 7
#define V_FLAG 6
#define B_FLAG 4
#define D_FLAG 3
#define I_FLAG 2
#define Z_FLAG 1
#define C_FLAG 0

#define N_MASK_SET 0x80
#define V_MASK_SET 0x40
#define B_MASK_SET 0x10
#define D_MASK_SET 0x08
#define I_MASK_SET 0x04
#define Z_MASK_SET 0x02
#define C_MASK_SET 0x01

#define N_MASK_CLEAR 0x7F
#define V_MASK_CLEAR 0xBF
#define B_MASK_CLEAR 0xEF
#define D_MASK_CLEAR 0xF7
#define I_MASK_CLEAR 0xFB
#define Z_MASK_CLEAR 0xFD
#define C_MASK_CLEAR 0xFE

#define CARRY_MASK_U16 0x100

struct register_u8 {
  uint8_t input;
  uint8_t output;
};

struct register_u16 {
  uint16_t input;
  uint16_t output;
};

#define TICK(reg) (reg).output = (reg).input

#define SET_HIGH(reg, value) (reg).input =  ((reg).input & 0x00FF) | (((uint16_t) value) << 8)

#define SET_LOW(reg, value) (reg).input  = ((reg).input & 0xFF00) | (value)

#define GET_HIGH(reg) (((reg).output & 0xFF00) >> 8)

#define GET_LOW(reg) ((reg).output & 0x00FF)

#define DECREMENT(reg)  (reg).input--

#define INCREMENT(reg)  (reg).input++

#define READ(reg) (reg).output

#define WRITE(reg, value) ((reg).input = (value))

#define AND_BIT(reg, mask) ((reg).input &= (mask))

#define SET_BIT(reg, position) ((reg).input |= (1 << position))

#define CLEAR_BIT(reg, position) ((reg).input &= ~(1 << position))

#define SET_OR_CLEAR_BIT(reg, condition, position) ((reg).input = ((reg).input & ~(1 << (position))) | ((condition) << (position)))

#define OR_BIT(reg, mask) ((reg).input |= (mask))

#define AND_BIT(reg, mask) ((reg).input &= (mask))



#endif //RETRO_EMULATOR_LIB_REGISTER_H_
