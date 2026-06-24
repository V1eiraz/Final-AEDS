#include "processador.hpp"
#include <algorithm>
#include <limits>
#include <array>

namespace {
    // [OTM-1] Tabela de mapeamento de caracteres compilada em tempo de compilação.
    // TABELA_CHARS[c] == 0  → c não é letra: age como delimitador.
    // TABELA_CHARS[c] != 0  → c é letra, valor já convertido para minúscula.
    //
    // Substitui dois desvios de fluxo (isalpha + tolower) por um único acesso
    // à tabela por iteração, eliminando overhead de função e desvio de branch
    // no loop mais chamado do sistema (~7 M iterações de caractere no CSV).
    constexpr std::array<char, 256> fazer_tabela() {
        std::array<char, 256> t{};
        for (int i = 'a'; i <= 'z'; ++i) t[i] = static_cast<char>(i);
        for (int i = 'A'; i <= 'Z'; ++i) t[i] = static_cast<char>(i + 32);
        return t;
    }
    constexpr auto TABELA_CHARS = fazer_tabela();
}

void Processador::processar(std::string_view texto, Dicionario& dicionario,
                             std::vector<uint32_t>& destino_plano) {
    // [OTM-6] Marca onde esta manchete começa no vetor plano para posterior sort+dedup
    const auto inicio = static_cast<uint32_t>(destino_plano.size());

    // [OTM-2] buffer_token é membro reutilizado; clear() não desaloca a capacidade
    buffer_token.clear();

    for (const unsigned char c : texto) {
        // [OTM-1] Consulta à tabela: substitui isalpha() + tolower() por acesso direto
        const char m = TABELA_CHARS[c];
        if (m) {
            buffer_token += m;
        } else if (!buffer_token.empty()) {
            if (buffer_token.size() > 3)
                // [OTM-6] Insere ID diretamente no vetor plano, sem vector intermediário
                destino_plano.push_back(dicionario.obter_ou_inserir(buffer_token));
            buffer_token.clear();
        }
    }
    // Trata o último token (sem delimitador final)
    if (buffer_token.size() > 3)
        destino_plano.push_back(dicionario.obter_ou_inserir(buffer_token));

    // Ordena e deduplica apenas a região recém-adicionada (representação de conjunto)
    auto it = destino_plano.begin() + inicio;
    std::sort(it, destino_plano.end());
    destino_plano.erase(std::unique(it, destino_plano.end()), destino_plano.end());
}

std::vector<uint32_t> Processador::processar_consulta(std::string_view texto,
                                                        const Dicionario& dicionario) {
    std::vector<uint32_t> resultado;
    buffer_token.clear(); // [OTM-2] reutiliza buffer

    // Lambda de flush: encapsula a lógica de fim de token
    auto flush = [&] {
        if (buffer_token.size() > 3) {
            const uint32_t id = dicionario.buscar(buffer_token); // [OTM-11] sem alocação
            if (id != std::numeric_limits<uint32_t>::max())
                resultado.push_back(id);
        }
        buffer_token.clear();
    };

    for (const unsigned char c : texto) {
        const char m = TABELA_CHARS[c]; // [OTM-1]
        if (m) buffer_token += m;
        else if (!buffer_token.empty()) flush();
    }
    if (!buffer_token.empty()) flush();

    std::sort(resultado.begin(), resultado.end());
    resultado.erase(std::unique(resultado.begin(), resultado.end()), resultado.end());
    return resultado;
}