//
// Created by Andres Fuentes Hernandez on 8/18/24.
//

#include <stdio.h>
#include "embedded_system.h"
#include "register.h"
#include "opcodes.h"

static void reset(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void fetch(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void halt(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void execute(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

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

static void adc_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void adc_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void adc_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void adc_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void adc_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void adc_absolute_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void adc_index_indirect_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void adc_indirect_index_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void and_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void and_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void and_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void and_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void and_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void and_absolute_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void and_index_indirect_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void and_indirect_index_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void asl_accumulator_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void asl_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void asl_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void asl_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void asl_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void bcc_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void bcs_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void beq_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void bne_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void bmi_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void bpl_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void brk_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void bvc_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void bvs_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void bit_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void bit_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void clc_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void cld_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void cli_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void clv_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void cmp_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void cmp_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void cmp_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void cmp_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void cmp_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void cmp_absolute_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void cmp_index_indirect_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void cmp_indirect_index_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void cpx_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void cpx_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void cpx_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void cpy_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void cpy_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void cpy_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void dec_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void dec_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void dec_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void dec_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void dec_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void dec_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void eor_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void eor_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void eor_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void eor_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void eor_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void eor_absolute_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void eor_index_indirect_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void eor_indirect_index_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void inc_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void inc_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void inc_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void inc_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void inc_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void inc_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void jmp_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void jmp_indirect_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void jsr_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void lda_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void lda_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void lda_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void lda_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void lda_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void lda_absolute_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void lda_index_indirect_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void lda_indirect_index_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void ldx_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void ldx_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void ldx_zeropage_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void ldx_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void ldx_absolute_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void ldy_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void ldy_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void ldy_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void ldy_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void ldy_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void lsr_accumulator_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void lsr_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void lsr_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void lsr_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void lsr_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void ora_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void ora_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void ora_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void ora_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void ora_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void ora_absolute_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void ora_index_indirect_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void ora_indirect_index_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void nop_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void pha_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void php_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void pla_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void plp_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void rol_accumulator_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void rol_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void rol_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void rol_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void rol_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void ror_accumulator_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void ror_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void ror_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void ror_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void ror_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void rti_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void rts_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void sbc_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void sbc_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void sbc_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void sbc_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void sbc_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void sbc_absolute_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void sbc_index_indirect_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void sbc_indirect_index_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void sec_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void sed_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void sei_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void sta_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void sta_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void sta_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void sta_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void sta_absolute_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void sta_index_indirect_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void sta_indirect_index_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void stx_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void stx_zeropage_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void stx_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void sty_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void sty_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void sty_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void tax_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void tay_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void tsx_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void txa_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void txs_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
static void tya_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static void branch_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

static uint16_t add_address(uint8_t data, uint8_t acc);
static uint8_t adc(struct cpu_internals *cpu, uint8_t data, uint8_t acc);
static uint8_t sbc(struct cpu_internals *cpu, uint8_t data, uint8_t acc);
static uint8_t and(struct cpu_internals *cpu, uint8_t data, uint8_t acc);
static uint8_t eor(struct cpu_internals *cpu, uint8_t data, uint8_t acc);
static uint8_t ora(struct cpu_internals *cpu, uint8_t data, uint8_t acc);
static uint8_t cmp(struct cpu_internals *cpu, uint8_t data, uint8_t acc);
static uint8_t identity(struct cpu_internals *cpu, uint8_t data, uint8_t acc);
static uint8_t ror(struct cpu_internals *cpu, uint8_t data);
static uint8_t rol(struct cpu_internals *cpu, uint8_t data);
static uint8_t asl(struct cpu_internals *cpu, uint8_t data);
static uint8_t lsr(struct cpu_internals *cpu, uint8_t data);
static uint8_t dec(struct cpu_internals *cpu, uint8_t data);
static uint8_t inc(struct cpu_internals *cpu, uint8_t data);
static void prepare_fetch(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);

// This will be used instead of a switch case which was slower
// I know I'm wasting some memory.
typedef void
(*generic_handler)(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]);
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
    [BRK] = brk_handler,
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

    [LDY_immediate] = ldy_immediate_handler,
    [LDY_zeropage] = ldy_zeropage_handler,
    [LDY_zeropage_X] = ldy_zeropage_x_handler,
    [LDY_absolute] = ldy_absolute_handler,
    [LDY_absolute_X] = ldy_absolute_x_handler,

    [LSR_accumulator] = lsr_accumulator_handler,
    [LSR_zeropage] = lsr_zeropage_handler,
    [LSR_zeropage_X] = lsr_zeropage_x_handler,
    [LSR_absolute] = lsr_absolute_handler,
    [LSR_absolute_X] = lsr_absolute_x_handler,

    [ORA_immediate] = ora_immediate_handler,
    [ORA_zeropage] = ora_zeropage_handler,
    [ORA_zeropage_X] = ora_zeropage_x_handler,
    [ORA_absolute] = ora_absolute_handler,
    [ORA_absolute_X] = ora_absolute_x_handler,
    [ORA_absolute_Y] = ora_absolute_y_handler,
    [ORA_index_indirect] = ora_index_indirect_handler,
    [ORA_indirect_index] = ora_indirect_index_handler,

    [NOP] = nop_handler,

    [PHA] = pha_handler,
    [PHP] = php_handler,
    [PLA] = pla_handler,
    [PLP] = plp_handler,

    [ROL_accumulator] = rol_accumulator_handler,
    [ROL_zeropage] = rol_zeropage_handler,
    [ROL_zeropage_X] = rol_zeropage_x_handler,
    [ROL_absolute] = rol_absolute_handler,
    [ROL_absolute_X] = rol_absolute_x_handler,

    [ROR_accumulator] = ror_accumulator_handler,
    [ROR_zeropage] = ror_zeropage_handler,
    [ROR_zeropage_X] = ror_zeropage_x_handler,
    [ROR_absolute] = ror_absolute_handler,
    [ROR_absolute_X] = ror_absolute_x_handler,

    [RTI] = rti_handler,
    [RTS] = rts_handler,

    [SBC_immediate] = sbc_immediate_handler,
    [SBC_zeropage] = sbc_zeropage_handler,
    [SBC_zeropage_X] = sbc_zeropage_x_handler,
    [SBC_absolute] = sbc_absolute_handler,
    [SBC_absolute_X] = sbc_absolute_x_handler,
    [SBC_absolute_Y] = sbc_absolute_y_handler,
    [SBC_index_indirect] = sbc_index_indirect_handler,
    [SBC_indirect_index] = sbc_indirect_index_handler,

    [SEC] = sec_handler,
    [SED] = sed_handler,
    [SEI] = sei_handler,

    [STA_zeropage] = sta_zeropage_handler,
    [STA_zeropage_X] = sta_zeropage_x_handler,
    [STA_absolute] = sta_absolute_handler,
    [STA_absolute_X] = sta_absolute_x_handler,
    [STA_absolute_Y] = sta_absolute_y_handler,
    [STA_index_indirect] = sta_index_indirect_handler,
    [STA_indirect_index] = sta_indirect_index_handler,

    [STX_zeropage] = stx_zeropage_handler,
    [STX_zeropage_Y] = stx_zeropage_y_handler,
    [STX_absolute] = stx_absolute_handler,

    [STY_zeropage] = sty_zeropage_handler,
    [STY_zeropage_X] = sty_zeropage_x_handler,
    [STY_absolute] = sty_absolute_handler,

    [TAX] = tax_handler,
    [TAY] = tay_handler,
    [TSX] = tsx_handler,
    [TXA] = txa_handler,
    [TXS] = txs_handler,
    [TYA] = tya_handler,

    [STOP] = halt,
};

static char* to_str[256] = {
    [ADC_immediate] = "ADC_immediate",
    [ADC_zeropage] = "ADC_zeropage",
    [ADC_zeropage_X] = "ADC_zeropage_X",
    [ADC_absolute] = "ADC_absolute",
    [ADC_absolute_X] = "ADC_absolute_X",
    [ADC_absolute_Y] = "ADC_absolute_Y",
    [ADC_index_indirect] = "ADC_index_indirect",
    [ADC_indirect_index] = "ADC_indirect_index",

    [AND_immediate] = "AND_immediate",
    [AND_zeropage] = "AND_zeropage",
    [AND_zeropage_X] = "AND_zeropage_X",
    [AND_absolute] = "AND_absolute",
    [AND_absolute_X] = "AND_absolute_X",
    [AND_absolute_Y] = "AND_absolute_Y",
    [AND_index_indirect] = "AND_index_indirect",
    [AND_indirect_index] = "AND_indirect_index",

    [ASL_accumulator] = "ASL_accumulator",
    [ASL_zeropage] = "ASL_zeropage",
    [ASL_zeropage_X] = "ASL_zeropage_X",
    [ASL_absolute] = "ASL_absolute",
    [ASL_absolute_X] = "ASL_absolute_X",

    [BCC] = "BCC",
    [BCS] = "BCS",
    [BEQ] = "BEQ",
    [BNE] = "BNE",
    [BMI] = "BMI",
    [BPL] = "BPL",
    [BRK] = "BRK",
    [BVC] = "BVC",
    [BVS] = "BVS",

    [BIT_zeropage] = "BIT_zeropage",
    [BIT_absolute] = "BIT_absolute",

    [CLC] = "CLC",
    [CLD] = "CLD",
    [CLI] = "CLI",
    [CLV] = "CLV",

    [CMP_immediate] = "CMP_immediate",
    [CMP_zeropage] = "CMP_zeropage",
    [CMP_zeropage_X] = "CMP_zeropage_X",
    [CMP_absolute] = "CMP_absolute",
    [CMP_absolute_X] = "CMP_absolute_X",
    [CMP_absolute_Y] = "CMP_absolute_Y",
    [CMP_index_indirect] = "CMP_index_indirect",
    [CMP_indirect_index] = "CMP_indirect_index",

    [CPX_immediate] = "CPX_immediate",
    [CPX_zeropage] = "CPX_zeropage",
    [CPX_absolute] = "CPX_absolute",

    [CPY_immediate] = "CPY_immediate",
    [CPY_zeropage] = "CPY_zeropage",
    [CPY_absolute] = "CPY_absolute",

    [DEC_zeropage] = "DEC_zeropage",
    [DEC_zeropage_X] = "DEC_zeropage_X",
    [DEC_absolute] = "DEC_absolute",
    [DEC_absolute_X] = "DEC_absolute_X",

    [DEX_implied] = "DEX_implied",
    [DEY_implied] = "DEY_implied",

    [EOR_immediate] = "EOR_immediate",
    [EOR_zeropage] = "EOR_zeropage",
    [EOR_zeropage_X] = "EOR_zeropage_X",
    [EOR_absolute] = "EOR_absolute",
    [EOR_absolute_X] = "EOR_absolute_X",
    [EOR_absolute_Y] = "EOR_absolute_Y",
    [EOR_index_indirect] = "EOR_index_indirect",
    [EOR_indirect_index] = "EOR_indirect_index",

    [INC_zeropage] = "INC_zeropage",
    [INC_zeropage_X] = "INC_zeropage_X",
    [INC_absolute] = "INC_absolute",
    [INC_absolute_X] = "INC_absolute_X",

    [INX_implied] = "INX_implied",
    [INY_implied] = "INY_implied",

    [JMP_absolute] = "JMP_absolute",
    [JMP_indirect] = "JMP_indirect",

    [JSR_absolute] = "JSR_absolute",

    [LDA_immediate] = "LDA_immediate",
    [LDA_zeropage] = "LDA_zeropage",
    [LDA_zeropage_X] = "LDA_zeropage_X",
    [LDA_absolute] = "LDA_absolute",
    [LDA_absolute_X] = "LDA_absolute_X",
    [LDA_absolute_Y] = "LDA_absolute_Y",
    [LDA_index_indirect] = "LDA_index_indirect",
    [LDA_indirect_index] = "LDA_indirect_index",

    [LDX_immediate] = "LDX_immediate",
    [LDX_zeropage] = "LDX_zeropage",
    [LDX_zeropage_Y] = "LDX_zeropage_Y",
    [LDX_absolute] = "LDX_absolute",
    [LDX_absolute_Y] = "LDX_absolute_Y",

    [LDY_immediate] = "LDY_immediate",
    [LDY_zeropage] = "LDY_zeropage",
    [LDY_zeropage_X] = "LDY_zeropage_X",
    [LDY_absolute] = "LDY_absolute",
    [LDY_absolute_X] = "LDY_absolute_X",

    [LSR_accumulator] = "LSR_accumulator",
    [LSR_zeropage] = "LSR_zeropage",
    [LSR_zeropage_X] = "LSR_zeropage_X",
    [LSR_absolute] = "LSR_absolute",
    [LSR_absolute_X] = "LSR_absolute_X",

    [ORA_immediate] = "ORA_immediate",
    [ORA_zeropage] = "ORA_zeropage",
    [ORA_zeropage_X] = "ORA_zeropage_X",
    [ORA_absolute] = "ORA_absolute",
    [ORA_absolute_X] = "ORA_absolute_X",
    [ORA_absolute_Y] = "ORA_absolute_Y",
    [ORA_index_indirect] = "ORA_index_indirect",
    [ORA_indirect_index] = "ORA_indirect_index",

    [NOP] = "NOP",

    [PHA] = "PHA",
    [PHP] = "PHP",
    [PLA] = "PLA",
    [PLP] = "PLP",

    [ROL_accumulator] = "ROL_accumulator",
    [ROL_zeropage] = "ROL_zeropage",
    [ROL_zeropage_X] = "ROL_zeropage_X",
    [ROL_absolute] = "ROL_absolute",
    [ROL_absolute_X] = "ROL_absolute_X",

    [ROR_accumulator] = "ROR_accumulator",
    [ROR_zeropage] = "ROR_zeropage",
    [ROR_zeropage_X] = "ROR_zeropage_X",
    [ROR_absolute] = "ROR_absolute",
    [ROR_absolute_X] = "ROR_absolute_X",

    [RTS] = "RTS",
    [RTI] = "RTI",

    [SBC_immediate] = "SBC_immediate",
    [SBC_zeropage] = "SBC_zeropage",
    [SBC_zeropage_X] = "SBC_zeropage_X",
    [SBC_absolute] = "SBC_absolute",
    [SBC_absolute_X] = "SBC_absolute_X",
    [SBC_absolute_Y] = "SBC_absolute_Y",
    [SBC_index_indirect] = "SBC_index_indirect",
    [SBC_indirect_index] = "SBC_indirect_index",

    [SEC] = "SEC",
    [SED] = "SED",
    [SEI] = "SEI",

    [STA_zeropage] = "STA_zeropage",
    [STA_zeropage_X] = "STA_zeropage_X",
    [STA_absolute] = "STA_absolute",
    [STA_absolute_X] = "STA_absolute_X",
    [STA_absolute_Y] = "STA_absolute_Y",
    [STA_index_indirect] = "STA_index_indirect",
    [STA_indirect_index] = "STA_indirect_index",

    [STX_zeropage] = "STX_zeropage",
    [STX_zeropage_Y] = "STX_zeropage_Y",
    [STX_absolute] = "STX_absolute",

    [STY_zeropage] = "STY_zeropage",
    [STY_zeropage_X] = "STY_zeropage_X",
    [STY_absolute] = "STY_absolute",

    [TAX] = "TAX",
    [TAY] = "TAY",
    [TSX] = "TSX",
    [TXA] = "TXA",
    [TXS] = "TXS",
    [TYA] = "TYA",

    [STOP] = "STOP",
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

void tick(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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


  LOG_SEPARATOR;
  LOG("[STATE]\tState: %d\tMicro Step: %d\n", cpu->state, cpu->micro_step);
  LOG("[REGISTERS]\t"
                   "  A:0x%02X\t"
                   "  X:0x%02X\t"
                   "  Y:0x%02X\t"
                   "  Status:0x%02X\t"
                   "  SP:0x%02X\t"
                   "  IR:%s\t"
                   "  PC:0x%04X\t"
                   "  Add:0x%04X\t"
                   "  DR:0x%02X\t"
                   "  Temp:0x%02X\n" RESET_COLOR,
      cpu->accumulator.output, cpu->x_register.output, cpu->y_register.output,
      cpu->status_register.output, cpu->stack_pointer.output, to_str[cpu->instruction_register.output],
      cpu->program_counter.output, cpu->address_register.output, cpu->data_register.output,
      cpu->temp_register.output);

  generic_handler state_handler = cpu_state_handlers[cpu->state];
  state_handler(cpu, device_manager);
}

void execute(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  generic_handler instruction = opcode_handlers[READ(cpu->instruction_register)];
  if(instruction == NULL){
    fprintf(stderr, "Not implemented\n");
    return;
  }
  instruction(cpu, device_manager);
}

void fetch(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  cpu->state = EXECUTE;
  cpu->micro_step = S0;
  uint8_t data =  read_device(device_manager, READ(cpu->address_register));
  WRITE(cpu->instruction_register, data);
  WRITE(cpu->address_register, READ(cpu->program_counter));
  INCREMENT(cpu->program_counter);
}

void reset(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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
      WRITE(cpu->stack_pointer, 0xFF); // Just because
      return;
    }
    default: fprintf(stderr, "Wrong!!!!");
  }

}

void adc_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  immediate_read(cpu, device_manager, adc);
}

