//
// Created by Andres Fuentes Hernandez on 8/18/24.
//

#include <stdio.h>
#include "embedded_system.h"
#include "register.h"
#include "opcodes.h"

static void reset(struct cpu_internals *cpu, struct device_manager *device_manager);
static void fetch(struct cpu_internals *cpu, struct device_manager *device_manager);
static void halt(struct cpu_internals *cpu, struct device_manager *device_manager);
static void execute(struct cpu_internals *cpu, struct device_manager *device_manager);

static void immediate_read(struct cpu_internals *cpu,
                           struct device_manager *device_manager,
                           uint8_t (*alu)(struct cpu_internals *, uint8_t, uint8_t));
static void zeropage_read(struct cpu_internals *cpu,
                          struct device_manager *device_manager,
                          uint8_t (*alu)(struct cpu_internals *, uint8_t, uint8_t));
static void zeropage_x_read(struct cpu_internals *cpu,
                            struct device_manager *device_manager,
                            uint8_t (*alu)(struct cpu_internals *, uint8_t, uint8_t));
static void absolute_read(struct cpu_internals *cpu,
                          struct device_manager *device_manager,
                          uint8_t (*alu)(struct cpu_internals *, uint8_t, uint8_t));
static void absolute_x_read(struct cpu_internals *cpu,
                            struct device_manager *device_manager,
                            uint8_t (*alu)(struct cpu_internals *, uint8_t, uint8_t));
static void absolute_y_read(struct cpu_internals *cpu,
                            struct device_manager *device_manager,
                            uint8_t (*alu)(struct cpu_internals *, uint8_t, uint8_t));
static void index_indirect_read(struct cpu_internals *cpu,
                                struct device_manager *device_manager,
                                uint8_t (*alu)(struct cpu_internals *, uint8_t, uint8_t));
static void indirect_index_read(struct cpu_internals *cpu,
                                struct device_manager *device_manager,
                                uint8_t (*alu)(struct cpu_internals *, uint8_t, uint8_t));

static void adc_immediate_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void adc_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void adc_zeropage_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void adc_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void adc_absolute_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void adc_absolute_y_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void adc_index_indirect_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void adc_indirect_index_handler(struct cpu_internals *cpu, struct device_manager *device_manager);

static void and_immediate_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void and_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void and_zeropage_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void and_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void and_absolute_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void and_absolute_y_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void and_index_indirect_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void and_indirect_index_handler(struct cpu_internals *cpu, struct device_manager *device_manager);

static void asl_accumulator_handler(struct cpu_internals *cpu, struct device_manager *device_manager);

static uint16_t add_address(uint8_t data, uint8_t acc);
static uint8_t adc(struct cpu_internals *cpu, uint8_t data, uint8_t acc);
static uint8_t and(struct cpu_internals *cpu, uint8_t data, uint8_t acc);
static uint8_t asl(struct cpu_internals *cpu, uint8_t data);
static void prepare_fetch(struct cpu_internals *cpu, struct device_manager *device_manager);

// This will be used instead of a switch case which was slower
// I know I'm wasting some memory.
typedef void
(*opcode_handler)(struct cpu_internals *cpu, struct device_manager *device_manager); //just this time I'll use typedef
opcode_handler opcode_handlers[256] = {
    [ADC_immediate] = adc_immediate_handler,
    [ADC_zeropage] = adc_zeropage_handler,
    [ADC_zeropage_X] = adc_zeropage_x_handler,
    [ADC_absolute] = adc_absolute_handler,
    [ADC_absolute_X] = adc_absolute_x_handler,
    [ADC_absolute_Y] = adc_absolute_y_handler,
    [ADC_index_indirect] = adc_index_indirect_handler,
    [ADC_indirect_index] = adc_indirect_index_handler,

    [AND_immediate] = and_immediate_handler,
    [AND_zeropage] = and_zeropage_handler,
    [AND_zeropage_X] = and_zeropage_x_handler,
    [AND_absolute] = and_absolute_handler,
    [AND_absolute_X] = and_absolute_x_handler,
    [AND_absolute_Y] = and_absolute_y_handler,
    [AND_index_indirect] = and_index_indirect_handler,
    [AND_indirect_index] = and_indirect_index_handler,

    [ASL_accumulator] = asl_accumulator_handler,

    [HALT_CODE] = halt,
};

