package com.afh.emulator;

import com.afh.emulator.system.EmbeddedSystem;

public class Emulator {
  public static void main(String[] args) {
    EmbeddedSystem embeddedSystem = new EmbeddedSystem();
    embeddedSystem.memory[0xfffc] = 0x00;
    embeddedSystem.memory[0xfffd] = 0x10;
    int i = 0;
    // Using first bit after byte to say if input is valid or not
    while (i < 10) {
      embeddedSystem.tick( i == 0);
      int address = embeddedSystem.getAddress();
      if(!embeddedSystem.isRead()){
        System.out.printf("W|\t0x%04X\t:\t0x%02X%n", address, embeddedSystem.memory[address]);
      } else {
        System.out.printf("R|\t0x%04X\t:\t0x%02X%n", address, embeddedSystem.memory[address]);
      }
      i++;
    }
  }
}