void adc_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_read(cpu, device_manager, adc);
}

void adc_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_x_read(cpu, device_manager, adc);
}

void adc_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_read(cpu, device_manager, adc);
}

void adc_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_x_read(cpu, device_manager, adc);
}

void adc_absolute_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_y_read(cpu, device_manager, adc);
}

void adc_index_indirect_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  index_indirect_read(cpu, device_manager, adc);
}

void adc_indirect_index_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  indirect_index_read(cpu, device_manager, adc);
}

void and_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  immediate_read(cpu, device_manager, and);
}

void and_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_read(cpu, device_manager, and);
}

void and_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_x_read(cpu, device_manager, and);
}

void and_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_read(cpu, device_manager, and);
}

void and_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_x_read(cpu, device_manager, and);
}

void and_absolute_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_y_read(cpu, device_manager, and);
}

void and_index_indirect_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  index_indirect_read(cpu, device_manager, and);
}

void and_indirect_index_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  indirect_index_read(cpu, device_manager, and);
}

void asl_accumulator_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  uint8_t result = asl(cpu, READ(cpu->accumulator));
  WRITE(cpu->accumulator, result);
  prepare_fetch(cpu, device_manager);
}

void asl_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_write_back(cpu, device_manager, asl);
}

void asl_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_x_write_back(cpu, device_manager, asl);
}

