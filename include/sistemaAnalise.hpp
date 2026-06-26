#pragma once

#include "config.hpp"
#include "dicionario.hpp"
#include "processador.hpp"
#include "indiceInvertido.hpp"
#include "janelas.hpp"

#include <string>
#include <vector>
#include <unordered_set>

struct DadosManchete{
    uint64_t offset_csv;
    uint32_t tamanho_texto;
    uint32_t inicio_tokens;
    uint32_t fim_tokens;
};

class SistemaAnalise{
public:
    void carregar(const std::string& arquivo_csv);
    void analisar(const std::string& arquivo_entrada, const std::string& arquivo_saida);
private:
    Dicionario dicionario;
    Processador processador;
    IndiceInvertido indice;
    JanelasTemporais janelas;

    std::string caminho_csv;
    std::vector<DadosManchete> manchetes;
    std::vector<uint32_t> tokens_planos;
    std::unordered_set<uint32_t> ids_emergentes;

    std::vector<std::pair<uint32_t, uint32_t>> ranking_global;
    std::vector<TermoEmergente> ranking_emergentes;

    void preparar_rankings();
};