typedef void(*cpu_state_handler)
    (struct cpu_internals *cpu, struct device_manager *device_manager); //just this time I'll use typedef
cpu_state_handler cpu_state_handlers[5] = {
    [RESET] = reset,
    [FETCH] = fetch,
    [EXECUTE] = execute,
    [HALT] = halt
};

void run(struct device_manager device_manager[1]) {

  struct cpu_internals cpu;
  cpu.accumulator.input = 0;
  cpu.x_register.input = 0;
  cpu.y_register.input = 0;
  cpu.status_register.input = 0;
  cpu.stack_pointer.input = 0;
  cpu.instruction_register.input = 0;
  cpu.program_counter.input = 0;
  cpu.address_register.input = 0;
  cpu.data_register.input = 0;
  cpu.temp_register.input = 0;

  cpu.state = RESET;
  cpu.micro_step = S0;

  bool run = true;
  int index = 0;
  while (run) {

    //Clock cycle
    cpu.accumulator.output = cpu.accumulator.input;
    cpu.x_register.output = cpu.x_register.input;
    cpu.y_register.output = cpu.y_register.input;
    cpu.status_register.output = cpu.status_register.input;
    cpu.stack_pointer.output = cpu.stack_pointer.input;
    cpu.instruction_register.output = cpu.instruction_register.input;
    cpu.program_counter.output = cpu.program_counter.input;
    cpu.address_register.output = cpu.address_register.input;
    cpu.data_register.output = cpu.data_register.input;
    cpu.temp_register.output = cpu.temp_register.input;

    cpu_state_handler state_handler = cpu_state_handlers[cpu.state];
    state_handler(&cpu, device_manager);

    printf("%d\t|\t%#06x\n", index, READ(cpu.address_register));

    index++;
    run = index < 10;
  }
  printf("accumulator:\t%#06x\n", READ(cpu.accumulator));

}

void execute(struct cpu_internals *cpu, struct device_manager *device_manager) {
  opcode_handler instruction = opcode_handlers[READ(cpu->instruction_register)];
  instruction(cpu, device_manager);
}

void fetch(struct cpu_internals *cpu, struct device_manager *device_manager) {
  cpu->state = EXECUTE;
  WRITE(cpu->address_register, READ(cpu->program_counter));
  WRITE(cpu->instruction_register, read_device(device_manager, READ(cpu->address_register)));
  INCREMENT(cpu->program_counter);
}

void reset(struct cpu_internals *cpu, struct device_manager *device_manager) {
  switch (cpu->micro_step) {
    case S0: {
      cpu->micro_step = S1;
      return;
    }
    case S1: {
      cpu->micro_step = S2;
      SET_HIGH(cpu->address_register, 0x01);
      SET_LOW(cpu->address_register, READ(cpu->stack_pointer));
      DECREMENT(cpu->stack_pointer);
      return;
    }
    case S2: {
      cpu->micro_step = S3;
      SET_LOW(cpu->address_register, READ(cpu->stack_pointer));
      DECREMENT(cpu->stack_pointer);
      SET_HIGH(cpu->program_counter, 0xFF);
      return;
    }
    case S3: {
      cpu->micro_step = S4;
      SET_LOW(cpu->address_register, READ(cpu->stack_pointer));
      SET_LOW(cpu->program_counter, 0xFC);
      return;
    }
    case S4: {
      cpu->micro_step = S5;
      WRITE(cpu->address_register, READ(cpu->program_counter));
      INCREMENT(cpu->program_counter);
      return;
    }
    case S5: {
      cpu->micro_step = S6;
      WRITE(cpu->address_register, READ(cpu->program_counter));
      SET_LOW(cpu->program_counter, read_device(device_manager, READ(cpu->address_register)));
      return;
    }
    case S6: {
      cpu->state = FETCH;
      uint8_t bus_data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->address_register, GET_LOW(cpu->program_counter));
      SET_HIGH(cpu->address_register, bus_data);
      INCREMENT(cpu->program_counter);
      SET_HIGH(cpu->program_counter, bus_data);
      return;
    }
    default: fprintf(stderr, "Wrong!!!!");
  }

}

void adc_immediate_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  immediate_read(cpu, device_manager, adc);
  prepare_fetch(cpu, device_manager);
  return;
}

