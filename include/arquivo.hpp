#pragma once
#include <string>
#include <fstream>
#include <functional>
#include <iostream>

namespace Arquivo{
    std::ifstream abrir_leitura(const std::string& caminho);
    std::ofstream abrir_escrita(const std::string& caminho);

    void ler_csv(const std::string& caminho, const std::function<void(std::string&&)>& processar);
    void ler_consultas(const std::string& caminho, const std::function<void(const std::string&)>& processar);
}
namespace Arquivo {
    std::ifstream abrir_leitura(const std::string& caminho);
    std::ofstream abrir_escrita(const std::string& caminho);
    std::ofstream abrir_escrita_bufferizada(const std::string& caminho);
    void ler_csv(const std::string& caminho, const std::function<void(std::string&&)>& processar);
    void ler_consultas(const std::string& caminho, const std::function<void(const std::string&)>& processar);
}