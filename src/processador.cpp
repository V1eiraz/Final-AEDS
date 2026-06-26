#include "processador.hpp"

#include <algorithm>
#include <array>
#include <limits>

namespace{
    constexpr std::array<char, 256> fazer_tabela(){
        std::array<char, 256> t{};
        for (int i = 'a'; i <= 'z'; ++i) t[i] = static_cast<char>(i);
        for (int i = 'A'; i <= 'Z'; ++i) t[i] = static_cast<char>(i + 32);
        return t;
    }
    constexpr auto TABELA_CHARS = fazer_tabela();
}

void Processador::processar(std::string_view texto, Dicionario& dicionario, std::vector<uint32_t>& destino_plano){
    const auto inicio = static_cast<uint32_t>(destino_plano.size());
    buffer_token.clear();

    for(const unsigned char c : texto){
        const char m = TABELA_CHARS[c];
        if(m) buffer_token += m;
        else if(!buffer_token.empty()){
            if(buffer_token.size() > 3) destino_plano.push_back(dicionario.obter_ou_inserir(buffer_token));
            buffer_token.clear();
        }
    }
    if(buffer_token.size() > 3) destino_plano.push_back(dicionario.obter_ou_inserir(buffer_token));

    auto it = destino_plano.begin() + inicio;
    std::sort(it, destino_plano.end());
    destino_plano.erase(std::unique(it, destino_plano.end()), destino_plano.end());
}

std::vector<uint32_t> Processador::processar_consulta(std::string_view texto, const Dicionario& dicionario){
    std::vector<uint32_t> resultado;
    buffer_token.clear();

    auto flush = [&]{
        if(buffer_token.size() > 3){
            const uint32_t id = dicionario.buscar(buffer_token);
            if(id != std::numeric_limits<uint32_t>::max()) resultado.push_back(id);
        }
        buffer_token.clear();
    };

    for(const unsigned char c : texto){
        const char m = TABELA_CHARS[c];
        if(m) buffer_token += m;
        else if(!buffer_token.empty()) flush();
    }
    if(!buffer_token.empty()) flush();

    std::sort(resultado.begin(), resultado.end());
    resultado.erase(std::unique(resultado.begin(), resultado.end()), resultado.end());
    return resultado;
}