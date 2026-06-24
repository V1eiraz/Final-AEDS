#pragma once
#include <cstdint>
#include <span>
#include <vector>
#include <unordered_map>
#include "config.hpp"

struct Resultado {
    uint32_t indice;
    float similaridade;
};

// Buffer de trabalho da busca: um por thread.
// Separado da classe para que threads concorrentes não compartilhem estado.
// contagem_plana[i] = nº de tokens em comum com a consulta atual.
// tocados = índices incrementados nesta chamada (para reset pontual).
struct BufferBusca {
    std::vector<uint32_t> contagem_plana;
    std::vector<uint32_t> tocados;

    // Aloca os buffers com o tamanho total do corpus.
    // Chamado uma vez por thread antes da primeira busca.
    void inicializar(uint32_t total_manchetes) {
        contagem_plana.assign(total_manchetes, 0u);
        tocados.reserve(4096);
    }
};

class IndiceInvertido {
public:
    void inserir(uint32_t indice, std::span<const uint32_t> tokens);

    // Chamado após todos os inserir(). Guarda o total para inicializar buffers.
    void finalizar(uint32_t total_manchetes);

    // Retorna o total de manchetes indexadas (necessário para inicializar BufferBusca).
    uint32_t total() const { return total_manchetes_; }

    // Thread-safe: toda a escrita é no BufferBusca da thread chamadora.
    // postagens e tamanhos são somente-leitura após finalizar().
    std::vector<Resultado> buscar(std::span<const uint32_t> consulta,
                                   float limiar,
                                   uint32_t quantidade,
                                   BufferBusca& buf) const;

private:
    std::unordered_map<uint32_t, std::vector<uint32_t>> postagens;
    std::vector<uint32_t> tamanhos;
    uint32_t total_manchetes_ = 0;
};