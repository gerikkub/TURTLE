
#include <stdint.h>

extern volatile uint8_t __magic_sim_end_success;

extern volatile uint32_t __magic_sim_end_failure;
extern volatile uint32_t __magic_sim_failure_expected;
extern volatile uint32_t __magic_sim_failure_actual;

void end_sim_success() {
    __magic_sim_end_success = 0xA5;
}

void end_sim_failure(int line) {
    __magic_sim_end_failure = line;
}

void end_sim_failure_exp(int line, int expected, int actual) {
    __magic_sim_failure_expected = expected;
    __magic_sim_failure_actual = actual;
    __magic_sim_end_failure = line;
}

