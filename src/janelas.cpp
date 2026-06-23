#include "janelas.hpp"
#include <queue>
#include <algorithm>

void Janelas::addManchete(int lineNumber, int totalLines,
                          const std::vector<uint32_t>& tokens) {

    // Calcula a quantidade de linhas por janela
    int linesPerWindow = totalLines / TOTAL_JANELAS;
    // Calcula em qual janela a linha vai ficar ex: lineNumber = 0 => j = 0 / 208771 = 0 => (Janela 1)
    int j = lineNumber / linesPerWindow;
    // última janela fica com as linhas excedentes 
    if (j >= TOTAL_JANELAS) j = TOTAL_JANELAS - 1;
    

    // Adiciona cada token à janela
    for (uint32_t id : tokens) {

        // Garante que o vetor tem espaço para esse ID
        if (id >= freq[j].size()) {
            freq[j].resize(id + 1, 0);
        }

        freq[j][id]++;
    }
}

uint32_t Janelas::getFreq(int janela, uint32_t id) const {
    if (id >= freq[janela].size()) return 0;
    return freq[janela][id];
}

uint32_t Janelas::freqGlobal(uint32_t id) const {
    uint32_t total = 0;
    for (int j = 0; j < TOTAL_JANELAS; j++) {
        if (id < freq[j].size()) total += freq[j][id];
    }
    return total;
}

std::vector<EntradaRanking> Janelas::rankingGlobal(uint32_t totalPalavras) const {
    // Min-heap de tamanho K=100.
    // O topo é sempre o MENOR dos top-100 vistos até agora.
    // Se a palavra atual é maior que o topo, troca — O(N log K).
    auto cmp = [](const EntradaRanking& a, const EntradaRanking& b){
        return a.freq > b.freq;
    };
    std::priority_queue<EntradaRanking, std::vector<EntradaRanking>, decltype(cmp)> heap(cmp);

    for (uint32_t id = 0; id < totalPalavras; id++) {
        float f = static_cast<float>(freqGlobal(id));
        if (f == 0) continue;

        if (heap.size() < TAMANHO_RANKING) {
            heap.push({id, f});
        } else if (f > heap.top().freq) {
            heap.pop();
            heap.push({id, f});
        }
    }

    std::vector<EntradaRanking> resultado;
    resultado.reserve(heap.size());
    while (!heap.empty()) { resultado.push_back(heap.top()); heap.pop(); }
    std::sort(resultado.begin(), resultado.end(), [](const EntradaRanking& a, const EntradaRanking& b){
        return a.freq > b.freq;
    });
    return resultado;
}

std::vector<EntradaRanking> Janelas::rankingEmergentes(uint32_t totalPalavras, uint32_t minFreq)    const {
    // C(p) = (FJ5 - FJ1) / (FJ1 + 1)
    // +1 no denominador evita divisão por zero quando a palavra não existia em J1
    // minFreq filtra palavras raras que distorceriam o ranking
    auto cmp = [](const EntradaRanking& a, const EntradaRanking& b){
        return a.freq > b.freq;
    };
    std::priority_queue<EntradaRanking, std::vector<EntradaRanking>, decltype(cmp)> heap(cmp);

    for (uint32_t id = 0; id < totalPalavras; id++) {
        if (freqGlobal(id) < minFreq) continue;

        float fj1 = static_cast<float>(getFreq(0, id));
        float fj5 = static_cast<float>(getFreq(TOTAL_JANELAS - 1, id));
        float cp  = (fj5 - fj1) / (fj1 + 1.0f);

        if (heap.size() < TAMANHO_RANKING) {
            heap.push({id, cp});
        } else if (cp > heap.top().freq) {
            heap.pop();
            heap.push({id, cp});
        }
    }

    std::vector<EntradaRanking> resultado;
    resultado.reserve(heap.size());
    while (!heap.empty()) { resultado.push_back(heap.top()); heap.pop(); }
    std::sort(resultado.begin(), resultado.end(), [](const EntradaRanking& a, const EntradaRanking& b){
        return a.freq > b.freq;
    });
    return resultado;
}