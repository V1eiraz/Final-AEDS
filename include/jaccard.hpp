#pragma once

#include "dicionario.hpp"
#include "processador.hpp"
#include <vector>
#include <string>
#include <cstdint>

struct Similar {
    int lineNumber;
    float jaccard;
    std::string texto; // preenchido só no final, não durante o heap
};

struct ResultadoConsulta {
    std::string textoOriginal;
    std::vector<Similar> similares;
};

class Jaccard {
public:
    static float calcular(const std::vector<uint32_t>& A,
                          const std::vector<uint32_t>& B);

    // Processa TODAS as consultas em uma única leitura do CSV
    std::vector<ResultadoConsulta> buscarTodas(
        const std::string& caminhoCSV,
        const std::vector<std::string>& consultas,
        Dicionario& dicionario,
        Processador& processador
    );
};