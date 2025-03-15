
#include "mod_test.hpp"
#include "Vread_registers.h"

typedef ClockedModTest<Vread_registers> ReadRegistersTest;

TEST_F(ReadRegistersTest, Reset) {

    reset();

    ASSERT_EQ(mod->decode_stall, 0);
    ASSERT_EQ(mod->valid, 0);
}

TEST_F(ReadRegistersTest, Pipeline) {
    reset();

    ASSERT_EQ(mod->decode_stall, 0);
    ASSERT_EQ(mod->valid, 0);

    clk();
    ASSERT_EQ(mod->decode_stall, 0);
    ASSERT_EQ(mod->valid, 0);

    clk();
    ASSERT_EQ(mod->decode_stall, 0);
    ASSERT_EQ(mod->valid, 0);

    mod->decode_valid = 1;
    mod->decode_opcode = 0x1a;
    mod->decode_rd = 0x1;
    mod->decode_rs1 = 0x2;
    mod->decode_rs2 = 0x3;
    mod->decode_funct3 = 0x4;
    mod->decode_funct7 = 0x5;
    mod->decode_imm = 0xAABBCDEF;
    mod->decode_pc = 0x12345566;
    mod->decode_exception_valid = 0;
    mod->exec_rd = 0;
    clk();

    ASSERT_EQ(mod->decode_stall, 0);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->read_rs1, 0x2);
    ASSERT_EQ(mod->read_rs2, 0x3);

    mod->rs1_val = 0x55667890;
    mod->rs2_val = 0x23456677;
    eval();

    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->opcode_out, 0x1a);
    ASSERT_EQ(mod->rd_out, 0x1);
    ASSERT_EQ(mod->rs2_out, 0x3);
    ASSERT_EQ(mod->funct3_out, 0x4);
    ASSERT_EQ(mod->funct7_out, 0x5);
    ASSERT_EQ(mod->imm_out, 0xAABBCDEF);
    ASSERT_EQ(mod->pc_out, 0x12345566);
    ASSERT_EQ(mod->rs1_val_out, 0x55667890);
    ASSERT_EQ(mod->rs2_val_out, 0x23456677);
    ASSERT_EQ(mod->exception_valid_out, 0);
}

TEST_F(ReadRegistersTest, Stall) {
    reset();

    ASSERT_EQ(mod->decode_stall, 0);
    ASSERT_EQ(mod->valid, 0);

    clk();
    ASSERT_EQ(mod->decode_stall, 0);
    ASSERT_EQ(mod->valid, 0);

    mod->stall = 1;
    clk();
    ASSERT_EQ(mod->decode_stall, 0);
    ASSERT_EQ(mod->valid, 0);

    mod->decode_valid = 1;
    mod->decode_opcode = 0x1a;
    mod->decode_rd = 0x1;
    mod->decode_rs1 = 0x2;
    mod->decode_rs2 = 0x3;
    mod->decode_funct3 = 0x4;
    mod->decode_funct7 = 0x5;
    mod->decode_imm = 0xAABBCDEF;
    mod->decode_pc = 0x12345566;
    mod->decode_exception_valid = 0;
    mod->exec_rd = 0;
    clk();

    ASSERT_EQ(mod->decode_stall, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->read_rs1, 0x2);
    ASSERT_EQ(mod->read_rs2, 0x3);

    mod->rs1_val = 0x55667890;
    mod->rs2_val = 0x23456677;
    eval();

    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->decode_stall, 1);
    ASSERT_EQ(mod->opcode_out, 0x1a);
    ASSERT_EQ(mod->rd_out, 0x1);
    ASSERT_EQ(mod->rs2_out, 0x3);
    ASSERT_EQ(mod->funct3_out, 0x4);
    ASSERT_EQ(mod->funct7_out, 0x5);
    ASSERT_EQ(mod->imm_out, 0xAABBCDEF);
    ASSERT_EQ(mod->pc_out, 0x12345566);
    ASSERT_EQ(mod->rs1_val_out, 0x55667890);
    ASSERT_EQ(mod->rs2_val_out, 0x23456677);
    ASSERT_EQ(mod->exception_valid_out, 0);

    mod->decode_valid = 1;
    mod->decode_opcode = 0x0F;
    clk();

    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->decode_stall, 1);
    ASSERT_EQ(mod->opcode_out, 0x1a);
    ASSERT_EQ(mod->rd_out, 0x1);
    ASSERT_EQ(mod->rs2_out, 0x3);
    ASSERT_EQ(mod->funct3_out, 0x4);
    ASSERT_EQ(mod->funct7_out, 0x5);
    ASSERT_EQ(mod->imm_out, 0xAABBCDEF);
    ASSERT_EQ(mod->pc_out, 0x12345566);
    ASSERT_EQ(mod->rs1_val_out, 0x55667890);
    ASSERT_EQ(mod->rs2_val_out, 0x23456677);
    ASSERT_EQ(mod->exception_valid_out, 0);

    mod->stall = 0;
    eval();
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->decode_stall, 0);
    ASSERT_EQ(mod->opcode_out, 0x1a);
    ASSERT_EQ(mod->rd_out, 0x1);
    ASSERT_EQ(mod->rs2_out, 0x3);
    ASSERT_EQ(mod->funct3_out, 0x4);
    ASSERT_EQ(mod->funct7_out, 0x5);
    ASSERT_EQ(mod->imm_out, 0xAABBCDEF);
    ASSERT_EQ(mod->pc_out, 0x12345566);
    ASSERT_EQ(mod->rs1_val_out, 0x55667890);
    ASSERT_EQ(mod->rs2_val_out, 0x23456677);
    ASSERT_EQ(mod->exception_valid_out, 0);

    clk();
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->decode_stall, 0);
    ASSERT_EQ(mod->opcode_out, 0x0F);
    ASSERT_EQ(mod->rd_out, 0x1);
    ASSERT_EQ(mod->rs2_out, 0x3);
    ASSERT_EQ(mod->funct3_out, 0x4);
    ASSERT_EQ(mod->funct7_out, 0x5);
    ASSERT_EQ(mod->imm_out, 0xAABBCDEF);
    ASSERT_EQ(mod->pc_out, 0x12345566);
    ASSERT_EQ(mod->rs1_val_out, 0x55667890);
    ASSERT_EQ(mod->rs2_val_out, 0x23456677);
    ASSERT_EQ(mod->exception_valid_out, 0);
}

