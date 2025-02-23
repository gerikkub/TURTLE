
#include "mod_test.hpp"
#include "Vjump_tester.h"

typedef ModTest<Vjump_tester> JumpTesterTest;

TEST_F(JumpTesterTest, Invalid) {
    eval();

    mod->decode_opcode = 0;
    mod->read_rs1_val = 15;
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
}

TEST_F(JumpTesterTest, Jal) {
    eval();

    // Jump forwards
    mod->decode_opcode = 0b1101111;
    mod->decode_funct3 = 0;
    mod->decode_imm = 0x24;
    mod->decode_pc = 0x40;
    mod->read_rs1_val = 15;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);

    ASSERT_EQ(mod->jump_pc, 1);
    ASSERT_EQ(mod->rd_val_out, 0x44);
    ASSERT_EQ(mod->pc_out, 0x64);

    // Jump backwards
    mod->decode_opcode = 0b1101111;
    mod->decode_funct3 = 0;
    mod->decode_imm = 0xFFFFFFFC;
    mod->decode_pc = 0x40;
    mod->read_rs1_val = 15;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);

    ASSERT_EQ(mod->jump_pc, 1);
    ASSERT_EQ(mod->rd_val_out, 0x44);
    ASSERT_EQ(mod->pc_out, 0x3C);

    // Exception
    mod->decode_opcode = 0b1101111;
    mod->decode_funct3 = 0;
    mod->decode_imm = 5;
    mod->decode_pc = 0x40;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);

    ASSERT_EQ(mod->jump_pc, 0);
    ASSERT_EQ(mod->rd_val_out, 0x44);
    ASSERT_EQ(mod->pc_out, 0x45);
    ASSERT_EQ(mod->exception_num_out, 0);
    ASSERT_EQ(mod->exception_valid_out, 1);
}

TEST_F(JumpTesterTest, Jalr) {
    eval();

    mod->decode_opcode = 0b1100111;
    mod->decode_funct3 = 0;
    mod->decode_imm = 0x24;
    mod->decode_pc = 0x40;
    mod->read_rs1_val = 0x120;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);

    ASSERT_EQ(mod->jump_pc, 1);
    ASSERT_EQ(mod->rd_val_out, 0x44);
    ASSERT_EQ(mod->pc_out, 0x144);

    mod->decode_opcode = 0b1100111;
    mod->decode_funct3 = 0;
    mod->decode_imm = 0xFFFFFFFC;
    mod->decode_pc = 0x40;
    mod->read_rs1_val = 0x120;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);

    ASSERT_EQ(mod->jump_pc, 1);
    ASSERT_EQ(mod->rd_val_out, 0x44);
    ASSERT_EQ(mod->pc_out, 0x11C);

    // Jump drop bit 0
    mod->decode_opcode = 0b1100111;
    mod->decode_funct3 = 0;
    mod->decode_imm = 5;
    mod->decode_pc = 0x40;
    mod->read_rs1_val = 0x120;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);

    ASSERT_EQ(mod->jump_pc, 1);
    ASSERT_EQ(mod->rd_val_out, 0x44);
    ASSERT_EQ(mod->pc_out, 0x124);

    // Exception
    mod->decode_opcode = 0b1100111;
    mod->decode_funct3 = 0;
    mod->decode_imm = 6;
    mod->decode_pc = 0x40;
    mod->read_rs1_val = 0x120;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);

    ASSERT_EQ(mod->jump_pc, 0);
    ASSERT_EQ(mod->rd_val_out, 0x44);
    ASSERT_EQ(mod->pc_out, 0x126);

    ASSERT_EQ(mod->exception_num_out, 0);
    ASSERT_EQ(mod->exception_valid_out, 1);
}

int main(int argc, char** argv) {

    int res;

    Verilated::commandArgs(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}


