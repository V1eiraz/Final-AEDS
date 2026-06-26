#include "dicionario.hpp"

void Dicionario::reservar(uint32_t n){
    mapa.reserve(n);
    vocabulario.reserve(n);
}

uint32_t Dicionario::obter_ou_inserir(std::string_view termo){
    auto it = mapa.find(termo);
    if(it != mapa.end()) return it->second;

    const uint32_t id = static_cast<uint32_t>(vocabulario.size());

    mapa.emplace(std::string(termo), id);
    vocabulario.emplace_back(termo);

    return id;
}

uint32_t Dicionario::buscar(std::string_view termo) const{
    auto it = mapa.find(termo);
    return it != mapa.end() ? it->second : std::numeric_limits<uint32_t>::max();
}

const std::string& Dicionario::obter_termo(uint32_t id) const{
    return vocabulario[id];
}

uint32_t Dicionario::tamanho() const{
    return static_cast<uint32_t>(vocabulario.size());
}