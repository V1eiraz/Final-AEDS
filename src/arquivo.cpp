#include "arquivo.hpp"

namespace Arquivo{
    std::ifstream abrir_leitura(const std::string& caminho){
        std::ifstream f(caminho);
        if(!f.is_open()) std::cerr << "Erro: não foi possível abrir " << caminho << '\n';
        return f;
    }

    std::ofstream abrir_escrita(const std::string& caminho){
        std::ofstream f(caminho);
        if(!f.is_open()) std::cerr << "Erro: não foi possivel abrir " << caminho << '\n';
        return f;
    }

    void ler_csv(const std::string& caminho, const std::function<void(std::string&&)>& processar){
        auto arquivo = abrir_leitura(caminho);
        if(!arquivo.is_open()) return;

        std::string linha;
        std::getline(arquivo, linha);

        while(std::getline(arquivo, linha)){
            if(!linha.empty() && linha.back() == '\r') linha.pop_back();
            const auto pos = linha.find(',');
            
            if(pos == std::string::npos){
                processar("");
                continue;
            }
            processar(linha.substr(pos + 1));
        }
    }

    void ler_consultas(const std::string& caminho, const std::function<void(const std::string&)>& processar){
        auto arquivo = abrir_leitura(caminho);
        if (!arquivo.is_open()) return;

        std::string linha;
        while(std::getline(arquivo, linha)){
            if(!linha.empty() && linha.back() == '\r') linha.pop_back();
            if(!linha.empty()) processar(linha);
        }
    }
}