#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include "config.hpp"
#include "dicionario.hpp"
#include "processador.hpp"
#include "indiceInvertido.hpp"
#include "janelas.hpp"

struct DadosManchete {
    std::string  texto;
    uint32_t     inicio_tokens;
    uint32_t     fim_tokens;
};

class SistemaAnalise {
public:
    void carregar(const std::string& arquivo_csv);
    void analisar(const std::string& arquivo_entrada, const std::string& arquivo_saida);

private:
    Dicionario       dicionario;
    Processador      processador;
    IndiceInvertido  indice;
    JanelasTemporais janelas;

    std::vector<DadosManchete> manchetes;
    std::vector<uint32_t>      tokens_planos;
    std::unordered_set<uint32_t> ids_emergentes;

    void preparar_rankings();
    void salvar_ranking_global(const std::string& caminho) const;
    void salvar_ranking_emergentes(const std::string& caminho) const;
};