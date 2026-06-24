#include "sistemaAnalise.hpp"
#include <ctime>
#include <iostream>

int main() {
    struct timespec inicio, fim;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    SistemaAnalise sistema;
    sistema.carregar("data/input.csv");
    sistema.analisar("data/input.dat", "data/output.dat");

    clock_gettime(CLOCK_MONOTONIC, &fim);

    long ms = (fim.tv_sec  - inicio.tv_sec)  * 1000
            + (fim.tv_nsec - inicio.tv_nsec) / 1000000;

    std::cout << ms << " ms\n";
    return 0;
}