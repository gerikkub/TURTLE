
#include "mod_test.hpp"
#include "Vdecode.h"
#include "Vdecode__Syms.h"

typedef ClockedModTest<Vdecode> DecodeTest;

TEST_F(DecodeTest, Reset) {

    reset();

    ASSERT_EQ(mod->fetch_stall, 0);
    ASSERT_EQ(mod->valid, 0);
}

TEST_F(DecodeTest, InstDecode) {

    reset();

    ASSERT_EQ(mod->fetch_stall, 0);
    ASSERT_EQ(mod->valid, 0);

    // Opcode LOAD
    // lw a5,-24(s0)
    mod->inst = 0xfe842783;
    mod->inst_pc = 0x40;
    mod->inst_valid = 1;
    mod->stall = 0;
    clk();

    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->exception_valid_out, 0);
    ASSERT_EQ(mod->opcode, 0b0000011);
    ASSERT_EQ(mod->rd, 0xF);
    ASSERT_EQ(mod->rs1, 0x8);
    ASSERT_EQ(mod->rs2, 0);
    ASSERT_EQ(mod->funct3, 2);
    ASSERT_EQ(mod->funct7, 0);
    ASSERT_EQ(mod->imm, 0xFFFFFFE8);
    ASSERT_EQ(mod->inst_pc_out, 0x40);

    mod->inst = 0x00178713;
    mod->inst_pc = 0x44;
    mod->inst_valid = 1;
    mod->stall = 0;
    clk();

    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->exception_valid_out, 0);
    ASSERT_EQ(mod->opcode, 0b0010011);
    ASSERT_EQ(mod->rd, 0xE);
    ASSERT_EQ(mod->rs1, 0xF);
    ASSERT_EQ(mod->rs2, 0);
    ASSERT_EQ(mod->funct3, 0);
    ASSERT_EQ(mod->funct7, 0);
    ASSERT_EQ(mod->imm, 0x1);
    ASSERT_EQ(mod->inst_pc_out, 0x44);

    mod->inst = 0x00008067;
    mod->inst_pc = 0x48;
    mod->inst_valid = 1;
    mod->stall = 0;
    clk();

    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->exception_valid_out, 0);
    ASSERT_EQ(mod->opcode, 0b1100111);
    ASSERT_EQ(mod->rd, 0);
    ASSERT_EQ(mod->rs1, 1);
    ASSERT_EQ(mod->rs2, 0);
    ASSERT_EQ(mod->funct3, 0);
    ASSERT_EQ(mod->funct7, 0);
    ASSERT_EQ(mod->imm, 0);
    ASSERT_EQ(mod->inst_pc_out, 0x48);

}

TEST_F(DecodeTest, Stall) {

    reset();

    ASSERT_EQ(mod->fetch_stall, 0);
    ASSERT_EQ(mod->valid, 0);

    // Opcode LOAD
    // lw a5,-24(s0)
    mod->inst = 0xfe842783;
    mod->inst_pc = 0x40;
    mod->inst_valid = 1;
    mod->stall = 1;
    clk();

    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);
    ASSERT_EQ(mod->opcode, 0b0000011);
    ASSERT_EQ(mod->rd, 0xF);
    ASSERT_EQ(mod->rs1, 0x8);
    ASSERT_EQ(mod->rs2, 0);
    ASSERT_EQ(mod->funct3, 2);
    ASSERT_EQ(mod->funct7, 0);
    ASSERT_EQ(mod->imm, 0xFFFFFFE8);
    ASSERT_EQ(mod->inst_pc_out, 0x40);

    ASSERT_EQ(mod->rootp->decode__DOT__inst_buffer_valid, 1);
    ASSERT_EQ(mod->fetch_stall, 1);

    mod->inst = 0;
    mod->inst_pc = 0;
    mod->inst_valid = 0;
    mod->stall = 1;
    clk();

    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);
    ASSERT_EQ(mod->opcode, 0b0000011);
    ASSERT_EQ(mod->rd, 0xF);
    ASSERT_EQ(mod->rs1, 0x8);
    ASSERT_EQ(mod->rs2, 0);
    ASSERT_EQ(mod->funct3, 2);
    ASSERT_EQ(mod->funct7, 0);
    ASSERT_EQ(mod->imm, 0xFFFFFFE8);
    ASSERT_EQ(mod->inst_pc_out, 0x40);

    ASSERT_EQ(mod->rootp->decode__DOT__inst_buffer_valid, 1);
    ASSERT_EQ(mod->fetch_stall, 1);

    mod->inst = 0;
    mod->inst_pc = 0;
    mod->inst_valid = 0;
    mod->stall = 0;
    eval();

    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->fetch_stall, 0);

    clk();

    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);
    ASSERT_EQ(mod->rootp->decode__DOT__inst_buffer_valid, 0);
    ASSERT_EQ(mod->fetch_stall, 0);
}

