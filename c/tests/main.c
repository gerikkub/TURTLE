
volatile int my_var = 0;
volatile int my_var2 = 0;

int main(int argc, char** argv) {

    volatile int* i = &my_var;
    volatile int* j = &my_var2;

    while (1) {
        (*i)++;
        if ((*i) % 16 == 0) {
            (*j)++;
        }
    }

    return 0;
}
