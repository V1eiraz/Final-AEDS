#include "sistemaAnalise.hpp"

#include <chrono>
#include <iostream>

int main(){
    auto inicio = std::chrono::high_resolution_clock::now();

    SistemaAnalise sistema;
    sistema.carregar("data/input.csv");
    sistema.analisar("data/input.dat", "data/output.dat");

    auto fim = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(fim - inicio).count();

    std::cout << ms << " ms\n";
    return 0;
}