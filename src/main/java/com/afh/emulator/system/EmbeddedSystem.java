package com.afh.emulator.system;

import com.afh.emulator.states.CPUState;
import com.afh.emulator.states.CycleState;
import com.afh.emulator.states.MicroStep;
import com.afh.emulator.states.ResetState;

import java.util.List;

import static com.afh.emulator.states.MicroStep.*;
import static com.afh.emulator.system.Instructions.*;

public class EmbeddedSystem {

  private enum DataSource{
    ACC,
    X,
    Y,
    //ALU,
    //A,
    PCL,
    PCH,
  }

  private Register accumulator;
  private Register xRegister;
  private Register yRegister;
  private StatusRegister statusRegister;
  private Register stackPointer;
  private Register instructionRegister;
  private Register addressRegister;
  private Register programCounter;
  private Register dataRegister;

  private List<Register> registerList;
  private boolean readWrite;

  public final int[] memory; // Byte size ignore extra bits

  private CycleState state;
  private MicroStep microStep;
  private DataSource busSource;

  public EmbeddedSystem() {
    this.accumulator = new Register();
    this.xRegister = new Register();
    this.yRegister = new Register();
    this.statusRegister = new StatusRegister();
    this.stackPointer = new Register();
    this.instructionRegister = new Register();
    this.programCounter = new Register(Register.Type.U16);
    this.addressRegister = new Register(Register.Type.U16);
    this.dataRegister = new Register();
    registerList  = List.of(this.dataRegister, this.accumulator, this.xRegister, this.yRegister, this.stackPointer, this.instructionRegister, this.addressRegister, this.programCounter);
    this.state = CycleState.RESET;
    this.busSource = DataSource.X;
    this.memory = new int[0x0ffff];
  }

  public void tick( boolean reset) {
    if (reset) {
      state = CycleState.RESET;
      this.microStep = MicroStep.S0;
    }
    this.readWrite = true;
    this.statusRegister.tick(reset);
    this.registerList.forEach(register -> register.tick(reset));
    switch (this.state) {
      case FETCH -> {
        this.state = CycleState.EXECUTE;
        this.addressRegister.setInput(this.programCounter.getOutput());
        this.instructionRegister.setInput(this.memory[this.addressRegister.getOutput()]);
        this.programCounter.increment();
      }
      case EXECUTE -> {
        this.state = CycleState.FETCH;
        this.addressRegister.setInput(this.programCounter.getOutput());
        this.execute();
      }
      case RESET -> reset();
    }
  }


  public int getData(){
    return switch (this.busSource){
      case DataSource.X -> this.xRegister.getOutput();
      case DataSource.Y -> this.yRegister.getOutput();
      case DataSource.ACC -> this.accumulator.getOutput();
      case DataSource.PCL -> this.programCounter.getOutputLow();
      case DataSource.PCH -> this.instructionRegister.getOutputHigh();
    };
  }

  public int getAddress(){
    return this.addressRegister.getOutput();
  }

  public boolean isRead(){
    return this.readWrite;
  }

  private void reset( ) {
    switch (this.microStep) {
      case S0 -> {
        this.microStep = S1;
      }
      case S1 -> {
        this.microStep = S2;
        this.addressRegister.setHigh(0x01);
        this.addressRegister.setLow(this.stackPointer.getOutput());
        this.stackPointer.decrement();
      }
      case S2 -> {
        this.microStep = S3;
        this.addressRegister.setLow(this.stackPointer.getOutput());
        this.stackPointer.decrement();
        this.programCounter.setHigh(0xFF);
      }
      case S3 -> {
        this.microStep = S4;
        this.addressRegister.setLow(this.stackPointer.getOutput());
        this.programCounter.setLow(0xFC);
      }
      case S4-> {
        this.microStep = S5;
        this.addressRegister.setInput(this.programCounter.getOutput());
        this.programCounter.increment();
      }
      case S5-> {
        this.microStep = S6;
        this.addressRegister.setInput(this.programCounter.getOutput());
        this.programCounter.setLow(this.memory[this.addressRegister.getOutput()]);
      }
      case S6-> {
        this.state = CycleState.EXECUTE;
        this.microStep = S7;
        int latch = this.memory[this.addressRegister.getOutput()] << 8;
        this.addressRegister.setInput(latch + this.programCounter.getOutputLow());
        this.programCounter.increment();
        this.programCounter.setHigh(this.memory[this.addressRegister.getOutput()]);
      }
      default -> throw new IllegalStateException("This should never happen");
      }
  }

  private void execute() {

    switch (this.instructionRegister.getOutput()) {
        case ADC_immediate -> {
          int data = this.memory[this.addressRegister.getOutput()];
          int result = this.add(data, this.accumulator.getOutput());
          this.accumulator.setInput(result);
          this.state = CycleState.FETCH;
        }
        case ADC_zeropage -> {
            switch (this.microStep){
              case S0 -> {
                int data = this.memory[this.addressRegister.getOutput()];
                this.addressRegister.setLow(data);
                this.addressRegister.setHigh(0x00);
                this.microStep = S1;
              }
              case S1 -> {
                int data = this.memory[this.addressRegister.getOutput()];
                int result = this.add(data, this.accumulator.getOutput());
                this.accumulator.setInput(result);
                this.state = CycleState.FETCH;
              }
              default -> throw new IllegalStateException("This should never happen");
            }
        }
        default -> {}
      }
  }

  private int add(int data, int acc){
    int result = data + acc;
    // Just to check carry between nibbles
    boolean V = ((data & 0x00000010) == 0x00000010) && ((acc & 0x00000010) == 0x00000010);
    this.statusRegister.setC(result > 0xff);
    this.statusRegister.setZ((result & 0x000000ff) == 0);
    this.statusRegister.setN((result & 0x00000080) ==  0x00000080);
    this.statusRegister.setV(V);
    return (result & 0x000000ff);
  }




}