void asl_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_write_back(cpu, device_manager, asl);
}

void asl_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_x_write_back(cpu, device_manager, asl);
}

void bcc_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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

void bcs_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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

void beq_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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

void bne_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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

void bmi_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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

void bpl_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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

void brk_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  switch (cpu->micro_step) {
    case S0: {
      cpu->program_counter.output--; // Dirty trick to avoid if in fetch
      cpu->program_counter.input--; // Dirty trick to avoid if in fetch
      cpu->micro_step = S1;
      SET_HIGH(cpu->address_register, 0x01);
      SET_LOW(cpu->address_register, READ(cpu->stack_pointer));
      WRITE(cpu->data_register, GET_HIGH(cpu->program_counter));
      DECREMENT(cpu->stack_pointer);
      SET_BIT(cpu->status_register, B_FLAG);
      return;
    }
    case S1: {
      cpu->micro_step = S2;
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      WRITE(cpu->data_register, GET_LOW(cpu->program_counter));
      SET_LOW(cpu->address_register, READ(cpu->stack_pointer));
      DECREMENT(cpu->stack_pointer);
      SET_HIGH(cpu->program_counter, 0xFF);
      return;
    }
    case S2: {
      cpu->micro_step = S3;
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      DECREMENT(cpu->stack_pointer);
      uint8_t data = READ(cpu->status_register);
      data |= B_MASK_SET;
      WRITE(cpu->data_register, data);
      SET_LOW(cpu->address_register, READ(cpu->stack_pointer));
      SET_LOW(cpu->program_counter, 0xFE);
      return;
    }
    case S3: {
      cpu->micro_step = S4;
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      WRITE(cpu->address_register, READ(cpu->program_counter));
      INCREMENT(cpu->program_counter);
      return;
    }
    case S4: {
      cpu->micro_step = S5;
      WRITE(cpu->address_register, READ(cpu->program_counter));
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->program_counter, data);
      return;
    }
    case S5: {
      cpu->state = FETCH;
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->address_register, GET_LOW(cpu->program_counter));
      SET_HIGH(cpu->address_register, data);
      INCREMENT(cpu->program_counter);
      SET_HIGH(cpu->program_counter, data);
      return;
    }
    default: fprintf(stderr, "BRK Wrong!!!!");
  }
}

