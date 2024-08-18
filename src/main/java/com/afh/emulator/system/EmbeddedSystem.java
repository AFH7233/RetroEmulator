package com.afh.emulator.system;

import static com.afh.emulator.states.MicroStep.*;
import static com.afh.emulator.system.Instructions.*;

import com.afh.emulator.states.CycleState;
import com.afh.emulator.states.MicroStep;
import java.util.List;

public class EmbeddedSystem {

  private final RegisterU8 accumulator;
  private final RegisterU8 xRegister;
  private final RegisterU8 yRegister;
  private final StatusRegister statusRegister;
  private final RegisterU8 stackPointer;
  private final RegisterU8 instructionRegister;
  private final RegisterU16 addressRegister;
  private final RegisterU16 programCounter;
  private final RegisterU8 dataRegister;
  private final RegisterU8 tempRegister;

  private final List<Register> registerList;
  private boolean readWrite;
  private CycleState state;
  private MicroStep microStep;
  private final DataSource busSource;
  private final DeviceManager deviceManager;
  public EmbeddedSystem(DeviceManager deviceManager) {
    this.accumulator = new RegisterU8();
    this.xRegister = new RegisterU8();
    this.yRegister = new RegisterU8();
    this.statusRegister = new StatusRegister();
    this.stackPointer = new RegisterU8();
    this.instructionRegister = new RegisterU8();
    this.programCounter = new RegisterU16();
    this.addressRegister = new RegisterU16();
    this.dataRegister = new RegisterU8();
    this.tempRegister = new RegisterU8();
    registerList = List.of(this.tempRegister, this.dataRegister, this.accumulator, this.xRegister, this.yRegister, this.stackPointer, this.instructionRegister, this.addressRegister, this.programCounter);
    this.state = CycleState.RESET;
    this.busSource = DataSource.X;
    this.deviceManager = deviceManager;
  }

  public void tick(boolean reset) {
    if (reset) {
      state = CycleState.RESET;
      this.microStep = MicroStep.S0;
    }
    this.readWrite = true;
    this.statusRegister.tick(reset);
    this.registerList.forEach(register -> register.tick(reset));
    this.deviceManager.tick(reset);
    switch (this.state) {
      case FETCH -> {
        this.state = CycleState.EXECUTE;
        this.addressRegister.setInput(this.programCounter.getOutput());
        this.instructionRegister.setInput(deviceManager.getData(this.addressRegister.getOutput()));
        this.programCounter.increment();
      }
      case EXECUTE -> {
        this.state = CycleState.FETCH;
        this.execute();
      }
      case RESET -> reset();
    }
  }

  public int getData() {
    return switch (this.busSource) {
      case DataSource.X -> this.xRegister.getOutput();
      case DataSource.Y -> this.yRegister.getOutput();
      case DataSource.ACC -> this.accumulator.getOutput();
      case DataSource.PCL -> this.programCounter.getOutputLow();
      case DataSource.PCH -> this.instructionRegister.getOutput();
    };
  }

  public int getAddress() {
    return this.addressRegister.getOutput();
  }

  public boolean isRead() {
    return this.readWrite;
  }

