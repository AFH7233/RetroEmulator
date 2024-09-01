#include <stdio.h>
#include "lib/embedded_system.h"
#include "lib/device_manager.h"
#include "lib/memory.h"
#include "lib/opcodes.h"

int main(int argc, char *argv[]) {
  // Initialize manager
    struct device_manager device_manager = new_device_manager();

  // Initialize memory
  struct memory memory = new_memory();

  // Set memory as bus device
  struct bus_device memory_bus = new_memory_bus(&memory);
  add_device(&device_manager, memory.start, memory.end, memory_bus);

  //Write test values
  write_device(&device_manager, 0xfffc, 0x00);
  write_device(&device_manager, 0xfffd, 0x10);
  write_device(&device_manager, 0x1000, JSR_absolute);
  write_device(&device_manager, 0x1001, 0x00);
  write_device(&device_manager, 0x1002, 0x50);
  write_device(&device_manager, 0x4055, 0x00);
  write_device(&device_manager, 0x4056, 0x50);
  write_device(&device_manager, 0x5000, LDA_immediate);
  write_device(&device_manager, 0x5001, 0x40);
  write_device(&device_manager, 0x5002, RTS);
  write_device(&device_manager, 0x1003, STA_zeropage);
  write_device(&device_manager, 0x1004, 0x50);
  write_device(&device_manager, 0x1005, DEX_implied);
  write_device(&device_manager, 0x1006, CPX_immediate);
  write_device(&device_manager, 0x1007, 0xEF);
  write_device(&device_manager, 0x1008, BNE);
  write_device(&device_manager, 0x1009, 0xFB);
  write_device(&device_manager, 0x100A, STOP);



  struct cpu_internals cpu = new_cpu();
  // run processor
  int index = 0;
  while (index < 200) {
    tick(&cpu, &device_manager);
    index++;
  }

  return 0;
}
