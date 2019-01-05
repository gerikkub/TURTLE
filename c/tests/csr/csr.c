
#include <stdint.h>
#include "test_lib.h"

volatile int scratch_test() {

    uint32_t scratch;
    uint32_t dummy;

    READ_CSR(CSR_MSCRATCH, scratch);

    if (scratch != 0) {
        return 1;
    }

    WRITE_CSR(CSR_MSCRATCH, 0xC0DEABCD);

    READ_CSR(CSR_MSCRATCH, scratch);

    if (scratch != 0xC0DEABCD) {
        return 1;
    }

    READ_SET_CSR(CSR_MSCRATCH, 0xFF00FF00, dummy);

    READ_CSR(CSR_MSCRATCH, scratch);

    if (scratch != 0xFFDEFFCD) {
        return 1;
    }

    READ_CLEAR_CSR(CSR_MSCRATCH, 0x0F0F0F0F, dummy);

    READ_CSR(CSR_MSCRATCH, scratch);

    if (scratch != 0xF0D0F0C0) {
        return 1;
    }

    return 0;
}

volatile int id_test() {

    uint32_t id;

    READ_CSR(CSR_MVENDORID, id);

    if (id != 0xC0DE1122) {
        return 1;
    }

    READ_CSR(CSR_MARCHID, id);

    if (id != 0xC0DE3344) {
        return 1;
    }

    READ_CSR(CSR_MIMPID, id);

    if (id != 0xC0DE5566) {
        return 1;
    }

    READ_CSR(CSR_MHARTID, id);

    if (id != 0xC0DE7788) {
        return 1;
    }

    return 0;
}

int main(int argc, char** argv) {

    if (scratch_test() != 0) {
        while (1);
    }

    if (id_test() != 0) {
        while (1);
    }

    end_sim();

    return 0;
}