void adc_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  zeropage_read(cpu, device_manager, adc);
  prepare_fetch(cpu, device_manager);
  return;
}

void adc_zeropage_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  zeropage_x_read(cpu, device_manager, adc);
  prepare_fetch(cpu, device_manager);
  return;
}

void adc_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_read(cpu, device_manager, adc);
  prepare_fetch(cpu, device_manager);
  return;
}

void adc_absolute_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_x_read(cpu, device_manager, adc);
  prepare_fetch(cpu, device_manager);
  return;
}

void adc_absolute_y_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_y_read(cpu, device_manager, adc);
  prepare_fetch(cpu, device_manager);
  return;
}

void adc_index_indirect_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  index_indirect_read(cpu, device_manager, adc);
  prepare_fetch(cpu, device_manager);
  return;
}

void adc_indirect_index_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  indirect_index_read(cpu, device_manager, adc);
  prepare_fetch(cpu, device_manager);
  return;
}

void and_immediate_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  immediate_read(cpu, device_manager, and);
  prepare_fetch(cpu, device_manager);
  return;
}

void and_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  zeropage_read(cpu, device_manager, and);
  prepare_fetch(cpu, device_manager);
  return;
}

void and_zeropage_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  zeropage_x_read(cpu, device_manager, and);
  prepare_fetch(cpu, device_manager);
  return;
}

void and_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_read(cpu, device_manager, and);
  prepare_fetch(cpu, device_manager);
  return;
}

void and_absolute_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_x_read(cpu, device_manager, and);
  prepare_fetch(cpu, device_manager);
  return;
}

void and_absolute_y_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_y_read(cpu, device_manager, and);
  prepare_fetch(cpu, device_manager);
  return;
}

void and_index_indirect_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  index_indirect_read(cpu, device_manager, and);
  prepare_fetch(cpu, device_manager);
  return;
}

void and_indirect_index_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  indirect_index_read(cpu, device_manager, and);
  prepare_fetch(cpu, device_manager);
  return;
}

void asl_accumulator_handler(struct cpu_internals *cpu, struct device_manager *device_manager){
  uint8_t result = asl(cpu, READ(cpu->accumulator));
  WRITE(cpu->accumulator, result);
  prepare_fetch(cpu, device_manager);
}

static void halt(struct cpu_internals *cpu, struct device_manager *device_manager) {
  cpu->state = HALT;
  return;
}

void prepare_fetch(struct cpu_internals *cpu, struct device_manager *device_manager) {
  WRITE(cpu->address_register, READ(cpu->program_counter));
  INCREMENT(cpu->program_counter);
  cpu->state = FETCH;
}

void immediate_read(struct cpu_internals *cpu,
                    struct device_manager *device_manager,
                    uint8_t (*alu)(struct cpu_internals *, uint8_t, uint8_t)) {
  uint8_t data = read_device(device_manager, READ(cpu->address_register));
  uint8_t result = alu(cpu, data, READ(cpu->accumulator));
  WRITE(cpu->accumulator, result);
}

void zeropage_read(struct cpu_internals *cpu,
                   struct device_manager *device_manager,
                   uint8_t (*alu)(struct cpu_internals *, uint8_t, uint8_t)) {
  switch (cpu->micro_step) {
    case S0: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->address_register, data);
      SET_HIGH(cpu->address_register, 0x00);
      cpu->micro_step = S1;
      return;
    }
    case S1: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      uint8_t result = alu(cpu, data, READ(cpu->accumulator));
      WRITE(cpu->accumulator, result);
      return;
    }
    default: fprintf(stderr, "Wrong zeropage step!!!!");
  }
}

void zeropage_x_read(struct cpu_internals *cpu,
                     struct device_manager *device_manager,
                     uint8_t (*alu)(struct cpu_internals *, uint8_t, uint8_t)) {
  switch (cpu->micro_step) {
    case S0: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->address_register, data);
      SET_HIGH(cpu->address_register, 0x00);
      cpu->micro_step = S1;
      return;
    }
    case S1: {
      uint16_t result = add_address(READ(cpu->x_register), READ(cpu->address_register));
      SET_LOW(cpu->address_register, (result & 0x000000ff));
      cpu->micro_step = S2;
    }
    case S2: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      uint8_t result = alu(cpu, data, READ(cpu->accumulator));
      WRITE(cpu->accumulator, result);
    }
    default: fprintf(stderr, "Wrong zeropage X step!!!!");
  }
}

