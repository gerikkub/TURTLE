
#include "mod_test.hpp"
#include "Valu_tester.h"

typedef ModTest<Valu_tester> AluTesterTest;

TEST_F(AluTesterTest, Invalid) {
    eval();

    mod->decode_opcode = 0b0110011;
    mod->decode_funct3 = 0b0;
    mod->decode_funct7 = 0;
    mod->read_rs1_val = 15;
    mod->read_rs2_val = 27;
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
}

TEST_F(AluTesterTest, UnknownOp) {
    eval();

    mod->decode_opcode = 0b0110011;
    mod->decode_funct3 = 0b001;
    mod->decode_funct7 = 0;
    mod->read_rs1_val = 15;
    mod->read_rs2_val = 27;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->rd_val_out, 0x003c0de);
}

TEST_F(AluTesterTest, Add) {
    eval();

    mod->decode_opcode = 0b0110011;
    mod->decode_funct3 = 0b0;
    mod->decode_funct7 = 0;
    mod->read_rs1_val = 15;
    mod->read_rs2_val = 27;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (15+27));

    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 0xFFEEDD11;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (mod->read_rs1_val +
                                mod->read_rs2_val));

    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 0xFFFFFFFF;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, 0xAABBCCDC);

    // ADDI
    mod->decode_opcode = 0b0010011;
    mod->decode_funct3 = 0b0;
    mod->decode_funct7 = 0;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->decode_imm = 0xABC;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, 0xAABBCCDD + 0xABC);
}

TEST_F(AluTesterTest, Sub) {
    eval();

    mod->decode_opcode = 0b0110011;
    mod->decode_funct3 = 0b0;
    mod->decode_funct7 = 0b0100000;
    mod->read_rs1_val = 15;
    mod->read_rs2_val = 27;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (15-27));

    mod->decode_opcode = 0b0110011;
    mod->decode_funct3 = 0b0;
    mod->decode_funct7 = 0b0100000;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 0xFFFFFFFF;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, 0xAABBCCDE);

    mod->decode_opcode = 0b0110011;
    mod->decode_funct3 = 0b0;
    mod->decode_funct7 = 0b0100000;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 0xAABBCCDD;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, 0);
}

TEST_F(AluTesterTest, Logic) {
    eval();

    // OR
    mod->decode_opcode = 0b0110011;
    mod->decode_funct3 = 0b110;
    mod->decode_funct7 = 0;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 0xFF00AAEE;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (0xAABBCCDD | 0xFF00AAEE));

    // AND
    mod->decode_opcode = 0b0110011;
    mod->decode_funct3 = 0b111;
    mod->decode_funct7 = 0;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 0xFF00AAEE;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (0xAABBCCDD & 0xFF00AAEE));

    // XOR
    mod->decode_opcode = 0b0110011;
    mod->decode_funct3 = 0b100;
    mod->decode_funct7 = 0;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 0xFF00AAEE;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (0xAABBCCDD ^ 0xFF00AAEE));

    // ORI
    mod->decode_opcode = 0b0010011;
    mod->decode_funct3 = 0b110;
    mod->decode_funct7 = 0;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 0xFF00AAEE;
    mod->decode_imm = 0xABC;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (0xAABBCCDD | 0xABC));

    // ANDI
    mod->decode_opcode = 0b0010011;
    mod->decode_funct3 = 0b111;
    mod->decode_funct7 = 0;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 0xFF00AAEE;
    mod->decode_imm = 0xABC;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (0xAABBCCDD & 0xABC));

    // XORI
    mod->decode_opcode = 0b0010011;
    mod->decode_funct3 = 0b100;
    mod->decode_funct7 = 0;
    mod->read_rs1_val = 0xAABBCCDD;
    mod->read_rs2_val = 0xFF00AAEE;
    mod->decode_imm = 0xABC;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, (0xAABBCCDD ^ 0xABC));
}

TEST_F(AluTesterTest, Sltu) {
    eval();

    mod->decode_opcode = 0b0110011;
    mod->decode_funct3 = 0b011;
    mod->decode_funct7 = 0;
    mod->read_rs1_val = 15;
    mod->read_rs2_val = 27;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, 1);
    ASSERT_EQ(mod->ltu, 1);
    ASSERT_EQ(mod->eq, 0);

    mod->read_rs1_val = 27;
    mod->read_rs2_val = 15;
    eval();
    ASSERT_EQ(mod->rd_val_out, 0);
    ASSERT_EQ(mod->ltu, 0);
    ASSERT_EQ(mod->eq, 0);

    mod->read_rs1_val = 0x80ABCDEF;
    mod->read_rs2_val = 15;
    eval();
    ASSERT_EQ(mod->rd_val_out, 0);
    ASSERT_EQ(mod->ltu, 0);
    ASSERT_EQ(mod->eq, 0);

    mod->read_rs1_val = 27;
    mod->read_rs2_val = 0xFFFFFFFF;
    eval();
    ASSERT_EQ(mod->rd_val_out, 1);
    ASSERT_EQ(mod->ltu, 1);
    ASSERT_EQ(mod->eq, 0);

    mod->read_rs1_val = 0x80ABCDEF;
    mod->read_rs2_val = 0xFFFFFFFF;
    eval();
    ASSERT_EQ(mod->rd_val_out, 1);
    ASSERT_EQ(mod->ltu, 1);
    ASSERT_EQ(mod->eq, 0);

    mod->read_rs1_val = 0xFFFFFFFF;
    mod->read_rs2_val = 0x80ABCDEF;
    eval();
    ASSERT_EQ(mod->rd_val_out, 0);
    ASSERT_EQ(mod->ltu, 0);
    ASSERT_EQ(mod->eq, 0);

    mod->read_rs1_val = 27;
    mod->read_rs2_val = 27;
    eval();
    ASSERT_EQ(mod->rd_val_out, 0);
    ASSERT_EQ(mod->ltu, 0);
    ASSERT_EQ(mod->eq, 1);

    mod->read_rs1_val = 0x80ABCDEF;
    mod->read_rs2_val = 0x80ABCDEF;
    eval();
    ASSERT_EQ(mod->rd_val_out, 0);
    ASSERT_EQ(mod->ltu, 0);
    ASSERT_EQ(mod->eq, 1);

    // SLTIU
    mod->decode_opcode = 0b0010011;
    mod->decode_funct3 = 0b011;
    mod->decode_funct7 = 0;
    mod->read_rs1_val = 27;
    mod->read_rs2_val = 0xFFFFFFFF;
    mod->decode_imm = 12;
    eval();
    ASSERT_EQ(mod->rd_val_out, 0);
    ASSERT_EQ(mod->ltu, 0);
    ASSERT_EQ(mod->eq, 0);

    mod->read_rs1_val = 27;
    mod->read_rs2_val = 0xFFFFFFFF;
    mod->decode_imm = 0xFFFFFFAB;
    eval();
    ASSERT_EQ(mod->rd_val_out, 1);
    ASSERT_EQ(mod->ltu, 1);
    ASSERT_EQ(mod->eq, 0);
}

