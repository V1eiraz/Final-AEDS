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

struct BufferBusca{
    std::vector<uint32_t> contagem_plana;
    std::vector<uint32_t> tocados;

    void inicializar(uint32_t total){
        contagem_plana.assign(total, 0u);
        tocados.reserve(4096);
    }
};

class IndiceInvertido{
public:
    void inserir(uint32_t indice, std::span<const uint32_t> tokens);
    void finalizar(uint32_t total_manchetes);
    uint32_t total() const{ 
        return total_;
    }
    std::vector<Resultado> buscar(std::span<const uint32_t> consulta, float limiar, uint32_t quantidade, BufferBusca& buf) const;
private:
    std::unordered_map<uint32_t, std::vector<uint32_t>> postagens;
    std::vector<uint32_t> tamanhos;
    uint32_t total_ = 0;
};