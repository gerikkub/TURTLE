
#include <stdint.h>
#include "test_lib.h"

void scratch_test() {

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

    READ_CSR(CSR_MISA, id);

    TEST_ASSERT_EQ(id, BIT_N(30) | BIT_N(8));

    READ_CSR(CSR_MTVEC, id);

    TEST_ASSERT_EQ(id, 0x8);
}

void mstatus_test() {
    uint32_t mstatus;

    // Check initial value
    READ_CSR(CSR_MSTATUS, mstatus);

    TEST_ASSERT_EQ(mstatus, 0);

    // Set MIE bit
    READ_SET_CSR(CSR_MSTATUS, CSR_MSTATUS_MIE, mstatus);

    TEST_ASSERT_EQ(mstatus, 0);

    READ_CSR(CSR_MSTATUS, mstatus);

    TEST_ASSERT_EQ(mstatus, CSR_MSTATUS_MIE);

    // Set MPIE bit
    READ_SET_CSR(CSR_MSTATUS, CSR_MSTATUS_MPIE, mstatus);

    TEST_ASSERT_EQ(mstatus, CSR_MSTATUS_MIE);

    READ_CSR(CSR_MSTATUS, mstatus);

    TEST_ASSERT_EQ(mstatus, (CSR_MSTATUS_MIE | CSR_MSTATUS_MPIE));

    // Clear MIE bit
    READ_CLEAR_CSR(CSR_MSTATUS, CSR_MSTATUS_MIE, mstatus);

    TEST_ASSERT_EQ(mstatus, (CSR_MSTATUS_MIE | CSR_MSTATUS_MPIE));

    READ_CSR(CSR_MSTATUS, mstatus);

    TEST_ASSERT_EQ(mstatus, CSR_MSTATUS_MPIE);

    // Clear MPIE
    READ_CLEAR_CSR(CSR_MSTATUS, CSR_MSTATUS_MPIE, mstatus);

    TEST_ASSERT_EQ(mstatus, CSR_MSTATUS_MPIE);

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

void mip_test() {
    uint32_t mip;

    // Check initial value
    READ_CSR(CSR_MIP, mip);

    TEST_ASSERT_EQ(mip, 0);

    // Set MSIP bit
    READ_SET_CSR(CSR_MIP, CSR_MIP_MSIP, mip);

    TEST_ASSERT_EQ(mip, 0);

    READ_CSR(CSR_MIP, mip);

    TEST_ASSERT_EQ(mip, CSR_MIP_MSIP);

    // Set MTIP bit
    READ_SET_CSR(CSR_MIP, CSR_MIP_MTIP, mip);

    TEST_ASSERT_EQ(mip, CSR_MIP_MSIP);

    READ_CSR(CSR_MIP, mip);

    TEST_ASSERT_EQ(mip, (CSR_MIP_MSIP | CSR_MIP_MTIP));

    // Set MEIP bit
    READ_SET_CSR(CSR_MIP, CSR_MIP_MEIP, mip);

    TEST_ASSERT_EQ(mip, (CSR_MIP_MSIP | CSR_MIP_MTIP));

    READ_CSR(CSR_MIP, mip);

    TEST_ASSERT_EQ(mip, (CSR_MIP_MSIP | CSR_MIP_MTIP | CSR_MIP_MEIP));

    // Clear MEIP bit
    READ_CLEAR_CSR(CSR_MIP, CSR_MIP_MSIP, mip);

    TEST_ASSERT_EQ(mip, (CSR_MIP_MSIP | CSR_MIP_MTIP | CSR_MIP_MEIP));

    READ_CSR(CSR_MIP, mip);

    TEST_ASSERT_EQ(mip, (CSR_MIP_MTIP | CSR_MIP_MEIP));

    // Clear MTIP bit
    READ_CLEAR_CSR(CSR_MIP, CSR_MIP_MTIP, mip);

    TEST_ASSERT_EQ(mip, (CSR_MIP_MTIP | CSR_MIP_MEIP));

    READ_CSR(CSR_MIP, mip);

    TEST_ASSERT_EQ(mip, CSR_MIP_MEIP);

    // Clear MEIP bit
    READ_CLEAR_CSR(CSR_MIP, CSR_MIP_MEIP, mip);

    TEST_ASSERT_EQ(mip, CSR_MIP_MEIP);

    READ_CSR(CSR_MIP, mip);

    TEST_ASSERT_EQ(mip, 0);

    // Set all bits
    READ_WRITE_CSR(CSR_MIP, 0xFFFFFFFF, mip);

    TEST_ASSERT_EQ(mip, 0);

    READ_CSR(CSR_MIP, mip);

    TEST_ASSERT_EQ(mip, (CSR_MIP_MSIP | CSR_MIP_MTIP | CSR_MIP_MEIP));

    // Clear all bits
    READ_WRITE_CSR(CSR_MIP, 0, mip);

    TEST_ASSERT_EQ(mip, (CSR_MIP_MSIP | CSR_MIP_MTIP | CSR_MIP_MEIP));

    READ_CSR(CSR_MIP, mip);

    TEST_ASSERT_EQ(mip, 0);

}

void mie_test() {
    uint32_t mie;

    // Check initial value
    READ_CSR(CSR_MIE, mie);

    TEST_ASSERT_EQ(mie, 0);

    // Set MSIE bit
    READ_SET_CSR(CSR_MIE, CSR_MIE_MSIE, mie);

    TEST_ASSERT_EQ(mie, 0);

    READ_CSR(CSR_MIE, mie);

    TEST_ASSERT_EQ(mie, CSR_MIE_MSIE);

    // Set MTIE bit
    READ_SET_CSR(CSR_MIE, CSR_MIE_MTIE, mie);

    TEST_ASSERT_EQ(mie, CSR_MIE_MSIE);

    READ_CSR(CSR_MIE, mie);

    TEST_ASSERT_EQ(mie, (CSR_MIE_MSIE | CSR_MIE_MTIE));

    // Set MEIE bit
    READ_SET_CSR(CSR_MIE, CSR_MIE_MEIE, mie);

    TEST_ASSERT_EQ(mie, (CSR_MIE_MSIE | CSR_MIE_MTIE));

    READ_CSR(CSR_MIE, mie);

    TEST_ASSERT_EQ(mie, (CSR_MIE_MSIE | CSR_MIE_MTIE | CSR_MIE_MEIE));

    // Clear MEIE bit
    READ_CLEAR_CSR(CSR_MIE, CSR_MIE_MSIE, mie);

    TEST_ASSERT_EQ(mie, (CSR_MIE_MSIE | CSR_MIE_MTIE | CSR_MIE_MEIE));

    READ_CSR(CSR_MIE, mie);

    TEST_ASSERT_EQ(mie, (CSR_MIE_MTIE | CSR_MIE_MEIE));

    // Clear MTIE bit
    READ_CLEAR_CSR(CSR_MIE, CSR_MIE_MTIE, mie);

    TEST_ASSERT_EQ(mie, (CSR_MIE_MTIE | CSR_MIE_MEIE));

    READ_CSR(CSR_MIE, mie);

    TEST_ASSERT_EQ(mie, CSR_MIE_MEIE);

    // Clear MEIE bit
    READ_CLEAR_CSR(CSR_MIE, CSR_MIE_MEIE, mie);

    TEST_ASSERT_EQ(mie, CSR_MIE_MEIE);

    READ_CSR(CSR_MIE, mie);

    TEST_ASSERT_EQ(mie, 0);

    // Set all bits
    READ_WRITE_CSR(CSR_MIE, 0xFFFFFFFF, mie);

    TEST_ASSERT_EQ(mie, 0);

    READ_CSR(CSR_MIE, mie);

    TEST_ASSERT_EQ(mie, (CSR_MIE_MSIE | CSR_MIE_MTIE | CSR_MIE_MEIE));

    // Clear all bits
    READ_WRITE_CSR(CSR_MIE, 0, mie);

    TEST_ASSERT_EQ(mie, (CSR_MIE_MSIE | CSR_MIE_MTIE | CSR_MIE_MEIE));

    READ_CSR(CSR_MIE, mie);

    TEST_ASSERT_EQ(mie, 0);
}

void mepc_test() {

    uint32_t mepc;

    // Default value should be 0
    READ_CSR(CSR_MEPC, mepc);

    TEST_ASSERT_EQ(mepc, 0);

    // Write value
    WRITE_CSR(CSR_MEPC, 0xC0DE1234);

    READ_CSR(CSR_MEPC, mepc);

    TEST_ASSERT_EQ(mepc, 0xC0DE1234);

    // Bit set
    READ_SET_CSR(CSR_MEPC, 0xFF00FF00, mepc);

    TEST_ASSERT_EQ(mepc, 0xC0DE1234);

    READ_CSR(CSR_MEPC, mepc);

    TEST_ASSERT_EQ(mepc, 0xFFDEFF34);

    // Bit clear
    READ_CLEAR_CSR(CSR_MEPC, 0xF0F0F0F0, mepc);

    TEST_ASSERT_EQ(mepc, 0xFFDEFF34);

    READ_CSR(CSR_MEPC, mepc);

    TEST_ASSERT_EQ(mepc, 0x0F0E0F04);

    // Clear MEPC
    WRITE_CSR(CSR_MEPC, 0);

    READ_CSR(CSR_MEPC, mepc);

    TEST_ASSERT_EQ(mepc, 0);
}

int main(int argc, char** argv) {

    (void)argc;
    (void)argv;

    scratch_test();

    id_test();

    mstatus_test();

    mip_test();

    mie_test();

    mepc_test();

    end_sim_success();

    return 0;
}
