#include "arquivo.hpp"

#include <cstdint>

namespace Arquivo{
    std::ifstream abrir_leitura(const std::string& caminho){
        std::ifstream f(caminho, std::ios::binary);
        if(!f.is_open()) std::cerr << "Erro: não foi possível abrir " << caminho << '\n';

        return f;
    }

    std::ofstream abrir_escrita(const std::string& caminho){
        std::ofstream f(caminho);
        if (!f.is_open()) std::cerr << "Erro: não foi possível abrir " << caminho << '\n';

        return f;
    }

    void ler_csv(const std::string& caminho, const std::function<void(std::string_view, uint64_t, uint32_t)>& processar){
        std::ifstream arquivo(caminho, std::ios::binary);
        if(!arquivo.is_open()){
            std::cerr << "Erro: não foi possível abrir " << caminho << '\n';
            return;
        }

        std::string linha;
        std::getline(arquivo, linha);

        while(true){
            const uint64_t pos_linha = static_cast<uint64_t>(arquivo.tellg());

            if(!std::getline(arquivo, linha)) break;
            if(!linha.empty() && linha.back() == '\r') linha.pop_back();

            const auto pos_virgula = linha.find(',');
            if(pos_virgula == std::string::npos){
                processar({}, 0, 0);
                continue;
            }

            const uint64_t offset  = pos_linha + pos_virgula + 1;
            const uint32_t tamanho = static_cast<uint32_t>(linha.size() - pos_virgula - 1);
            processar(std::string_view(linha.data() + pos_virgula + 1, tamanho), offset, tamanho);
        }
    }

    void ler_consultas(const std::string& caminho, const std::function<void(const std::string&)>& processar){
        std::ifstream arquivo(caminho, std::ios::binary);
        if(!arquivo.is_open()){
            std::cerr << "Erro: não foi possível abrir " << caminho << '\n';
            return;
        }

        std::string linha;
        while(std::getline(arquivo, linha)){
            if(!linha.empty() && linha.back() == '\r') linha.pop_back();
            if(!linha.empty()) processar(linha);
        }
    }

}