void bvc_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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

void bvs_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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

void bit_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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

void bit_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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

void clc_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  CLEAR_BIT(cpu->status_register, C_FLAG);
  cpu->state = FETCH;
}

void cld_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  CLEAR_BIT(cpu->status_register, D_FLAG);
  cpu->state = FETCH;
}

void cli_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  CLEAR_BIT(cpu->status_register, I_FLAG);
  cpu->state = FETCH;
}

void clv_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  CLEAR_BIT(cpu->status_register, V_FLAG);
  cpu->state = FETCH;
}

void cmp_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  immediate_read(cpu, device_manager, cmp);
}

void cmp_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_read(cpu, device_manager, cmp);
}

void cmp_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_x_read(cpu, device_manager, cmp);
}

void cmp_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_read(cpu, device_manager, cmp);
}

void cmp_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_x_read(cpu, device_manager, cmp);
}

void cmp_absolute_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_y_read(cpu, device_manager, cmp);
}

void cmp_index_indirect_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  index_indirect_read(cpu, device_manager, cmp);
}

void cmp_indirect_index_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  indirect_index_read(cpu, device_manager, and);
}

void cpx_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  uint8_t data = read_device(device_manager, READ(cpu->address_register));
  cmp(cpu, data, READ(cpu->x_register));
  prepare_fetch(cpu, device_manager);
}

void cpx_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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

void cpx_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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

void cpy_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  uint8_t data = read_device(device_manager, READ(cpu->address_register));
  cmp(cpu, data, READ(cpu->x_register));
  prepare_fetch(cpu, device_manager);
}

void cpy_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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

void cpy_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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

void dec_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_write_back(cpu, device_manager, dec);
}

void dec_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_x_write_back(cpu, device_manager, dec);
}

void dec_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_write_back(cpu, device_manager, dec);
}

void dec_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_write_back(cpu, device_manager, dec);
}

void dec_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  uint8_t  result = dec(cpu, READ(cpu->x_register));
  WRITE(cpu->x_register, result);
  cpu->state = FETCH;
}

void dec_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  uint8_t  result = dec(cpu, READ(cpu->y_register));
  WRITE(cpu->y_register, result);
  cpu->state = FETCH;
}

void eor_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  immediate_read(cpu, device_manager, eor);
}

void eor_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_read(cpu, device_manager, eor);
}

void eor_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_x_read(cpu, device_manager, eor);
}

void eor_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_read(cpu, device_manager, eor);
}

