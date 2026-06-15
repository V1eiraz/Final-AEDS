#pragma once

#include "dicionario.hpp"

#include <string>
#include <string_view>
#include <vector>
#include <cstdint>

class Processador{
    public:
        void processar(std::string_view texto, Dicionario& dicionario, std::vector<uint32_t>& destinoPlano);
        std::vector<uint32_t> processarConsulta(std::string_view texto, const Dicionario& dicionario);
    private:
        std::string bufferToken;
};
