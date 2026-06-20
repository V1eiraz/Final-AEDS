#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "dicionario.hpp"
#include "processador.hpp"
#include "janelas.hpp"
#include "config.hpp"

int main() {
    clock_t inicio = clock();

    // Conta o total de linhas do CSV (necessário para dividir as janelas)
    int totalLinhas = 0;
    {
        std::ifstream f("data/input.csv");
        std::string linha;
        while (std::getline(f, linha)) totalLinhas++;
    }
    std::cout << "Total de linhas: " << totalLinhas << "\n";

    // Inicializa as estruturas
    Dicionario dicionario;
    dicionario.reservar(CAPACIDADE_DICIONARIO);

    Processador processador;
    Janelas janelas;

    // Vetor reutilizado a cada linha para receber os IDs
    std::vector<uint32_t> destinoPlano;

    // Lê e processa o CSV linha a linha
    std::ifstream csv("data/input.csv");
    std::string linha;
    int lineNumber = 0;

    while (std::getline(csv, linha)) {
        destinoPlano.clear();

        // Processa a manchete => preenche destinoPlano com IDs
        processador.processar(linha, dicionario, destinoPlano);

        // Registra na janela certa
        janelas.addManchete(lineNumber, totalLinhas, destinoPlano);

        lineNumber++;
    }

    clock_t fim = clock();
    double ms = (double)(fim - inicio) * 1000.0 / CLOCKS_PER_SEC;
    std::cout << ms << " ms\n";

    return 0;
}