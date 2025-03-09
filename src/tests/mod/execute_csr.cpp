#include "mod_test.hpp"
#include "Vexecute_csr.h"

typedef ClockedModTest<Vexecute_csr> ExecuteCsrTest;

TEST_F(ExecuteCsrTest, Reset) {

    reset();

    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
    ASSERT_EQ(mod->csr_write_valid, 0);
    ASSERT_EQ(mod->exception_num_out, 0);
}

TEST_F(ExecuteCsrTest, WriteOnly) {

    reset();

    mod->decode_opcode = 0b1110011;
    mod->decode_funct3 = 0b001;
    mod->decode_rd = 0;
    mod->decode_rs1 = 1;
    mod->decode_imm = 0xABC;

    mod->read_rs1_val = 0x11223456;
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
    ASSERT_EQ(mod->csr_write_valid, 0);
    ASSERT_EQ(mod->exception_num_out, 0);

    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);

    ASSERT_EQ(mod->csr_write_valid, 1);
    ASSERT_EQ(mod->csr_write_addr, 0xABC);
    ASSERT_EQ(mod->csr_write_val, 0x11223456);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
    ASSERT_EQ(mod->exception_num_out, 0);
}

TEST_F(ExecuteCsrTest, ReadOnly) {
    reset();

    mod->decode_opcode = 0b1110011;
    mod->decode_funct3 = 0b001;
    mod->decode_rd = 1;
    mod->decode_rs1 = 0;
    mod->decode_imm = 0xABC;

    mod->read_rs1_val = 0;
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
    ASSERT_EQ(mod->csr_write_valid, 0);
    ASSERT_EQ(mod->exception_num_out, 0);

    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 1);
    ASSERT_EQ(mod->axil_csr_araddr, 0xABC);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 1);
    ASSERT_EQ(mod->axil_csr_araddr, 0xABC);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    // Note: Nothing should change until clk
    mod->axil_csr_arready = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 1);
    ASSERT_EQ(mod->axil_csr_araddr, 0xABC);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);
    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);

    mod->axil_csr_rdata = 0x98776654;
    mod->axil_csr_rresp = 0;
    mod->axil_csr_rvalid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);
    ASSERT_EQ(mod->rd_val_out, 0x98776654);
    ASSERT_EQ(mod->csr_write_valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
}

TEST_F(ExecuteCsrTest, ReadWrite) {
    reset();

    mod->decode_opcode = 0b1110011;
    mod->decode_funct3 = 0b001;
    mod->decode_rd = 1;
    mod->decode_rs1 = 2;
    mod->decode_imm = 0xABC;

    mod->read_rs1_val = 0xAABBCDEF;
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
    ASSERT_EQ(mod->csr_write_valid, 0);
    ASSERT_EQ(mod->exception_num_out, 0);

    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 1);
    ASSERT_EQ(mod->axil_csr_araddr, 0xABC);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    mod->axil_csr_arready = 1;
    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);

    mod->axil_csr_rdata = 0x98776654;
    mod->axil_csr_rresp = 0;
    mod->axil_csr_rvalid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);
    ASSERT_EQ(mod->rd_val_out, 0x98776654);
    ASSERT_EQ(mod->csr_write_valid, 1);
    ASSERT_EQ(mod->csr_write_addr, 0xABC);
    ASSERT_EQ(mod->csr_write_val, 0xAABBCDEF);
    ASSERT_EQ(mod->exception_valid_out, 0);
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
}

TEST_F(ExecuteCsrTest, Set) {
    reset();

    mod->decode_opcode = 0b1110011;
    mod->decode_funct3 = 0b010;
    mod->decode_rd = 1;
    mod->decode_rs1 = 2;
    mod->decode_imm = 0xABC;

    mod->read_rs1_val = 0xFF00AA66;
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
    ASSERT_EQ(mod->csr_write_valid, 0);
    ASSERT_EQ(mod->exception_num_out, 0);

    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 1);
    ASSERT_EQ(mod->axil_csr_araddr, 0xABC);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    mod->axil_csr_arready = 1;
    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);

    mod->axil_csr_rdata = 0x98776654;
    mod->axil_csr_rresp = 0;
    mod->axil_csr_rvalid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);
    ASSERT_EQ(mod->rd_val_out, 0x98776654);
    ASSERT_EQ(mod->csr_write_valid, 1);
    ASSERT_EQ(mod->csr_write_addr, 0xABC);
    ASSERT_EQ(mod->csr_write_val, (0x98776654 | 0xFF00AA66));
    ASSERT_EQ(mod->exception_valid_out, 0);
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
}

