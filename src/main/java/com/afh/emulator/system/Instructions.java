package com.afh.emulator.system;

public class Instructions {
  private Instructions(){}
  public static final int ADC_immediate = 0x69;
  public static final int ADC_zeropage = 0x65;
  public static final int ADC_zeropage_X = 0x75;
  public static final int ADC_absolute = 0x6D;
  public static final int ADC_absolute_X = 0x7D;
  public static final int ADC_absolute_Y = 0x79;
  public static final int ADC_indirect_index = 0x71;
  public static final int ADC_index_indirect = 0x61;

  public static final int AND_immediate = 0x29;
  public static final int AND_zeropage = 0x25;
  public static final int AND_zeropage_X = 0x35;
  public static final int AND_absolute = 0x2D;
  public static final int AND_absolute_X = 0x3D;
  public static final int AND_absolute_Y = 0x39;
  public static final int AND_indirect_index = 0x21;
  public static final int AND_index_indirect = 0x31;

  public static final int ASL_accumulator = 0x0A;
  public static final int ASL_zeropage = 0x06;
  public static final int ASL_zeropage_X = 0x16;
  public static final int ASL_absolute = 0x0E;
  public static final int ASL_absolute_X = 0x1E;

  public static final int BCC = 0x90;
  public static final int BCS = 0xB0;
  public static final int BEQ = 0xF0;
  public static final int BMI = 0x30;
  public static final int BNE = 0xD0;
  public static final int BPL = 0x10;
  public static final int BVC = 0x50;
  public static final int BVS = 0x70;

  public static final int CLC = 0x18;


  public static final int BIT_zeropage = 0x24;
  public static final int BIT_absolute = 0x2C;


  public static final int HALT = 0xFF;

}

