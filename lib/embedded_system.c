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

static void zeropage_write_back(struct cpu_internals *cpu,
                                struct device_manager *device_manager,
                                uint8_t (*alter)(struct cpu_internals *, uint8_t));

static void zeropage_x_write_back(struct cpu_internals *cpu,
                                  struct device_manager *device_manager,
                                  uint8_t (*alter)(struct cpu_internals *, uint8_t));

static void absolute_write_back(struct cpu_internals *cpu,
                                struct device_manager *device_manager,
                                uint8_t (*alter)(struct cpu_internals *, uint8_t));

static void absolute_x_write_back(struct cpu_internals *cpu,
                                  struct device_manager *device_manager,
                                  uint8_t (*alter)(struct cpu_internals *, uint8_t));

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
static void asl_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void asl_zeropage_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void asl_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void asl_absolute_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager);

static void bcc_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void bcs_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void beq_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void bne_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void bmi_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void bpl_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void bvc_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void bvs_handler(struct cpu_internals *cpu, struct device_manager *device_manager);

static void bit_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void bit_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager);

static void clc_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void cld_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void cli_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void clv_handler(struct cpu_internals *cpu, struct device_manager *device_manager);

static void cmp_immediate_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void cmp_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void cmp_zeropage_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void cmp_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void cmp_absolute_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void cmp_absolute_y_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void cmp_index_indirect_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void cmp_indirect_index_handler(struct cpu_internals *cpu, struct device_manager *device_manager);

static void cpx_immediate_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void cpx_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void cpx_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void cpy_immediate_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void cpy_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void cpy_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager);

static void dec_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void dec_zeropage_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void dec_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void dec_absolute_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager);

static void dec_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void dec_y_handler(struct cpu_internals *cpu, struct device_manager *device_manager);

static void eor_immediate_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void eor_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void eor_zeropage_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void eor_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void eor_absolute_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void eor_absolute_y_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void eor_index_indirect_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void eor_indirect_index_handler(struct cpu_internals *cpu, struct device_manager *device_manager);

static void inc_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void inc_zeropage_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void inc_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void inc_absolute_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager);

static void inc_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void inc_y_handler(struct cpu_internals *cpu, struct device_manager *device_manager);

static void jmp_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void jmp_indirect_handler(struct cpu_internals *cpu, struct device_manager *device_manager);

static void jsr_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager);

static void lda_immediate_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void lda_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void lda_zeropage_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void lda_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void lda_absolute_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void lda_absolute_y_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void lda_index_indirect_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void lda_indirect_index_handler(struct cpu_internals *cpu, struct device_manager *device_manager);

static void ldx_immediate_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void ldx_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void ldx_zeropage_y_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void ldx_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager);
static void ldx_absolute_y_handler(struct cpu_internals *cpu, struct device_manager *device_manager);

static void branch_handler(struct cpu_internals *cpu, struct device_manager *device_manager);

static uint16_t add_address(uint8_t data, uint8_t acc);
static uint8_t adc(struct cpu_internals *cpu, uint8_t data, uint8_t acc);
static uint8_t and(struct cpu_internals *cpu, uint8_t data, uint8_t acc);
static uint8_t eor(struct cpu_internals *cpu, uint8_t data, uint8_t acc);
static uint8_t cmp(struct cpu_internals *cpu, uint8_t data, uint8_t acc);
static uint8_t identity(struct cpu_internals *cpu, uint8_t data, uint8_t acc);
static uint8_t asl(struct cpu_internals *cpu, uint8_t data);
static uint8_t dec(struct cpu_internals *cpu, uint8_t data);
static uint8_t inc(struct cpu_internals *cpu, uint8_t data);
static void prepare_fetch(struct cpu_internals *cpu, struct device_manager *device_manager);