TEST_F(ExecuteCsrTest, Clear) {
    reset();

    mod->decode_opcode = 0b1110011;
    mod->decode_funct3 = 0b011;
    mod->decode_rd = 1;
    mod->decode_rs1 = 2;
    mod->decode_imm = 0xABC;

    mod->read_rs1_val = 0xFF00AA66;
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
    ASSERT_EQ(mod->csr_write_valid, 0);
    ASSERT_EQ(mod->exception_num_out, 0);

    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 1);
    ASSERT_EQ(mod->axil_csr_araddr, 0xABC);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    mod->axil_csr_arready = 1;
    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);

    mod->axil_csr_rdata = 0x98776654;
    mod->axil_csr_rresp = 0;
    mod->axil_csr_rvalid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);
    ASSERT_EQ(mod->rd_val_out, 0x98776654);
    ASSERT_EQ(mod->csr_write_valid, 1);
    ASSERT_EQ(mod->csr_write_addr, 0xABC);
    ASSERT_EQ(mod->csr_write_val, (0x98776654 & (~0xFF00AA66)));
    ASSERT_EQ(mod->exception_valid_out, 0);
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
}

TEST_F(ExecuteCsrTest, ReadWriteImm) {
    reset();

    mod->decode_opcode = 0b1110011;
    mod->decode_funct3 = 0b101;
    mod->decode_rd = 1;
    mod->decode_rs1 = 0x14;
    mod->decode_imm = 0xABC;

    mod->read_rs1_val = 0xAABBCDEF;
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
    ASSERT_EQ(mod->csr_write_valid, 0);
    ASSERT_EQ(mod->exception_num_out, 0);

    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 1);
    ASSERT_EQ(mod->axil_csr_araddr, 0xABC);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    mod->axil_csr_arready = 1;
    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);

    mod->axil_csr_rdata = 0x98776654;
    mod->axil_csr_rresp = 0;
    mod->axil_csr_rvalid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);
    ASSERT_EQ(mod->rd_val_out, 0x98776654);
    ASSERT_EQ(mod->csr_write_valid, 1);
    ASSERT_EQ(mod->csr_write_addr, 0xABC);
    ASSERT_EQ(mod->csr_write_val, 0x14);
    ASSERT_EQ(mod->exception_valid_out, 0);
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
}

TEST_F(ExecuteCsrTest, SetImm) {
    reset();

    mod->decode_opcode = 0b1110011;
    mod->decode_funct3 = 0b110;
    mod->decode_rd = 1;
    mod->decode_rs1 = 0x14;
    mod->decode_imm = 0xABC;

    mod->read_rs1_val = 0xAABBCDEF;
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
    ASSERT_EQ(mod->csr_write_valid, 0);
    ASSERT_EQ(mod->exception_num_out, 0);

    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 1);
    ASSERT_EQ(mod->axil_csr_araddr, 0xABC);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    mod->axil_csr_arready = 1;
    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);

    mod->axil_csr_rdata = 0x98776654;
    mod->axil_csr_rresp = 0;
    mod->axil_csr_rvalid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);
    ASSERT_EQ(mod->rd_val_out, 0x98776654);
    ASSERT_EQ(mod->csr_write_valid, 1);
    ASSERT_EQ(mod->csr_write_addr, 0xABC);
    ASSERT_EQ(mod->csr_write_val, (0x98776654 | 0x14));
    ASSERT_EQ(mod->exception_valid_out, 0);
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
}

