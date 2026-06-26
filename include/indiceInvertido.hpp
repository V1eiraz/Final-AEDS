#pragma once

#include "config.hpp"

#include <cstdint>
#include <span>
#include <vector>
#include <unordered_map>

struct Resultado{
    uint32_t indice;
    float similaridade;
};

class IndiceInvertido{
public:
    void inserir(uint32_t indice, std::span<const uint32_t> tokens);
    void finalizar(uint32_t total_manchetes);
    uint32_t total() const{ return total_manchetes_; }
    std::vector<Resultado> buscar(std::span<const uint32_t> consulta, float limiar, uint32_t quantidade) const;
private:
    std::unordered_map<uint32_t, std::vector<uint32_t>> postagens;
    std::vector<uint32_t> tamanhos;
    uint32_t total_manchetes_ = 0;
    mutable std::vector<uint32_t> contagem_plana;
    mutable std::vector<uint32_t> tocados;
};