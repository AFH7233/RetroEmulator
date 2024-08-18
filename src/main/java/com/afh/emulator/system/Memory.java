package com.afh.emulator.system;

public class Memory implements BusDevice {

  private final int[] slots;
  private final int start;
  private final int end;
  private int input;
  private int address;

  public Memory(int start, int end) {
    if (start > end) {
      throw new IllegalArgumentException("start > end");
    }
    this.slots = new int[end - start + 1];
    this.start = start;
    this.end = end;
  }

  @Override
  public void tick(boolean reset) {
    this.slots[this.address] = this.input;
  }

  @Override
  public void setData(int address, int input) {
    this.address = address;
    this.input = input;
  }

  public void setImmediate(int address, int input) {
    this.slots[address] = input;
  }

  @Override
  public int getData(int address) {
    return this.slots[address];
  }

  @Override
  public boolean isAddressValid(int address) {
    return start <= address && address <= end;
  }

  @Override
  public boolean isOverlapping(BusDevice device) {
    return (start < device.getEnd() && device.getStart() < end)
        || (start < device.getStart() && device.getEnd() < end)
        || (device.getStart() < end && end < device.getEnd());
  }

  @Override
  public int getStart() {
    return this.start;
  }

  @Override
  public int getEnd() {
    return this.end;
  }
}
