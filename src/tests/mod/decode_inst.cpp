
#include "mod_test.hpp"
#include "Vdecode_inst.h"

typedef ModTest<Vdecode_inst> DecodeInstTest;

TEST_F(DecodeInstTest, InstR) {
    mod->inst = 0;
    eval();

    // Opcode OP
    // xor x11, x27, x28
    mod->inst = 0x01cdc5b3;
    eval();

    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->opcode, 0b0110011);
    ASSERT_EQ(mod->rd, 11);
    ASSERT_EQ(mod->rs1, 27);
    ASSERT_EQ(mod->rs2, 28);
    ASSERT_EQ(mod->funct3, 4);
    ASSERT_EQ(mod->funct7, 0);
    ASSERT_EQ(mod->imm, 0);

}

TEST_F(DecodeInstTest, InstI) {
    mod->inst = 0;
    eval();

    // Opcode LOAD
    // lw a5,-24(s0)
    mod->inst = 0xfe842783;
    eval();

    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->opcode, 0b0000011);
    ASSERT_EQ(mod->rd, 0xF);
    ASSERT_EQ(mod->rs1, 0x8);
    ASSERT_EQ(mod->rs2, 0);
    ASSERT_EQ(mod->funct3, 2);
    ASSERT_EQ(mod->funct7, 0);
    ASSERT_EQ(mod->imm, 0xFFFFFFE8);

    // Opcode OP-IMM
    // addi a4, a5, 1
    mod->inst = 0x00178713;
    eval();

    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->opcode, 0b0010011);
    ASSERT_EQ(mod->rd, 0xE);
    ASSERT_EQ(mod->rs1, 0xF);
    ASSERT_EQ(mod->rs2, 0);
    ASSERT_EQ(mod->funct3, 0);
    ASSERT_EQ(mod->funct7, 0);
    ASSERT_EQ(mod->imm, 0x1);

    // Opcode JALR
    // ret
    mod->inst = 0x00008067;
    eval();

    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->opcode, 0b1100111);
    ASSERT_EQ(mod->rd, 0);
    ASSERT_EQ(mod->rs1, 1);
    ASSERT_EQ(mod->rs2, 0);
    ASSERT_EQ(mod->funct3, 0);
    ASSERT_EQ(mod->funct7, 0);
    ASSERT_EQ(mod->imm, 0);
}

TEST_F(DecodeInstTest, InstS) {
    mod->inst = 0;
    eval();

    // Opcode STORE
    // sw x1,12(x2)
    mod->inst = 0x00112623;
    eval();

    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->opcode, 0b0100011);
    ASSERT_EQ(mod->rd, 0);
    ASSERT_EQ(mod->rs1, 0x2);
    ASSERT_EQ(mod->rs2, 0x1);
    ASSERT_EQ(mod->funct3, 2);
    ASSERT_EQ(mod->funct7, 0);
    ASSERT_EQ(mod->imm, 12);

    
}

TEST_F(DecodeInstTest, InstB) {
    mod->inst = 0;
    eval();

    // Opcode BRANCH
    // bnez a8, 78
    mod->inst = 0x00079c63;
    eval();

    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->opcode, 0b1100011);
    ASSERT_EQ(mod->rd, 0);
    ASSERT_EQ(mod->rs1, 0xF);
    ASSERT_EQ(mod->rs2, 0);
    ASSERT_EQ(mod->funct3, 1);
    ASSERT_EQ(mod->funct7, 0);
    ASSERT_EQ(mod->imm, 0b0000000011000);
}

TEST_F(DecodeInstTest, InstU) {
    mod->inst = 0;
    eval();

    // Opcode LUI
    // LUI a5,0x8abcd
    mod->inst = 0x8abcd7b7;
    eval();

    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->opcode, 0b0110111);
    ASSERT_EQ(mod->rd, 0xF);
    ASSERT_EQ(mod->rs1, 0);
    ASSERT_EQ(mod->rs2, 0);
    ASSERT_EQ(mod->funct3, 0);
    ASSERT_EQ(mod->funct7, 0);
    ASSERT_EQ(mod->imm, 0x8abcd000);

    // Opcode AUIPC
    // auipc a1, 0x80000
    mod->inst = 0x80000597;
    eval();

    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->opcode, 0b0010111);
    ASSERT_EQ(mod->rd, 0xB);
    ASSERT_EQ(mod->rs1, 0);
    ASSERT_EQ(mod->rs2, 0);
    ASSERT_EQ(mod->funct3, 0);
    ASSERT_EQ(mod->funct7, 0);
    ASSERT_EQ(mod->imm, 0x80000000);

}

TEST_F(DecodeInstTest, InstJ) {
    mod->inst = 0;
    eval();

    // Opcode JAL
    // jal x1 <end_sim_success>
    mod->inst = 0x01c000ef;
    eval();

    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->opcode, 0b1101111);
    ASSERT_EQ(mod->rd, 0x1);
    ASSERT_EQ(mod->rs1, 0);
    ASSERT_EQ(mod->rs2, 0);
    ASSERT_EQ(mod->funct3, 0);
    ASSERT_EQ(mod->funct7, 0);
    ASSERT_EQ(mod->imm, 0x0000001C);
}

TEST_F(DecodeInstTest, Invalid) {
    mod->inst = 0;
    eval();

    mod->inst = 0x00000007;
    eval();

    ASSERT_EQ(mod->valid, 0);

    mod->inst = 0x00000000;
    eval();

    ASSERT_EQ(mod->valid, 0);
}

int main(int argc, char** argv) {

    int res;

    Verilated::commandArgs(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