void eor_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_x_read(cpu, device_manager, eor);
}

void eor_absolute_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_y_read(cpu, device_manager, eor);
}

void eor_index_indirect_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  index_indirect_read(cpu, device_manager, eor);
}

void eor_indirect_index_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  indirect_index_read(cpu, device_manager, eor);
}

void inc_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_write_back(cpu, device_manager, inc);
}

void inc_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_x_write_back(cpu, device_manager, inc);
}

void inc_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_write_back(cpu, device_manager, inc);
}

void inc_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_write_back(cpu, device_manager, inc);
}

void inc_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  uint8_t result = inc(cpu, READ(cpu->x_register));
  WRITE(cpu->x_register, result);
  cpu->state = FETCH;
}

void inc_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  uint8_t result = inc(cpu, READ(cpu->y_register));
  WRITE(cpu->y_register, result);
  cpu->state = FETCH;
}

void jmp_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  switch (cpu->micro_step) {
    case S0: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      WRITE(cpu->temp_register, data);
      WRITE(cpu->address_register, READ(cpu->program_counter));
      cpu->micro_step = S1;
      return;
    }
    case S1: {
      cpu->state = FETCH;
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->program_counter, READ(cpu->temp_register));
      SET_HIGH(cpu->program_counter, data);
      SET_LOW(cpu->address_register, READ(cpu->temp_register));
      SET_HIGH(cpu->address_register, data);
      INCREMENT(cpu->program_counter);
      return;
    }
    default: fprintf(stderr, "Wrong jmp_absolute step!!!!");
  }
}

void jmp_indirect_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  switch (cpu->micro_step) {
    case S0: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      WRITE(cpu->temp_register, data);
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
      INCREMENT(cpu->program_counter);
      cpu->micro_step = S2;
      return;
    }
    case S2: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      WRITE(cpu->temp_register, data);
      WRITE(cpu->address_register, READ(cpu->program_counter));
      cpu->micro_step = S3;
      return;
    }
    case S3: {
      cpu->state = FETCH;
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->program_counter, READ(cpu->temp_register));
      SET_HIGH(cpu->program_counter, data);
      SET_LOW(cpu->address_register, READ(cpu->temp_register));
      SET_HIGH(cpu->address_register, data);
      INCREMENT(cpu->program_counter);
      return;
    }
    default: fprintf(stderr, "Wrong jmp_absolute step!!!!");
  }
}

void jsr_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  switch (cpu->micro_step) {
    case S0: {
      cpu->micro_step = S1;
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      WRITE(cpu->temp_register, data);
      return;
    }
    case S1: {
      cpu->micro_step = S2;
      SET_HIGH(cpu->address_register, 0x01);
      SET_LOW(cpu->address_register, READ(cpu->stack_pointer));
      DECREMENT(cpu->stack_pointer);
      WRITE(cpu->data_register, GET_LOW(cpu->program_counter));
      return;
    }
    case S2: {
      cpu->micro_step = S3;
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      SET_HIGH(cpu->address_register, 0x01);
      SET_LOW(cpu->address_register, READ(cpu->stack_pointer));
      DECREMENT(cpu->stack_pointer);
      WRITE(cpu->data_register, GET_HIGH(cpu->program_counter));
      return;
    }
    case S3: {
      cpu->micro_step = S4;
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
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
      INCREMENT(cpu->program_counter);
      return;
    }
    default: fprintf(stderr, "Wrong!!!!");
  }

}

void lda_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  immediate_read(cpu, device_manager, identity);
}

void lda_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_read(cpu, device_manager, identity);
}

void lda_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_x_read(cpu, device_manager, identity);
}

void lda_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_read(cpu, device_manager, identity);
}

void lda_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_x_read(cpu, device_manager, identity);
}

void lda_absolute_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_y_read(cpu, device_manager, identity);
}

void lda_index_indirect_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  index_indirect_read(cpu, device_manager, identity);
}

void lda_indirect_index_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  indirect_index_read(cpu, device_manager, identity);
}

void ldx_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  uint8_t data = read_device(device_manager, READ(cpu->address_register));
  uint8_t result = identity(cpu, data, 0);
  WRITE(cpu->x_register, result);
  prepare_fetch(cpu, device_manager);
}

void ldx_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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

void ldx_zeropage_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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

void ldx_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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

void ldx_absolute_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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

void ldy_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  uint8_t data = read_device(device_manager, READ(cpu->address_register));
  uint8_t result = identity(cpu, data, 0);
  WRITE(cpu->y_register, result);
  prepare_fetch(cpu, device_manager);
}

void ldy_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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
      WRITE(cpu->y_register, result);
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong zeropage step!!!!");
  }
}

void ldy_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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
      uint8_t result = identity(cpu, data, 0);
      WRITE(cpu->y_register, result);
      prepare_fetch(cpu, device_manager);
    }
    default: fprintf(stderr, "Wrong zeropage X step!!!!");
  }
}

void ldy_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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
      WRITE(cpu->y_register, result);
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong absolute step!!!!");
  }
}

void ldy_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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
      uint8_t result = identity(cpu, data, 0);
      WRITE(cpu->y_register, result);
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong absolute x step!!!!");
  }
}

void lsr_accumulator_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  uint8_t result = lsr(cpu, READ(cpu->accumulator));
  WRITE(cpu->accumulator, result);
  prepare_fetch(cpu, device_manager);
}

void lsr_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_write_back(cpu, device_manager, lsr);
}

void lsr_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_x_write_back(cpu, device_manager, lsr);
}

void lsr_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_write_back(cpu, device_manager, lsr);
}

void lsr_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_x_write_back(cpu, device_manager, lsr);
}

void ora_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  immediate_read(cpu, device_manager, ora);
}

void ora_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_read(cpu, device_manager, ora);
}

void ora_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_x_read(cpu, device_manager, ora);
}

void ora_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_read(cpu, device_manager, ora);
}

