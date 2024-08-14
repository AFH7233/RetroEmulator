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
    }
    this.readWrite = true;
    this.registerList.forEach(register -> register.tick(reset));
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
      case ResetState.RESET_01 -> {
        this.state = ResetState.RESET_02;
      }
      case ResetState.RESET_02 -> {
        this.state = ResetState.RESET_03;
        this.addressRegister.setInput(0x100 + this.stackPointer.getOutput());
        this.stackPointer.decrement();
      }
      case ResetState.RESET_03 -> {
        this.state = ResetState.RESET_04;
        this.addressRegister.setInput(0x100 + this.stackPointer.getOutput());
        this.stackPointer.decrement();
      }
      case ResetState.RESET_04 -> {
        this.state = ResetState.RESET_05;
        this.addressRegister.setInput(0x100 + this.stackPointer.getOutput());
      }
      case ResetState.RESET_05 -> {
        this.state = ResetState.RESET_06;
        this.addressRegister.setInput(0xFFFC);
        this.programCounter.increment();
      }
      case ResetState.RESET_06 -> {
        this.state = ResetState.RESET_07;
        this.addressRegister.setInput(0xFFFD);
        this.programCounter.setLow(this.memory[this.addressRegister.getOutput()]);
      }
      case ResetState.RESET_07 -> {
        this.state = CycleState.EXECUTE;
        int latch = this.memory[this.addressRegister.getOutput()] << 8;
        this.addressRegister.setInput(latch + this.programCounter.getOutputLow());
        this.programCounter.setHigh(latch);
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
