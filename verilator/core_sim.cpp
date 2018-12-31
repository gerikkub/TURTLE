
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vcore.h"
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>

#include "simulation.hpp"

int main(int argc, char** argv) {

    int res;

    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    auto sim = new TurtleSimulation(4096, true);

    res = sim->load_rom("c/tests/main.bin");
    if (!res) {
        return 1;
    }

    sim->run_reset_cycles(5);

    res = sim->run_to_completion(8000);
    if (!res) {
        return 1;
    }

    return 0;

}
