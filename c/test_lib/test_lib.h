#ifndef __TEST_LIB_H
#define __TEST_LIB_H

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
    			  : "r" (csr_num), "r" (csr_in) \
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


extern volatile unsigned char __magic_sim_end;

inline void end_sim() {
	__magic_sim_end = 0xA5;
}

#endif