void absolute_read(struct cpu_internals *cpu,
                   struct device_manager *device_manager,
                   uint8_t (*alu)(struct cpu_internals *, uint8_t, uint8_t)) {
  switch (cpu->micro_step) {
    case S0: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      WRITE(cpu->temp_register, data);
      INCREMENT(cpu->program_counter);
      WRITE(cpu->address_register, READ(cpu->program_counter));
      cpu->micro_step = S1;
      return;
    }
    case S1: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->address_register, READ(cpu->temp_register));
      SET_HIGH(cpu->address_register, data);
      cpu->micro_step = S2;
      return;
    }
    case S2: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      uint8_t result = alu(cpu, data, READ(cpu->accumulator));
      WRITE(cpu->accumulator, result);
      return;
    }
    default: fprintf(stderr, "Wrong absolute step!!!!");
  }
}

void absolute_x_read(struct cpu_internals *cpu,
                     struct device_manager *device_manager,
                     uint8_t (*alu)(struct cpu_internals *, uint8_t, uint8_t)) {
  switch (cpu->micro_step) {
    case S0: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      WRITE(cpu->temp_register, data);
      WRITE(cpu->address_register, READ(cpu->program_counter));
      INCREMENT(cpu->program_counter);
      cpu->micro_step = S1;
      return;
    }
    case S1: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      uint16_t result = add_address(READ(cpu->temp_register), READ(cpu->x_register));
      SET_LOW(cpu->address_register, (result & 0x00ff));
      SET_HIGH(cpu->address_register, data);
      INCREMENT(cpu->program_counter);
      cpu->micro_step = (result & N_MASK_SET) > 0 ? S2 : S3;
      return;
    }
    case S2: {
      uint16_t result = add_address(GET_HIGH(cpu->address_register), 1);
      SET_HIGH(cpu->address_register, (result & 0x000000ff));
      cpu->micro_step = S3;
      return;
    }
    case S3: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      uint8_t result = alu(cpu, data, READ(cpu->accumulator));
      WRITE(cpu->accumulator, result);
      return;
    }
    default: fprintf(stderr, "Wrong absolute x step!!!!");
  }
}

void absolute_y_read(struct cpu_internals *cpu,
                     struct device_manager *device_manager,
                     uint8_t (*alu)(struct cpu_internals *, uint8_t, uint8_t)) {
  switch (cpu->micro_step) {
    case S0: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      WRITE(cpu->temp_register, data);
      WRITE(cpu->address_register, READ(cpu->program_counter));
      INCREMENT(cpu->program_counter);
      cpu->micro_step = S1;
      return;
    }
    case S1: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      uint16_t result = add_address(READ(cpu->temp_register), READ(cpu->y_register));
      SET_LOW(cpu->address_register, (result & 0x00ff));
      SET_HIGH(cpu->address_register, data);
      INCREMENT(cpu->program_counter);
      cpu->micro_step = (result & N_MASK_SET) > 0 ? S2 : S3;
      return;
    }
    case S2: {
      uint16_t result = add_address(GET_HIGH(cpu->address_register), 1);
      SET_HIGH(cpu->address_register, (result & 0x000000ff));
      cpu->micro_step = S3;
      return;
    }
    case S3: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      uint8_t result = alu(cpu, data, READ(cpu->accumulator));
      WRITE(cpu->accumulator, result);
      return;
    }
    default: fprintf(stderr, "Wrong absolute x step!!!!");
  }
}

void index_indirect_read(struct cpu_internals *cpu,
                         struct device_manager *device_manager,
                         uint8_t (*alu)(struct cpu_internals *, uint8_t, uint8_t)) {
  switch (cpu->micro_step) {
    case S0: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->address_register, data);
      SET_HIGH(cpu->address_register, 0x00);
      cpu->micro_step = S1;
      return;
    }
    case S1: {
      uint16_t result = add_address(GET_LOW(cpu->address_register), READ(cpu->x_register));
      SET_LOW(cpu->address_register, (result & 0x000000ff));
      cpu->micro_step = S2;
      return;
    }
    case S2: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      uint16_t result = add_address(GET_LOW(cpu->address_register), 1);
      SET_LOW(cpu->address_register, (result & 0x000000ff));
      WRITE(cpu->temp_register, data);
      cpu->micro_step = S3;
      return;
    }
    case S3: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->address_register, READ(cpu->temp_register));
      SET_HIGH(cpu->address_register, data);
      cpu->micro_step = S4;
      return;
    }
    case S4: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      uint8_t result = alu(cpu, data, READ(cpu->accumulator));
      WRITE(cpu->accumulator, result);
      return;
    }
    default: fprintf(stderr, "Wrong index indirect step!!!!");
  }
}

