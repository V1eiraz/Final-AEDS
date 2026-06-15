#include "dicionario.hpp"

void Dicionario::reservar(uint32_t n){
    mapa.reserve(n);
    vocabulario.reserve(n);
}

uint32_t Dicionario::obterOuInserir(std::string_view termo){
    auto it = mapa.find(termo);
    if(it != mapa.end()) return it->second;

    const uint32_t id = static_cast<uint32_t>(vocabulario.size());
    
    vocabulario.emplace_back(termo);
    mapa.emplace(std::string_view(vocabulario.back()), id);

    return id;
}

uint32_t Dicionario::buscar(std::string_view termo) const{
    auto it = mapa.find(termo);
    if(it != mapa.end()) return it->second;

    return std::numeric_limits<uint32_t>::max();
}

const std::string& Dicionario::obterTermo(uint32_t id) const{
    return vocabulario[id];
}

uint32_t Dicionario::tamanho() const{
    return static_cast<uint32_t>(vocabulario.size());
}