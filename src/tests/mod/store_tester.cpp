#include "mod_test.hpp"
#include "Vstore_tester.h"

typedef ModTest<Vstore_tester> StoreTesterTest;

TEST_F(StoreTesterTest, Invalid) {
    eval();

    mod->decode_opcode = 0b0110011;
    mod->decode_funct3 = 0b0;
    mod->read_rs1_val = 15;
    mod->read_rs2_val = 27;
    mod->read_valid = 0;
    eval();
    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);
}

TEST_F(StoreTesterTest, Store) {

    eval();

    mod->decode_opcode = 0b0100011;
    mod->decode_funct3 = 0b010; // SW
    mod->read_rs1_val = 0x80004000; // Address Base
    mod->read_rs2_val = 27; // Store value
    mod->decode_imm = 0x84; // Address Base
    mod->read_valid = 1;
    eval();

    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->exception_valid_out, 0);

    ASSERT_EQ(mod->store_addr, 0x80004084);
    ASSERT_EQ(mod->store_val, 27);
    ASSERT_EQ(mod->store_size, 2);
    ASSERT_EQ(mod->store_valid, 1);

    mod->read_valid = 0;
    eval();

    ASSERT_EQ(mod->processing, 0);
    ASSERT_EQ(mod->valid, 0);

    mod->decode_opcode = 0b0100011;
    mod->decode_funct3 = 0b010; // SW
    mod->read_rs1_val = 0x80002000; // Address Base
    mod->read_rs2_val = 39; // Store value
    mod->decode_imm = 0x14; // Address Base
    mod->read_valid = 1;
    eval();
    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->exception_valid_out, 0);

    ASSERT_EQ(mod->store_addr, 0x80002014);
    ASSERT_EQ(mod->store_val, 39);
    ASSERT_EQ(mod->store_size, 2);
    ASSERT_EQ(mod->store_valid, 1);

    // Store Half
    mod->decode_opcode = 0b0100011;
    mod->decode_funct3 = 0b001; // SH
    mod->read_rs1_val = 0x80004000; // Address Base
    mod->read_rs2_val = 27; // Store value
    mod->decode_imm = 0x82; // Address Base
    mod->read_valid = 1;
    eval();

    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->exception_valid_out, 0);

    ASSERT_EQ(mod->store_addr, 0x80004082);
    ASSERT_EQ(mod->store_val, 27);
    ASSERT_EQ(mod->store_size, 1);
    ASSERT_EQ(mod->store_valid, 1);

    // Store Byte
    mod->decode_opcode = 0b0100011;
    mod->decode_funct3 = 0b000; // SB
    mod->read_rs1_val = 0x80004000; // Address Base
    mod->read_rs2_val = 27; // Store value
    mod->decode_imm = 0x87; // Address Base
    mod->read_valid = 1;
    eval();

    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->exception_valid_out, 0);

    ASSERT_EQ(mod->store_addr, 0x80004087);
    ASSERT_EQ(mod->store_val, 27);
    ASSERT_EQ(mod->store_size, 0);
    ASSERT_EQ(mod->store_valid, 1);
}

TEST_F(StoreTesterTest, Exception) {
    eval();

    mod->decode_opcode = 0b0100011;
    mod->decode_funct3 = 0b010; // SW
    mod->read_rs1_val = 0x80004000; // Address Base
    mod->read_rs2_val = 27; // Store value
    mod->decode_imm = 0x83; // Address Base
    mod->read_valid = 1;
    eval();

    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->exception_valid_out, 1);
    ASSERT_EQ(mod->exception_num_out, 6);

    mod->decode_opcode = 0b0100011;
    mod->decode_funct3 = 0b001; // SH
    mod->read_rs1_val = 0x80004000; // Address Base
    mod->read_rs2_val = 27; // Store value
    mod->decode_imm = 0x83; // Address Base
    mod->read_valid = 1;
    eval();

    ASSERT_EQ(mod->processing, 1);
    ASSERT_EQ(mod->valid, 1);
    ASSERT_EQ(mod->exception_valid_out, 1);
    ASSERT_EQ(mod->exception_num_out, 6);
}

int main(int argc, char** argv) {

    int res;

    Verilated::commandArgs(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

