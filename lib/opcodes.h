//
// Created by Andres Fuentes Hernandez on 8/18/24.
//

#ifndef RETRO_EMULATOR_LIB_OPCODES_H_
#define RETRO_EMULATOR_LIB_OPCODES_H_

enum opcode {
  // ADC instructions
  ADC_immediate,
  ADC_zeropage,
  ADC_zeropage_X,
  ADC_absolute,
  ADC_absolute_X,
  ADC_absolute_Y,
  ADC_indirect_index,
  ADC_index_indirect,

  // AND instructions
  AND_immediate,
  AND_zeropage,
  AND_zeropage_X,
  AND_absolute,
  AND_absolute_X,
  AND_absolute_Y,
  AND_indirect_index,
  AND_index_indirect,

  // ASL instructions
  ASL_accumulator,
  ASL_zeropage,
  ASL_zeropage_X,
  ASL_absolute,
  ASL_absolute_X,

  // Branch instructions
  BCC,
  BCS,
  BEQ,
  BMI,
  BNE,
  BPL,
  BVC,
  BVS,

  CLC,
  BIT_zeropage,
  BIT_absolute,

  INVALID_OPCODE = 0xFF
};

enum opcode lookup_table[256] = {
    [0x69] = ADC_immediate,
    [0x65] = ADC_zeropage,
    [0x75] = ADC_zeropage_X,
    [0x6D] = ADC_absolute,
    [0x7D] = ADC_absolute_X,
    [0x79] = ADC_absolute_Y,
    [0x71] = ADC_indirect_index,
    [0x61] = ADC_index_indirect,

    [0x29] = AND_immediate,
    [0x25] = AND_zeropage,
    [0x35] = AND_zeropage_X,
    [0x2D] = AND_absolute,
    [0x3D] = AND_absolute_X,
    [0x39] = AND_absolute_Y,
    [0x21] = AND_indirect_index,
    [0x31] = AND_index_indirect,

    [0x0A] = ASL_accumulator,
    [0x06] = ASL_zeropage,
    [0x16] = ASL_zeropage_X,
    [0x0E] = ASL_absolute,
    [0x1E] = ASL_absolute_X,

    [0x90] = BCC,
    [0xB0] = BCS,
    [0xF0] = BEQ,
    [0x30] = BMI,
    [0xD0] = BNE,
    [0x10] = BPL,
    [0x50] = BVC,
    [0x70] = BVS,

    [0x18] = CLC,
    [0x24] = BIT_zeropage,
    [0x2C] = BIT_absolute,
};

#endif //RETRO_EMULATOR_LIB_OPCODES_H_
