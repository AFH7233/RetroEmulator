package com.afh.emulator;

import com.afh.emulator.cpu.CPU;

public class Emulator {
  public static void main(String[] args) {
    CPU cpu = new CPU(0xffff);
    cpu.memory[0xfffc] = 0x00;
    cpu.memory[0xfffd] = 0x10;
    int i = 0;
    // Using first bit after byte to say if input is valid or not
    while (i < 10) {
      cpu.tick( i == 0);
      int address = cpu.getAddress();
      if(!cpu.isRead()){
        System.out.printf("W|\t0x%04X\t:\t0x%02X%n", address, cpu.memory[address]);
      } else {
        System.out.printf("R|\t0x%04X\t:\t0x%02X%n", address, cpu.memory[address]);
      }
      i++;
    }
  }
}
