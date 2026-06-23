#include "jaccard.hpp"
#include "arquivo.hpp"
#include "config.hpp"
#include <algorithm>
#include <queue>

float Jaccard::calcular(const std::vector<uint32_t>& A,
                        const std::vector<uint32_t>& B) {
    if (A.empty() && B.empty()) return 1.0f;
    if (A.empty() || B.empty()) return 0.0f;

    int intersecao = 0;
    int i = 0, j = 0;

    while (i < (int)A.size() && j < (int)B.size()) {
        if      (A[i] == B[j]) {
            intersecao++; i++; j++; 
        }else if (A[i] <  B[j]) {
            i++; 
        }else{
            j++; }
    }

    int uniao = (int)A.size() + (int)B.size() - intersecao;
    return (float)intersecao / (float)uniao;
}

std::vector<Similar> Jaccard::buscarSimilares(const std::string& caminhoCSV,
                                               const std::vector<uint32_t>& tokensAlvo,
                                               const std::string& textoAlvo,
                                               Dicionario& dicionario,
                                               Processador& processador) {
    using Par = std::pair<float, Similar>;

    // Min-heap: topo é sempre o PIOR dos top 10
    auto cmp = [](const Par& a, const Par& b) {
        return a.first > b.first;
    };
    std::priority_queue<Par, std::vector<Par>, decltype(cmp)> heap(cmp);

    std::vector<uint32_t> tokensLinha;
    int n = 0;

    Arquivo::ler_csv(caminhoCSV, [&](std::string&& linha) {
        // Ignora se for exatamente igual à manchete de consulta
        if (linha != textoAlvo) {
            tokensLinha.clear();
            processador.processar(linha, dicionario, tokensLinha);

            float score = calcular(tokensAlvo, tokensLinha);

            if (score >= LIMIAR_JACCARD) {
                Similar s{n, linha, score};

                if (heap.size() < MAX_RESULTADOS_SIMILARES) {
                    heap.push({score, s});
                } else if (score > heap.top().first) {
                    heap.pop();
                    heap.push({score, s});
                }
            }
        }
        n++;
    });

    // Extrai em ordem decrescente
    std::vector<Similar> resultado;
    resultado.reserve(heap.size());

    while (!heap.empty()) {
        resultado.push_back(heap.top().second);
        heap.pop();
    }

    std::reverse(resultado.begin(), resultado.end());
    return resultado;
}