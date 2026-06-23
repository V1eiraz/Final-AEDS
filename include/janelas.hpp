#pragma once

#include "config.hpp"
#include <vector>
#include <cstdint>

struct EntradaRanking {
    uint32_t id;
    uint32_t freq;
};

class Janelas {
public:
    // Cada janela é um vetor de frequências indexado pelo ID da palavra
    // janelas[2][42] = frequência do ID 42 na janela 2
    std::vector<uint32_t> freq[TOTAL_JANELAS];

    // Registra os tokens de uma manchete na janela correta
    void addManchete(int lineNumber, int totalLines,
                       const std::vector<uint32_t>& tokens);

    // Retorna a frequência de um ID em uma janela específica
    uint32_t getFreq(int janela, uint32_t id) const;

    // Função que soma a frquencia das 5 janelas para um ID
    uint32_t freqGlobal(uint32_t id) const;

    // Top 100 por frequencia total - usando o heap min O(n log k)
    std::vector<EntradaRanking> rankingGlobal(uint32_t totalPalavras) const; 

    // Top 100 por C(p)
    std::vector<EntradaRanking> rankingEmegentes(uint32_t totalPalavras, uint32_t minFreq = 5) const;
};