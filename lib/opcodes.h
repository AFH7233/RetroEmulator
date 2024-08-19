//
// Created by Andres Fuentes Hernandez on 8/18/24.
//

#ifndef RETRO_EMULATOR_LIB_OPCODES_H_
#define RETRO_EMULATOR_LIB_OPCODES_H_

enum opcode {
  // ADC instructions
  ADC_immediate = 0x69,
  ADC_zeropage = 0x65,
  ADC_zeropage_X = 0x75,
  ADC_absolute = 0x6D,
  ADC_absolute_X = 0x7D,
  ADC_absolute_Y = 0x79,
  ADC_indirect_index = 0x71,
  ADC_index_indirect = 0x61,

  // AND instructions
  AND_immediate = 0x29,
  AND_zeropage = 0x25,
  AND_zeropage_X = 0x35,
  AND_absolute = 0x2D,
  AND_absolute_X = 0x3D,
  AND_absolute_Y = 0x39,
  AND_indirect_index = 0x21,
  AND_index_indirect = 0x31,

  // ASL instructions
  ASL_accumulator = 0x0A,
  ASL_zeropage = 0x06,
  ASL_zeropage_X = 0x16,
  ASL_absolute = 0x0E,
  ASL_absolute_X = 0x1E,

  // Branch instructions
  BCC = 0x90,
  BCS = 0xB0,
  BEQ = 0xF0,
  BMI = 0x30,
  BNE = 0xD0,
  BPL = 0x10,
  BVC = 0x50,
  BVS = 0x70,

  // Other instructions
  CLC = 0x18,
  BIT_zeropage = 0x24,
  BIT_absolute = 0x2C,

  INVALID_OPCODE = 0xFF
};

#endif //RETRO_EMULATOR_LIB_OPCODES_H_
