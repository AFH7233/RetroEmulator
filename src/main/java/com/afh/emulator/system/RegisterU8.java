package com.afh.emulator.system;

public class RegisterU8 implements Register {
  private int output;
  private int input;
  private final int mask = 0x000000FF;


  public void tick(boolean reset){
    this.output = reset ? 0x00 : this.input;
  }

  public void increment() {
    this.input = this.output + 1;
    this.input = this.input & this.mask;
  }

  public void decrement() {
    this.input = this.output - 1;
    this.input = this.input & this.mask;
  }

  public void setInput(int input) {
    this.input = input & this.mask;
  }

  public int getOutput() {
    return this.output;
  }
}
