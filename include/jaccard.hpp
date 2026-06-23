#pragma once

#include "dicionario.hpp"
#include "processador.hpp"
#include <vector>
#include <string>
#include <cstdint>

struct Similar {
    int lineNumber;
    std::string texto;
    float jaccard;
};

class Jaccard {
public:
    // Calcula o índice de Jaccard entre dois vetores de IDs ordenados
    static float calcular(const std::vector<uint32_t>& A,
                          const std::vector<uint32_t>& B);

    // Recebe os tokens JÁ PROCESSADOS da manchete de consulta
    // e busca as top 10 similares no CSV
    std::vector<Similar> buscarSimilares(const std::string& caminhoCSV,
                                         const std::vector<uint32_t>& tokensAlvo,
                                         const std::string& textoAlvo,
                                         Dicionario& dicionario,
                                         Processador& processador);
};