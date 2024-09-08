#include "lib/common.h"
#include "lib/device_manager.h"
#include "lib/memory.h"
#include "lib/log.h"
#include "lib/embedded_system.h"

static void load_fibonacci_program(struct device_manager *device_manager);

int main(int argc, char *argv[]) {
  // Initialize manager
  struct device_manager device_manager = new_device_manager();

  // Initialize memory
  struct memory memory = new_memory(0x0000, 0xffff);

  // Set memory as bus device
  struct bus_device memory_bus = new_memory_bus(&memory);
  add_device(&device_manager, memory.start, memory.end, &memory_bus);

  //Write test values
  load_fibonacci_program(&device_manager);

  struct cpu_internals cpu = new_cpu();
  // run processor
  int index = 0;
  while (index < 1000) {
    tick(&cpu, &device_manager);
    index++;
  }
  LOG_MEMORY(0x0000, 0x002, memory.slots);
  return 0;
}

void load_fibonacci_program(struct device_manager *device_manager) {
  // Set the reset vector to point to $1000
  write_device(device_manager, 0xfffc, 0x00); // Low byte of reset vector
  write_device(device_manager, 0xfffd, 0x10); // High byte of reset vector

  // Load instructions into memory starting at $1000
  int address = 0x1000;

  // LDX #$01; x = 1
  write_device(device_manager, address++, LDX_immediate); //0x1000
  write_device(device_manager, address++, 0x01); //0x1001

  // STX $00; stores x
  write_device(device_manager, address++, STX_zeropage); //0x1002
  write_device(device_manager, address++, 0x00); //0x1003

  // SEC; clean carry
  write_device(device_manager, address++, SEC); //0x1004

  // LDY #$07; calculate 7th Fibonacci number (13 = D in hex)
  write_device(device_manager, address++, LDY_immediate); //0x1005
  write_device(device_manager, address++, 0x07); //0x1006

  // TYA; transfer Y register to accumulator
  write_device(device_manager, address++, TYA); //0x1007

  // SBC #$03; handles the algorithm iteration counting
  write_device(device_manager, address++, SBC_immediate); //0x1008
  write_device(device_manager, address++, 0x03); //0x1009

  // TAY; transfer accumulator to Y register
  write_device(device_manager, address++, TAY); //0x100A

  // CLC; clean carry
  write_device(device_manager, address++, CLC); //0x100B

  // LDA #$02; a = 2
  write_device(device_manager, address++, LDA_immediate); //0x100C
  write_device(device_manager, address++, 0x02); //0x100D

  // STA $01; stores a
  write_device(device_manager, address++, STA_zeropage);  //0x100E
  write_device(device_manager, address++, 0x01); //0x100F

  // loop:
  // LDX $01; x = a
  write_device(device_manager, address++, LDX_zeropage); //0x1010
  write_device(device_manager, address++, 0x01);  //0x1011

  // ADC $00; a += x
  write_device(device_manager, address++, ADC_zeropage);  //0x1012
  write_device(device_manager, address++, 0x00); //0x1013

  // STA $01; stores a
  write_device(device_manager, address++, STA_zeropage); //0x1014
  write_device(device_manager, address++, 0x01); //0x1015

  // STX $00; stores x
  write_device(device_manager, address++, STX_zeropage); //0x1016
  write_device(device_manager, address++, 0x00); //0x1017

  // DEY; y -= 1
  write_device(device_manager, address++, DEY_implied); //0x1018

  // BNE loop; jumps back to loop if y != 0
  write_device(device_manager, address++, BNE);  //0x1019
  write_device(device_manager, address++, 0xF5); //0x101A

  // BRK
  write_device(device_manager, address++, BRK); //0x101B
  write_device(device_manager, address++, STOP); //0x101C

  write_device(device_manager, 0xFFFE, 0x1C);
  write_device(device_manager, 0xFFFF, 0x10);

  printf("Fibonacci program loaded into memory.\n");
}

