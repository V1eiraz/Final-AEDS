#include <iomanip>
#include <algorithm>
#include <span>

#include "sistemaAnalise.hpp"
#include "arquivo.hpp"

void SistemaAnalise::carregar(const std::string& arquivo_csv){
    caminho_csv = arquivo_csv;

    dicionario.reservar(CAPACIDADE_DICIONARIO);
    janelas.reservar(CAPACIDADE_DICIONARIO);
    manchetes.reserve(CAPACIDADE_MANCHETES);
    tokens_planos.reserve(CAPACIDADE_TOKENS_PLANOS);

    Arquivo::ler_csv(arquivo_csv, [&](std::string_view texto, uint64_t offset, uint32_t tamanho){
        const uint32_t idx = static_cast<uint32_t>(manchetes.size());
        const uint32_t ini_tok = static_cast<uint32_t>(tokens_planos.size());

        processador.processar(texto, dicionario, tokens_planos);

        const uint32_t fim_tok = static_cast<uint32_t>(tokens_planos.size());

        for(uint32_t ti = ini_tok; ti < fim_tok; ++ti) janelas.registrar(tokens_planos[ti], idx);

        indice.inserir(idx, std::span<const uint32_t>(tokens_planos.data() + ini_tok, fim_tok - ini_tok));
        manchetes.push_back({offset, tamanho, ini_tok, fim_tok});
    });

    indice.finalizar(static_cast<uint32_t>(manchetes.size()));
    preparar_rankings();
}

void SistemaAnalise::preparar_rankings(){
    ranking_global = janelas.top_global(TAMANHO_RANKING);
    ranking_emergentes = janelas.top_emergentes(TAMANHO_RANKING);

    ids_emergentes.reserve(TAMANHO_RANKING);
    for(const auto& te : ranking_emergentes) ids_emergentes.insert(te.id);
}

void SistemaAnalise::analisar(const std::string& arquivo_entrada, const std::string& arquivo_saida){
    auto saida = Arquivo::abrir_escrita(arquivo_saida);
    if(!saida.is_open()) return;

    std::ifstream csv(caminho_csv, std::ios::binary);
    if(!csv.is_open()) return;

    std::vector<char> buf_io(TAMANHO_BUFFER_IO);
    saida.rdbuf()->pubsetbuf(buf_io.data(), static_cast<std::streamsize>(buf_io.size()));

    struct QueryKeywords{
        std::string linha;
        std::vector<std::string> palavras;
    };
    std::vector<QueryKeywords> todas_keywords;

    bool primeiro = true;

    Arquivo::ler_consultas(arquivo_entrada, [&](const std::string& linha){
        const auto tokens = processador.processar_consulta(linha, dicionario);
        const auto resultados = indice.buscar(std::span<const uint32_t>(tokens), LIMIAR_JACCARD, MAX_RESULTADOS_SIMILARES);

        if(!primeiro) saida << '\n';
        primeiro = false;

        saida << "Manchete Original: " << linha << '\n';
        saida << "Top " << MAX_RESULTADOS_SIMILARES << " similares:\n";

        if(resultados.empty()){
            saida << "(nenhuma manchete similar encontrada)\n";
            todas_keywords.push_back({linha, {}});
            return;
        }

        std::vector<std::string> palavras_chave;

        for(size_t i = 0; i < resultados.size(); ++i){
            const auto& r = resultados[i];
            const auto& m = manchetes[r.indice];

            csv.seekg(static_cast<std::streamoff>(m.offset_csv));
            std::string texto(m.tamanho_texto, '\0');
            csv.read(texto.data(), m.tamanho_texto);

            saida << (i + 1) << " - " << texto
                  << " [Jaccard = "
                  << std::fixed << std::setprecision(4)
                  << r.similaridade << "]\n";

            for(uint32_t ti = m.inicio_tokens; ti < m.fim_tokens; ++ti){
                if(ids_emergentes.count(tokens_planos[ti])) palavras_chave.push_back(dicionario.obter_termo(tokens_planos[ti]));
            }
        }

        std::sort(palavras_chave.begin(), palavras_chave.end());
        palavras_chave.erase(std::unique(palavras_chave.begin(), palavras_chave.end()), palavras_chave.end());
        todas_keywords.push_back({linha, std::move(palavras_chave)});
    });

    auto outros = Arquivo::abrir_escrita("data/outros.dat");
    if(!outros.is_open()) return;

    outros << "Top " << TAMANHO_RANKING << " Global:\n";
    for(size_t i = 0; i < ranking_global.size(); ++i)
        outros << (i + 1) << " - "
               << dicionario.obter_termo(ranking_global[i].first)
               << " = " << ranking_global[i].second << '\n';

    outros << '\n';

    outros << "Top " << TAMANHO_RANKING << " Emergentes:\n";
    for(size_t i = 0; i < ranking_emergentes.size(); ++i)
        outros << (i + 1) << " - "
               << dicionario.obter_termo(ranking_emergentes[i].id)
               << " = " << std::fixed << std::setprecision(4)
               << ranking_emergentes[i].taxa << '\n';

    outros << '\n';

    outros << "Palavras-chave por Consulta:\n";
    for(const auto& qk : todas_keywords){
        outros << "Manchete Original: " << qk.linha << '\n';
        outros << "Palavras-chave:";
        if(qk.palavras.empty()) outros << " (nenhuma)\n";
        else{
            for(const auto& p : qk.palavras) outros << ' ' << p;
            outros << '\n';
        }
    }
}