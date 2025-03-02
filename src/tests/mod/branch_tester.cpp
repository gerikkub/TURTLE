
#include "mod_test.hpp"
#include "Vbranch_tester.h"
#include "Vbranch_tester__Syms.h"

typedef ClockedModTest<Vbranch_tester> BranchTesterTest;

TEST_F(BranchTesterTest, Reset) {
    reset();

    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->jump_pc, 0);
}

TEST_F(BranchTesterTest, InvalidOpcode) {
    reset();

    mod->decode_opcode = 0b0110111; //LUI
    mod->decode_funct3 = 0b000;
    mod->decode_imm = 0;
    mod->decode_pc = 0x20;
    mod->read_rs1_val = 0xABCD1122;
    mod->read_rs2_val = 0xABCD1234;
    mod->read_valid = 1;

    eval();

    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->jump_pc, 0);
    ASSERT_EQ(mod->rootp->branch_tester__DOT__alu_valid_jump, 0);

    clk();

    // Branch w/ bad funct3
    mod->decode_opcode = 0b1100011;
    mod->decode_funct3 = 0b010;
    mod->decode_imm = 0;
    mod->decode_pc = 0x20;
    mod->read_rs1_val = 0xABCD1122;
    mod->read_rs2_val = 0xABCD1234;
    mod->read_valid = 1;

    eval();

    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->jump_pc, 0);
    ASSERT_EQ(mod->rootp->branch_tester__DOT__alu_valid_jump, 0);
    ASSERT_EQ(mod->rootp->branch_tester__DOT__ej0__DOT__next_state, 0);

}

TEST_F(BranchTesterTest, NotTaken) {
    reset();

    // Equal
    mod->decode_opcode = 0b1100011;
    mod->decode_funct3 = 0b000; //BEQ
    mod->decode_imm = 0;
    mod->decode_pc = 0x20;
    mod->read_rs1_val = 0xABCD1122;
    mod->read_rs2_val = 0xABCD1234;
    mod->read_valid = 1;

    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->jump_pc, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    clk();

    // Not Equal
    mod->decode_opcode = 0b1100011;
    mod->decode_funct3 = 0b001; //BNE
    mod->decode_imm = 0;
    mod->decode_pc = 0x20;
    mod->read_rs1_val = 0xABCD1122;
    mod->read_rs2_val = 0xABCD1122;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->jump_pc, 0);

    clk();

    // Less Than
    mod->decode_opcode = 0b1100011;
    mod->decode_funct3 = 0b100; //BLT
    mod->decode_imm = 0;
    mod->decode_pc = 0x20;
    mod->read_rs1_val = 27;
    mod->read_rs2_val = 0x80ABCDEF;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->jump_pc, 0);

    clk();

    mod->decode_opcode = 0b1100011;
    mod->decode_funct3 = 0b100; //BLT
    mod->decode_imm = 0;
    mod->decode_pc = 0x20;
    mod->read_rs1_val = 27;
    mod->read_rs2_val = 27;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->jump_pc, 0);

    clk();

    // Greater Than or Equal
    mod->decode_funct3 = 0b101; //BGE
    mod->decode_imm = 0;
    mod->decode_pc = 0x20;
    mod->read_rs1_val = 27;
    mod->read_rs2_val = 30;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->jump_pc, 0);

    clk();

    // Less Than Unsigned
    mod->decode_funct3 = 0b110; //BLTU
    mod->decode_imm = 0;
    mod->decode_pc = 0x20;
    mod->read_rs1_val = 0x80ABCDEF;
    mod->read_rs2_val = 27;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->jump_pc, 0);

    clk();

    mod->decode_funct3 = 0b110; //BLTU
    mod->decode_imm = 0;
    mod->decode_pc = 0x20;
    mod->read_rs1_val = 0x80ABCDEF;
    mod->read_rs2_val = 0x80ABCDEF;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->jump_pc, 0);

    clk();

    // Greater Than or Equal Unsigned
    mod->decode_funct3 = 0b111; //BGEU
    mod->decode_imm = 0;
    mod->decode_pc = 0x20;
    mod->read_rs1_val = 27;
    mod->read_rs2_val = 0x80ABCDEF;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->jump_pc, 0);

    mod->read_valid = 0;
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->jump_pc, 0);
}

TEST_F(BranchTesterTest, Taken) {
    reset();

    // Equal
    mod->decode_opcode = 0b1100011;
    mod->decode_funct3 = 0b000; //BEQ
    mod->decode_imm = 0x18;
    mod->decode_pc = 0x20;
    mod->read_rs1_val = 0xABCD1122;
    mod->read_rs2_val = 0xABCD1122;
    mod->read_valid = 1;

    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->jump_pc, 0);

    clk();
    mod->read_valid = 0;

    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->jump_pc, 1);

    ASSERT_EQ(mod->pc_out, 0x38);
    ASSERT_EQ(mod->exception_valid_out, 0);

    clk();

    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->jump_pc, 0);
}

TEST_F(BranchTesterTest, Exception) {
    reset();

    // Equal
    mod->decode_opcode = 0b1100011;
    mod->decode_funct3 = 0b000; //BEQ
    mod->decode_imm = 0x19;
    mod->decode_pc = 0x20;
    mod->read_rs1_val = 0xABCD1122;
    mod->read_rs2_val = 0xABCD1122;
    mod->read_valid = 1;

    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->jump_pc, 0);

    clk();
    mod->read_valid = 0;

    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->jump_pc, 0);

    ASSERT_EQ(mod->pc_out, 0x39);
    ASSERT_EQ(mod->exception_valid_out, 1);

}

int main(int argc, char** argv) {

    int res;

    Verilated::commandArgs(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}


