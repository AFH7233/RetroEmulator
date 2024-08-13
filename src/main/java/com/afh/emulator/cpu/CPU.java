package com.afh.emulator.cpu;

import com.afh.emulator.states.CPUState;
import com.afh.emulator.states.CycleState;
import com.afh.emulator.states.ResetState;

import java.util.List;

public class CPU {

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
  private Register statusRegister;
  private Register stackPointer;
  private Register instructionRegister;
  private Register addressRegister;
  private Register programCounter;

  private List<Register> registerList;
  private boolean readWrite;

  public final int[] memory; // Byte size ignore extra bits

  private CPUState state;
  private DataSource busSource;

  public CPU(int size) {
    this.accumulator = new Register();
    this.xRegister = new Register();
    this.yRegister = new Register();
    this.statusRegister = new Register();
    this.stackPointer = new Register();
    this.instructionRegister = new Register();
    this.programCounter = new Register(Register.Type.U16);
    this.addressRegister = new Register(Register.Type.U16);
    registerList  = List.of(this.accumulator, this.xRegister, this.yRegister, this.statusRegister, this.stackPointer, this.instructionRegister, this.addressRegister, this.programCounter);
    this.state = ResetState.RESET_01;
    this.busSource = DataSource.X;
    this.memory = new int[size];
  }

  public void tick( boolean reset) {
    if (reset) {
      state = ResetState.RESET_01;
      this.stackPointer.setInput(0x00);
      return;
    }
    this.readWrite = true;
    this.registerList.forEach(Register::tick);
    switch (state) {
      case ResetState resetstate -> reset( resetstate);
      case CycleState cycleState -> cycle( cycleState);
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

  private void reset( ResetState resetstate) {
    switch (resetstate) {
      case ResetState.RESET_01 -> {  // ? + 1
        this.state = ResetState.RESET_02;
        // Equivalent to setting latch to //0x01
        this.addressRegister.setInput(0x100 + this.stackPointer.getOutput());
        this.stackPointer.decrement();
        this.programCounter.setLow(0xFC);
      }
      case ResetState.RESET_02 -> { // 100
        this.state = ResetState.RESET_03;
        this.addressRegister.setInput(0x100 + this.stackPointer.getOutput());
        this.stackPointer.decrement();
        this.programCounter.setHigh(0xFF);
      }
      case ResetState.RESET_03 -> { // 1FF
        this.state = ResetState.RESET_04;
        this.addressRegister.setInput(0x100 + this.stackPointer.getOutput());
      }
      case ResetState.RESET_04 -> { // 1FE
        this.state = ResetState.RESET_05;
        this.addressRegister.setInput(this.programCounter.getOutput());
        this.programCounter.increment();
      }
      case ResetState.RESET_05 -> { // FFFC
        this.state = ResetState.RESET_06;
        this.addressRegister.setInput(this.programCounter.getOutput());
        this.programCounter.setLow(this.memory[this.addressRegister.getOutput()]);
      }
      case ResetState.RESET_06 -> { // FFFC
        this.state = CycleState.EXECUTE;
        int latch = this.memory[this.addressRegister.getOutput()] << 8;
        this.addressRegister.setInput(latch + this.programCounter.getOutputLow());
        this.programCounter.setHigh(this.memory[this.addressRegister.getOutput()] );
      }
      }
  }

  private void cycle(CycleState cycleState) {
    switch (cycleState) {
      case FETCH -> {
        this.state = CycleState.EXECUTE;
        this.addressRegister.setInput(this.programCounter.getOutput());
        this.instructionRegister.setInput(this.memory[this.addressRegister.getOutput()]);
        this.programCounter.increment();
      }
      case EXECUTE -> {}
    }
  }



}
