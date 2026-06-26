#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <fstream>
#include <functional>
#include <iostream>

namespace Arquivo{
    std::ifstream abrir_leitura(const std::string& caminho);
    std::ofstream abrir_escrita(const std::string& caminho);

    void ler_csv(const std::string& caminho, const std::function<void(std::string_view, uint64_t, uint32_t)>& processar);
    void ler_consultas(const std::string& caminho, const std::function<void(const std::string&)>& processar);
}