
volatile int my_var = 0;
volatile int my_var2 = 0;

extern volatile unsigned char __magic_sim_end;

void end_sim() {
	__magic_sim_end = 0xA5;
}

int main(int argc, char** argv) {

    volatile int* i = &my_var;
    volatile int* j = &my_var2;

    while (*j < 10) {
        (*i)++;
        if ((*i) % 16 == 0) {
            (*j)++;
        }
    }

    end_sim();

    return 0;
}
