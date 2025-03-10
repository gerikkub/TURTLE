
#include "mod_test.hpp"
#include "Vinstruction_fetch.h"

typedef ClockedModTest<Vinstruction_fetch> InstructionFetchTest;

TEST_F(InstructionFetchTest, Reset) {

    reset();

    ASSERT_EQ(mod->mem_fetch_addr_en, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->mem_fetch_addr, 0);
    ASSERT_EQ(mod->inst, 0x000c0de0);
    ASSERT_EQ(mod->inst_pc, 0);
}

TEST_F(InstructionFetchTest, Fetch) {

    reset();

    mod->stall = 0;
    mod->override_pc = 0;
    mod->mem_inst_valid = 0;
    clk();

    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 0);
    mod->mem_inst_valid = 1;
    mod->mem_inst_in = 0x1122ABCD;
    eval();
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->inst, 0x1122ABCD);
    ASSERT_EQ(mod->inst_pc, 0);

    clk();
    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 4);
    mod->mem_inst_valid = 1;
    mod->mem_inst_in = 0x1234AABB;
    eval();
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->inst, 0x1234AABB);
    ASSERT_EQ(mod->inst_pc, 4);

    clk();
    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 8);
    mod->mem_inst_valid = 1;
    mod->mem_inst_in = 0x11223344;
    eval();
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->inst, 0x11223344);
    ASSERT_EQ(mod->inst_pc, 8);
}

TEST_F(InstructionFetchTest, Stall) {

    reset();

    mod->stall = 0;
    mod->override_pc = 0;
    mod->mem_inst_valid = 0;
    clk();

    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 0);
    mod->mem_inst_valid = 1;
    mod->mem_inst_in = 0x1122ABCD;
    eval();
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->inst, 0x1122ABCD);
    ASSERT_EQ(mod->inst_pc, 0);

    clk();
    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 4);
    mod->mem_inst_valid = 1;
    mod->mem_inst_in = 0x1234AABB;
    mod->stall = 1;
    eval();
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->inst, 0x000c0de0);
    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->inst_pc, 4);

    clk();
    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 4);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->inst, 0x000c0de0);
    ASSERT_EQ(mod->inst_pc, 4);

    clk();
    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 4);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->inst, 0x000c0de0);
    ASSERT_EQ(mod->inst_pc, 4);

    mod->stall = 0;
    eval();
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->inst, 0x1234AABB);
    ASSERT_EQ(mod->inst_pc, 4);

    clk();
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 8);
    ASSERT_EQ(mod->inst_pc, 8);
}

TEST_F(InstructionFetchTest, Override) {

    reset();

    mod->stall = 0;
    mod->override_pc = 0;
    mod->mem_inst_valid = 0;
    clk();

    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 0);
    mod->mem_inst_valid = 1;
    mod->mem_inst_in = 0x1122ABCD;
    eval();
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->inst, 0x1122ABCD);
    ASSERT_EQ(mod->inst_pc, 0);

    clk();
    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 4);
    mod->mem_inst_valid = 1;
    mod->mem_inst_in = 0x1234AABB;
    mod->override_pc = 1;
    mod->override_pc_addr = 0x20;
    eval();
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->inst, 0x000c0de0);
    ASSERT_EQ(mod->mem_fetch_addr_en, 1);

    clk();
    mod->override_pc = 0;
    eval();
    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 0x20);
    mod->mem_inst_valid = 1;
    mod->mem_inst_in = 0xABCD1234;
    eval();
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->inst, 0xABCD1234);
    ASSERT_EQ(mod->inst_pc, 0x20);
}

TEST_F(InstructionFetchTest, Exception) {
    reset();

    // Misalign exception
    mod->stall = 0;
    mod->override_pc = 1;
    mod->override_pc_addr = 2;
    mod->mem_inst_valid = 0;
    clk();
    mod->override_pc = 0;
    mod->mem_inst_valid = 1;
    eval();

    ASSERT_EQ(mod->mem_fetch_addr_en, 0);
    ASSERT_EQ(mod->mem_fetch_addr, 2);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->inst, 0x001c0de0);
    ASSERT_EQ(mod->inst_pc, 2);
    ASSERT_EQ(mod->exception_num, 0);
    ASSERT_EQ(mod->exception_valid, 1);

    mod->stall = 0;
    mod->override_pc = 1;
    mod->override_pc_addr = 4;
    mod->mem_inst_valid = 1;
    mod->mem_inst_in = 0xAABB1234;
    clk();
    mod->override_pc = 0;
    eval();

    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 4);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->inst, 0xAABB1234);
    ASSERT_EQ(mod->inst_pc, 4);
    ASSERT_EQ(mod->exception_num, 0);
    ASSERT_EQ(mod->exception_valid, 0);

    mod->mem_inst_access_fault = 1;
    eval();

    ASSERT_EQ(mod->mem_fetch_addr_en, 1);
    ASSERT_EQ(mod->mem_fetch_addr, 4);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->inst, 0x001c0de0);
    ASSERT_EQ(mod->inst_pc, 4);
    ASSERT_EQ(mod->exception_num, 1);
    ASSERT_EQ(mod->exception_valid, 1);

}


