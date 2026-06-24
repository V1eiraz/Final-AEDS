#pragma once
#include <string>
#include <fstream>
#include <functional>
#include <iostream>

namespace Arquivo {

    std::ifstream abrir_leitura(const std::string& caminho);
    std::ofstream abrir_escrita(const std::string& caminho);

    // Lê o CSV linha a linha, descarta a data e chama processar(texto_manchete)
    // para cada headline válida. A lógica de I/O fica aqui; a de processamento,
    // no chamador.
    void ler_csv(const std::string& caminho,
                 const std::function<void(std::string&&)>& processar);

    // Lê input.dat linha a linha e chama processar(linha) para cada linha não-vazia.
    void ler_consultas(const std::string& caminho,
                       const std::function<void(const std::string&)>& processar);

}