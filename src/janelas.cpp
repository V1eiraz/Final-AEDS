#include "janelas.hpp"

#include <algorithm>

void JanelasTemporais::reservar(uint32_t n){
    global.reserve(n);

    for(auto& j : janelas) j.reserve(n / 2);
}

void JanelasTemporais::registrar(uint32_t id_termo, uint32_t indice){
    const uint32_t j = std::min(indice / TAMANHO_JANELA, static_cast<uint32_t>(TOTAL_JANELAS - 1));

    janelas[j][id_termo]++;
    global[id_termo]++;
}

std::vector<std::pair<uint32_t, uint32_t>> JanelasTemporais::top_global(uint32_t n) const{
    using Par = std::pair<uint32_t, uint32_t>;
    std::vector<Par> pares(global.begin(), global.end());
    const uint32_t k = std::min(n, static_cast<uint32_t>(pares.size()));

    std::partial_sort(pares.begin(), pares.begin() + k, pares.end(), [](const Par& a, const Par& b){ 
        return a.second > b.second; 
    });
    pares.resize(k);

    return pares;
}

std::vector<TermoEmergente> JanelasTemporais::top_emergentes(uint32_t n) const{
    std::vector<TermoEmergente> emergentes;
    emergentes.reserve(global.size());

    for(const auto& par : global){
        const uint32_t id = par.first;
        uint32_t fj1 = 0, fj5 = 0;

        const auto it1 = janelas[0].find(id);
        if(it1 != janelas[0].end()) fj1 = it1->second;

        const auto it5 = janelas[TOTAL_JANELAS - 1].find(id);
        if(it5 != janelas[TOTAL_JANELAS - 1].end()) fj5 = it5->second;

        const float taxa = static_cast<float>((static_cast<int64_t>(fj5) - static_cast<int64_t>(fj1))) / (fj1 + 1.0f);

        emergentes.push_back({id, taxa});
    }

    const uint32_t k = std::min(n, static_cast<uint32_t>(emergentes.size()));
    std::partial_sort(emergentes.begin(), emergentes.begin() + k, emergentes.end(), [](const TermoEmergente& a, const TermoEmergente& b){ 
        return a.taxa > b.taxa; 
    });
    emergentes.resize(k);
    
    return emergentes;
}