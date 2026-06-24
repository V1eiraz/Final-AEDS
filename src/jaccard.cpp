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
        if      (A[i] == B[j]) { intersecao++; i++; j++; }
        else if (A[i] <  B[j]) { i++; }
        else                   { j++; }
    }

    int uniao = (int)A.size() + (int)B.size() - intersecao;
    return (float)intersecao / (float)uniao;
}

std::vector<ResultadoConsulta> Jaccard::buscarTodas(
    const std::string& caminhoCSV,
    const std::vector<std::string>& consultas,
    Dicionario& dicionario,
    Processador& processador)
{
    const int Q = (int)consultas.size();

    // Pré-processa todas as consultas em tokens
    std::vector<std::vector<uint32_t>> tokensConsultas(Q);
    for (int q = 0; q < Q; q++) {
        processador.processar(consultas[q], dicionario, tokensConsultas[q]);
    }

    // Um min-heap por consulta — mantém top 10 de cada
    using Par = std::pair<float, Similar>;
    auto cmp = [](const Par& a, const Par& b) {
        return a.first > b.first; // min-heap
    };
    using Heap = std::priority_queue<Par, std::vector<Par>, decltype(cmp)>;

    std::vector<Heap> heaps(Q, Heap(cmp));

    // UMA ÚNICA leitura do CSV — compara cada linha com TODAS as consultas
    std::vector<uint32_t> tokensLinha;
    int n = 0;

    Arquivo::ler_csv(caminhoCSV, [&](std::string&& linha) {
        tokensLinha.clear();
        processador.processar(linha, dicionario, tokensLinha);

        for (int q = 0; q < Q; q++) {
            // Pula se for exatamente a manchete da consulta
            if (linha == consultas[q]) { continue; }

            float score = calcular(tokensConsultas[q], tokensLinha);

            if (score >= LIMIAR_JACCARD) {
                Similar s{n, score, ""};

                if (heaps[q].size() < MAX_RESULTADOS_SIMILARES) {
                    heaps[q].push({score, s});
                } else if (score > heaps[q].top().first) {
                    heaps[q].pop();
                    heaps[q].push({score, s});
                }
            }
        }
        n++;
    });

    // Monta resultados — agora relê só as linhas necessárias para pegar o texto
    // Coleta todos os lineNumbers que precisamos buscar
    std::vector<int> linhasNecessarias;
    std::vector<std::vector<Similar>> similaresPorConsulta(Q);

    for (int q = 0; q < Q; q++) {
        while (!heaps[q].empty()) {
            similaresPorConsulta[q].push_back(heaps[q].top().second);
            linhasNecessarias.push_back(heaps[q].top().second.lineNumber);
            heaps[q].pop();
        }
        // Ordena por jaccard decrescente
        std::sort(similaresPorConsulta[q].begin(), similaresPorConsulta[q].end(),
                  [](const Similar& a, const Similar& b) {
                      return a.jaccard > b.jaccard;
                  });
    }

    // Remove duplicatas de linhasNecessarias e cria mapa lineNumber → texto
    std::sort(linhasNecessarias.begin(), linhasNecessarias.end());
    linhasNecessarias.erase(
        std::unique(linhasNecessarias.begin(), linhasNecessarias.end()),
        linhasNecessarias.end()
    );

    // Uma última leitura só para buscar os textos das similares encontradas
    std::unordered_map<int, std::string> textos;
    textos.reserve(linhasNecessarias.size());

    {
        int n2 = 0;
        int idx = 0; // índice em linhasNecessarias (já ordenado)
        Arquivo::ler_csv(caminhoCSV, [&](std::string&& linha) {
            if (idx < (int)linhasNecessarias.size() &&
                n2 == linhasNecessarias[idx]) {
                textos[n2] = std::move(linha);
                idx++;
            }
            n2++;
            // Para de ler quando já achou todas as linhas necessárias
        });
    }

    // Monta resultado final
    std::vector<ResultadoConsulta> resultado(Q);
    for (int q = 0; q < Q; q++) {
        resultado[q].textoOriginal = consultas[q];
        resultado[q].similares = std::move(similaresPorConsulta[q]);
    }

    // Adiciona texto às similares
    for (auto& r : resultado) {
        for (auto& s : r.similares) {
            s.texto = textos.count(s.lineNumber) ? textos[s.lineNumber] : "";
        }
    }

    return resultado;
}