// This will be used instead of a switch case which was slower
// I know I'm wasting some memory.
typedef void
(*generic_handler)(struct cpu_internals *cpu, struct device_manager *device_manager);
static generic_handler opcode_handlers[256] = {
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
    [ASL_zeropage] = asl_zeropage_handler,
    [ASL_zeropage_X] = asl_zeropage_x_handler,
    [ASL_absolute] = asl_absolute_handler,
    [ASL_absolute_X] = asl_absolute_x_handler,

    [BCC] = bcc_handler,
    [BCS] = bcs_handler,
    [BEQ] = beq_handler,
    [BNE] = bne_handler,
    [BMI] = bmi_handler,
    [BPL] = bpl_handler,
    [BVC] = bvc_handler,
    [BVS] = bvs_handler,

    [BIT_zeropage] = bit_zeropage_handler,
    [BIT_absolute] = bit_absolute_handler,

    [CLC] = clc_handler,
    [CLD] = cld_handler,
    [CLI] = cli_handler,
    [CLV] = clv_handler,

    [CMP_immediate] = cmp_immediate_handler,
    [CMP_zeropage] = cmp_zeropage_handler,
    [CMP_zeropage_X] = cmp_zeropage_x_handler,
    [CMP_absolute] = cmp_absolute_handler,
    [CMP_absolute_X] = cmp_absolute_x_handler,
    [CMP_absolute_Y] = cmp_absolute_y_handler,
    [CMP_index_indirect] = cmp_index_indirect_handler,
    [CMP_indirect_index] = cmp_indirect_index_handler,

    [CPX_immediate] = cpx_immediate_handler,
    [CPX_zeropage] = cpx_zeropage_handler,
    [CPX_absolute] = cpx_absolute_handler,

    [CPY_immediate] = cpy_immediate_handler,
    [CPY_zeropage] = cpy_zeropage_handler,
    [CPY_absolute] = cpy_absolute_handler,

    [DEC_zeropage] = dec_zeropage_handler,
    [DEC_zeropage_X] = dec_zeropage_x_handler,
    [DEC_absolute] = dec_absolute_handler,
    [DEC_absolute_X] = dec_absolute_x_handler,

    [DEX_implied] = dec_x_handler,
    [DEY_implied] = dec_y_handler,

    [EOR_immediate] = eor_immediate_handler,
    [EOR_zeropage] = eor_zeropage_handler,
    [EOR_zeropage_X] = eor_zeropage_x_handler,
    [EOR_absolute] = eor_absolute_handler,
    [EOR_absolute_X] = eor_absolute_x_handler,
    [EOR_absolute_Y] = eor_absolute_y_handler,
    [EOR_index_indirect] = eor_index_indirect_handler,
    [EOR_indirect_index] = eor_indirect_index_handler,

    [INC_zeropage] = inc_zeropage_handler,
    [INC_zeropage_X] = inc_zeropage_x_handler,
    [INC_absolute] = inc_absolute_handler,
    [INC_absolute_X] = inc_absolute_x_handler,

    [INX_implied] = inc_x_handler,
    [INY_implied] = inc_y_handler,

    [JMP_absolute] = jmp_absolute_handler,
    [JMP_indirect] = jmp_indirect_handler,

    [JSR_absolute] = jsr_absolute_handler,

    [LDA_immediate] = lda_immediate_handler,
    [LDA_zeropage] = lda_zeropage_handler,
    [LDA_zeropage_X] = lda_zeropage_x_handler,
    [LDA_absolute] = lda_absolute_handler,
    [LDA_absolute_X] = lda_absolute_x_handler,
    [LDA_absolute_Y] = lda_absolute_y_handler,
    [LDA_index_indirect] = lda_index_indirect_handler,
    [LDA_indirect_index] = lda_indirect_index_handler,

    [LDX_immediate] = ldx_immediate_handler,
    [LDX_zeropage] = ldx_zeropage_handler,
    [LDX_zeropage_Y] = ldx_zeropage_y_handler,
    [LDX_absolute] = ldx_absolute_handler,
    [LDX_absolute_Y] = ldx_absolute_y_handler,

    [HALT_CODE] = halt,
};

static generic_handler cpu_state_handlers[5] = {
    [RESET] = reset,
    [FETCH] = fetch,
    [EXECUTE] = execute,
    [HALT] = halt
};

struct cpu_internals new_cpu(void) {
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
  return cpu;
}

