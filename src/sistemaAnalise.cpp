#include "sistemaAnalise.hpp"
#include "arquivo.hpp"
#include "jaccard.hpp"

#include <fstream>
#include <iomanip>
#include <unordered_set>

void SistemaAnalise::carregar(const std::string& arquivo_csv) {
    caminhoCSV = arquivo_csv;
    dicionario.reservar(CAPACIDADE_DICIONARIO);

    Arquivo::ler_csv(arquivo_csv, [&](std::string&&) {
        totalLinhas++;
    });

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
    auto saida = Arquivo::abrir_escrita_bufferizada(arquivo_saida);
    Jaccard jaccard;

    // Rankings calculados uma vez só
    auto emergentes = janelas.rankingEmergentes(dicionario.tamanho());
    auto global     = janelas.rankingGlobal(dicionario.tamanho());

    // Lê todas as consultas do input.dat de uma vez
    std::vector<std::string> consultas;
    Arquivo::ler_consultas(arquivo_entrada, [&](const std::string& linha) {
        consultas.push_back(linha);
    });

    // UMA única leitura do CSV para todas as consultas
    auto resultados = jaccard.buscarTodas(
        caminhoCSV, consultas, dicionario, processador
    );

    // vector<bool> reutilizável para cruzamento com emergentes
    std::vector<bool> marcado(dicionario.tamanho(), false);
    std::vector<uint32_t> marcados; // rastreia quais foram marcados para limpar
    std::vector<uint32_t> tokensSimilar;

    for (auto& r : resultados) {
        saida << "Manchete Original: " << r.textoOriginal << "\n";
        saida << "Top 10 similares:\n";

        for (int i = 0; i < (int)r.similares.size(); i++) {
            saida << (i + 1) << " - "
                  << r.similares[i].texto
                  << " [Jaccard = " << std::fixed << std::setprecision(4)
                  << r.similares[i].jaccard << "]\n";
        }

        // Marca palavras das similares usando vector<bool>
        marcados.clear();
        for (auto& s : r.similares) {
            tokensSimilar.clear();
            processador.processar(s.texto, dicionario, tokensSimilar);
            for (uint32_t id : tokensSimilar) {
                if (!marcado[id]) {
                    marcado[id] = true;
                    marcados.push_back(id);
                }
            }
        }

        // Cruza com emergentes
        saida << "Palavras Emergentes presentes nessas manchetes:\n";
        int count = 0;
        for (auto& e : emergentes) {
            if (marcado[e.id]) {
                saida << "  " << dicionario.obterTermo(e.id)
                      << " [C(p) = " << std::fixed << std::setprecision(4)
                      << e.freq << "]\n";
                count++;
            }
        }

        if (count == 0) saida << "  (nenhuma)\n";
        saida << "\n";

        // Limpa só os IDs que foram marcados (não o vetor inteiro)
        for (uint32_t id : marcados) marcado[id] = false;
    }
}