void ora_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_x_read(cpu, device_manager, ora);
}

void ora_absolute_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_y_read(cpu, device_manager, ora);
}

void ora_index_indirect_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  index_indirect_read(cpu, device_manager, ora);
}

void ora_indirect_index_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  indirect_index_read(cpu, device_manager, ora);
}

void nop_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  cpu->state = FETCH;
}

void pha_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  switch (cpu->micro_step) {
    case S0: {
      cpu->micro_step = S1;
      SET_HIGH(cpu->address_register, 0x01);
      SET_LOW(cpu->address_register, READ(cpu->stack_pointer));
      DECREMENT(cpu->stack_pointer);
      WRITE(cpu->data_register, READ(cpu->accumulator));
      return;
    }
    case S1: {
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong pha_handler step!!!!");

  }
}

void php_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  switch (cpu->micro_step) {
    case S0: {
      cpu->micro_step = S1;
      SET_HIGH(cpu->address_register, 0x01);
      SET_LOW(cpu->address_register, READ(cpu->stack_pointer));
      DECREMENT(cpu->stack_pointer);
      uint8_t data = READ(cpu->status_register);
      data |= B_MASK_SET;
      WRITE(cpu->data_register, data);
      return;
    }
    case S1: {
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong pha_handler step!!!!");

  }
}

void pla_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  switch (cpu->micro_step) {
    case S0: {
      cpu->micro_step = S1;
      INCREMENT(cpu->stack_pointer);
      return;
    }
    case S1: {
      cpu->micro_step = S2;
      SET_HIGH(cpu->address_register, 0x01);
      SET_LOW(cpu->address_register, READ(cpu->stack_pointer));
      return;
    }
    case S2: {
      uint8_t  data = read_device(device_manager, READ(cpu->address_register));
      WRITE(cpu->accumulator, data);
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong pha_handler step!!!!");

  }
}

void plp_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  switch (cpu->micro_step) {
    case S0: {
      cpu->micro_step = S1;
      INCREMENT(cpu->stack_pointer);
      return;
    }
    case S1: {
      cpu->micro_step = S2;
      SET_HIGH(cpu->address_register, 0x01);
      SET_LOW(cpu->address_register, READ(cpu->stack_pointer));
      uint8_t data = READ(cpu->status_register);
      data |= B_MASK_SET;
      WRITE(cpu->data_register, data);
      return;
    }
    case S2: {

      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong pha_handler step!!!!");

  }
}

void rol_accumulator_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  uint8_t result = rol(cpu, READ(cpu->accumulator));
  WRITE(cpu->accumulator, result);
  prepare_fetch(cpu, device_manager);
}

void rol_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_write_back(cpu, device_manager, rol);
}

void rol_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_x_write_back(cpu, device_manager, rol);
}

void rol_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_write_back(cpu, device_manager, rol);
}

void rol_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_x_write_back(cpu, device_manager, rol);
}

void ror_accumulator_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  uint8_t result = ror(cpu, READ(cpu->accumulator));
  WRITE(cpu->accumulator, result);
  prepare_fetch(cpu, device_manager);
}

void ror_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_write_back(cpu, device_manager, ror);
}

void ror_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_x_write_back(cpu, device_manager, ror);
}

void ror_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_write_back(cpu, device_manager, ror);
}

void ror_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_x_write_back(cpu, device_manager, ror);
}

void rti_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  switch (cpu->micro_step) {
    case S0: {
      cpu->micro_step = S1;
      INCREMENT(cpu->stack_pointer);
      return;
    }
    case S1: {
      cpu->micro_step = S2;
      SET_LOW(cpu->address_register, READ(cpu->stack_pointer));
      SET_HIGH(cpu->address_register, 0x01);
      INCREMENT(cpu->stack_pointer);
      return;
    }
    case S2: {
      cpu->micro_step = S3;
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      data &= B_MASK_CLEAR;
      WRITE(cpu->status_register, data);
      SET_LOW(cpu->address_register, READ(cpu->stack_pointer));
      INCREMENT(cpu->stack_pointer);
      return;
    }
    case S3: {
      cpu->micro_step = S4;
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->program_counter, data);
      SET_LOW(cpu->address_register, READ(cpu->stack_pointer));
      return;
    }
    case S4: {
      cpu->state = FETCH;
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_HIGH(cpu->program_counter, data);
      INCREMENT(cpu->program_counter);
      SET_LOW(cpu->address_register, GET_LOW(cpu->program_counter));
      SET_HIGH(cpu->address_register, data);
      return;
    }
    default: fprintf(stderr, "BRK Wrong!!!!");
  }
}

void rts_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]){
  switch (cpu->micro_step) {
    case S0: {
      cpu->micro_step = S1;
      INCREMENT(cpu->stack_pointer);
      return;
    }
    case S1: {
      cpu->micro_step = S2;
      SET_HIGH(cpu->address_register, 0x01);
      SET_LOW(cpu->address_register, READ(cpu->stack_pointer));
      INCREMENT(cpu->stack_pointer);
      return;
    }
    case S2: {
      cpu->micro_step = S3;
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      WRITE(cpu->temp_register, data);
      SET_HIGH(cpu->address_register, 0x01);
      SET_LOW(cpu->address_register, READ(cpu->stack_pointer));
      return;
    }
    case S3: {
      cpu->micro_step = S4;
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_HIGH(cpu->program_counter, READ(cpu->temp_register));
      SET_LOW(cpu->program_counter, data);
      INCREMENT(cpu->program_counter);
      return;
    }
    case S4: {
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong rts_handler step!!!!");
  }
}

void sbc_immediate_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  immediate_read(cpu, device_manager, sbc);
}

void sbc_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_read(cpu, device_manager, sbc);
}

void sbc_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  zeropage_x_read(cpu, device_manager, sbc);
}

void sbc_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_read(cpu, device_manager, sbc);
}

void sbc_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_x_read(cpu, device_manager, sbc);
}