  private void reset() {
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
      case S4 -> {
        this.microStep = S5;
        this.addressRegister.setInput(this.programCounter.getOutput());
        this.programCounter.increment();
      }
      case S5 -> {
        this.microStep = S6;
        this.addressRegister.setInput(this.programCounter.getOutput());
        this.programCounter.setLow(deviceManager.getData(this.addressRegister.getOutput()));
      }
      case S6 -> {
        this.state = CycleState.EXECUTE;
        this.microStep = S7;
        int data = deviceManager.getData(this.addressRegister.getOutput());
        this.addressRegister.setLow(this.programCounter.getOutputLow());
        this.addressRegister.setHigh(data);
        this.programCounter.increment();
        this.programCounter.setHigh(data);
      }
      default -> throw new IllegalStateException("This should never happen");
    }
  }

  private void execute() {
    switch (this.instructionRegister.getOutput()) {
      case ADC_immediate -> {
        int data = deviceManager.getData(this.addressRegister.getOutput());
        int result = this.add(data, this.accumulator.getOutput());
        this.accumulator.setInput(result);
        afterExecution();
      }
      case ADC_zeropage -> {
        switch (this.microStep) {
          case S0 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.addressRegister.setLow(data);
            this.addressRegister.setHigh(0x00);
            this.microStep = S1;
          }
          case S1 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.adc(data, this.accumulator.getOutput());
            this.accumulator.setInput(result);
            afterExecution();
          }
          default -> throw new IllegalStateException("This should never happen");
        }
      }
      case ADC_zeropage_X -> {
        switch (this.microStep) {
          case S0 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.addressRegister.setLow(data);
            this.addressRegister.setHigh(0x00);
            this.microStep = S1;
          }
          case S1 -> {
            int result = this.addAddress(this.xRegister.getOutput(), this.addressRegister.getOutputLow());
            this.addressRegister.setLow(result & 0x000000ff);
            this.microStep = S2;
          }
          case S2 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.adc(data, this.accumulator.getOutput());
            this.accumulator.setInput(result);
            afterExecution();
          }
          default -> throw new IllegalStateException("This should never happen");
        }
      }
      case ADC_absolute -> {
        switch (this.microStep) {
          case S0 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.tempRegister.setInput(data);
            this.programCounter.increment();
            this.addressRegister.setInput(this.programCounter.getOutput());
            this.microStep = S1;
          }
          case S1 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.addressRegister.setLow(this.tempRegister.getOutput());
            this.addressRegister.setHigh(data);
            this.microStep = S2;
          }
          case S2 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.adc(data, this.accumulator.getOutput());
            this.accumulator.setInput(result);
            afterExecution();
          }
          default -> throw new IllegalStateException("This should never happen");
        }
      }
      case ADC_absolute_X -> {
        switch (this.microStep) {
          case S0 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.tempRegister.setInput(data);
            this.addressRegister.setInput(this.programCounter.getOutput());
            this.microStep = S1;
            this.programCounter.increment();
          }
          case S1 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.addAddress(this.tempRegister.getOutput(), this.xRegister.getOutput());
            this.microStep = (result & 0x00000100) > 0 ? S2 : S3;
            this.addressRegister.setLow(result);
            this.addressRegister.setHigh(data);
            this.programCounter.increment();
          }
          case S2 -> {
            int result = this.addAddress(this.addressRegister.getOutputHigh(), 1);
            this.addressRegister.setHigh(result & 0x000000ff);
            this.microStep = S3;
          }
          case S3 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.adc(data, this.accumulator.getOutput());
            this.accumulator.setInput(result);
            afterExecution();
          }
          default -> throw new IllegalStateException("This should never happen");
        }
      }
      case ADC_absolute_Y -> {
        switch (this.microStep) {
          case S0 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.tempRegister.setInput(data);
            this.addressRegister.setInput(this.programCounter.getOutput());
            this.microStep = S1;
            this.programCounter.increment();
          }
          case S1 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.addAddress(this.tempRegister.getOutput(), this.yRegister.getOutput());
            this.microStep = (result & 0x00000100) > 0 ? S2 : S3;
            this.addressRegister.setLow(result);
            this.addressRegister.setHigh(data);
            this.programCounter.increment();
          }
          case S2 -> {
            int result = this.addAddress(this.addressRegister.getOutputHigh(), 1);
            this.addressRegister.setHigh(result & 0x000000ff);
            this.microStep = S3;
          }
          case S3 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.adc(data, this.accumulator.getOutput());
            this.accumulator.setInput(result);
            afterExecution();
          }
          default -> throw new IllegalStateException("This should never happen");
        }
      }
      case ADC_index_indirect -> {
        switch (this.microStep) {
          case S0 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.addressRegister.setLow(data);
            this.addressRegister.setHigh(0x00);
            this.microStep = S1;
          }
          case S1 -> {
            int result = this.addAddress(this.addressRegister.getOutputLow(), this.xRegister.getOutput());
            this.addressRegister.setLow(result & 0x000000ff);
            this.microStep = S2;
          }
          case S2 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.addAddress(this.addressRegister.getOutputLow(), 1);
            this.addressRegister.setLow(result & 0x000000ff);
            this.tempRegister.setInput(data);
            this.microStep = S3;
          }
          case S3 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.addressRegister.setLow(this.tempRegister.getOutput());
            this.addressRegister.setHigh(data);
            this.microStep = S4;
          }
          case S4 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.adc(data, this.accumulator.getOutput());
            this.accumulator.setInput(result);
            afterExecution();
          }
          default -> throw new IllegalStateException("This should never happen");
        }
      }
      case ADC_indirect_index -> {
        switch (this.microStep) {
          case S0 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.addressRegister.setLow(data);
            this.addressRegister.setHigh(0x00);
            this.microStep = S1;
          }
          case S1 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.addAddress(this.addressRegister.getOutputLow(), 1);
            this.addressRegister.setLow(result & 0x000000ff);
            this.tempRegister.setInput(data);
            this.microStep = S2;
          }
          case S2 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.addAddress(this.tempRegister.getOutput(), this.yRegister.getOutput());
            this.microStep = (result & 0x00000100) > 0 ? S3 : S4;
            this.addressRegister.setLow(result & 0x000000ff);
            this.addressRegister.setHigh(data);
            this.microStep = S3;
          }
          case S3 -> {
            int result = this.addAddress(this.addressRegister.getOutputHigh(), 1);
            this.addressRegister.setHigh(result & 0x000000ff);
            this.microStep = S4;
          }
          case S4 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.adc(data, this.accumulator.getOutput());
            this.accumulator.setInput(result);
            afterExecution();
          }
          default -> throw new IllegalStateException("This should never happen");
        }
      }
      case AND_immediate -> {
        int data = deviceManager.getData(this.addressRegister.getOutput());
        int result = this.and(data, this.accumulator.getOutput());
        this.accumulator.setInput(result);
        afterExecution();
      }
      case AND_zeropage -> {
        switch (this.microStep) {
          case S0 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.addressRegister.setLow(data);
            this.addressRegister.setHigh(0x00);
            this.microStep = S1;
          }
          case S1 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.and(data, this.accumulator.getOutput());
            this.accumulator.setInput(result);
            afterExecution();
          }
          default -> throw new IllegalStateException("This should never happen");
        }
      }
      case AND_zeropage_X -> {
        switch (this.microStep) {
          case S0 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.addressRegister.setLow(data);
            this.addressRegister.setHigh(0x00);
            this.microStep = S1;
          }
          case S1 -> {
            int result = this.addAddress(this.xRegister.getOutput(), this.addressRegister.getOutputLow());
            this.addressRegister.setLow(result & 0x000000ff);
            this.microStep = S2;
          }
          case S2 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.and(data, this.accumulator.getOutput());
            this.accumulator.setInput(result);
            afterExecution();
          }
          default -> throw new IllegalStateException("This should never happen");
        }
      }
      case AND_absolute -> {
        switch (this.microStep) {
          case S0 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.tempRegister.setInput(data);
            this.programCounter.increment();
            this.addressRegister.setInput(this.programCounter.getOutput());
            this.microStep = S1;
          }
          case S1 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.addressRegister.setLow(this.tempRegister.getOutput());
            this.addressRegister.setHigh(data);
            this.microStep = S2;
          }
          case S2 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.and(data, this.accumulator.getOutput());
            this.accumulator.setInput(result);
            afterExecution();
          }
          default -> throw new IllegalStateException("This should never happen");
        }
      }
      case AND_absolute_X -> {
        switch (this.microStep) {
          case S0 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.tempRegister.setInput(data);
            this.addressRegister.setInput(this.programCounter.getOutput());
            this.microStep = S1;
            this.programCounter.increment();
          }
          case S1 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.addAddress(this.tempRegister.getOutput(), this.xRegister.getOutput());
            this.microStep = (result & 0x00000100) > 0 ? S2 : S3;
            this.addressRegister.setLow(result);
            this.addressRegister.setHigh(data);
            this.programCounter.increment();
          }
          case S2 -> {
            int result = this.addAddress(this.addressRegister.getOutputHigh(), 1);
            this.addressRegister.setHigh(result & 0x000000ff);
            this.microStep = S3;
          }
          case S3 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.and(data, this.accumulator.getOutput());
            this.accumulator.setInput(result);
            afterExecution();
          }
          default -> throw new IllegalStateException("This should never happen");
        }
      }
      case AND_absolute_Y -> {
        switch (this.microStep) {
          case S0 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.tempRegister.setInput(data);
            this.addressRegister.setInput(this.programCounter.getOutput());
            this.microStep = S1;
            this.programCounter.increment();
          }
          case S1 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.addAddress(this.tempRegister.getOutput(), this.yRegister.getOutput());
            this.microStep = (result & 0x00000100) > 0 ? S2 : S3;
            this.addressRegister.setLow(result);
            this.addressRegister.setHigh(data);
            this.programCounter.increment();
          }
          case S2 -> {
            int result = this.addAddress(this.addressRegister.getOutputHigh(), 1);
            this.addressRegister.setHigh(result & 0x000000ff);
            this.microStep = S3;
          }
          case S3 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.and(data, this.accumulator.getOutput());
            this.accumulator.setInput(result);
            afterExecution();
          }
          default -> throw new IllegalStateException("This should never happen");
        }
      }
      case AND_index_indirect -> {
        switch (this.microStep) {
          case S0 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.addressRegister.setLow(data);
            this.addressRegister.setHigh(0x00);
            this.microStep = S1;
          }
          case S1 -> {
            int result = this.addAddress(this.addressRegister.getOutputLow(), this.xRegister.getOutput());
            this.addressRegister.setLow(result & 0x000000ff);
            this.microStep = S2;
          }
          case S2 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.addAddress(this.addressRegister.getOutputLow(), 1);
            this.addressRegister.setLow(result & 0x000000ff);
            this.tempRegister.setInput(data);
            this.microStep = S3;
          }
          case S3 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.addressRegister.setLow(this.tempRegister.getOutput());
            this.addressRegister.setHigh(data);
            this.microStep = S4;
          }
          case S4 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.and(data, this.accumulator.getOutput());
            this.accumulator.setInput(result);
            afterExecution();
          }
          default -> throw new IllegalStateException("This should never happen");
        }
      }
      case AND_indirect_index -> {
        switch (this.microStep) {
          case S0 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.addressRegister.setLow(data);
            this.addressRegister.setHigh(0x00);
            this.microStep = S1;
          }
          case S1 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.addAddress(this.addressRegister.getOutputLow(), 1);
            this.addressRegister.setLow(result & 0x000000ff);
            this.tempRegister.setInput(data);
            this.microStep = S2;
          }
          case S2 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.addAddress(this.tempRegister.getOutput(), this.yRegister.getOutput());
            this.microStep = (result & 0x00000100) > 0 ? S3 : S4;
            this.addressRegister.setLow(result & 0x000000ff);
            this.addressRegister.setHigh(data);
            this.microStep = S3;
          }
          case S3 -> {
            int result = this.addAddress(this.addressRegister.getOutputHigh(), 1);
            this.addressRegister.setHigh(result & 0x000000ff);
            this.microStep = S4;
          }
          case S4 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.and(data, this.accumulator.getOutput());
            this.accumulator.setInput(result);
            afterExecution();
          }
          default -> throw new IllegalStateException("This should never happen");
        }
      }
      case ASL_accumulator -> {
        int result = this.asl(this.accumulator.getOutput());
        this.accumulator.setInput(result);
        afterExecution();
      }
      case ASL_zeropage -> {
        switch (this.microStep) {
          case S0 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.addressRegister.setLow(data);
            this.addressRegister.setHigh(0x00);
            this.microStep = S1;
          }
          case S1 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.asl(data);
            this.dataRegister.setInput(result);
            this.microStep = S2;
          }
          case S2 -> {
            deviceManager.setData(this.addressRegister.getOutput(), this.dataRegister.getOutput());
            // set write to true
            this.microStep = S3;
          }
          case S3 -> {
            afterExecution();
          }
          default -> throw new IllegalStateException("This should never happen");
        }
      }
      case ASL_zeropage_X -> {
        switch (this.microStep) {
          case S0 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.addressRegister.setLow(data);
            this.addressRegister.setHigh(0x00);
            this.microStep = S1;
          }
          case S1 -> {
            int result = this.addAddress(this.xRegister.getOutput(), this.addressRegister.getOutputLow());
            this.addressRegister.setLow(result & 0x000000ff);
            this.microStep = S2;
          }
          case S2 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.asl(data);
            this.dataRegister.setInput(result);
            this.microStep = S3;
          }
          case S3 -> {
            deviceManager.setData(this.addressRegister.getOutput(), this.dataRegister.getOutput());
            // set write to true
            this.microStep = S4;
          }
          case S4 -> {
            afterExecution();
          }
          default -> throw new IllegalStateException("This should never happen");
        }
      }
      case ASL_absolute -> {
        switch (this.microStep) {
          case S0 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.tempRegister.setInput(data);
            this.programCounter.increment();
            this.addressRegister.setInput(this.programCounter.getOutput());
            this.microStep = S1;
          }
          case S1 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.addressRegister.setLow(this.tempRegister.getOutput());
            this.addressRegister.setHigh(data);
            this.microStep = S2;
          }
          case S2 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.asl(data);
            this.dataRegister.setInput(result);
            this.microStep = S3;
          }
          case S3 -> {
            deviceManager.setData(this.addressRegister.getOutput(), this.dataRegister.getOutput());
            // set write to true
            this.microStep = S4;
          }
          case S4 -> {
            afterExecution();
          }
          default -> throw new IllegalStateException("This should never happen");
        }
      }
      case ASL_absolute_X -> {
        switch (this.microStep) {
          case S0 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.tempRegister.setInput(data);
            this.addressRegister.setInput(this.programCounter.getOutput());
            this.microStep = S1;
            this.programCounter.increment();
          }
          case S1 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.addAddress(this.tempRegister.getOutput(), this.xRegister.getOutput());
            this.microStep = (result & 0x00000100) > 0 ? S2 : S3;
            this.addressRegister.setLow(result);
            this.addressRegister.setHigh(data);
            this.programCounter.increment();
          }
          case S2 -> {
            int result = this.addAddress(this.addressRegister.getOutputHigh(), 1);
            this.addressRegister.setHigh(result & 0x000000ff);
            this.microStep = S3;
          }
          case S3 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.asl(data);
            this.dataRegister.setInput(result);
            this.microStep = S4;
          }
          case S4 -> {
            deviceManager.setData(this.addressRegister.getOutput(), this.dataRegister.getOutput());
            // set write to true
            this.microStep = S5;
          }
          case S5 -> {
            afterExecution();
          }
          default -> throw new IllegalStateException("This should never happen");
        }
      }
      case BCC -> {
        switch (this.microStep) {
          case S0 -> {
            byte data = (byte) deviceManager.getData(this.addressRegister.getOutput());
            if(!this.statusRegister.isC()){
              int result = this.addAddress(this.programCounter.getOutputLow(), data);
              this.programCounter.setLow(result & 0x000000ff);
              this.microStep = (result & 0x00000100) > 0 ? S1 : S2;
            } else {
              this.dataRegister.setInput(data);
            }
          }
          case S1 -> {
            int result = this.addAddress(this.programCounter.getOutputHigh(), 1);
            this.programCounter.setHigh(result & 0x000000ff);
            this.microStep = S2;
          }
          case S2 -> {
            this.addressRegister.setInput(this.programCounter.getOutput());
            this.state = CycleState.FETCH;
          }
        }
      }
      case BCS -> {
        switch (this.microStep) {
          case S0 -> {
            byte data = (byte) deviceManager.getData(this.addressRegister.getOutput());
            if(this.statusRegister.isC()){
              int result = this.addAddress(this.programCounter.getOutputLow(), data);
              this.programCounter.setLow(result & 0x000000ff);
              this.microStep = (result & 0x00000100) > 0 ? S1 : S2;
            } else {
              this.dataRegister.setInput(data);
            }
          }
          case S1 -> {
            int result = this.addAddress(this.programCounter.getOutputHigh(), 1);
            this.programCounter.setHigh(result & 0x000000ff);
            this.microStep = S2;
          }
          case S2 -> {
            this.addressRegister.setInput(this.programCounter.getOutput());
            this.state = CycleState.FETCH;
          }
        }
      }
      case BEQ -> {
        switch (this.microStep) {
          case S0 -> {
            byte data = (byte) deviceManager.getData(this.addressRegister.getOutput());
            if(this.statusRegister.isZ()){
              int result = this.addAddress(this.programCounter.getOutputLow(), data);
              this.programCounter.setLow(result & 0x000000ff);
              this.microStep = (result & 0x00000100) > 0 ? S1 : S2;
            } else {
              this.dataRegister.setInput(data);
            }
          }
          case S1 -> {
            int result = this.addAddress(this.programCounter.getOutputHigh(), 1);
            this.programCounter.setHigh(result & 0x000000ff);
            this.microStep = S2;
          }
          case S2 -> {
            this.addressRegister.setInput(this.programCounter.getOutput());
            this.state = CycleState.FETCH;
          }
        }
      }
      case BIT_zeropage -> {
        switch (this.microStep) {
          case S0 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.addressRegister.setLow(data);
            this.addressRegister.setHigh(0x00);
            this.microStep = S1;
          }
          case S1 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.and(data, this.accumulator.getOutput());
            this.statusRegister.setN((result & 0x00000080) == 0x00000080);
            this.statusRegister.setV((result & 0x00000040) == 0x00000040);
            afterExecution();
          }
          default -> throw new IllegalStateException("This should never happen");
        }
      }
      case BIT_absolute -> {
        switch (this.microStep) {
          case S0 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.tempRegister.setInput(data);
            this.programCounter.increment();
            this.addressRegister.setInput(this.programCounter.getOutput());
            this.microStep = S1;
          }
          case S1 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            this.addressRegister.setLow(this.tempRegister.getOutput());
            this.addressRegister.setHigh(data);
            this.microStep = S2;
          }
          case S2 -> {
            int data = deviceManager.getData(this.addressRegister.getOutput());
            int result = this.and(data, this.accumulator.getOutput());
            this.statusRegister.setN((result & 0x00000080) == 0x00000080);
            this.statusRegister.setV((result & 0x00000040) == 0x00000040);
            afterExecution();
          }
          default -> throw new IllegalStateException("This should never happen");
        }
      }
      case BMI -> {
        switch (this.microStep) {
          case S0 -> {
            byte data = (byte) deviceManager.getData(this.addressRegister.getOutput());
            if(this.statusRegister.isN()){
              int result = this.addAddress(this.programCounter.getOutputLow(), data);
              this.programCounter.setLow(result & 0x000000ff);
              this.microStep = (result & 0x00000100) > 0 ? S1 : S2;
            } else {
              this.dataRegister.setInput(data);
            }
          }
          case S1 -> {
            int result = this.addAddress(this.programCounter.getOutputHigh(), 1);
            this.programCounter.setHigh(result & 0x000000ff);
            this.microStep = S2;
          }
          case S2 -> {
            this.addressRegister.setInput(this.programCounter.getOutput());
            this.state = CycleState.FETCH;
          }
        }
      }
      case BNE -> {
        switch (this.microStep) {
          case S0 -> {
            byte data = (byte) deviceManager.getData(this.addressRegister.getOutput());
            if(!this.statusRegister.isZ()){
              int result = this.addAddress(this.programCounter.getOutputLow(), data);
              this.programCounter.setLow(result & 0x000000ff);
              this.microStep = (result & 0x00000100) > 0 ? S1 : S2;
            } else {
              this.dataRegister.setInput(data);
            }
          }
          case S1 -> {
            int result = this.addAddress(this.programCounter.getOutputHigh(), 1);
            this.programCounter.setHigh(result & 0x000000ff);
            this.microStep = S2;
          }
          case S2 -> {
            this.addressRegister.setInput(this.programCounter.getOutput());
            this.state = CycleState.FETCH;
          }
        }
      }
      case BPL -> {
        switch (this.microStep) {
          case S0 -> {
            byte data = (byte) deviceManager.getData(this.addressRegister.getOutput());
            if(!this.statusRegister.isN()){
              int result = this.addAddress(this.programCounter.getOutputLow(), data);
              this.programCounter.setLow(result & 0x000000ff);
              this.microStep = (result & 0x00000100) > 0 ? S1 : S2;
            } else {
              this.dataRegister.setInput(data);
            }
          }
          case S1 -> {
            int result = this.addAddress(this.programCounter.getOutputHigh(), 1);
            this.programCounter.setHigh(result & 0x000000ff);
            this.microStep = S2;
          }
          case S2 -> {
            this.addressRegister.setInput(this.programCounter.getOutput());
            this.state = CycleState.FETCH;
          }
        }
      }
      case BVC -> {
        switch (this.microStep) {
          case S0 -> {
            byte data = (byte) deviceManager.getData(this.addressRegister.getOutput());
            if(!this.statusRegister.isV()){
              int result = this.addAddress(this.programCounter.getOutputLow(), data);
              this.programCounter.setLow(result & 0x000000ff);
              this.microStep = (result & 0x00000100) > 0 ? S1 : S2;
            } else {
              this.dataRegister.setInput(data);
            }
          }
          case S1 -> {
            int result = this.addAddress(this.programCounter.getOutputHigh(), 1);
            this.programCounter.setHigh(result & 0x000000ff);
            this.microStep = S2;
          }
          case S2 -> {
            this.addressRegister.setInput(this.programCounter.getOutput());
            this.state = CycleState.FETCH;
          }
        }
      }
      case BVS -> {
        switch (this.microStep) {
          case S0 -> {
            byte data = (byte) deviceManager.getData(this.addressRegister.getOutput());
            if(this.statusRegister.isV()){
              int result = this.addAddress(this.programCounter.getOutputLow(), data);
              this.programCounter.setLow(result & 0x000000ff);
              this.microStep = (result & 0x00000100) > 0 ? S1 : S2;
            } else {
              this.dataRegister.setInput(data);
            }
          }
          case S1 -> {
            int result = this.addAddress(this.programCounter.getOutputHigh(), 1);
            this.programCounter.setHigh(result & 0x000000ff);
            this.microStep = S2;
          }
          case S2 -> {
            this.addressRegister.setInput(this.programCounter.getOutput());
            this.state = CycleState.FETCH;
          }
        }
      }
      case CLC -> {
        this.statusRegister.setC(false);
        afterExecution();
      }
    }
  }

  private void afterExecution() {
    this.addressRegister.setInput(this.programCounter.getOutput());
    this.programCounter.increment();
    this.state = CycleState.FETCH;
  }

  private int adc(int data, int acc) {
    int carry = this.statusRegister.isC() ? 1 : 0;
    int result = data + acc + carry;
    boolean bothNegative = ((data & 0x00000080) == 0x00000080) && ((acc & 0x00000080) == 0x00000080);
    boolean bothPositive = ((data & 0x00000080) == 0x00000000) && ((acc & 0x00000080) == 0x00000000);
    boolean V = (bothNegative && ((data & 0x00000080) == 0x00000000)) || (bothPositive && ((data & 0x00000080) == 0x00000080));
    this.statusRegister.setC(result > 0xff);
    this.statusRegister.setZ((result & 0x000000ff) == 0);
    this.statusRegister.setN((result & 0x00000080) == 0x00000080);
    this.statusRegister.setV(V);
    return (result & 0x000000ff);
  }

  private int addAddress(int data, int acc) {
    int result = data + acc;
    return (result & 0x000001ff);
  }

  private int add(int data, int acc) {
    int result = data + acc;
    boolean bothNegative = ((data & 0x00000080) == 0x00000080) && ((acc & 0x00000080) == 0x00000080);
    boolean bothPositive = ((data & 0x00000080) == 0x00000000) && ((acc & 0x00000080) == 0x00000000);
    boolean V = (bothNegative && ((data & 0x00000080) == 0x00000000)) || (bothPositive && ((data & 0x00000080) == 0x00000080));
    this.statusRegister.setC(result > 0xff);
    this.statusRegister.setZ((result & 0x000000ff) == 0);
    this.statusRegister.setN((result & 0x00000080) == 0x00000080);
    this.statusRegister.setV(V);
    return (result & 0x000000ff);
  }

  private int and(int data, int acc) {
    int result = data & acc;
    this.statusRegister.setZ((result & 0x000000ff) == 0);
    this.statusRegister.setN((result & 0x00000080) == 0x00000080);
    return (result & 0x000000ff);
  }

  private int asl(int data) {
    int result = data << 1;
    this.statusRegister.setC(result > 0xff);
    return (result & 0x000000ff);
  }

  private enum DataSource {
    ACC,
    X,
    Y,
    //ALU,
    //A,
    PCL,
    PCH,
  }


}