void tick(struct cpu_internals cpu[1], struct device_manager device_manager[1]) {
  //Clock cycle
  cpu->accumulator.output = cpu->accumulator.input;
  cpu->x_register.output = cpu->x_register.input;
  cpu->y_register.output = cpu->y_register.input;
  cpu->status_register.output = cpu->status_register.input;
  cpu->stack_pointer.output = cpu->stack_pointer.input;
  cpu->instruction_register.output = cpu->instruction_register.input;
  cpu->program_counter.output = cpu->program_counter.input;
  cpu->address_register.output = cpu->address_register.input;
  cpu->data_register.output = cpu->data_register.input;
  cpu->temp_register.output = cpu->temp_register.input;

  generic_handler state_handler = cpu_state_handlers[cpu->state];
  state_handler(cpu, device_manager);
}

void execute(struct cpu_internals *cpu, struct device_manager *device_manager) {
  generic_handler instruction = opcode_handlers[READ(cpu->instruction_register)];
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
      INCREMENT(cpu->stack_pointer);
      return;
    }
    default: fprintf(stderr, "Wrong!!!!");
  }

}

void adc_immediate_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  immediate_read(cpu, device_manager, adc);
}

void adc_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  zeropage_read(cpu, device_manager, adc);
}

void adc_zeropage_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  zeropage_x_read(cpu, device_manager, adc);
}

void adc_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_read(cpu, device_manager, adc);
}

void adc_absolute_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_x_read(cpu, device_manager, adc);
}

void adc_absolute_y_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_y_read(cpu, device_manager, adc);
}

void adc_index_indirect_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  index_indirect_read(cpu, device_manager, adc);
}

void adc_indirect_index_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  indirect_index_read(cpu, device_manager, adc);
}

void and_immediate_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  immediate_read(cpu, device_manager, and);
}

void and_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  zeropage_read(cpu, device_manager, and);
}

void and_zeropage_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  zeropage_x_read(cpu, device_manager, and);
}

void and_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_read(cpu, device_manager, and);
}

void and_absolute_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_x_read(cpu, device_manager, and);
}

void and_absolute_y_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_y_read(cpu, device_manager, and);
}

void and_index_indirect_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  index_indirect_read(cpu, device_manager, and);
}

void and_indirect_index_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  indirect_index_read(cpu, device_manager, and);
}

void asl_accumulator_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  uint8_t result = asl(cpu, READ(cpu->accumulator));
  WRITE(cpu->accumulator, result);
  prepare_fetch(cpu, device_manager);
}

void asl_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  zeropage_write_back(cpu, device_manager, asl);
}

void asl_zeropage_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  zeropage_x_write_back(cpu, device_manager, asl);
}

void asl_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_write_back(cpu, device_manager, asl);
}

void asl_absolute_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_x_write_back(cpu, device_manager, asl);
}

void bcc_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  // This is an attempt to reduce branching in the doe to make it faster.
  // lower bit is if data is positive or negative
  // higher bit is if the branch condition is met
  // But the question remains, is this really faster?
  static enum micro_state branches[] = {
      [false] = S0,
      [true] = S3
  };
  if (cpu->micro_step == S0) {
    bool condition = (READ(cpu->status_register) & C_MASK_SET) == 0;
    if (!condition) {
      prepare_fetch(cpu, device_manager);
      return;
    }
    uint8_t data = read_device(device_manager, READ(cpu->address_register));
    bool isN = (data & N_MASK_SET) > 0;
    cpu->micro_step = branches[isN];
  }
  branch_handler(cpu, device_manager);
}

void bcs_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  static enum micro_state branches[] = {
      [false] = S0,
      [true] = S3
  };
  if (cpu->micro_step == S0) {
    bool condition = (READ(cpu->status_register) & C_MASK_SET) > 0;
    if (!condition) {
      prepare_fetch(cpu, device_manager);
      return;
    }
    uint8_t data = read_device(device_manager, READ(cpu->address_register));
    bool isN = (data & N_MASK_SET) > 0;
    cpu->micro_step = branches[isN];
  }
  branch_handler(cpu, device_manager);
}

void beq_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  static enum micro_state branches[] = {
      [false] = S0,
      [true] = S3
  };
  if (cpu->micro_step == S0) {
    bool condition = (READ(cpu->status_register) & Z_MASK_SET) > 0;
    if (!condition) {
      prepare_fetch(cpu, device_manager);
      return;
    }
    uint8_t data = read_device(device_manager, READ(cpu->address_register));
    bool isN = (data & N_MASK_SET) > 0;
    cpu->micro_step = branches[isN];
  }
  branch_handler(cpu, device_manager);
}