TEST_F(DecodeTest, Flush) {

    reset();

    ASSERT_EQ(mod->fetch_stall, 0);
    ASSERT_EQ(mod->valid, 0);

    // Opcode LOAD
    // lw a5,-24(s0)
    mod->inst = 0xfe842783;
    mod->inst_pc = 0x40;
    mod->inst_valid = 1;
    mod->stall = 0;
    clk();

    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->exception_valid_out, 0);
    ASSERT_EQ(mod->opcode, 0b0000011);
    ASSERT_EQ(mod->rd, 0xF);
    ASSERT_EQ(mod->rs1, 0x8);
    ASSERT_EQ(mod->rs2, 0);
    ASSERT_EQ(mod->funct3, 2);
    ASSERT_EQ(mod->funct7, 0);
    ASSERT_EQ(mod->imm, 0xFFFFFFE8);
    ASSERT_EQ(mod->inst_pc_out, 0x40);

    mod->inst = 0x00178713;
    mod->inst_pc = 0x44;
    mod->inst_valid = 1;
    mod->stall = 0;
    clk();

    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->exception_valid_out, 0);
    ASSERT_EQ(mod->opcode, 0b0010011);
    ASSERT_EQ(mod->rd, 0xE);
    ASSERT_EQ(mod->rs1, 0xF);
    ASSERT_EQ(mod->rs2, 0);
    ASSERT_EQ(mod->funct3, 0);
    ASSERT_EQ(mod->funct7, 0);
    ASSERT_EQ(mod->imm, 0x1);
    ASSERT_EQ(mod->inst_pc_out, 0x44);

    mod->flush = 1;
    eval();
    ASSERT_EQ(mod->valid, 0);
}

TEST_F(DecodeTest, StallFlush) {

    reset();

    ASSERT_EQ(mod->fetch_stall, 0);
    ASSERT_EQ(mod->valid, 0);

    // Opcode LOAD
    // lw a5,-24(s0)
    mod->inst = 0xfe842783;
    mod->inst_pc = 0x40;
    mod->inst_valid = 1;
    mod->stall = 1;
    clk();

    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);
    ASSERT_EQ(mod->opcode, 0b0000011);
    ASSERT_EQ(mod->rd, 0xF);
    ASSERT_EQ(mod->rs1, 0x8);
    ASSERT_EQ(mod->rs2, 0);
    ASSERT_EQ(mod->funct3, 2);
    ASSERT_EQ(mod->funct7, 0);
    ASSERT_EQ(mod->imm, 0xFFFFFFE8);

    ASSERT_EQ(mod->rootp->decode__DOT__inst_buffer_valid, 1);
    ASSERT_EQ(mod->fetch_stall, 1);

    mod->inst = 0;
    mod->inst_pc = 0;
    mod->inst_valid = 0;
    mod->stall = 1;
    clk();

    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);
    ASSERT_EQ(mod->opcode, 0b0000011);
    ASSERT_EQ(mod->rd, 0xF);
    ASSERT_EQ(mod->rs1, 0x8);
    ASSERT_EQ(mod->rs2, 0);
    ASSERT_EQ(mod->funct3, 2);
    ASSERT_EQ(mod->funct7, 0);
    ASSERT_EQ(mod->imm, 0xFFFFFFE8);

    ASSERT_EQ(mod->rootp->decode__DOT__inst_buffer_valid, 1);
    ASSERT_EQ(mod->fetch_stall, 1);

    mod->flush = 1;
    eval();

    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);
    ASSERT_EQ(mod->fetch_stall, 0);

    clk();

    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->rootp->decode__DOT__inst_buffer_valid, 0);
    ASSERT_EQ(mod->fetch_stall, 0);
}

TEST_F(DecodeTest, Exception) {

    reset();

    ASSERT_EQ(mod->fetch_stall, 0);
    ASSERT_EQ(mod->valid, 0);

    // Opcode LOAD
    // lw a5,-24(s0)
    mod->inst = 0x0;
    mod->inst_pc = 0x40;
    mod->inst_valid = 1;
    clk();

    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->inst_pc_out, 0x40);
    ASSERT_EQ(mod->exception_valid_out, 1);
    ASSERT_EQ(mod->exception_num_out, 2);

    mod->inst = 0x000c0de0;
    mod->inst_pc = 0x44;
    mod->inst_valid = 1;
    mod->exception_num_in = 0;
    mod->exception_valid_in = 1;
    clk();

    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->inst_pc_out, 0x44);
    ASSERT_EQ(mod->exception_valid_out, 1);
    ASSERT_EQ(mod->exception_num_out, 0);

    mod->inst = 0x001c0de0;
    mod->inst_pc = 0x48;
    mod->inst_valid = 1;
    mod->exception_num_in = 1;
    mod->exception_valid_in = 1;
    clk();

    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->inst_pc_out, 0x48);
    ASSERT_EQ(mod->exception_valid_out, 1);
    ASSERT_EQ(mod->exception_num_out, 1);
}

int main(int argc, char** argv) {

    int res;

    Verilated::commandArgs(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