void indirect_index_read(struct cpu_internals *cpu,
                         struct device_manager *device_manager,
                         uint8_t (*alu)(struct cpu_internals *, uint8_t, uint8_t)) {
  switch (cpu->micro_step) {
    case S0: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->address_register, data);
      SET_HIGH(cpu->address_register, 0x00);
      cpu->micro_step = S1;
      return;
    }
    case S1: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      uint16_t result = add_address(GET_LOW(cpu->address_register), 1);
      SET_LOW(cpu->address_register, (result & 0x00ff));
      WRITE(cpu->temp_register, data);
      cpu->micro_step = S2;
      return;
    }
    case S2: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      uint16_t result = add_address(READ(cpu->temp_register), READ(cpu->y_register));
      SET_LOW(cpu->address_register, (result & 0x000000ff));
      SET_HIGH(cpu->address_register, data);
      cpu->micro_step = (result & N_MASK_SET) > 0 ? S3 : S4;
      return;
    }
    case S3: {
      uint16_t result = add_address(GET_HIGH(cpu->address_register), 1);
      SET_HIGH(cpu->address_register, (result & 0x000000ff));
      cpu->micro_step = S4;
      return;
    }
    case S4: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      uint8_t result = alu(cpu, data, READ(cpu->accumulator));
      WRITE(cpu->accumulator, result);
      return;
    }
    default: fprintf(stderr, "Wrong indirect index step!!!!");
  }
}

uint16_t add_address(uint8_t data, uint8_t acc) {
  uint16_t result = ((uint16_t) data) + ((uint16_t) acc);
  return (result & 0x000001ff);
}

uint8_t adc(struct cpu_internals *cpu, uint8_t data, uint8_t acc) {
  uint16_t carry = (READ(cpu->status_register) & C_MASK_SET);
  uint16_t result = ((uint16_t) data) + ((uint16_t) acc) + carry;
  bool bothNegative = ((data & N_MASK_SET) == N_MASK_SET) && ((acc & N_MASK_SET) == N_MASK_SET);
  bool bothPositive = ((data & N_MASK_SET) == 0) && ((acc & N_MASK_SET) == 0);
  bool V = (bothNegative && ((data & N_MASK_SET) == 0)) || (bothPositive && ((data & N_MASK_SET) == N_MASK_SET));
  WRITE(cpu->status_register, 0);
  SET_BIT(cpu->status_register, (((uint8_t) (result > 0xff)) << C_FLAG));
  SET_BIT(cpu->status_register, (((uint8_t) ((result & 0x000000ff) == 0)) << Z_FLAG));
  SET_BIT(cpu->status_register, (((uint8_t) ((result & N_MASK_SET) == N_MASK_SET)) << N_FLAG));
  SET_BIT(cpu->status_register, (((uint8_t) V) << V_FLAG));
  return (uint8_t) (result & 0x000000ff);
}

uint8_t and(struct cpu_internals *cpu, uint8_t data, uint8_t acc) {
  uint8_t result = data & acc;
  WRITE(cpu->status_register, 0);
  SET_BIT(cpu->status_register, (((uint8_t) (result == 0)) << Z_FLAG));
  SET_BIT(cpu->status_register, (((uint8_t) ((result & N_MASK_SET) == N_MASK_SET)) << N_FLAG));
  return result;
}

uint8_t asl(struct cpu_internals *cpu, uint8_t data) {
  uint16_t result = ((uint16_t) data) << 1;
  WRITE(cpu->status_register, 0);
  SET_BIT(cpu->status_register, (((uint8_t) (result > 0xff)) << Z_FLAG));
  return (uint8_t) (result & 0x000000ff);
}

