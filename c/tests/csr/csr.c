
#include <stdint.h>

extern volatile unsigned char __magic_sim_end;

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

void end_sim() {
	__magic_sim_end = 0xA5;
}


int main(int argc, char** argv) {

    uint32_t scratch;

    READ_CSR(0x340, scratch);

    if (scratch != 0) {
        while(1);
    }

    WRITE_CSR(0x340, 0xC0DEABCD);

    READ_CSR(0x340, scratch);

    if (scratch != 0xC0DEABCD) {
        while(1);
    }

    end_sim();

    return 0;
}
