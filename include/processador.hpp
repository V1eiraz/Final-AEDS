#pragma once
#include <string_view>
#include <vector>
#include <cstdint>
#include <string>
#include "dicionario.hpp"

class Processador {
public:
    // [OTM-3] string_view: sem cópia do parâmetro de texto
    // [OTM-6] Escreve tokens diretamente no vetor plano (CSR) em vez de retornar vector
    void processar(std::string_view texto, Dicionario& dicionario,
                   std::vector<uint32_t>& destino_plano);

    // Para consultas: retorna vector próprio (poucas chamadas, overhead aceitável)
    std::vector<uint32_t> processar_consulta(std::string_view texto,
                                              const Dicionario& dicionario);

private:
    // [OTM-2] Buffer reutilizado entre chamadas: elimina alocação de string por token
    std::string buffer_token;
};