void bne_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  static enum micro_state branches[] = {
      [false] = S0,
      [true] = S3
  };
  if (cpu->micro_step == S0) {
    bool condition = (READ(cpu->status_register) & Z_MASK_SET) == 0;
    if (!condition) {
      prepare_fetch(cpu, device_manager);
      return;
    }
    uint8_t data = read_device(device_manager, READ(cpu->address_register));
    bool isN = (data & N_MASK_SET) > 0;
    cpu->micro_step = branches[isN];
  }
  branch_handler(cpu, device_manager);
}

void bmi_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  static enum micro_state branches[] = {
      [false] = S0,
      [true] = S3
  };
  if (cpu->micro_step == S0) {
    bool condition = (READ(cpu->status_register) & N_MASK_SET) > 0;
    if (!condition) {
      prepare_fetch(cpu, device_manager);
      return;
    }
    uint8_t data = read_device(device_manager, READ(cpu->address_register));
    bool isN = (data & N_MASK_SET) > 0;
    cpu->micro_step = branches[isN];
  }
  branch_handler(cpu, device_manager);
}

void bpl_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  static enum micro_state branches[] = {
      [false] = S0,
      [true] = S3
  };
  if (cpu->micro_step == S0) {
    bool condition = (READ(cpu->status_register) & N_MASK_SET) == 0;
    if (!condition) {
      prepare_fetch(cpu, device_manager);
      return;
    }
    uint8_t data = read_device(device_manager, READ(cpu->address_register));
    bool isN = (data & N_MASK_SET) > 0;
    cpu->micro_step = branches[isN];
  }
  branch_handler(cpu, device_manager);
}

void bvc_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  static enum micro_state branches[] = {
      [false] = S0,
      [true] = S3
  };
  if (cpu->micro_step == S0) {
    bool condition = (READ(cpu->status_register) & V_MASK_SET) == 0;
    if (!condition) {
      prepare_fetch(cpu, device_manager);
      return;
    }
    uint8_t data = read_device(device_manager, READ(cpu->address_register));
    bool isN = (data & N_MASK_SET) > 0;
    cpu->micro_step = branches[isN];
  }
  branch_handler(cpu, device_manager);
}

void bvs_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  static enum micro_state branches[] = {
      [false] = S0,
      [true] = S3
  };
  if (cpu->micro_step == S0) {
    bool condition = (READ(cpu->status_register) & N_MASK_SET) > 0;
    if (!condition) {
      prepare_fetch(cpu, device_manager);
      return;
    }
    uint8_t data = read_device(device_manager, READ(cpu->address_register));
    bool isN = (data & N_MASK_SET) > 0;
    cpu->micro_step = branches[isN];
  }
  branch_handler(cpu, device_manager);
}

void bit_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
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
      uint8_t result = and(cpu, READ(cpu->accumulator), data);
      SET_OR_CLEAR_BIT(cpu->status_register, (result & N_MASK_SET) > 0, N_FLAG);
      SET_OR_CLEAR_BIT(cpu->status_register, (result & V_MASK_SET) > 0, V_FLAG);
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong bit_zeropage_handler step!!!!");
  }
}

void bit_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
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
      uint8_t result = and(cpu, READ(cpu->accumulator), data);
      SET_OR_CLEAR_BIT(cpu->status_register, (result & N_MASK_SET) > 0, N_FLAG);
      SET_OR_CLEAR_BIT(cpu->status_register, (result & V_MASK_SET) > 0, V_FLAG);
      prepare_fetch(cpu, device_manager);
    }
    default: fprintf(stderr, "Wrong bit_absolute_handler step!!!!");
  }
}

void clc_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  CLEAR_BIT(cpu->status_register, C_FLAG);
  prepare_fetch(cpu, device_manager);
}

void cld_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  CLEAR_BIT(cpu->status_register, D_FLAG);
  prepare_fetch(cpu, device_manager);
}

void cli_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  CLEAR_BIT(cpu->status_register, I_FLAG);
  prepare_fetch(cpu, device_manager);
}