void sbc_absolute_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  absolute_y_read(cpu, device_manager, sbc);
}

void sbc_index_indirect_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  index_indirect_read(cpu, device_manager, sbc);
}

void sbc_indirect_index_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  indirect_index_read(cpu, device_manager, sbc);
}

void sec_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  SET_BIT(cpu->status_register, C_FLAG);
  cpu->state = FETCH;
}

void sed_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  SET_BIT(cpu->status_register, D_FLAG);
  cpu->state = FETCH;
}

void sei_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  SET_BIT(cpu->status_register, I_FLAG);
  cpu->state = FETCH;
}

void sta_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  switch (cpu->micro_step) {
    case S0: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->address_register, data);
      SET_HIGH(cpu->address_register, 0x00);
      WRITE(cpu->data_register, READ(cpu->accumulator));
      cpu->micro_step = S1;
      return;
    }
    case S1: {
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong sta_zeropage step!!!!");
  }
}

void sta_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  switch (cpu->micro_step) {
    case S0: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->address_register, data);
      SET_HIGH(cpu->address_register, 0x00);
      WRITE(cpu->data_register, READ(cpu->accumulator));
      cpu->micro_step = S1;
      return;
    }
    case S1: {
      uint16_t result = add_address(READ(cpu->x_register), READ(cpu->address_register));
      SET_LOW(cpu->address_register, (result & 0x000000ff));
      cpu->micro_step = S2;
    }
    case S2: {
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong sta_zeropage_x step!!!!");
  }
}

void sta_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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
      WRITE(cpu->data_register, READ(cpu->accumulator));
      cpu->micro_step = S2;
      return;
    }
    case S2: {
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong sta_absolute_x step!!!!");
  }
}

void sta_absolute_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) { // Not sure why alwas take 5
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
      uint8_t carry = (result & CARRY_MASK_U16) > 0;
      WRITE(cpu->temp_register, carry); // this is really far-fetched
      cpu->micro_step = S2;
      return;
    }
    case S2: {
      uint16_t result = add_address(GET_HIGH(cpu->address_register), READ(cpu->temp_register));
      SET_HIGH(cpu->address_register, (result & 0x000000ff));
      WRITE(cpu->data_register, READ(cpu->accumulator));
      cpu->micro_step = S3;
      return;
    }
    case S3: {
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong absolute x step!!!!");
  }
}

void sta_absolute_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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
      uint8_t carry = (result & CARRY_MASK_U16) > 0;
      WRITE(cpu->temp_register, carry); // this is really far-fetched
      cpu->micro_step = S2;
      return;
    }
    case S2: {
      uint16_t result = add_address(GET_HIGH(cpu->address_register), READ(cpu->temp_register));
      SET_HIGH(cpu->address_register, (result & 0x000000ff));
      WRITE(cpu->data_register, READ(cpu->accumulator));
      cpu->micro_step = S3;
      return;
    }
    case S3: {
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong absolute x step!!!!");
  }
}

void sta_index_indirect_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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
      WRITE(cpu->data_register, READ(cpu->accumulator));
      cpu->micro_step = S4;
      return;
    }
    case S4: {
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong index indirect step!!!!");
  }
}

void sta_indirect_index_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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
      uint8_t carry = (result & CARRY_MASK_U16) > 0;
      WRITE(cpu->temp_register, carry);
      cpu->micro_step = S3;
      return;
    }
    case S3: {
      uint16_t result = add_address(GET_HIGH(cpu->address_register), READ(cpu->temp_register));
      SET_HIGH(cpu->address_register, (result & 0x000000ff));
      cpu->micro_step = S4;
      return;
    }
    case S4: {
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong indirect index step!!!!");
  }
}

void stx_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  switch (cpu->micro_step) {
    case S0: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->address_register, data);
      SET_HIGH(cpu->address_register, 0x00);
      WRITE(cpu->data_register, READ(cpu->x_register));
      cpu->micro_step = S1;
      return;
    }
    case S1: {
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong zeropage step!!!!");
  }
}

void stx_zeropage_y_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  switch (cpu->micro_step) {
    case S0: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->address_register, data);
      SET_HIGH(cpu->address_register, 0x00);
      WRITE(cpu->data_register, READ(cpu->x_register));
      cpu->micro_step = S1;
      return;
    }
    case S1: {
      uint16_t result = add_address(READ(cpu->y_register), READ(cpu->address_register));
      SET_LOW(cpu->address_register, (result & 0x000000ff));
      cpu->micro_step = S2;
    }
    case S2: {
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong zeropage X step!!!!");
  }
}

void stx_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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
      WRITE(cpu->data_register, READ(cpu->x_register));
      cpu->micro_step = S2;
      return;
    }
    case S2: {
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong absolute step!!!!");
  }
}

void sty_zeropage_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  switch (cpu->micro_step) {
    case S0: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->address_register, data);
      SET_HIGH(cpu->address_register, 0x00);
      WRITE(cpu->data_register, READ(cpu->y_register));
      cpu->micro_step = S1;
      return;
    }
    case S1: {
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong zeropage step!!!!");
  }
}

void sty_zeropage_x_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  switch (cpu->micro_step) {
    case S0: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      SET_LOW(cpu->address_register, data);
      SET_HIGH(cpu->address_register, 0x00);
      WRITE(cpu->data_register, READ(cpu->y_register));
      cpu->micro_step = S1;
      return;
    }
    case S1: {
      uint16_t result = add_address(READ(cpu->x_register), READ(cpu->address_register));
      SET_LOW(cpu->address_register, (result & 0x000000ff));
      cpu->micro_step = S2;
    }
    case S2: {
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong zeropage X step!!!!");
  }
}

void sty_absolute_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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
      WRITE(cpu->data_register, READ(cpu->y_register));
      cpu->micro_step = S2;
      return;
    }
    case S2: {
      write_device(device_manager, READ(cpu->address_register), READ(cpu->data_register));
      prepare_fetch(cpu, device_manager);
      return;
    }
    default: fprintf(stderr, "Wrong absolute step!!!!");
  }
}

