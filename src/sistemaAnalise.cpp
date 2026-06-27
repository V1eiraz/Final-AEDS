#include "sistemaAnalise.hpp"
#include "arquivo.hpp"

#include <iomanip>
#include <algorithm>
#include <span>
#include <thread>
#include <sstream>

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
    std::vector<std::string> consultas;

    Arquivo::ler_consultas(arquivo_entrada, [&](const std::string& l){
        consultas.push_back(l);
    });

    if(consultas.empty()) return;

    const uint32_t total = static_cast<uint32_t>(consultas.size());
    const uint32_t nthreads = std::max(1u, static_cast<uint32_t>(std::thread::hardware_concurrency()));

    struct QueryResult{
        std::string saida_txt;
        std::string linha;
        std::vector<std::string> palavras_chave;
    };
    std::vector<QueryResult> resultados(total);

    auto trabalho = [&](uint32_t ini, uint32_t fim){
        Processador proc_local;
        std::ifstream csv_local(caminho_csv, std::ios::binary);

        BufferBusca buf;
        buf.inicializar(indice.total());

        for(uint32_t i = ini; i < fim; ++i){
            const auto tokens = proc_local.processar_consulta(consultas[i], dicionario);

            const auto similares = indice.buscar(std::span<const uint32_t>(tokens), LIMIAR_JACCARD, MAX_RESULTADOS_SIMILARES, buf);

            std::ostringstream oss;
            oss << "Manchete Original: " << consultas[i] << '\n';
            oss << "Top " << MAX_RESULTADOS_SIMILARES << " similares:\n";

            std::vector<std::string> palavras;

            if(similares.empty()) oss << "(nenhuma manchete similar encontrada)\n";
            else{
                for(size_t k = 0; k < similares.size(); ++k){
                    const auto& r = similares[k];
                    const auto& m = manchetes[r.indice];

                    csv_local.seekg(static_cast<std::streamoff>(m.offset_csv));
                    std::string texto(m.tamanho_texto, '\0');
                    csv_local.read(texto.data(), m.tamanho_texto);

                    oss << (k + 1) << " - " << texto
                        << " [Jaccard = "
                        << std::fixed << std::setprecision(4)
                        << r.similaridade << "]\n";

                    for(uint32_t ti = m.inicio_tokens; ti < m.fim_tokens; ++ti)
                        if(ids_emergentes.count(tokens_planos[ti])) palavras.push_back(dicionario.obter_termo(tokens_planos[ti]));
                }

                std::sort(palavras.begin(), palavras.end());
                palavras.erase(std::unique(palavras.begin(), palavras.end()), palavras.end());
            }
            resultados[i] = {oss.str(), consultas[i], std::move(palavras)};
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(nthreads);
    const uint32_t por = (total + nthreads - 1) / nthreads;

    for(uint32_t t = 0; t < nthreads; ++t){
        const uint32_t ini = t * por;
        const uint32_t fim = std::min(ini + por, total);
        if(ini >= fim) break;

        threads.emplace_back(trabalho, ini, fim);
    }
    for(auto& th : threads) th.join();

    auto saida = Arquivo::abrir_escrita(arquivo_saida);
    if(!saida.is_open()) return;

    std::vector<char> buf_io(TAMANHO_BUFFER_IO);
    saida.rdbuf()->pubsetbuf(buf_io.data(), static_cast<std::streamsize>(buf_io.size()));

    for(size_t i = 0; i < resultados.size(); ++i){
        if(i > 0) saida << '\n';
        saida << resultados[i].saida_txt;
    }

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
    for(const auto& r : resultados){
        outros << "Manchete Original: " << r.linha << '\n';
        outros << "Palavras-chave:";

        if(r.palavras_chave.empty()) outros << " (nenhuma)\n";
        else{
            for(const auto& p : r.palavras_chave) outros << ' ' << p;
            outros << '\n';
        }
    }
}