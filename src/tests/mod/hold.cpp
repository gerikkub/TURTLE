#include "mod_test.hpp"
#include "Vhold.h"

typedef ClockedModTest<Vhold> HoldTest;

TEST_F(HoldTest, Reset) {

    reset();

    ASSERT_EQ(mod->data_out, 0);
    ASSERT_EQ(mod->valid_out, 0);
}

TEST_F(HoldTest, Passthrough) {

    reset();

    mod->data_in = 0xFA;
    mod->input_valid = 1;
    mod->stall = 0;
    eval();
    ASSERT_EQ(mod->data_out, 0xFA);
    ASSERT_EQ(mod->valid_out, 1);

    mod->data_in = 0x12;
    eval();
    ASSERT_EQ(mod->data_out, 0x12);
    ASSERT_EQ(mod->valid_out, 1);

    mod->data_in = 0x98;
    eval();
    ASSERT_EQ(mod->data_out, 0x98);
    ASSERT_EQ(mod->valid_out, 1);

    mod->data_in = 0x54;
    eval();
    ASSERT_EQ(mod->data_out, 0x54);
    ASSERT_EQ(mod->valid_out, 1);

    mod->stall = 1;
    eval();
    ASSERT_EQ(mod->valid_out, 0);
}

TEST_F(HoldTest, Hold) {

    reset();

    // One cycle hold
    mod->data_in = 0xAB;
    mod->input_valid = 1;
    mod->stall = 1;
    eval();
    ASSERT_EQ(mod->valid_out, 0);
    clk();

    mod->stall = 0;
    mod->input_valid = 0;
    eval();
    ASSERT_EQ(mod->data_out, 0xAB);
    ASSERT_EQ(mod->valid_out, 1);

    clk();
    ASSERT_EQ(mod->valid_out, 0);

    // Multi cycle hold
    mod->data_in = 0xF4;
    mod->input_valid = 1;
    mod->stall = 1;
    eval();
    ASSERT_EQ(mod->valid_out, 0);
    clk();

    mod->input_valid = 0;
    clk();
    ASSERT_EQ(mod->valid_out, 0);

    clk();
    ASSERT_EQ(mod->valid_out, 0);
    clk();
    ASSERT_EQ(mod->valid_out, 0);
    clk();
    ASSERT_EQ(mod->valid_out, 0);

    mod->stall = 0;
    eval();
    ASSERT_EQ(mod->data_out, 0xF4);
    ASSERT_EQ(mod->valid_out, 1);

    clk();
    ASSERT_EQ(mod->valid_out, 0);

    mod->data_in = 0xCD;
    mod->input_valid = 1;
    mod->stall = 0;
    eval();
    ASSERT_EQ(mod->data_out, 0xCD);
    ASSERT_EQ(mod->valid_out, 1);
}

int main(int argc, char** argv) {

    int res;

    Verilated::commandArgs(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
