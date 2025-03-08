
#include "mod_test.hpp"
#include "Vexecute_shift.h"

typedef ClockedModTest<Vexecute_shift> ExecuteShiftTest;

TEST_F(ExecuteShiftTest, Reset) {

    reset();

    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
}

TEST_F(ExecuteShiftTest, Sll) {
    reset();

    mod->decode_opcode = 0b0110011;
    mod->decode_funct3 = 0b001;
    mod->decode_funct7 = 0b0;
    mod->read_valid = 1;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 1;
    // Should complete without delay
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (0xAABBCCDD << 1));

    mod->read_valid = 0;
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);

    mod->read_valid = 1;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 2;
    // Should complete without delay
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (0xAABBCCDD << 2));


    mod->read_valid = 1;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 7;
    // Should complete without delay
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (0xAABBCCDD << 7));

    mod->read_valid = 1;
    mod->read_rs1_val = 0xAABBCCDD;
    // Only keep the lower 5 bits
    mod->read_rs2_val = 0x123456E7;
    // Should complete without delay
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (0xAABBCCDD << 7));

    mod->read_valid = 1;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 8;
    // Should complete in 1 clock
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    clk();
    mod->read_valid = 0;
    eval();

    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (0xAABBCCDD << 8));

    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);

    mod->read_valid = 1;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 27;
    // Should complete in 3 clock
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    clk();
    mod->read_valid = 0;
    eval();

    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (0xAABBCCDD << 27));

    clk();

    // SLLI
    mod->decode_opcode = 0b0010011;
    mod->decode_funct3 = 0b001;
    mod->decode_funct7 = 0b0;
    mod->read_valid = 1;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 1;
    mod->decode_imm = 5;
    // Should complete without delay
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (0xAABBCCDD << 5));

    clk();

    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 1;
    // Should be an invalid opcode
    mod->decode_imm = 0x00000ABE5;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
}

TEST_F(ExecuteShiftTest, Srl) {
    reset();

    mod->decode_opcode = 0b0110011;
    mod->decode_funct3 = 0b101;
    mod->decode_funct7 = 0b0;
    mod->read_valid = 1;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 1;
    // Should complete without delay
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, 0x555DE66E);

    mod->read_valid = 0;
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);

    mod->read_valid = 1;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 2;
    // Should complete without delay
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (0xAABBCCDD >> 2));


    mod->read_valid = 1;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 7;
    // Should complete without delay
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (0xAABBCCDD >> 7));

    mod->read_valid = 1;
    mod->read_rs1_val = 0xAABBCCDD;
    // Only keep the lower 5 bits
    mod->read_rs2_val = 0x123456E7;
    // Should complete without delay
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (0xAABBCCDD >> 7));

    mod->read_valid = 1;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 8;
    // Should complete in 1 clock
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    clk();
    mod->read_valid = 0;
    eval();

    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, 0x00AABBCC);

    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);

    mod->read_valid = 1;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 27;
    // Should complete in 3 clock
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    clk();
    mod->read_valid = 0;
    eval();

    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (0xAABBCCDD >> 27));

    clk();

    // SRLI
    mod->decode_opcode = 0b0010011;
    mod->decode_funct3 = 0b101;
    mod->decode_funct7 = 0b0;
    mod->read_valid = 1;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 1;
    mod->decode_imm = 5;
    // Should complete without delay
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (0xAABBCCDD >> 5));

    clk();

    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 1;
    // Should be an invalid opcode
    mod->decode_imm = 0x00000ABE5;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
}

TEST_F(ExecuteShiftTest, Sra) {
    reset();

    mod->decode_opcode = 0b0110011;
    mod->decode_funct3 = 0b101;
    mod->decode_funct7 = 0b0100000;
    mod->read_valid = 1;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 1;
    // Should complete without delay
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, 0xD55DE66E);

    mod->read_valid = 0;
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);

    mod->read_valid = 1;
    mod->read_rs1_val = 0x2ABBCCDD;
    mod->read_rs2_val = 1;
    // Should complete without delay
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, 0x155DE66E);

    mod->read_valid = 1;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 17;
    // Should complete without delay
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, 0xFFFFD55D);
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);

    mod->read_valid = 1;
    mod->read_rs1_val = 0x2ABBCCDD;
    mod->read_rs2_val = 17;
    // Should complete without delay
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, 0x0000155D);

    clk();

    // SRLI
    mod->decode_opcode = 0b0010011;
    mod->decode_funct3 = 0b101;
    mod->decode_funct7 = 0b0;
    mod->read_valid = 1;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 1;
    mod->decode_imm = 0x404;
    // Should complete without delay
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, 0xFAABBCCD);

    clk();

    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 1;
    // Should be an invalid opcode
    mod->decode_imm = 0x00000ABE5;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
}

TEST_F(ExecuteShiftTest, Flush) {
    reset();

    mod->decode_opcode = 0b0110011;
    mod->decode_funct3 = 0b101;
    mod->decode_funct7 = 0b0;
    mod->read_valid = 1;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 27;
    // Should complete without delay
    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->processing, 1);

    clk();
    mod->flush = 1;
    eval();
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->processing, 0);
    clk();
    mod->flush = 0;
    eval();
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->processing, 0);

    clk();
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->processing, 0);

    clk();
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->processing, 0);

    clk();
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->processing, 0);

    clk();
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->processing, 0);

    mod->decode_opcode = 0b0110011;
    mod->decode_funct3 = 0b101;
    mod->decode_funct7 = 0b0;
    mod->read_valid = 1;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 2;
    mod->flush = 1;
    eval();
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->processing, 0);

    mod->decode_opcode = 0b0110011;
    mod->decode_funct3 = 0b101;
    mod->decode_funct7 = 0b0;
    mod->read_valid = 1;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 9;
    mod->flush = 1;
    clk();
    mod->read_valid = 0;
    mod->flush = 0;
    eval();
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->processing, 0);
}

