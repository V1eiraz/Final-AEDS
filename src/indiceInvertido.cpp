#include "indiceInvertido.hpp"

#include <algorithm>

void IndiceInvertido::inserir(uint32_t indice, std::span<const uint32_t> tokens){
    tamanhos.push_back(static_cast<uint32_t>(tokens.size()));

    for(const uint32_t token : tokens) postagens[token].push_back(indice);
}

void IndiceInvertido::finalizar(uint32_t total){
    total_ = total;
}

std::vector<Resultado> IndiceInvertido::buscar(std::span<const uint32_t> consulta, float limiar, uint32_t quantidade, BufferBusca& buf) const{
    if(consulta.empty()) return {};

    std::vector<uint32_t> consulta_ord(consulta.begin(), consulta.end());
    std::sort(consulta_ord.begin(), consulta_ord.end(), [this](uint32_t a, uint32_t b){
        const auto ia = postagens.find(a), ib = postagens.find(b);

        const size_t sa = (ia != postagens.end()) ? ia->second.size() : 0;
        const size_t sb = (ib != postagens.end()) ? ib->second.size() : 0;

        return sa < sb;
    });

    for(const uint32_t token : consulta_ord){
        const auto it = postagens.find(token);
        if(it == postagens.end() || it->second.size() > LIMITE_POSTAGENS_TOKEN) continue;

        for(const uint32_t idx : it->second){
            if(!buf.contagem_plana[idx]) buf.tocados.push_back(idx);
            ++buf.contagem_plana[idx];
        }
    }

    const uint32_t tam_q = static_cast<uint32_t>(consulta.size());

    using Par = std::pair<float, uint32_t>;
    const auto cmp_min = std::greater<Par>{};
    std::vector<Par> heap;
    heap.reserve(quantidade + 1);

    for(const uint32_t idx : buf.tocados){
        const uint32_t inter = buf.contagem_plana[idx];
        buf.contagem_plana[idx] = 0;

        const uint32_t tam_m = tamanhos[idx];
        if(tam_m == 0) continue;
        if(tam_m == tam_q && inter == tam_q) continue;

        const uint32_t uniao = tam_q + tam_m - inter;
        const float j = static_cast<float>(inter) / static_cast<float>(uniao);

        if(limiar > 0.0f && j < limiar) continue;

        heap.emplace_back(j, idx);
        std::push_heap(heap.begin(), heap.end(), cmp_min);

        if(heap.size() > quantidade){
            std::pop_heap(heap.begin(), heap.end(), cmp_min);
            heap.pop_back();
        }
    }
    buf.tocados.clear();

    std::sort_heap(heap.begin(), heap.end(), cmp_min);

    std::vector<Resultado> resultados;
    resultados.reserve(heap.size());
    for(auto& [sim, idx] : heap) resultados.push_back({idx, sim});
    
    return resultados;
}