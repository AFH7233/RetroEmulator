package com.afh.emulator.states;

public sealed interface CPUState permits ResetState, CycleState {

}