void clv_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  CLEAR_BIT(cpu->status_register, V_FLAG);
  prepare_fetch(cpu, device_manager);
}

void cmp_immediate_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  immediate_read(cpu, device_manager, cmp);
}

void cmp_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  zeropage_read(cpu, device_manager, cmp);
}

void cmp_zeropage_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  zeropage_x_read(cpu, device_manager, cmp);
}

void cmp_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_read(cpu, device_manager, cmp);
}

void cmp_absolute_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_x_read(cpu, device_manager, cmp);
}

void cmp_absolute_y_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_y_read(cpu, device_manager, cmp);
}

void cmp_index_indirect_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  index_indirect_read(cpu, device_manager, cmp);
}

void cmp_indirect_index_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  indirect_index_read(cpu, device_manager, and);
}

void cpx_immediate_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  uint8_t data = read_device(device_manager, READ(cpu->address_register));
  cmp(cpu, data, READ(cpu->x_register));
  prepare_fetch(cpu, device_manager);
}

void cpx_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
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
      cmp(cpu, data, READ(cpu->x_register));
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong cpx_zeropage_handler step!!!!");
  }
}

void cpx_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
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
      cmp(cpu, data, READ(cpu->x_register));
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong cpx_absolute_handler step!!!!");
  }
}

void cpy_immediate_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  uint8_t data = read_device(device_manager, READ(cpu->address_register));
  cmp(cpu, data, READ(cpu->x_register));
  prepare_fetch(cpu, device_manager);
}

void cpy_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
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
      cmp(cpu, data, READ(cpu->x_register));
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong cpy_zeropage_handler step!!!!");
  }
}

void cpy_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
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
      cmp(cpu, data, READ(cpu->x_register));
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong cpy_absolute_handler step!!!!");
  }
}

void dec_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  zeropage_write_back(cpu, device_manager, dec);
}

void dec_zeropage_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  zeropage_x_write_back(cpu, device_manager, dec);
}

void dec_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_write_back(cpu, device_manager, dec);
}

void dec_absolute_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_write_back(cpu, device_manager, dec);
}

void dec_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  DECREMENT(cpu->x_register);
  prepare_fetch(cpu, device_manager);
}

void dec_y_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  DECREMENT(cpu->y_register);
  prepare_fetch(cpu, device_manager);
}

void eor_immediate_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  immediate_read(cpu, device_manager, eor);
}

void eor_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  zeropage_read(cpu, device_manager, eor);
}

void eor_zeropage_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  zeropage_x_read(cpu, device_manager, eor);
}

void eor_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_read(cpu, device_manager, eor);
}

void eor_absolute_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_x_read(cpu, device_manager, eor);
}

void eor_absolute_y_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_y_read(cpu, device_manager, eor);
}

void eor_index_indirect_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  index_indirect_read(cpu, device_manager, eor);
}

void eor_indirect_index_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  indirect_index_read(cpu, device_manager, eor);
}

void inc_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  zeropage_write_back(cpu, device_manager, inc);
}

void inc_zeropage_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  zeropage_x_write_back(cpu, device_manager, inc);
}

void inc_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_write_back(cpu, device_manager, inc);
}

void inc_absolute_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_write_back(cpu, device_manager, inc);
}

void inc_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  INCREMENT(cpu->x_register);
  prepare_fetch(cpu, device_manager);
}

void inc_y_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  INCREMENT(cpu->y_register);
  prepare_fetch(cpu, device_manager);
}

void jmp_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
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
      SET_LOW(cpu->program_counter, READ(cpu->temp_register));
      SET_HIGH(cpu->program_counter, data);
      SET_LOW(cpu->address_register, READ(cpu->temp_register));
      SET_HIGH(cpu->address_register, data);
      cpu->state = FETCH;
      return;
    }
    default: fprintf(stderr, "Wrong jmp_absolute step!!!!");
  }
}

