package com.afh.emulator.system;

public interface BusDevice {
  void setData(int address, int input);
  int getData(int address);
  void tick(boolean reset);
  boolean isAddressValid(int address);
  boolean isOverlapping(BusDevice busDevice);
  int getStart();
  int getEnd();
}
