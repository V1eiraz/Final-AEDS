#include "sistemaAnalise.hpp"

#include <chrono>
#include <iostream>

int main(){
    auto inicio = std::chrono::high_resolution_clock::now();

    SistemaAnalise sistema;
    sistema.carregar("data/input.csv");
    sistema.analisar("data/input.dat", "data/output.dat");

    auto fim = std::chrono::high_resolution_clock::now();

    double ms = std::chrono::duration<double, std::milli>(fim - inicio).count();

    std::cout << std::fixed << std::setprecision(3) << ms << " ms\n";
    return 0;
}