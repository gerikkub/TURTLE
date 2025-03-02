#include "mod_test.hpp"
#include "Vexecute.h"

typedef ClockedModTest<Vexecute> ExecuteTest;

TEST_F(ExecuteTest, Reset) {
    reset();

    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->read_stall, 0);
}

TEST_F(ExecuteTest, Alu) {
    reset();

    // Addi
    mod->decode_opcode = 0b0010011;
    mod->decode_rd = 0b001;
    mod->decode_rs2 = 0;
    mod->decode_funct3 = 0b000;
    mod->decode_funct7 = 0;
    mod->decode_imm = 5;
    mod->decode_pc = 0xAABBCCDD;

    mod->read_rs1_val = 20;
    mod->read_rs2_val = 0;

    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->read_stall, 0);
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->read_stall, 0);

    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->read_stall, 0);

    clk();
    mod->read_valid = 0;
    eval();

    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->read_stall, 0);

    ASSERT_EQ(mod->rd_out, 1);
    ASSERT_EQ(mod->rd_val_out, 25);
    ASSERT_EQ(mod->inst_pc_out, 0xAABBCCDD);
    ASSERT_EQ(mod->jump_out, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);
}

TEST_F(ExecuteTest, Branch) {
    reset();

    // BLT (Not Taken)
    mod->decode_opcode = 0b1100011;
    mod->decode_funct3 = 0b100;
    mod->decode_imm = 0x14;
    mod->decode_pc = 0xAABBCCDC;

    mod->read_rs1_val = 17;
    mod->read_rs2_val = 0xFFFFFFFF;

    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->read_stall, 0);

    mod->read_valid = 1;
    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->read_stall, 0);

    ASSERT_EQ(mod->rd_out, 0);
    ASSERT_EQ(mod->inst_pc_out, 0xAABBCCDC);
    ASSERT_EQ(mod->jump_out, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    clk();

    // BLT (Taken)
    mod->decode_opcode = 0b1100011;
    mod->decode_funct3 = 0b100;
    mod->decode_imm = 0x14;
    mod->decode_pc = 0xAABBCCDC;

    mod->read_rs1_val = 0xFFFFFFFF;
    mod->read_rs2_val = 17;

    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->read_stall, 0);

    mod->read_valid = 1;
    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->read_stall, 1);

    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->read_stall, 0);

    ASSERT_EQ(mod->rd_out, 0);
    ASSERT_EQ(mod->inst_pc_out, 0xAABBCCDC);
    ASSERT_EQ(mod->jump_out, 1);
    ASSERT_EQ(mod->jump_pc_out, 0xAABBCCF0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    clk();

    // Exception
    mod->decode_opcode = 0b1100011;
    mod->decode_funct3 = 0b100;
    mod->decode_imm = 0x12;
    mod->decode_pc = 0xAABBCCDC;

    mod->read_rs1_val = 0xFFFFFFFF;
    mod->read_rs2_val = 17;

    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->read_stall, 0);

    mod->read_valid = 1;
    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->read_stall, 1);

    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->read_stall, 0);

    ASSERT_EQ(mod->rd_out, 0);
    ASSERT_EQ(mod->inst_pc_out, 0xAABBCCDC);
    ASSERT_EQ(mod->jump_out, 0);
    ASSERT_EQ(mod->exception_valid_out, 1);
    ASSERT_EQ(mod->exception_num_out, 0);
}

TEST_F(ExecuteTest, Hold) {
    reset();

    // Addi
    mod->decode_opcode = 0b0010011;
    mod->decode_rd = 0b001;
    mod->decode_rs2 = 0;
    mod->decode_funct3 = 0b000;
    mod->decode_funct7 = 0;
    mod->decode_imm = 5;
    mod->decode_pc = 0xAABBCCDD;

    mod->read_rs1_val = 20;
    mod->read_rs2_val = 0;

    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->read_stall, 0);
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->read_stall, 0);

    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->read_stall, 0);

    clk();
    mod->read_valid = 0;
    mod->stall = 1;
    eval();

    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->read_stall, 1);

    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->read_stall, 1);

    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->read_stall, 1);

    clk();
    mod->stall = 0;
    eval();

    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->read_stall, 0);

    ASSERT_EQ(mod->rd_out, 1);
    ASSERT_EQ(mod->rd_val_out, 25);
    ASSERT_EQ(mod->inst_pc_out, 0xAABBCCDD);
    ASSERT_EQ(mod->jump_out, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);
}


int main(int argc, char** argv) {

    int res;

    Verilated::commandArgs(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
