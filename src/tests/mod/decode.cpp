
#include "mod_test.hpp"
#include "Vdecode.h"

typedef ClockedModTest<Vdecode> DecodeTest;

TEST_F(DecodeTest, Reset) {

    reset();

    ASSERT_EQ(mod->fetch_stall, 0);
    ASSERT_EQ(mod->valid, 0);
}

int main(int argc, char** argv) {

    int res;

    Verilated::commandArgs(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
