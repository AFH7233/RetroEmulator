package com.afh.emulator.cpu;

public class Register {
  public enum Type {
    U8,
    U16
  }
  private int output;
  private int input;
  private final Type type;
  private final int mask;

  public Register(Type type) {
    this.type = type;
    this.mask = type.equals(Type.U8) ? 0xFF : 0xFFFF;
  }

  public Register(){
    this.type = Type.U8;
    this.mask = 0xFF;
  }

  public void tick(){
    this.output = this.input;
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

  public int getOutputLow() {
    return this.output;
  }

  public int getOutputHigh() {
    return this.output >> 8;
  }

  public void setLow(int input){
    int temp = input & 0x000000ff;
    this.input = temp;
  }

  public void setHigh(int input){
    int temp = (input << 8) & 0x0000ff00;
    this.input |= temp;
  }
}