void jmp_indirect_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
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
      SET_LOW(cpu->program_counter, READ(cpu->temp_register));
      SET_HIGH(cpu->program_counter, data);
      SET_LOW(cpu->address_register, READ(cpu->temp_register));
      SET_HIGH(cpu->address_register, data);
      cpu->micro_step = S2;
      return;
    }
    case S2: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      WRITE(cpu->temp_register, data);
      INCREMENT(cpu->program_counter);
      WRITE(cpu->address_register, READ(cpu->program_counter));
      cpu->micro_step = S3;
      return;
    }
    case S3: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->program_counter, READ(cpu->temp_register));
      SET_HIGH(cpu->program_counter, data);
      SET_LOW(cpu->address_register, READ(cpu->temp_register));
      SET_HIGH(cpu->address_register, data);
      cpu->state = FETCH;
      return;
    }
    default: fprintf(stderr, "Wrong jmp_absolute step!!!!");
  }
}

void jsr_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  switch (cpu->micro_step) {
    case S0: {
      cpu->micro_step = S1;
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      WRITE(cpu->temp_register, data);
      INCREMENT(cpu->program_counter);
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
      write_device(device_manager, READ(cpu->address_register), GET_LOW(cpu->program_counter));
      SET_HIGH(cpu->address_register, 0x01);
      SET_LOW(cpu->address_register, READ(cpu->stack_pointer));
      DECREMENT(cpu->stack_pointer);
      return;
    }
    case S3: {
      cpu->micro_step = S4;
      write_device(device_manager, READ(cpu->address_register), GET_HIGH(cpu->program_counter));
      WRITE(cpu->address_register, READ(cpu->program_counter));
      return;
    }
    case S4: {
      cpu->state = FETCH;
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->address_register, READ(cpu->temp_register));
      SET_HIGH(cpu->address_register, data);
      SET_LOW(cpu->program_counter, READ(cpu->temp_register));
      SET_HIGH(cpu->program_counter, data);
      return;
    }
    default: fprintf(stderr, "Wrong!!!!");
  }

}

void lda_immediate_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  immediate_read(cpu, device_manager, identity);
}

void lda_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  zeropage_read(cpu, device_manager, identity);
}

void lda_zeropage_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  zeropage_x_read(cpu, device_manager, identity);
}

void lda_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_read(cpu, device_manager, identity);
}

void lda_absolute_x_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_x_read(cpu, device_manager, identity);
}

void lda_absolute_y_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  absolute_y_read(cpu, device_manager, identity);
}

void lda_index_indirect_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  index_indirect_read(cpu, device_manager, identity);
}

void lda_indirect_index_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  indirect_index_read(cpu, device_manager, identity);
}

void ldx_immediate_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  uint8_t data = read_device(device_manager, READ(cpu->address_register));
  uint8_t result = identity(cpu, data, 0);
  WRITE(cpu->x_register, result);
  prepare_fetch(cpu, device_manager);
}

void ldx_zeropage_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
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
      uint8_t result = identity(cpu, data, 0);
      WRITE(cpu->x_register, result);
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong zeropage step!!!!");
  }
}

void ldx_zeropage_y_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  switch (cpu->micro_step) {
    case S0: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->address_register, data);
      SET_HIGH(cpu->address_register, 0x00);
      cpu->micro_step = S1;
      return;
    }
    case S1: {
      uint16_t result = add_address(READ(cpu->y_register), READ(cpu->address_register));
      SET_LOW(cpu->address_register, (result & 0x000000ff));
      cpu->micro_step = S2;
    }
    case S2: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      uint8_t result = identity(cpu, data, 0);
      WRITE(cpu->x_register, result);
      prepare_fetch(cpu, device_manager);
    }
    default: fprintf(stderr, "Wrong zeropage X step!!!!");
  }
}

void ldx_absolute_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
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
      uint8_t result = identity(cpu, data, 0);
      WRITE(cpu->x_register, result);
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong absolute step!!!!");
  }
}

void ldx_absolute_y_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
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
      cpu->micro_step = (result & CARRY_MASK_U16) > 0 ? S2 : S3;
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
      uint8_t result = identity(cpu, data, 0);
      WRITE(cpu->x_register, result);
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong absolute x step!!!!");
  }
}

