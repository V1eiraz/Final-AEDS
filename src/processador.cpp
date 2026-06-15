#include "processador.hpp"

#include <algorithm>
#include <limits>
#include <array>

namespace{
    constexpr std::array<char, 256> fazerTabela(){
        std::array<char, 256> t{};

        for(int i = 'a'; i <= 'z'; ++i) t[i] = static_cast<char>(i);
        for(int i = 'A'; i <= 'Z'; ++i) t[i] = static_cast<char> (i + 32);

        return t;
    }

    constexpr auto TABELA_CHARS = fazerTabela();
}

void Processador::processar(std::string_view texto, Dicionario& dicionario, std::vector<uint32_t>& destinoPlano){
    const auto inicio = static_cast<uint32_t>(destinoPlano.size());

    bufferToken.clear();

    for(const unsigned char c : texto){
        const char m = TABELA_CHARS[c];

        if(m) bufferToken += m;
        else if(!bufferToken.empty()){
            if(bufferToken.size() > 3) destinoPlano.push_back(dicionario.obterOuInserir(bufferToken));
            bufferToken.clear();
        }
    }

    if(bufferToken.size() > 3) destinoPlano.push_back(dicionario.obterOuInserir(bufferToken));

    auto it = destinoPlano.begin() + inicio;
    std::sort(it, destinoPlano.end());
    destinoPlano.erase(std::unique(it, destinoPlano.end()), destinoPlano.end());
}

std::vector<uint32_t> Processador::processarConsulta(std::string_view texto, const Dicionario& dicionario){
    std::vector<uint32_t> resultado;

    bufferToken.clear();

    auto flush = [&]{
        if(bufferToken.size() > 3){
            const uint32_t id = dicionario.buscar(bufferToken);
            if(id != std::numeric_limits<uint32_t>::max()) resultado.push_back(id);
        }
        bufferToken.clear();
    };

    for(const unsigned char c : texto){
        const char m = TABELA_CHARS[c];

        if(m) bufferToken += m;
        else if(!bufferToken.empty()) flush();
    }

    if(!bufferToken.empty()) flush();

    std::sort(resultado.begin(), resultado.end());
    resultado.erase(std::unique(resultado.begin(), resultado.end()), resultado.end());

    return resultado;
}