TEST_F(AluTesterTest, Slt) {
    eval();

    mod->decode_opcode = 0b0110011;
    mod->decode_funct3 = 0b010;
    mod->decode_funct7 = 0;
    mod->read_rs1_val = 15;
    mod->read_rs2_val = 27;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, 1);
    ASSERT_EQ(mod->lt, 1);
    ASSERT_EQ(mod->eq, 0);

    mod->read_rs1_val = 27;
    mod->read_rs2_val = 15;
    eval();
    ASSERT_EQ(mod->rd_val_out, 0);
    ASSERT_EQ(mod->lt, 0);
    ASSERT_EQ(mod->eq, 0);

    mod->read_rs1_val = 0x80ABCDEF;
    mod->read_rs2_val = 15;
    eval();
    ASSERT_EQ(mod->rd_val_out, 1);
    ASSERT_EQ(mod->lt, 1);
    ASSERT_EQ(mod->eq, 0);

    mod->read_rs1_val = 27;
    mod->read_rs2_val = 0xFFFFFFFF;
    eval();
    ASSERT_EQ(mod->rd_val_out, 0);
    ASSERT_EQ(mod->lt, 0);
    ASSERT_EQ(mod->eq, 0);

    mod->read_rs1_val = 0x80ABCDEF;
    mod->read_rs2_val = 0xFFFFFFFF;
    eval();
    ASSERT_EQ(mod->rd_val_out, 1);
    ASSERT_EQ(mod->lt, 1);
    ASSERT_EQ(mod->eq, 0);

    mod->read_rs1_val = 0xFFFFFFFF;
    mod->read_rs2_val = 0x80ABCDEF;
    eval();
    ASSERT_EQ(mod->rd_val_out, 0);
    ASSERT_EQ(mod->lt, 0);
    ASSERT_EQ(mod->eq, 0);

    mod->read_rs1_val = 27;
    mod->read_rs2_val = 27;
    eval();
    ASSERT_EQ(mod->rd_val_out, 0);
    ASSERT_EQ(mod->lt, 0);
    ASSERT_EQ(mod->eq, 1);

    mod->read_rs1_val = 0x80ABCDEF;
    mod->read_rs2_val = 0x80ABCDEF;
    eval();
    ASSERT_EQ(mod->rd_val_out, 0);
    ASSERT_EQ(mod->lt, 0);
    ASSERT_EQ(mod->eq, 1);

    // SLTI
    mod->decode_opcode = 0b0010011;
    mod->decode_funct3 = 0b010;
    mod->decode_funct7 = 0;
    mod->read_rs1_val = 27;
    mod->read_rs2_val = 30;
    mod->decode_imm = 12;
    eval();
    ASSERT_EQ(mod->rd_val_out, 0);
    ASSERT_EQ(mod->lt, 0);
    ASSERT_EQ(mod->eq, 0);

    mod->read_rs1_val = 27;
    mod->read_rs2_val = 30;
    mod->decode_imm = 0xFFFFFFAB;
    eval();
    ASSERT_EQ(mod->rd_val_out, 0);
    ASSERT_EQ(mod->lt, 0);
    ASSERT_EQ(mod->eq, 0);

    mod->read_rs1_val = 0x80ABCDEF;
    mod->read_rs2_val = 30;
    mod->decode_imm = 0xFFFFFFAB;
    eval();
    ASSERT_EQ(mod->rd_val_out, 1);
    ASSERT_EQ(mod->lt, 1);
    ASSERT_EQ(mod->eq, 0);

    mod->read_rs1_val = 0xFFFFFAB;
    mod->read_rs2_val = 30;
    mod->decode_imm = 0xFFFFF8AB;
    eval();
    ASSERT_EQ(mod->rd_val_out, 0);
    ASSERT_EQ(mod->lt, 0);
    ASSERT_EQ(mod->eq, 0);
}

TEST_F(AluTesterTest, Lui) {
    eval();

    mod->decode_opcode = 0b0110111;
    mod->decode_imm = 0xABCDF000;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, 0xABCDF000);
}

TEST_F(AluTesterTest, Auipc) {
    eval();

    mod->decode_opcode = 0b0010111;
    mod->decode_imm = 0xABCDF000;
    mod->decode_pc = 0x00000123;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->rd_val_out, 0xABCDF123);

    mod->decode_imm = 0xABCDF000;
    mod->decode_pc = 0x00002123;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->rd_val_out, 0xABCE1123);
}

