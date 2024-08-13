package com.afh.emulator;

public class Memory {

    private final int[] slots; // Byte size ignore extra bits

    public Memory(int size) {
        this.slots = new int[size];
    }


}
