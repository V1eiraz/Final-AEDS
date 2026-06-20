#pragma once

#include "config.hpp"
#include <vector>
#include <cstdint>

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
};