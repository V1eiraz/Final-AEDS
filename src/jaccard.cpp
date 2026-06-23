#include "jaccard.hpp"
#include "arquivo.hpp"
#include "config.hpp"
#include <algorithm>
#include <queue>

float Jaccard::calcular(const std::vector<uint32_t>& A, const std::vector<uint32_t>& B) {
    if (A.empty() && B.empty()) return 1.0f;
    if (A.empty() || B.empty()) return 0.0f;

    int intersecao = 0;
    int i = 0, j = 0;

    while (i < (int)A.size() && j < (int)B.size()) {
        if      (A[i] == B[j]) { intersecao++; i++; j++; }
        else if (A[i] <  B[j]) { i++; }
        else                   { j++; }
    }

    int uniao = (int)A.size() + (int)B.size() - intersecao;
    return (float)intersecao / (float)uniao;
}

std::vector<Similar> Jaccard::buscarSimilares(const std::string& caminhoCSV,
                                               int indiceBusca,
                                               Dicionario& dicionario,
                                               Processador& processador) {
    // PASSO 1: pegar os tokens da manchete alvo
    std::vector<uint32_t> tokensAlvo;
    std::string textoAlvo;

    int n = 0;
    Arquivo::ler_csv(caminhoCSV, [&](std::string&& linha) {
        if (n == indiceBusca) {
            textoAlvo = linha;
            processador.processar(linha, dicionario, tokensAlvo);
        }
        n++;
    });

    // PASSO 2: min-heap de tamanho fixo MAX_RESULTADOS_SIMILARES
    // O heap guarda os MELHORES scores — o topo é sempre o PIOR dos melhores
    // Quando o heap está cheio e um novo score é maior que o topo,
    // descarta o topo e insere o novo
    using Par = std::pair<float, Similar>; // (jaccard, Similar)

    // Comparador: min-heap pelo score (menor score no topo)
    auto cmp = [](const Par& a, const Par& b) {
        return a.first > b.first; // inverte para min-heap
    };
    std::priority_queue<Par, std::vector<Par>, decltype(cmp)> heap(cmp);

    std::vector<uint32_t> tokensLinha;
    int m = 0;

    Arquivo::ler_csv(caminhoCSV, [&](std::string&& linha) {
        if (m != indiceBusca) {
            tokensLinha.clear();
            processador.processar(linha, dicionario, tokensLinha);

            float score = calcular(tokensAlvo, tokensLinha);

            if (score >= LIMIAR_JACCARD) {
                Similar s{m, linha, score};

                if (heap.size() < MAX_RESULTADOS_SIMILARES) {
                    // Heap ainda não está cheio, só insere
                    heap.push({score, s});
                } else if (score > heap.top().first) {
                    // Score melhor que o pior dos top 10: substitui
                    heap.pop();
                    heap.push({score, s});
                }
                // Se score <= topo do heap, descarta — não entra no top 10
            }
        }
        m++;
    });

    // Extrai os resultados do heap em ordem decrescente
    std::vector<Similar> resultado;
    resultado.reserve(heap.size());

    while (!heap.empty()) {
        resultado.push_back(heap.top().second);
        heap.pop();
    }

    // O heap extrai do menor para o maior, então inverte
    std::reverse(resultado.begin(), resultado.end());

    return resultado;
}