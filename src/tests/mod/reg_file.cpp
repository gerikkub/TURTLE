
#include "mod_test.hpp"
#include "Vreg_file.h"
#include "Vreg_file__Syms.h"

typedef ClockedModTest<Vreg_file> RegFileTest;

TEST_F(RegFileTest, Reset) {

    reset();

    ASSERT_EQ(mod->rootp->reg_file__DOT__xreg[0], 0);
    ASSERT_EQ(mod->rootp->reg_file__DOT__xreg[1], 0x80000800);
    for (int i = 3; i < 32; i++) {
        ASSERT_EQ(mod->rootp->reg_file__DOT__xreg[i], 0);
    }
}

TEST_F(RegFileTest, ReadWrite) {

    reset();

    mod->write_en_rd = 1;
    mod->write_rd = 27;
    mod->write_val = 0xAABBCDEF;
    clk();

    ASSERT_EQ(mod->rootp->reg_file__DOT__xreg[27-1], 0xAABBCDEF);

    mod->write_en_rd = 0;
    mod->write_rd = 15;
    mod->write_val = 0x12345566;
    clk();

    ASSERT_EQ(mod->rootp->reg_file__DOT__xreg[27-1], 0xAABBCDEF);
    ASSERT_EQ(mod->rootp->reg_file__DOT__xreg[15-1], 0);

    mod->write_en_rd = 1;
    clk();

    ASSERT_EQ(mod->rootp->reg_file__DOT__xreg[27-1], 0xAABBCDEF);
    ASSERT_EQ(mod->rootp->reg_file__DOT__xreg[15-1], 0x12345566);

    mod->write_en_rd = 0;
    mod->read_rs1 = 27;
    mod->read_rs2 = 15;
    eval();

    ASSERT_EQ(mod->rs1_val, 0xAABBCDEF);
    ASSERT_EQ(mod->rs2_val, 0x12345566);
}

TEST_F(RegFileTest, ReadWriteOverride) {

    reset();

    mod->write_en_rd = 1;
    mod->write_rd = 27;
    mod->write_val = 0xAABBCDEF;
    clk();

    ASSERT_EQ(mod->rootp->reg_file__DOT__xreg[27-1], 0xAABBCDEF);

    mod->write_en_rd = 0;
    mod->read_rs1 = 27;
    mod->read_rs2 = 28;
    eval();
    ASSERT_EQ(mod->rs1_val, 0xAABBCDEF);
    ASSERT_EQ(mod->rs2_val, 0);

    mod->write_en_rd = 1;
    mod->write_rd = 27;
    mod->write_val = 0x12345566;
    eval();
    ASSERT_EQ(mod->rs1_val, 0x12345566);
    ASSERT_EQ(mod->rs2_val, 0);

    mod->write_rd = 28;
    eval();
    ASSERT_EQ(mod->rs1_val, 0xAABBCDEF);
    ASSERT_EQ(mod->rs2_val, 0x12345566);

    mod->write_en_rd = 0;
    eval();
    ASSERT_EQ(mod->rs1_val, 0xAABBCDEF);
    ASSERT_EQ(mod->rs2_val, 0);
}

TEST_F(RegFileTest, ReadWriteZero) {

    reset();

    mod->write_en_rd = 1;
    mod->write_rd = 0;
    mod->write_val = 0xAABBCDEF;
    clk();

    mod->write_en_rd = 0;
    mod->read_rs1 = 0;
    mod->read_rs2 = 0;
    eval();
    ASSERT_EQ(mod->rs1_val, 0);
    ASSERT_EQ(mod->rs2_val, 0);

    mod->write_en_rd = 1;
    eval();
    ASSERT_EQ(mod->rs1_val, 0);
    ASSERT_EQ(mod->rs2_val, 0);


}

int main(int argc, char** argv) {

    int res;

    Verilated::commandArgs(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}