TEST_F(ReadRegistersTest, RegisterConflict) {
    reset();

    ASSERT_EQ(mod->decode_stall, 0);
    ASSERT_EQ(mod->valid, 0);

    mod->decode_valid = 1;
    mod->decode_opcode = 0x1a;
    mod->decode_rd = 0x1;
    mod->decode_rs1 = 0x2;
    mod->decode_rs2 = 0x3;
    mod->decode_funct3 = 0x4;
    mod->decode_funct7 = 0x5;
    mod->decode_imm = 0xAABBCDEF;
    mod->decode_pc = 0x12345566;
    mod->decode_exception_valid = 0;
    mod->exec_rd = 1 << 2;

    mod->rs1_val = 0x55667890;
    mod->rs2_val = 0x23456677;
    clk();

    ASSERT_EQ(mod->decode_stall, 1);
    ASSERT_EQ(mod->valid, 0);

    clk();
    ASSERT_EQ(mod->decode_stall, 1);
    ASSERT_EQ(mod->valid, 0);

    mod->exec_rd = 1 << 3;
    clk();
    ASSERT_EQ(mod->decode_stall, 1);
    ASSERT_EQ(mod->valid, 0);

    clk();
    ASSERT_EQ(mod->decode_stall, 1);
    ASSERT_EQ(mod->valid, 0);

    mod->exec_rd = 1 << 4;
    eval();
    ASSERT_EQ(mod->decode_stall, 0);
    ASSERT_EQ(mod->valid, 1);
}

TEST_F(ReadRegistersTest, RegisterNoConflict) {
    reset();

    ASSERT_EQ(mod->decode_stall, 0);
    ASSERT_EQ(mod->valid, 0);

    mod->decode_valid = 1;
    mod->decode_opcode = 0x1a;
    mod->decode_rd = 0x1;
    mod->decode_rs1 = 0x0;
    mod->decode_rs2 = 0x0;
    mod->decode_funct3 = 0x4;
    mod->decode_funct7 = 0x5;
    mod->decode_imm = 0xAABBCDEF;
    mod->decode_pc = 0x12345566;
    mod->decode_exception_valid = 0;
    mod->exec_rd = 0;

    mod->rs1_val = 0;
    mod->rs2_val = 0;
    clk();

    ASSERT_EQ(mod->decode_stall, 0);
    ASSERT_EQ(mod->valid, 1);
}

TEST_F(ReadRegistersTest, Flush) {

    reset();

    mod->decode_valid = 1;
    mod->decode_opcode = 0x1a;
    mod->decode_rd = 0x1;
    mod->decode_rs1 = 0x2;
    mod->decode_rs2 = 0x3;
    mod->decode_funct3 = 0x4;
    mod->decode_funct7 = 0x5;
    mod->decode_imm = 0xAABBCDEF;
    mod->decode_pc = 0x12345566;
    mod->decode_exception_valid = 0;
    mod->exec_rd = 0;
    clk();

    ASSERT_EQ(mod->decode_stall, 0);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->read_rs1, 0x2);
    ASSERT_EQ(mod->read_rs2, 0x3);

    mod->rs1_val = 0x55667890;
    mod->rs2_val = 0x23456677;
    eval();

    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->opcode_out, 0x1a);
    ASSERT_EQ(mod->rd_out, 0x1);
    ASSERT_EQ(mod->rs2_out, 0x3);
    ASSERT_EQ(mod->funct3_out, 0x4);
    ASSERT_EQ(mod->funct7_out, 0x5);
    ASSERT_EQ(mod->imm_out, 0xAABBCDEF);
    ASSERT_EQ(mod->pc_out, 0x12345566);
    ASSERT_EQ(mod->rs1_val_out, 0x55667890);
    ASSERT_EQ(mod->rs2_val_out, 0x23456677);
    ASSERT_EQ(mod->exception_valid_out, 0);

    mod->flush = 1;
    mod->decode_valid = 0;
    eval();
    ASSERT_EQ(mod->valid, 0);

    clk();
    ASSERT_EQ(mod->valid, 0);
}

