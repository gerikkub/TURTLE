#ifndef __TEST_LIB_H
#define __TEST_LIB_H

#include <stdint.h>

#define BIT_N(x) (1 << x)

#define READ_CSR(csr_num, csr_out) \
    asm volatile ("csrr %0, %1\n" \
                  : "=r" (csr_out) \
                  : "i" (csr_num) \
                  );

#define WRITE_CSR(csr_num, csr_in) \
    asm volatile ("csrw %0, %1\n" \
                  : \
                  : "i" (csr_num), "r" (csr_in) \
                  );

#define READ_WRITE_CSR(csr_num, csr_in, csr_out) \
    asm volatile ("csrrw %0, %1, %2\n" \
                  : "=r" (csr_out) \
                  : "i" (csr_num), "r" (csr_in) \
                  );

#define READ_SET_CSR(csr_num, csr_set, csr_out) \
    asm volatile ("csrrs %0, %1, %2\n" \
                  : "=r" (csr_out) \
                  : "i" (csr_num), "r" (csr_set) \
                  );

#define READ_CLEAR_CSR(csr_num, csr_clear, csr_out) \
    asm volatile ("csrrc %0, %1, %2\n" \
                  : "=r" (csr_out) \
                  : "i" (csr_num), "r" (csr_clear) \
                  );


#define READ_WRITE_CSR_IMM(csr_num, csr_in, csr_out) \
    asm volatile ("csrrw %0, %1, %2\n" \
                  : "=r" (csr_out) \
                  : "i" (csr_num), "i" (csr_in) \
                  );

#define READ_SET_CSR_IMM(csr_num, csr_set, csr_out) \
    asm volatile ("csrrs %0, %1, %2\n" \
                  : "=r" (csr_out) \
                  : "i" (csr_num), "i" (csr_set) \
                  );

#define READ_CLEAR_CSR_IMM(csr_num, csr_clear, csr_out) \
    asm volatile ("csrrc %0, %1, %2\n" \
                  : "=r" (csr_out) \
                  : "i" (csr_num), "i" (csr_clear) \
                  );

#define TEST_ASSERT(x) \
    do { \
        if (!(x)) { \
            end_sim_failure(__LINE__); \
        } \
    }

#define TEST_ASSERT_EQ(x, y) \
    do { \
        if (!((x) == (y))) { \
            end_sim_failure_exp(__LINE__, (y), (x)); \
        } \
    } while(0)

enum csr_t {
    CSR_MSTATUS = 0x300,
    CSR_MISA = 0x301,
    CSR_MEDELG = 0x302,
    CSR_MIDELEG = 0x303,
    CSR_MIE = 0x304,
    CSR_MTVEC = 0x305,
    CSR_MCOUTEREN = 0x306,
    CSR_MSCRATCH = 0x340,
    CSR_MEPC = 0x341,
    CSR_MCAUSE = 0x342,
    CSR_MTVAL = 0x343,
    CSR_MIP = 0x344,
    CSR_MCYCLE = 0xB00,
    CSR_MTIME = 0xB01,
    CSR_MINSTRET = 0xB02,
    CSR_MCYCLEH = 0xB80,
    CSR_MTIMEH = 0xB81,
    CSR_MINSTRETH = 0xB82,
    CSR_MVENDORID = 0xF11,
    CSR_MARCHID = 0xF12,
    CSR_MIMPID = 0xF13,
    CSR_MHARTID = 0xF14,
};

#define CSR_MSTATUS_MIE BIT_N(3)
#define CSR_MSTATUS_MPIE BIT_N(7)

#define CSR_MIP_MSIP BIT_N(3)
#define CSR_MIP_MTIP BIT_N(7)
#define CSR_MIP_MEIP BIT_N(11)

#define CSR_MIE_MSIE BIT_N(3)
#define CSR_MIE_MTIE BIT_N(7)
#define CSR_MIE_MEIE BIT_N(11)

void end_sim_success();

void end_sim_failure(int line);

void end_sim_failure_exp(int line, int expected, int actual);

#endif
