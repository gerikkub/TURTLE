
#include "mod_test.hpp"
#include "Vdecoder.h"

typedef ModTest<Vdecoder> DecoderTest;

TEST_F(DecoderTest, All) {
    for (int i = 0; i < 16; i++) {
        mod->bus_in = i;
        eval();
        ASSERT_EQ(mod->data_out, 1UL << i);
    }
}

