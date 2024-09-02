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
  BRK = 0x00,
  BVC = 0x50,
  BVS = 0x70,

  // Other instructions
  BIT_zeropage = 0x24,
  BIT_absolute = 0x2C,

  // Clear flags
  CLC = 0x18,
  CLD = 0xD8,
  CLI = 0x58,
  CLV = 0xB8,

  // Compare instructions
  CMP_immediate = 0xC9,
  CMP_zeropage = 0xC5,
  CMP_zeropage_X = 0xD5,
  CMP_absolute = 0xCD,
  CMP_absolute_X = 0xDD,
  CMP_absolute_Y = 0xD9,
  CMP_indirect_index = 0xC1,
  CMP_index_indirect = 0xD1,

  CPX_immediate = 0xE0,
  CPX_zeropage = 0xE4,
  CPX_absolute = 0xEC,

  CPY_immediate = 0xC0,
  CPY_zeropage = 0xC4,
  CPY_absolute = 0xCC,

  DEC_zeropage = 0xC6,
  DEC_zeropage_X = 0xD6,
  DEC_absolute = 0xCE,
  DEC_absolute_X = 0xDE,

  DEX_implied = 0xCA,
  DEY_implied = 0x88,

  EOR_immediate = 0x49,
  EOR_zeropage = 0x45,
  EOR_zeropage_X = 0x55,
  EOR_absolute = 0x4D,
  EOR_absolute_X = 0x5D,
  EOR_absolute_Y = 0x59,
  EOR_indirect_index = 0x41,
  EOR_index_indirect = 0x51,

  INC_zeropage = 0xE6,
  INC_zeropage_X = 0xF6,
  INC_absolute = 0xEE,
  INC_absolute_X = 0xFE,

  INX_implied = 0xE8,
  INY_implied = 0xC8,

  JMP_absolute = 0x4C,
  JMP_indirect = 0x6C,

  JSR_absolute = 0x20,

  LDA_immediate = 0xA9,
  LDA_zeropage = 0xA5,
  LDA_zeropage_X = 0xB5,
  LDA_absolute = 0xAD,
  LDA_absolute_X = 0xBD,
  LDA_absolute_Y = 0xB9,
  LDA_indirect_index = 0xA1,
  LDA_index_indirect = 0xB1,

  LDX_immediate = 0xA2,
  LDX_zeropage = 0xA6,
  LDX_zeropage_Y = 0xB6,
  LDX_absolute = 0xAE,
  LDX_absolute_Y = 0xBE,

  LDY_immediate = 0xA0,
  LDY_zeropage = 0xA4,
  LDY_zeropage_X = 0xB4,
  LDY_absolute = 0xAC,
  LDY_absolute_X = 0xBC,

  LSR_accumulator = 0x4A,
  LSR_zeropage = 0x46,
  LSR_zeropage_X = 0x56,
  LSR_absolute = 0x4E,
  LSR_absolute_X = 0x5E,

  ORA_immediate = 0x09,
  ORA_zeropage = 0x05,
  ORA_zeropage_X = 0x15,
  ORA_absolute = 0x0D,
  ORA_absolute_X = 0x1D,
  ORA_absolute_Y = 0x19,
  ORA_indirect_index = 0x01,
  ORA_index_indirect = 0x11,

  NOP = 0xEA,

  PHA = 0x48,
  PHP = 0x08,
  PLA = 0x68,
  PLP = 0x28,

  ROL_accumulator = 0x2A,
  ROL_zeropage = 0x26,
  ROL_zeropage_X = 0x36,
  ROL_absolute = 0x2E,
  ROL_absolute_X = 0x3E,

  ROR_accumulator = 0x6A,
  ROR_zeropage = 0x66,
  ROR_zeropage_X = 0x76,
  ROR_absolute = 0x6E,
  ROR_absolute_X = 0x7E,

  RTI = 0x40,
  RTS = 0x60,

  SBC_immediate = 0xE9,
  SBC_zeropage = 0xE5,
  SBC_zeropage_X = 0xF5,
  SBC_absolute = 0xED,
  SBC_absolute_X = 0xFD,
  SBC_absolute_Y = 0xF9,
  SBC_indirect_index = 0xE1,
  SBC_index_indirect = 0xF1,

  SEC = 0x38,
  SED = 0xF8,
  SEI = 0x78,

  STA_zeropage = 0x85,
  STA_zeropage_X = 0x95,
  STA_absolute = 0x8D,
  STA_absolute_X = 0x9D,
  STA_absolute_Y = 0x99,
  STA_indirect_index = 0x81,
  STA_index_indirect = 0x91,

  STX_zeropage = 0x86,
  STX_zeropage_Y = 0x96,
  STX_absolute = 0x8E,

  STY_zeropage = 0x84,
  STY_zeropage_X = 0x94,
  STY_absolute = 0x8C,

  TAX = 0xAA,
  TAY = 0xA8,
  TSX = 0xBA,
  TXA = 0x8A,
  TXS = 0x9A,
  TYA = 0x98,

  STOP = 0xFF
};

#endif //RETRO_EMULATOR_LIB_OPCODES_H_
