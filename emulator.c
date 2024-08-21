#include <stdio.h>
#include "lib/embedded_system.h"
#include "lib/device_manager.h"
#include "lib/memory.h"

int main(int argc, char *argv[]) {
  // Initialize manager
  struct device_manager device_manager;
  for (size_t i = 0; i < 0x0FFFF; i++) {
    device_manager.devices[i] = (struct bus_device) {
        .hardware = NULL,
        .read = NULL,
        .write = NULL
    };
  }

  // Initialize memory
  struct memory memory = {
      .start = 0x1000,
      .end = 0xffff
  };
  for (size_t i = 0; i < 256; i++) {
    memory.slots[i] = 0;
  }


  // Set memory as bus device
  struct bus_device memory_bus = {
      .hardware = (void *) &memory,
      .read = (uint8_t (*)(void *, uint16_t)) read_memory,
      .write = (void (*)(void *, uint16_t, uint8_t)) write_memory,
  };

  add_device(&device_manager, memory.start, memory.end, memory_bus);
  
  //Write test values
  write_device(&device_manager, 0xfffc, 0x00);
  write_device(&device_manager, 0xfffd, 0x10);
  write_device(&device_manager, 0x1000, 0x69);
  write_device(&device_manager, 0x1001, 0x50);
  write_device(&device_manager, 0x1002, 0xFF);
  struct cpu_internals cpu = new_cpu();
  // run processor
  int index = 0;
  while(index < 7){
    tick(&cpu, &device_manager);
    index++;
  }
  return 0;
}
