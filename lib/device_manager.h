//
// Created by Andres Fuentes Hernandez on 8/18/24.
//

#ifndef RETRO_EMULATOR_LIB_DEVICE_MANAGER_H_
#define RETRO_EMULATOR_LIB_DEVICE_MANAGER_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "log.h"

struct bus_device {
  void* hardware;
  uint8_t (*read)(void*, uint16_t);
  void (*write)(void*, uint16_t, uint8_t);
};

struct device_manager {
  struct bus_device* devices[0x10000];
};

struct device_manager new_device_manager(void);
uint8_t read_device(struct device_manager* manager, uint16_t address);
void write_device(struct device_manager* manager, uint16_t address, uint8_t value);
void add_device(struct device_manager* manager, uint16_t start, uint16_t end, struct bus_device device[1]);


#endif //RETRO_EMULATOR_LIB_DEVICE_MANAGER_H_
