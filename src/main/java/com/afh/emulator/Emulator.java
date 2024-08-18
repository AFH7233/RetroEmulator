package com.afh.emulator;

import com.afh.emulator.system.DeviceManager;
import com.afh.emulator.system.EmbeddedSystem;
import com.afh.emulator.system.Memory;

public class Emulator {
  public static void main(String[] args) {
    DeviceManager deviceManager = new DeviceManager();
    EmbeddedSystem embeddedSystem = new EmbeddedSystem(deviceManager);
    Memory memory = new Memory(0x00,0xffff);
    memory.setImmediate(0xfffc, 0x00);
    memory.setImmediate(0xfffd, 0x10);
    deviceManager.addDevice(memory);

    int i = 0;
    // Using first bit after byte to say if input is valid or not
    while (i < 10) {
      embeddedSystem.tick( i == 0);
      int address = embeddedSystem.getAddress();
      if(!embeddedSystem.isRead()){
        System.out.printf("W|\t0x%04X\t:\t0x%02X%n", address, memory.getData(address));
      } else {
        System.out.printf("R|\t0x%04X\t:\t0x%02X%n", address, memory.getData(address));
      }
      i++;
    }
  }
}
