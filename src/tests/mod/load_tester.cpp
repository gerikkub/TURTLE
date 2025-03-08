#include "mod_test.hpp"
#include "Vload_tester.h"

typedef ClockedModTest<Vload_tester> LoadTesterTest;

TEST_F(LoadTesterTest, Reset) {

    reset();

    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
}

TEST_F(LoadTesterTest, LoadWord) {
    reset();

    mod->mem_data_in = 0;
    mod->mem_data_valid = 0;
    mod->mem_data_access_fault = 0;

    mod->decode_opcode = 0b0000011;
    mod->decode_funct3 = 0b010;
    mod->decode_imm = 0x7BC;

    mod->read_rs1_val = 0x8004300;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->mem_data_addr_valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    ASSERT_EQ(mod->mem_data_addr_valid, 1);
    ASSERT_EQ(mod->mem_data_addr, 0x8004ABC);
    ASSERT_EQ(mod->mem_data_size, 2);

    clk();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    ASSERT_EQ(mod->mem_data_addr_valid, 1);
    ASSERT_EQ(mod->mem_data_addr, 0x8004ABC);
    ASSERT_EQ(mod->mem_data_size, 2);

    mod->mem_data_in = 0x8899ABCD;
    mod->mem_data_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->exception_valid_out, 0);

    ASSERT_EQ(mod->rd_val, 0x8899ABCD);
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);
    ASSERT_EQ(mod->mem_data_addr_valid, 0);

}

TEST_F(LoadTesterTest, LoadByte) {
    reset();

    mod->mem_data_in = 0;
    mod->mem_data_valid = 0;
    mod->mem_data_access_fault = 0;

    mod->decode_opcode = 0b0000011;
    mod->decode_funct3 = 0b000;
    mod->decode_imm = 0x7BF;

    mod->read_rs1_val = 0x8004300;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->mem_data_addr_valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    ASSERT_EQ(mod->mem_data_addr_valid, 1);
    ASSERT_EQ(mod->mem_data_addr, 0x8004ABF);
    ASSERT_EQ(mod->mem_data_size, 0);

    mod->mem_data_in = 0xAB;
    mod->mem_data_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->exception_valid_out, 0);

    ASSERT_EQ(mod->rd_val, 0xFFFFFFAB);
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);
    ASSERT_EQ(mod->mem_data_addr_valid, 0);

    mod->mem_data_in = 0;
    mod->mem_data_valid = 0;
    mod->mem_data_access_fault = 0;

    mod->decode_opcode = 0b0000011;
    mod->decode_funct3 = 0b000;
    mod->decode_imm = 0x7BE;

    mod->read_rs1_val = 0x8004300;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->mem_data_addr_valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    ASSERT_EQ(mod->mem_data_addr_valid, 1);
    ASSERT_EQ(mod->mem_data_addr, 0x8004ABE);
    ASSERT_EQ(mod->mem_data_size, 0);

    mod->mem_data_in = 0x6B;
    mod->mem_data_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->exception_valid_out, 0);

    ASSERT_EQ(mod->rd_val, 0x0000006B);
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);
    ASSERT_EQ(mod->mem_data_addr_valid, 0);
}

TEST_F(LoadTesterTest, LoadHalf) {
    reset();

    mod->mem_data_in = 0;
    mod->mem_data_valid = 0;
    mod->mem_data_access_fault = 0;

    mod->decode_opcode = 0b0000011;
    mod->decode_funct3 = 0b001;
    mod->decode_imm = 0x7BE;

    mod->read_rs1_val = 0x8004300;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->mem_data_addr_valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    ASSERT_EQ(mod->mem_data_addr_valid, 1);
    ASSERT_EQ(mod->mem_data_addr, 0x8004ABE);
    ASSERT_EQ(mod->mem_data_size, 1);

    mod->mem_data_in = 0xABCD;
    mod->mem_data_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->exception_valid_out, 0);

    ASSERT_EQ(mod->rd_val, 0xFFFFABCD);
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);
    ASSERT_EQ(mod->mem_data_addr_valid, 0);

    mod->mem_data_in = 0;
    mod->mem_data_valid = 0;
    mod->mem_data_access_fault = 0;

    mod->decode_opcode = 0b0000011;
    mod->decode_funct3 = 0b001;
    mod->decode_imm = 0x7BE;

    mod->read_rs1_val = 0x8004300;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->mem_data_addr_valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    ASSERT_EQ(mod->mem_data_addr_valid, 1);
    ASSERT_EQ(mod->mem_data_addr, 0x8004ABE);
    ASSERT_EQ(mod->mem_data_size, 1);

    mod->mem_data_in = 0x7BCD;
    mod->mem_data_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->exception_valid_out, 0);

    ASSERT_EQ(mod->rd_val, 0x00007BCD);
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);
    ASSERT_EQ(mod->mem_data_addr_valid, 0);
}

TEST_F(LoadTesterTest, LoadHalfU) {
    reset();

    mod->mem_data_in = 0;
    mod->mem_data_valid = 0;
    mod->mem_data_access_fault = 0;

    mod->decode_opcode = 0b0000011;
    mod->decode_funct3 = 0b101;
    mod->decode_imm = 0x7BE;

    mod->read_rs1_val = 0x8004300;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->mem_data_addr_valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    ASSERT_EQ(mod->mem_data_addr_valid, 1);
    ASSERT_EQ(mod->mem_data_addr, 0x8004ABE);
    ASSERT_EQ(mod->mem_data_size, 1);

    mod->mem_data_in = 0xABCD;
    mod->mem_data_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->exception_valid_out, 0);

    ASSERT_EQ(mod->rd_val, 0xABCD);
    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);
    ASSERT_EQ(mod->mem_data_addr_valid, 0);
}

TEST_F(LoadTesterTest, Exception) {
    reset();

    mod->mem_data_in = 0;
    mod->mem_data_valid = 0;
    mod->mem_data_access_fault = 0;

    mod->decode_opcode = 0b0000011;
    mod->decode_funct3 = 0b010;
    mod->decode_imm = 0x7BC;

    mod->read_rs1_val = 0x8004300;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->mem_data_addr_valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    ASSERT_EQ(mod->mem_data_addr_valid, 1);
    ASSERT_EQ(mod->mem_data_addr, 0x8004ABC);
    ASSERT_EQ(mod->mem_data_size, 2);

    mod->mem_data_in = 0;
    mod->mem_data_valid = 1;
    mod->mem_data_access_fault = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->exception_valid_out, 1);
    ASSERT_EQ(mod->exception_num_out, 5);

    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);
    ASSERT_EQ(mod->mem_data_addr_valid, 0);
}

TEST_F(LoadTesterTest, Flush) {
    reset();

    mod->mem_data_in = 0;
    mod->mem_data_valid = 0;
    mod->mem_data_access_fault = 0;

    mod->decode_opcode = 0b0000011;
    mod->decode_funct3 = 0b010;
    mod->decode_imm = 0x7BC;

    mod->read_rs1_val = 0x8004300;
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->mem_data_addr_valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    clk();
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    ASSERT_EQ(mod->mem_data_addr_valid, 1);
    ASSERT_EQ(mod->mem_data_addr, 0x8004ABC);
    ASSERT_EQ(mod->mem_data_size, 2);

    mod->mem_data_in = 0x8899ABCD;
    mod->mem_data_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->exception_valid_out, 0);
    ASSERT_EQ(mod->rd_val, 0x8899ABCD);

    mod->flush = 1;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);

    clk();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
    ASSERT_EQ(mod->exception_valid_out, 0);
    ASSERT_EQ(mod->mem_data_addr_valid, 0);
}

