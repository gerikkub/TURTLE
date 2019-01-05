
#include <stdint.h>
#include "test_lib.h"

void scratch_test() {

    uint32_t scratch_old;
    uint32_t scratch;
    uint32_t dummy;

    READ_CSR(CSR_MSCRATCH, scratch);

    TEST_ASSERT_EQ(scratch, 0);

    WRITE_CSR(CSR_MSCRATCH, 0xC0DEABCD);

    READ_CSR(CSR_MSCRATCH, scratch);

    TEST_ASSERT_EQ(scratch, 0xC0DEABCD);

    READ_SET_CSR(CSR_MSCRATCH, 0xFF00FF00, dummy);

    READ_CSR(CSR_MSCRATCH, scratch);

    TEST_ASSERT_EQ(scratch, 0xFFDEFFCD);

    READ_CLEAR_CSR(CSR_MSCRATCH, 0x0F0F0F0F, dummy);

    READ_CSR(CSR_MSCRATCH, scratch);

    TEST_ASSERT_EQ(scratch, 0xF0D0F0C0);

    READ_WRITE_CSR(CSR_MSCRATCH, 0xABCD1234, scratch);

    TEST_ASSERT_EQ(scratch, 0xF0D0F0C0);

    READ_WRITE_CSR(CSR_MSCRATCH, 0xFEDC9876, scratch)

    TEST_ASSERT_EQ(scratch, 0xABCD1234);

    READ_CSR(CSR_MSCRATCH, scratch);

    TEST_ASSERT_EQ(scratch, 0xFEDC9876);
}

void id_test() {

    uint32_t id;

    READ_CSR(CSR_MVENDORID, id);

    TEST_ASSERT_EQ(id, 0xC0DE1122);

    READ_CSR(CSR_MARCHID, id);

    TEST_ASSERT_EQ(id, 0xC0DE3344);

    READ_CSR(CSR_MIMPID, id);

    TEST_ASSERT_EQ(id, 0xC0DE5566);

    READ_CSR(CSR_MHARTID, id);

    TEST_ASSERT_EQ(id, 0xC0DE7788);

    WRITE_CSR(CSR_MVENDORID, 0xFFFFFFFF);
    WRITE_CSR(CSR_MARCHID, 0xFFFFFFFF);
    WRITE_CSR(CSR_MIMPID, 0xFFFFFFFF);
    WRITE_CSR(CSR_MHARTID, 0xFFFFFFFF);

    READ_CSR(CSR_MVENDORID, id);

    TEST_ASSERT_EQ(id, 0xC0DE1122);

    READ_CSR(CSR_MARCHID, id);

    TEST_ASSERT_EQ(id, 0xC0DE3344);

    READ_CSR(CSR_MIMPID, id);

    TEST_ASSERT_EQ(id, 0xC0DE5566);

    READ_CSR(CSR_MHARTID, id);

    TEST_ASSERT_EQ(id, 0xC0DE7788);
}

void mstatus_test() {
    uint32_t mstatus;

    // Check initial value
    READ_CSR(CSR_MSTATUS, mstatus);

    TEST_ASSERT_EQ(mstatus, 0);

    // Set MIE bit
    READ_SET_CSR(CSR_MSTATUS, CSR_MSTATUS_MIE, mstatus);

    TEST_ASSERT_EQ(mstatus, CSR_MSTATUS_MIE);

    READ_CSR(CSR_MSTATUS, mstatus);

    TEST_ASSERT_EQ(mstatus, CSR_MSTATUS_MIE);

    // Set MPIE bit
    READ_SET_CSR(CSR_MSTATUS, CSR_MSTATUS_MPIE, mstatus);

    TEST_ASSERT_EQ(mstatus, (CSR_MSTATUS_MIE | CSR_MSTATUS_MPIE));

    READ_CSR(CSR_MSTATUS, mstatus);

    TEST_ASSERT_EQ(mstatus, (CSR_MSTATUS_MIE | CSR_MSTATUS_MPIE));

    // Clear MIE bit
    READ_CLEAR_CSR(CSR_MSTATUS, CSR_MSTATUS_MIE, mstatus);

    TEST_ASSERT_EQ(CSR_MSTATUS, CSR_MSTATUS_MPIE);

    READ_CSR(CSR_MSTATUS, mstatus);

    TEST_ASSERT_EQ(mstatus, CSR_MSTATUS_MPIE);

    // Clear MPIE
    READ_CLEAR_CSR(CSR_MSTATUS, CSR_MSTATUS_MPIE, mstatus);

    TEST_ASSERT_EQ(mstatus, 0);

    READ_CSR(CSR_MSTATUS, mstatus);

    TEST_ASSERT_EQ(mstatus, 0);

    // Set both MIE and MPIE
    WRITE_CSR(CSR_MSTATUS, (CSR_MSTATUS_MIE | CSR_MSTATUS_MPIE));

    READ_CSR(CSR_MSTATUS, mstatus);

    TEST_ASSERT_EQ(mstatus, (CSR_MSTATUS_MIE | CSR_MSTATUS_MPIE));

    // Clear MIE and MPIE

    WRITE_CSR(CSR_MSTATUS, 0);

    READ_CSR(CSR_MSTATUS, mstatus);

    TEST_ASSERT_EQ(mstatus, 0);
}

int main(int argc, char** argv) {

    scratch_test();

    id_test();

    mstatus_test();

    end_sim_success();

    return 0;
}