void branch_handler(struct cpu_internals *cpu, struct device_manager *device_manager) {
  switch (cpu->micro_step) {
    case S0: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      uint16_t result = add_address(GET_LOW(cpu->program_counter), data);
      SET_LOW(cpu->program_counter, (result & 0x000000ff));
      cpu->micro_step = (result & CARRY_MASK_U16) > 0 ? S1 : S2;
      return;
    }
    case S1: {
      uint16_t result = add_address(GET_HIGH(cpu->program_counter), 1);
      SET_HIGH(cpu->program_counter, (result & 0x000000ff));
      cpu->micro_step = S2;
      return;
    }
    case S2: {
      WRITE(cpu->address_register, READ(cpu->program_counter));
      cpu->state = FETCH;
      return;
    }
      // Starts negative case
    case S3: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      uint16_t result = add_address(GET_LOW(cpu->program_counter), data);
      SET_LOW(cpu->program_counter, (result & 0x000000ff));
      cpu->micro_step = (result & CARRY_MASK_U16) > 0 ? S4 : S5;
      return;
    }
    case S4: {
      uint16_t result = add_address(GET_HIGH(cpu->program_counter), 0xFF); // This represents -1 in 8 bit
      SET_HIGH(cpu->program_counter, (result & 0x000000ff));
      cpu->micro_step = S5;
      return;
    }
    case S5: {
      WRITE(cpu->address_register, READ(cpu->program_counter));
      cpu->state = FETCH;
      return;
    }

    default: fprintf(stderr, "Wrong branch_handler step!!!!");
  }
}

static void halt(struct cpu_internals *cpu, struct device_manager *device_manager) {
  cpu->state = HALT;
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
  prepare_fetch(cpu, device_manager);
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
      prepare_fetch(cpu, device_manager);
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
      prepare_fetch(cpu, device_manager);
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
      prepare_fetch(cpu, device_manager);
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
      cpu->micro_step = (result & CARRY_MASK_U16) > 0 ? S2 : S3;
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
      prepare_fetch(cpu, device_manager);
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
      cpu->micro_step = (result & CARRY_MASK_U16) > 0 ? S2 : S3;
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
      prepare_fetch(cpu, device_manager);
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
      prepare_fetch(cpu, device_manager);
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
      cpu->micro_step = (result & CARRY_MASK_U16) > 0 ? S3 : S4;
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
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong indirect index step!!!!");
  }
}

void zeropage_write_back(struct cpu_internals *cpu,
                         struct device_manager *device_manager,
                         uint8_t (*alter)(struct cpu_internals *, uint8_t)) {
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
      uint8_t result = alter(cpu, data);
      WRITE(cpu->data_register, result);
      cpu->micro_step = S2;
      return;
    }
    case S2: {
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      cpu->micro_step = S3;
      return;
    }
    case S3: {
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong asl_zeropage_handler step!!!!");
  }
}

void zeropage_x_write_back(struct cpu_internals *cpu,
                           struct device_manager *device_manager,
                           uint8_t (*alter)(struct cpu_internals *, uint8_t)) {
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
      return;
    }
    case S2: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      uint8_t result = alter(cpu, data);
      WRITE(cpu->data_register, result);
      cpu->micro_step = S3;
      return;
    }
    case S3: {
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      cpu->micro_step = S4;
      return;
    }
    case S4: {
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong asl_zeropage_x_handler step!!!!");
  }
}

void absolute_write_back(struct cpu_internals *cpu,
                         struct device_manager *device_manager,
                         uint8_t (*alter)(struct cpu_internals *, uint8_t)) {
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
      uint8_t result = alter(cpu, data);
      WRITE(cpu->data_register, result);
      cpu->micro_step = S3;
      return;
    }
    case S3: {
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      cpu->micro_step = S4;
      return;
    }
    case S4: {
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong asl_absolute_handler step!!!!");
  }
}