void tax_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  uint8_t  result = identity(cpu, READ(cpu->accumulator), 0);
  WRITE(cpu->x_register, result);
  cpu->state = FETCH;
}

void tay_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  uint8_t  result = identity(cpu, READ(cpu->accumulator), 0);
  WRITE(cpu->y_register, result);
  cpu->state = FETCH;}

void tsx_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  uint8_t  result = identity(cpu, READ(cpu->stack_pointer), 0);
  WRITE(cpu->x_register, result);
  cpu->state = FETCH;}

void txa_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  uint8_t  result = identity(cpu, READ(cpu->x_register), 0);
  WRITE(cpu->accumulator, result);
  cpu->state = FETCH;}

void txs_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  uint8_t  result = identity(cpu, READ(cpu->x_register), 0);
  WRITE(cpu->stack_pointer, result);
  cpu->state = FETCH;
}

void tya_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  uint8_t  result = identity(cpu, READ(cpu->y_register), 0);
  WRITE(cpu->accumulator, result);
  cpu->state = FETCH;
}

void branch_handler(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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
      prepare_fetch(cpu, device_manager);
      return;
    }
      // Starts negative case
    case S3: {
      uint8_t data = read_device(device_manager, READ(cpu->address_register));
      uint16_t result = add_address(GET_LOW(cpu->program_counter), data);
      SET_LOW(cpu->program_counter, (result & 0x000000ff));
      cpu->micro_step = !((result & CARRY_MASK_U16) > 0) ? S4 : S5;
      return;
    }
    case S4: {
      uint16_t result = add_address(GET_HIGH(cpu->program_counter), 0xFF); // This represents -1 in 8 bit
      SET_HIGH(cpu->program_counter, (result & 0x000000ff));
      cpu->micro_step = S5;
      return;
    }
    case S5: {
      prepare_fetch(cpu, device_manager);
      return;
    }

    default: fprintf(stderr, "Wrong branch_handler step!!!!");
  }
}

void halt(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
  cpu->state = HALT;
}

void prepare_fetch(struct cpu_internals cpu[static 1], struct device_manager device_manager[static 1]) {
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
      return;
    }
    default: fprintf(stderr, "Wrong zeropage X step %d!!!!", cpu->micro_step);
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

uint8_t sbc(struct cpu_internals *cpu, uint8_t data, uint8_t acc) {
  uint16_t carry = (READ(cpu->status_register) & C_MASK_SET);
  uint16_t result = ((uint16_t) acc) - ((uint16_t) data) - (!carry);
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

uint8_t ora(struct cpu_internals *cpu, uint8_t data, uint8_t acc) {
  uint8_t result = data | acc;
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
  SET_OR_CLEAR_BIT(cpu->status_register, (result & N_MASK_SET) > 0, N_FLAG);
  SET_OR_CLEAR_BIT(cpu->status_register, (result & CARRY_MASK_U16) > 0, C_FLAG);
  return (uint8_t) (result & 0x000000ff);
}

uint8_t rol(struct cpu_internals *cpu, uint8_t data) {
  uint16_t carry = (READ(cpu->status_register) & C_MASK_SET);
  uint16_t result = ((uint16_t) data) << 1;
  SET_OR_CLEAR_BIT(cpu->status_register, (result & 0xff) == 0, Z_FLAG);
  SET_OR_CLEAR_BIT(cpu->status_register, (result & N_MASK_SET) > 0, N_FLAG);
  SET_OR_CLEAR_BIT(cpu->status_register, (result & CARRY_MASK_U16) > 0, C_FLAG);
  result |= carry;
  return (uint8_t) (result & 0x000000ff);
}

uint8_t ror(struct cpu_internals *cpu, uint8_t data) {
  uint16_t extended_data = ((uint16_t) data);
  uint16_t carry = (READ(cpu->status_register) & C_MASK_SET);
  uint16_t result = 1 >> extended_data ;
  SET_OR_CLEAR_BIT(cpu->status_register, (result & 0xff) == 0, Z_FLAG);
  SET_OR_CLEAR_BIT(cpu->status_register, (result & N_MASK_SET) > 0, N_FLAG);
  SET_OR_CLEAR_BIT(cpu->status_register, (extended_data & 0x01) > 0, C_FLAG);
  result |= (carry << 7);
  return (uint8_t) (result & 0x000000ff);
}

uint8_t dec(struct cpu_internals *cpu, uint8_t data) {
  uint16_t result = ((uint16_t) data) - 1;
  SET_OR_CLEAR_BIT(cpu->status_register, (result & 0xff) == 0, Z_FLAG);
  LOG("%d\n",cpu->status_register.input);
  SET_OR_CLEAR_BIT(cpu->status_register, (result & N_MASK_SET) > 0, N_FLAG);
  return (uint8_t) (result & 0x000000ff);
}

uint8_t inc(struct cpu_internals *cpu, uint8_t data) {
  uint16_t result = ((uint16_t) data) + 1;
  SET_OR_CLEAR_BIT(cpu->status_register, (result & 0xff) == 0, Z_FLAG);
  SET_OR_CLEAR_BIT(cpu->status_register, (result & N_MASK_SET) > 0, N_FLAG);
  return (uint8_t) (result & 0x000000ff);
}

uint8_t lsr(struct cpu_internals *cpu, uint8_t data) {
  uint16_t extended_data = ((uint16_t) data);
  uint16_t result = 1 >> extended_data;
  SET_OR_CLEAR_BIT(cpu->status_register, (result & 0xff) == 0, Z_FLAG);
  SET_OR_CLEAR_BIT(cpu->status_register, 0, N_FLAG);
  SET_OR_CLEAR_BIT(cpu->status_register, (extended_data & 0x01) > 0, C_FLAG);
  return (uint8_t) (result & 0x000000ff);
}