TEST_F(ExecuteCsrTest, SetImmZero) {
    reset();

    mod->decode_opcode = 0b1110011;
    mod->decode_funct3 = 0b110;
    mod->decode_rd = 1;
    mod->decode_rs1 = 0;
    mod->decode_imm = 0xABC;

    mod->read_rs1_val = 0xAABBCDEF;
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
    ASSERT_EQ(mod->csr_write_valid, 0);
    ASSERT_EQ(mod->exception_num_out, 0);

    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 1);
    ASSERT_EQ(mod->axil_csr_araddr, 0xABC);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    mod->axil_csr_arready = 1;
    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);

    mod->axil_csr_rdata = 0x98776654;
    mod->axil_csr_rresp = 0;
    mod->axil_csr_rvalid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);
    ASSERT_EQ(mod->rd_val_out, 0x98776654);
    ASSERT_EQ(mod->csr_write_valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
}

TEST_F(ExecuteCsrTest, ClearImm) {
    reset();

    mod->decode_opcode = 0b1110011;
    mod->decode_funct3 = 0b111;
    mod->decode_rd = 1;
    mod->decode_rs1 = 0x14;
    mod->decode_imm = 0xABC;

    mod->read_rs1_val = 0xAABBCDEF;
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
    ASSERT_EQ(mod->csr_write_valid, 0);
    ASSERT_EQ(mod->exception_num_out, 0);

    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 1);
    ASSERT_EQ(mod->axil_csr_araddr, 0xABC);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    mod->axil_csr_arready = 1;
    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);

    mod->axil_csr_rdata = 0x98776654;
    mod->axil_csr_rresp = 0;
    mod->axil_csr_rvalid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);
    ASSERT_EQ(mod->rd_val_out, 0x98776654);
    ASSERT_EQ(mod->csr_write_valid, 1);
    ASSERT_EQ(mod->csr_write_addr, 0xABC);
    ASSERT_EQ(mod->csr_write_val, (0x98776654 & (~0x14)));
    ASSERT_EQ(mod->exception_valid_out, 0);
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
}

TEST_F(ExecuteCsrTest, ClearImmZero) {
    reset();

    mod->decode_opcode = 0b1110011;
    mod->decode_funct3 = 0b111;
    mod->decode_rd = 1;
    mod->decode_rs1 = 0;
    mod->decode_imm = 0xABC;

    mod->read_rs1_val = 0xAABBCDEF;
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
    ASSERT_EQ(mod->csr_write_valid, 0);
    ASSERT_EQ(mod->exception_num_out, 0);

    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 1);
    ASSERT_EQ(mod->axil_csr_araddr, 0xABC);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    mod->axil_csr_arready = 1;
    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);

    mod->axil_csr_rdata = 0x98776654;
    mod->axil_csr_rresp = 0;
    mod->axil_csr_rvalid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);
    ASSERT_EQ(mod->rd_val_out, 0x98776654);
    ASSERT_EQ(mod->csr_write_valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
}

TEST_F(ExecuteCsrTest, ReadWriteFlush) {
    reset();

    // Flush during Address Read
    mod->decode_opcode = 0b1110011;
    mod->decode_funct3 = 0b001;
    mod->decode_rd = 1;
    mod->decode_rs1 = 2;
    mod->decode_imm = 0xABC;

    mod->read_rs1_val = 0xAABBCDEF;
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
    ASSERT_EQ(mod->csr_write_valid, 0);
    ASSERT_EQ(mod->exception_num_out, 0);

    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 1);
    ASSERT_EQ(mod->axil_csr_araddr, 0xABC);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    mod->axil_csr_arready = 0;
    clk();

    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 1);
    ASSERT_EQ(mod->axil_csr_araddr, 0xABC);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    mod->flush = 1;
    eval();

    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    clk();
    mod->flush = 0;
    eval();

    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    // Flush during Read
    mod->decode_opcode = 0b1110011;
    mod->decode_funct3 = 0b001;
    mod->decode_rd = 1;
    mod->decode_rs1 = 2;
    mod->decode_imm = 0xABC;

    mod->read_rs1_val = 0xAABBCDEF;
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
    ASSERT_EQ(mod->csr_write_valid, 0);
    ASSERT_EQ(mod->exception_num_out, 0);

    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 1);
    ASSERT_EQ(mod->axil_csr_araddr, 0xABC);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    mod->axil_csr_arready = 1;
    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);

    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 1);

    mod->flush = 1;
    eval();

    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);

    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);

    clk();
    mod->flush = 0;
    eval();

    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->axil_csr_arvalid, 0);
    ASSERT_EQ(mod->axil_csr_rready, 0);
}


