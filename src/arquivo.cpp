#include "arquivo.hpp"

namespace Arquivo {

std::ifstream abrir_leitura(const std::string& caminho) {
    std::ifstream f(caminho);
    if (!f.is_open())
        std::cerr << "Erro: nao foi possivel abrir " << caminho << '\n';
    return f;
}

std::ofstream abrir_escrita(const std::string& caminho) {
    std::ofstream f(caminho);
    if (!f.is_open())
        std::cerr << "Erro: nao foi possivel abrir " << caminho << '\n';
    return f;
}

void ler_csv(const std::string& caminho,
             const std::function<void(std::string&&)>& processar) {
    auto arquivo = abrir_leitura(caminho);
    if (!arquivo.is_open()) return;

    std::string linha;
    std::getline(arquivo, linha); // descarta cabeçalho

    while (std::getline(arquivo, linha)) {
        if (!linha.empty() && linha.back() == '\r')
            linha.pop_back();

        const auto pos = linha.find(',');
        if (pos == std::string::npos) {
            processar(""); // linha malformada: sem vírgula separadora
            continue;
        }

        // Extrai apenas o título — a data (pos primeiros chars) é descartada aqui
        processar(linha.substr(pos + 1));
    }
}

void ler_consultas(const std::string& caminho,
                   const std::function<void(const std::string&)>& processar) {
    auto arquivo = abrir_leitura(caminho);
    if (!arquivo.is_open()) return;

    std::string linha;
    while (std::getline(arquivo, linha)) {
        if (!linha.empty() && linha.back() == '\r')
            linha.pop_back();
        if (!linha.empty())
            processar(linha);
    }
}

} // namespace Arquivo