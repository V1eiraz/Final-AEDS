#pragma once

#include "dicionario.hpp"
#include "processador.hpp"
#include <vector>
#include <string>
#include <cstdint>

// Vai representar as manchetes similares
struct Similar{
    int lineNumber;     //indice da linha no CSV 
    std::string texto;  //manchete original
    float jaccard;      // similaridade
};

class Jaccard{
public:
    // Calcula o indice de Jaccard entre dois vetores de IDs ordenados
    // os vetores ja vem ordenados do Processador
    static float calcular(const std::vector<uint32_t>& A,
                          const std::vector<uint32_t>& B);

    //Dado o índice de uma manchete alvo, busca as top 10 similares
    // vai reler o CSV duas vezes< uma pra pegar a alvo, outra pra comparar
    std::vector<Similar> buscarSimilares(const std::string& caminhoCSV,
                                        int indiceBusca,
                                        Dicionario& dicionario,
                                        Processador& processador);

};