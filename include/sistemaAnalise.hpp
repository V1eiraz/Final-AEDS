#pragma once

#include "processador.hpp"
#include "dicionario.hpp"
#include "config.hpp"

#include <string>

class SistemaAnalise{
    public:
        void carregar(const std::string& arquivo_csv);
        void analisar(const std::string& arquivo_entrada, const std::string& arquivo_saida);
    private:
        Dicionario dicionario;
        Processador processador;
};