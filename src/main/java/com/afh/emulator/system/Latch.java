package com.afh.emulator.system;

public class Latch {

  int value;

  public void setInput(int input) {
    this.value = input & 0x000000FF;
  }

  public int getOutput() {
    return this.value;
  }
}
