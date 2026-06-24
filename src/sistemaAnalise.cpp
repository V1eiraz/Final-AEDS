#include "sistemaAnalise.hpp"
#include "arquivo.hpp"
#include <iomanip>
#include <algorithm>
#include <span>
#include <thread>
#include <vector>
#include <sstream>

void SistemaAnalise::carregar(const std::string& arquivo_csv) {
    dicionario.reservar(CAPACIDADE_DICIONARIO);
    janelas.reservar(CAPACIDADE_DICIONARIO);
    manchetes.reserve(CAPACIDADE_MANCHETES);
    tokens_planos.reserve(CAPACIDADE_TOKENS_PLANOS);

    Arquivo::ler_csv(arquivo_csv, [&](std::string&& texto) {
        const uint32_t idx     = static_cast<uint32_t>(manchetes.size());
        const uint32_t ini_tok = static_cast<uint32_t>(tokens_planos.size());

        processador.processar(texto, dicionario, tokens_planos);

        const uint32_t fim_tok = static_cast<uint32_t>(tokens_planos.size());

        for (uint32_t ti = ini_tok; ti < fim_tok; ++ti)
            janelas.registrar(tokens_planos[ti], idx);

        indice.inserir(idx, std::span<const uint32_t>(
            tokens_planos.data() + ini_tok, fim_tok - ini_tok));

        manchetes.push_back({std::move(texto), ini_tok, fim_tok});
    });

    indice.finalizar(static_cast<uint32_t>(manchetes.size()));
    preparar_rankings();
}

void SistemaAnalise::preparar_rankings() {
    const auto top_em = janelas.top_emergentes(TAMANHO_RANKING);
    ids_emergentes.reserve(TAMANHO_RANKING);
    for (const auto& te : top_em)
        ids_emergentes.insert(te.id);

    salvar_ranking_global("data/top_global.dat");
    salvar_ranking_emergentes("data/top_emergentes.dat");
}

void SistemaAnalise::salvar_ranking_global(const std::string& caminho) const {
    auto saida = Arquivo::abrir_escrita(caminho);
    if (!saida.is_open()) return;
    const auto top = janelas.top_global(TAMANHO_RANKING);
    for (size_t i = 0; i < top.size(); ++i)
        saida << (i + 1) << " - "
              << dicionario.obter_termo(top[i].first)
              << " = " << top[i].second << '\n';
}

void SistemaAnalise::salvar_ranking_emergentes(const std::string& caminho) const {
    auto saida = Arquivo::abrir_escrita(caminho);
    if (!saida.is_open()) return;
    const auto top = janelas.top_emergentes(TAMANHO_RANKING);
    for (size_t i = 0; i < top.size(); ++i)
        saida << (i + 1) << " - "
              << dicionario.obter_termo(top[i].id)
              << " = " << std::fixed << std::setprecision(4) << top[i].taxa << '\n';
}

void SistemaAnalise::analisar(const std::string& arquivo_entrada,
                               const std::string& arquivo_saida) {
    // Coleta todas as consultas primeiro para poder distribuir entre threads
    std::vector<std::string> consultas;
    Arquivo::ler_consultas(arquivo_entrada, [&](const std::string& linha) {
        consultas.push_back(linha);
    });

    if (consultas.empty()) return;

    // Detecta quantos núcleos estão disponíveis no hardware.
    // Se não for detectável, usa 1 (comportamento seguro).
    const uint32_t num_threads = std::max(1u,
        static_cast<uint32_t>(std::thread::hardware_concurrency()));

    // Cada posição guarda os resultados já formatados de uma consulta.
    // O vetor tem tamanho fixo igual ao número de consultas — sem race condition
    // na escrita porque cada thread escreve em índices distintos.
    std::vector<std::string> saidas(consultas.size());

    // Função que cada thread executa: processa um subconjunto das consultas.
    // ini e fim definem o intervalo de índices atribuído a esta thread.
    // Cada thread tem seu próprio BufferBusca e Processador — sem estado compartilhado.
    auto processar_fatia = [&](uint32_t ini, uint32_t fim) {
        // BufferBusca local: contagem_plana e tocados exclusivos desta thread.
        // Sem isso, duas threads escreveriam no mesmo array e corromperiam os dados.
        BufferBusca buf;
        buf.inicializar(indice.total());

        // Processador local: buffer_token é membro da classe.
        // Criar um por thread evita disputas no buffer de tokenização.
        Processador proc_local;

        for (uint32_t i = ini; i < fim; ++i) {
            const auto tokens = proc_local.processar_consulta(consultas[i], dicionario);
            const auto resultados = indice.buscar(
                std::span<const uint32_t>(tokens),
                LIMIAR_JACCARD,
                MAX_RESULTADOS_SIMILARES,
                buf);  // passa o buffer exclusivo desta thread

            // Formata o resultado em string e armazena na posição i.
            // Nenhuma outra thread escreve nesta posição — sem mutex necessário.
            std::ostringstream oss;
            oss << "Manchete Original: " << consultas[i] << '\n';
            oss << "Top " << MAX_RESULTADOS_SIMILARES << " similares:\n";

            if (resultados.empty()) {
                oss << "(nenhuma manchete similar encontrada)\n";
            } else {
                for (size_t k = 0; k < resultados.size(); ++k) {
                    const auto& r = resultados[k];
                    oss << (k + 1) << " - "
                        << manchetes[r.indice].texto
                        << " [Jaccard = "
                        << std::fixed << std::setprecision(4)
                        << r.similaridade << "]\n";
                }
            }

            saidas[i] = oss.str();
        }
    };

    // Divide as consultas em fatias iguais, uma por thread
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    const uint32_t total    = static_cast<uint32_t>(consultas.size());
    const uint32_t por_thread = (total + num_threads - 1) / num_threads;

    for (uint32_t t = 0; t < num_threads; ++t) {
        const uint32_t ini = t * por_thread;
        const uint32_t fim = std::min(ini + por_thread, total);
        if (ini >= fim) break; // menos consultas que threads: para de criar threads
        threads.emplace_back(processar_fatia, ini, fim);
    }

    // Aguarda todas as threads terminarem antes de escrever
    for (auto& th : threads) th.join();

    // Escrita sequencial no arquivo: feita pelo thread principal após join().
    // A ordem original das consultas é preservada porque saidas[i] corresponde
    // à consulta i, independente de qual thread a processou.
    auto saida = Arquivo::abrir_escrita(arquivo_saida);
    if (!saida.is_open()) return;

    std::vector<char> buf_io(TAMANHO_BUFFER_IO);
    saida.rdbuf()->pubsetbuf(buf_io.data(),
                              static_cast<std::streamsize>(buf_io.size()));

    for (size_t i = 0; i < saidas.size(); ++i) {
        if (i > 0) saida << '\n';
        saida << saidas[i];
    }
}