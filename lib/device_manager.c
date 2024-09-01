//
// Created by Andres Fuentes Hernandez on 8/18/24.
//
#include "device_manager.h"

struct device_manager new_device_manager(void){
  struct device_manager device_manager;
  for (size_t i = 0; i < 0x0FFFF; i++) {
    device_manager.devices[i] = (struct bus_device) {
        .hardware = NULL,
        .read = NULL,
        .write = NULL
    };
  }
  return device_manager;
}

uint8_t read_device(struct device_manager* manager, uint16_t address){
  struct bus_device device = manager->devices[address];
  uint8_t data = device.read(device.hardware, address);
  LOG("[READ] Address:0x%04X\tData:0x%02X\n", address, data);
  return data;
}

void write_device(struct device_manager* manager, uint16_t address, uint8_t data){
  struct bus_device device = manager->devices[address];
  LOG("[WRITE] Address:0x%04X,\tData:0x%02X\n", address, data);
  device.write(device.hardware, address, data);
}

void add_device(struct device_manager* manager, uint16_t start, uint16_t end, struct bus_device device){
  end = end > 0xFFFF? 0xFFFF: end;
  for(size_t i=start; i<((size_t) end)+1;i++){
    manager->devices[i] = device;
  }
}
