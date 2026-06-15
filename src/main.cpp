#include <time.h>
#include <iostream>

int main(){
    clock_t inicio = clock();
    clock_t fim = clock();

    double ms = (double)(fim - inicio) * 1000.0 / CLOCKS_PER_SEC;
    std::cout << ms << " ms\n";

    return 0;
}