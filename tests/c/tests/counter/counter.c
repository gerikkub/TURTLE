
#include "test_lib.h"

volatile int my_var = 0;
volatile int my_var2 = 0;

int main(int argc, char** argv) {

    volatile int* i = &my_var;
    volatile int* j = &my_var2;

    while (*j < 10) {
        (*i)++;
        if ((*i) % 16 == 0) {
            (*j)++;
        }
    }

    end_sim_success();

    return 0;
}
