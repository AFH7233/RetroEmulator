//
// Created by Andres Fuentes Hernandez on 8/18/24.
//
#include <stdlib.h>
#include "device_manager.h"

uint8_t read_device(struct device_manager* manager, uint16_t address){
  struct bus_device device = manager->devices[address];
  return device.read(device.hardware, address);
}

void write_device(struct device_manager* manager, uint16_t address, uint8_t value){
  struct bus_device device = manager->devices[address];
  device.write(device.hardware, address, value);
}

void add_device(struct device_manager* manager, uint16_t start, uint16_t end, struct bus_device device){
  end = end > 0xFFFF? 0xFFFF: end;
  for(size_t i=start; i<((size_t) end)+1;i++){
    manager->devices[i] = device;
  }
}