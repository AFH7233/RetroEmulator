package com.afh.emulator.system;

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
    this.mask = type.equals(Type.U8) ? 0x000000FF : 0x0000FFFF;
  }

  public Register(){
    this.type = Type.U8;
    this.mask = 0xFF;
  }

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

  public int getOutputLow() {
    return this.output & 0x000000ff;
  }

  public int getOutputHigh() {
    return (this.output & 0x0000ff00)>> 8;
  }

  public void setLow(int input){
    int temp = input & 0x000000ff;
    this.input = (this.input & 0x0000ff00) | temp;
  }

  public void setHigh(int input){
    int temp = (input << 8) & 0x0000ff00;
    this.input = (this.input & 0xffff00ff) | temp;
  }
}
