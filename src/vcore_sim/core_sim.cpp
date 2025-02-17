
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vcore.h"
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>

#include <gtest/gtest.h>

#include "simulation.hpp"


TEST(CTest, Counter) {

    auto sim = std::make_unique<TurtleSimulation>(4096, true);

    ASSERT_NE(sim, nullptr);

    ASSERT_TRUE(sim->load_rom("tests/bin/counter.bin"));

    sim->run_reset_cycles(5);

    ASSERT_TRUE(sim->run_to_completion(8000));
}

TEST(CTest, CSR) {

    auto sim = std::make_unique<TurtleSimulation>(4096, false);

    ASSERT_NE(sim, nullptr);

    ASSERT_TRUE(sim->load_rom("tests/bin/csr.bin"));

    sim->run_reset_cycles(5);

    ASSERT_TRUE(sim->run_to_completion(8000));
}


int main(int argc, char** argv) {

    int res;

    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
