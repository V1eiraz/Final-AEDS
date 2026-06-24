#include "dicionario.hpp"

void Dicionario::reservar(uint32_t n) {
    // [OTM-9] Pré-aloca buckets e capacidade do vetor vocabulário.
    // Sem esta reserva, inserções em massa causam rehashing progressivo
    // (copia todos os elementos) a cada fator de carga atingido.
    mapa.reserve(n);
    vocabulario.reserve(n);
}

uint32_t Dicionario::obter_ou_inserir(std::string_view termo) {
    // [OTM-11] find() com string_view não constrói std::string temporário.
    // Possível porque HashString e IgualString têm is_transparent (C++20).
    auto it = mapa.find(termo);
    if (it != mapa.end()) return it->second;

    // Só chega aqui ~100k vezes (vocab único); aloca std::string uma única vez por termo.
    const uint32_t id = static_cast<uint32_t>(vocabulario.size());
    mapa.emplace(std::string(termo), id);
    vocabulario.emplace_back(termo);
    return id;
}

uint32_t Dicionario::buscar(std::string_view termo) const {
    // [OTM-11] Idem: sem alocação de string temporária
    auto it = mapa.find(termo);
    return it != mapa.end() ? it->second : std::numeric_limits<uint32_t>::max();
}

const std::string& Dicionario::obter_termo(uint32_t id) const {
    return vocabulario[id];
}

uint32_t Dicionario::tamanho() const {
    return static_cast<uint32_t>(vocabulario.size());
}