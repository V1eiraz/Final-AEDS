#pragma once

#include "dicionario.hpp"

#include <string_view>
#include <vector>
#include <cstdint>
#include <string>

class Processador{
public:
    void processar(std::string_view texto, Dicionario& dicionario, std::vector<uint32_t>& destino_plano);
    std::vector<uint32_t> processar_consulta(std::string_view texto, const Dicionario& dicionario);
private:
    std::string buffer_token;
};