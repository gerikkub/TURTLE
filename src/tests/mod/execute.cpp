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

TEST_F(ExecuteTest, Jump) {
    reset();

    // JAL
    mod->decode_opcode = 0b1101111;
    mod->decode_funct3 = 0;
    mod->decode_rd = 5;
    mod->decode_imm = 0xFFFFFF00;
    mod->decode_pc = 0xAABBCCDC;

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

    ASSERT_EQ(mod->rd_out, 5);
    ASSERT_EQ(mod->rd_val_out, 0xAABBCCE0);
    ASSERT_EQ(mod->inst_pc_out, 0xAABBCCDC);
    ASSERT_EQ(mod->jump_out, 1);
    ASSERT_EQ(mod->jump_pc_out, 0xAABBCBDC);
    ASSERT_EQ(mod->exception_valid_out, 0);

    clk();

    // JALR
    mod->decode_opcode = 0b1100111;
    mod->decode_funct3 = 0;
    mod->decode_rd = 5;
    mod->decode_imm = 0xABC;
    mod->decode_pc = 0xAABBCCDC;

    mod->read_rs1_val = 0x80004000;

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

    ASSERT_EQ(mod->rd_out, 5);
    ASSERT_EQ(mod->rd_val_out, 0xAABBCCE0);
    ASSERT_EQ(mod->inst_pc_out, 0xAABBCCDC);
    ASSERT_EQ(mod->jump_out, 1);
    ASSERT_EQ(mod->jump_pc_out, 0x80004ABC);
    ASSERT_EQ(mod->exception_valid_out, 0);

    clk();

    // JALR Exception
    mod->decode_opcode = 0b1100111;
    mod->decode_funct3 = 0;
    mod->decode_rd = 5;
    mod->decode_imm = 0xABC;
    mod->decode_pc = 0xAABBCCDC;

    mod->read_rs1_val = 0x80004002;

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

    ASSERT_EQ(mod->inst_pc_out, 0xAABBCCDC);
    ASSERT_EQ(mod->jump_out, 0);
    ASSERT_EQ(mod->exception_valid_out, 1);
    ASSERT_EQ(mod->exception_num_out, 0);
}

TEST_F(ExecuteTest, Shift) {
    reset();

    // SLLI. Processing should take 2 clocks
    mod->decode_opcode = 0b0010011;
    mod->decode_funct3 = 0b001;
    mod->decode_rd = 25;
    mod->decode_imm = 18;
    mod->decode_pc = 0xAABBCCDC;

    mod->read_rs1_val = 0x99876543;

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
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->read_stall, 1);
    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->read_stall, 0);

    ASSERT_EQ(mod->rd_out, 25);
    ASSERT_EQ(mod->rd_val_out, (0x99876543 << 18));
    ASSERT_EQ(mod->inst_pc_out, 0xAABBCCDC);
    ASSERT_EQ(mod->jump_out, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    // SRA. Processing should take 0 clocks
    mod->decode_opcode = 0b0110011;
    mod->decode_funct3 = 0b101;
    mod->decode_funct7 = 0b0100000;
    mod->decode_rd = 12;
    mod->decode_imm = 0;
    mod->decode_pc = 0xAABBCCDC;

    mod->read_rs1_val = 0x99876543;
    mod->read_rs2_val = 0x7;

    mod->read_valid = 1;
    clk();
    mod->read_valid = 0;
    eval();

    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->read_stall, 0);

    ASSERT_EQ(mod->rd_out, 12);
    ASSERT_EQ(mod->rd_val_out, (0x99876543UL >> 7) | 0xFE000000);
    ASSERT_EQ(mod->inst_pc_out, 0xAABBCCDC);
    ASSERT_EQ(mod->jump_out, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);
}

TEST_F(ExecuteTest, Store) {
    reset();

    // Store Word
    mod->decode_opcode = 0b0100011;
    mod->decode_funct3 = 0b010;
    mod->decode_imm = 0xFFFFFFF0;
    mod->decode_pc = 0xAABBCCDC;

    mod->read_rs1_val = 0x1234FDEC;
    mod->read_rs2_val = 0x98765432;

    mod->read_valid = 0;
    clk();
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

    ASSERT_EQ(mod->store_addr_out, 0x1234FDDC);
    ASSERT_EQ(mod->store_val_out, 0x98765432);
    ASSERT_EQ(mod->store_size_out, 2);
    ASSERT_EQ(mod->store_valid_out, 1);
    ASSERT_EQ(mod->jump_out, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    // SW exception
    mod->decode_opcode = 0b0100011;
    mod->decode_funct3 = 0b010;
    mod->decode_imm = 1;
    mod->decode_pc = 0xAABBCCDC;

    mod->read_rs1_val = 0x1234FDEC;
    mod->read_rs2_val = 0x98765432;

    mod->read_valid = 1;
    clk();
    mod->read_valid = 0;
    eval();

    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->read_stall, 0);

    ASSERT_EQ(mod->store_valid_out, 0);
    ASSERT_EQ(mod->jump_out, 0);
    ASSERT_EQ(mod->exception_valid_out, 1);
    ASSERT_EQ(mod->exception_num_out, 6);
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


