#include "sistemaAnalise.hpp"
#include "arquivo.hpp"
#include "jaccard.hpp"

#include <fstream>
#include <iomanip>
#include <unordered_set>

void SistemaAnalise::carregar(const std::string& arquivo_csv) {
    caminhoCSV = arquivo_csv;
    dicionario.reservar(CAPACIDADE_DICIONARIO);

    // Primeira passada: contar linhas
    Arquivo::ler_csv(arquivo_csv, [&](std::string&&) {
        totalLinhas++;
    });

    // Segunda passada: processar e popular janelas
    int n = 0;
    std::vector<uint32_t> tokens;

    Arquivo::ler_csv(arquivo_csv, [&](std::string&& linha) {
        tokens.clear();
        processador.processar(linha, dicionario, tokens);
        janelas.addManchete(n, totalLinhas, tokens);
        n++;
    });
}

void SistemaAnalise::analisar(const std::string& arquivo_entrada,
                               const std::string& arquivo_saida) {
    auto saida = Arquivo::abrir_escrita(arquivo_saida);
    Jaccard jaccard;

    // Calcula rankings uma vez só
    auto emergentes = janelas.rankingEmergentes(dicionario.tamanho());
    auto global     = janelas.rankingGlobal(dicionario.tamanho());

    // Escreve Top-100 global no topo do output
    saida << "=== TOP 100 PALAVRAS MAIS FREQUENTES ===\n";
    for (int i = 0; i < (int)global.size(); i++) {
        saida << (i + 1) << " - "
              << dicionario.obterTermo(global[i].id)
              << " [freq = " << (uint32_t)global[i].freq << "]\n";
    }

    // Escreve Top-100 emergentes no topo do output
    saida << "\n=== TOP 100 PALAVRAS EMERGENTES ===\n";
    for (int i = 0; i < (int)emergentes.size(); i++) {
        saida << (i + 1) << " - "
              << dicionario.obterTermo(emergentes[i].id)
              << " [C(p) = " << std::fixed << std::setprecision(4)
              << emergentes[i].freq << "]\n";
    }

    saida << "\n=== CONSULTAS ===\n\n";

    // Monta set de IDs emergentes para cruzamento O(1)
    std::unordered_set<uint32_t> idsEmergentes;
    for (auto& e : emergentes) idsEmergentes.insert(e.id);

    std::vector<uint32_t> tokensConsulta;

    Arquivo::ler_consultas(arquivo_entrada, [&](const std::string& consulta) {
        tokensConsulta.clear();
        processador.processar(consulta, dicionario, tokensConsulta);

        // Busca top 10 similares
        auto similares = jaccard.buscarSimilares(
            caminhoCSV, tokensConsulta, consulta, dicionario, processador
        );

        // Escreve manchete consultada
        saida << "Manchete Original: " << consulta << "\n";

        // Escreve top 10 similares
        saida << "Top 10 similares:\n";
        for (int i = 0; i < (int)similares.size(); i++) {
            saida << (i + 1) << " - "
                  << similares[i].texto
                  << " [Jaccard = " << std::fixed << std::setprecision(4)
                  << similares[i].jaccard << "]\n";
        }

        // Coleta palavras de todas as manchetes similares
        std::unordered_set<uint32_t> palavrasSimilares;
        std::vector<uint32_t> tokensSimilar;

        for (auto& s : similares) {
            tokensSimilar.clear();
            processador.processar(s.texto, dicionario, tokensSimilar);
            for (uint32_t id : tokensSimilar) palavrasSimilares.insert(id);
        }

        // Cruza com emergentes — mantém ordem do ranking global
        saida << "Palavras Emergentes presentes nessas manchetes:\n";
        int count = 0;
        for (auto& e : emergentes) {
            if (palavrasSimilares.count(e.id)) {
                saida << "  " << dicionario.obterTermo(e.id)
                      << " [C(p) = " << std::fixed << std::setprecision(4)
                      << e.freq << "]\n";
                count++;
            }
        }

        if (count == 0) saida << "  (nenhuma)\n";
        saida << "\n";
    });
}