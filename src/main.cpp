#include "sistemaAnalise.hpp"

#include <time.h>
#include <iostream>

#include "dicionario.hpp"
#include "processador.hpp"
#include "janelas.hpp"
#include "config.hpp"

int main(){
    clock_t inicio = clock();

    SistemaAnalise sistema;
    sistema.carregar("data/input.csv");
    sistema.analisar("data/input.dat", "data/output.dat");
    
    clock_t fim = clock();
    double ms = (double)(fim - inicio) * 1000.0 / CLOCKS_PER_SEC;
    std::cout << ms << " ms\n";

    return 0;
}