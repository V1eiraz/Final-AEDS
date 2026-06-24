#include "arquivo.hpp"
#include "config.hpp"

namespace Arquivo {

    std::ifstream abrir_leitura(const std::string& caminho) {
        std::ifstream f(caminho);
        if (!f.is_open()) std::cerr << "Erro: não foi possível abrir " << caminho << '\n';
        return f;
    }

    std::ofstream abrir_escrita(const std::string& caminho) {
        std::ofstream f(caminho);
        if (!f.is_open()) std::cerr << "Erro: não foi possivel abrir " << caminho << '\n';
        return f;
    }

    void ler_csv(const std::string& caminho,
                 const std::function<void(std::string&&)>& processar) {
        auto arquivo = abrir_leitura(caminho);
        if (!arquivo.is_open()) return;

        // Otimização 3: buffer de I/O de 1MB — reduz chamadas ao kernel
        static std::vector<char> bufIO(TAMANHO_BUFFER_IO);
        arquivo.rdbuf()->pubsetbuf(bufIO.data(), bufIO.size());

        std::string linha;
        std::getline(arquivo, linha); // pula header

        while (std::getline(arquivo, linha)) {
            if (!linha.empty() && linha.back() == '\r') linha.pop_back();

            const auto pos = linha.find(',');
            if (pos == std::string::npos) {
                processar("");
                continue;
            }

            // Otimização 2: string_view aponta direto para dentro de linha
            // sem alocar nova string com substr
            std::string_view sv(linha.c_str() + pos + 1, linha.size() - pos - 1);

            // Move a substring para evitar cópia — constrói só uma vez
            processar(std::string(sv));
        }
    }

    void ler_consultas(const std::string& caminho,
                       const std::function<void(const std::string&)>& processar) {
        auto arquivo = abrir_leitura(caminho);
        if (!arquivo.is_open()) return;

        // Otimização 3: mesmo buffer de I/O
        static std::vector<char> bufIO(TAMANHO_BUFFER_IO);
        arquivo.rdbuf()->pubsetbuf(bufIO.data(), bufIO.size());

        // Otimização 8: buffer de escrita (vale para leitura sequencial também)
        std::string linha;
        while (std::getline(arquivo, linha)) {
            if (!linha.empty() && linha.back() == '\r') linha.pop_back();
            if (!linha.empty()) processar(linha);
        }
    }

    std::ofstream abrir_escrita_bufferizada(const std::string& caminho) {
        std::ofstream f(caminho);
        if (!f.is_open()) {
            std::cerr << "Erro: não foi possivel abrir " << caminho << '\n';
            return f;
        }
    static std::vector<char> bufEscrita(TAMANHO_BUFFER_IO);
    f.rdbuf()->pubsetbuf(bufEscrita.data(), bufEscrita.size());
    return f;
}
}