void absolute_x_write_back(struct cpu_internals *cpu,
                           struct device_manager *device_manager,
                           uint8_t (*alter)(struct cpu_internals *, uint8_t)) {
  switch (cpu->micro_step) {
    case S0: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      WRITE(cpu->temp_register, data);
      WRITE(cpu->address_register, READ(cpu->program_counter));
      cpu->micro_step = S1;
      INCREMENT(cpu->program_counter);
      return;
    }
    case S1: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      uint16_t result = add_address(READ(cpu->temp_register), READ(cpu->x_register));
      cpu->micro_step = (result & CARRY_MASK_U16) > 0 ? S2 : S3;
      SET_LOW(cpu->address_register, result & 0xFF);
      SET_HIGH(cpu->address_register, data);
      INCREMENT(cpu->program_counter);
      return;
    }
    case S2: {
      uint16_t result = add_address(GET_HIGH(cpu->address_register), 1);
      SET_HIGH(cpu->address_register, result & 0xFF);
      cpu->micro_step = S3;
      return;
    }
    case S3: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      uint8_t result = alter(cpu, data);
      WRITE(cpu->data_register, result);
      cpu->micro_step = S4;
      return;
    }
    case S4: {
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      cpu->micro_step = S5;
      return;
    }
    case S5: {
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong asl_absolute_x_handler step!!!!");
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
  SET_OR_CLEAR_BIT(cpu->status_register, ((result & CARRY_MASK_U16) > 0), C_FLAG);
  SET_OR_CLEAR_BIT(cpu->status_register, ((result & 0xff) == 0), Z_FLAG);
  SET_OR_CLEAR_BIT(cpu->status_register, ((result & N_MASK_SET) > 0), N_FLAG);
  SET_OR_CLEAR_BIT(cpu->status_register, V, V_FLAG);
  return (uint8_t) (result & 0x000000ff);
}

uint8_t cmp(struct cpu_internals *cpu, uint8_t data, uint8_t acc) {
  uint8_t complement = (~acc) + 1;
  uint16_t result = ((uint16_t) data) + ((uint16_t) complement);
  SET_OR_CLEAR_BIT(cpu->status_register, (result & CARRY_MASK_U16) > 0, C_FLAG);
  SET_OR_CLEAR_BIT(cpu->status_register, (result & 0xff) == 0, Z_FLAG);
  SET_OR_CLEAR_BIT(cpu->status_register, (result & N_MASK_SET) > 0, N_FLAG);
  return (uint8_t) (result & 0x000000ff);
}

uint8_t and(struct cpu_internals *cpu, uint8_t data, uint8_t acc) {
  uint8_t result = data & acc;
  SET_OR_CLEAR_BIT(cpu->status_register, result == 0, Z_FLAG);
  SET_OR_CLEAR_BIT(cpu->status_register, (result & N_MASK_SET) > 0, N_FLAG);
  return result;
}

uint8_t eor(struct cpu_internals *cpu, uint8_t data, uint8_t acc) {
  uint8_t result = data ^ acc;
  SET_OR_CLEAR_BIT(cpu->status_register, result == 0, Z_FLAG);
  SET_OR_CLEAR_BIT(cpu->status_register, (result & N_MASK_SET) > 0, N_FLAG);
  return result;
}

uint8_t identity(struct cpu_internals *cpu, uint8_t data, uint8_t acc) {
  uint8_t result = data;
  SET_OR_CLEAR_BIT(cpu->status_register, result == 0, Z_FLAG);
  SET_OR_CLEAR_BIT(cpu->status_register, (result & N_MASK_SET) > 0, N_FLAG);
  return result;
}

uint8_t asl(struct cpu_internals *cpu, uint8_t data) {
  uint16_t result = ((uint16_t) data) << 1;
  SET_OR_CLEAR_BIT(cpu->status_register, (result & 0xff) == 0, Z_FLAG);
  return (uint8_t) (result & 0x000000ff);
}

uint8_t dec(struct cpu_internals *cpu, uint8_t data) {
  uint16_t result = ((uint16_t) data) - 1;
  SET_OR_CLEAR_BIT(cpu->status_register, (result & 0xff) == 0, Z_FLAG);
  SET_OR_CLEAR_BIT(cpu->status_register, (result & N_MASK_SET) > 0, N_FLAG);
  return (uint8_t) (result & 0x000000ff);
}

uint8_t inc(struct cpu_internals *cpu, uint8_t data) {
  uint16_t result = ((uint16_t) data) + 1;
  SET_OR_CLEAR_BIT(cpu->status_register, (result & 0xff) == 0, Z_FLAG);
  SET_OR_CLEAR_BIT(cpu->status_register, (result & N_MASK_SET) > 0, N_FLAG);
  return (uint8_t) (result & 0x000000ff);
}

