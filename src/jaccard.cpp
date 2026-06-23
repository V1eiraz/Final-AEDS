#include "jaccard.hpp"
#include "config.hpp"
#include <fstream>
#include <algorithm>
#include <queue>


// Calcula Jaccard entre dois vetores de IDs ORDENADOS
// Usa merge para calcular interseção e união em O(n+m)
float Jaccard::calcular(const std::vector<uint32_t>& A, const std::vector<uint32_t>& B) {
    if (A.empty() && B.empty()) return 1.0f;
    if (A.empty() || B.empty()) return 0.0f;

    int intersecao = 0;
    int i = 0, j = 0;

    // Percorre os dois vetores ao mesmo tempo (como merge sort)
    // contando quantos IDs aparecem nos dois (interseção)
    while (i < (int)A.size() && j < (int)B.size()) {
        if(A[i] == B[j]){
            intersecao++;
            i++;
            j++;
        }else if(A[i] < B[j]){
            i++;
        }else{
            j++;
        }
    }
    //uniao = tamanho a + tamanho b - interseçao
    int uniao = (int)A.size() + (int)B.size() - intersecao;

    return (float)intersecao / (float)uniao;
}

std::vector<Similar> Jaccard::buscarSimilares(const std::string& caminhoCSV,int indiceBusca, Dicionario& dicionario, Processador& processador){

    std::vector<uint32_t> tokensAlvo;
    std::string textoAlvo;
    {
        std::ifstream f(caminhoCSV);
        std::string linha;
        int n=0;

        while (std::getline(f, linha)){
            if (n == indiceBusca){
                textoAlvo=linha;
                processador.processar(linha, dicionario, tokensAlvo);
                break;
            }
            n++;
        }
    }

      std::vector<Similar> resultado;

    {
        std::ifstream f(caminhoCSV);
        std::string linha;
        int n = 0;

        std::vector<uint32_t> tokensLinha;

        while (std::getline(f, linha)) {
            if (n != indiceBusca) {  // não compara com ela mesma
                tokensLinha.clear();
                processador.processar(linha, dicionario, tokensLinha);

                float score = calcular(tokensAlvo, tokensLinha);

                if (score >= LIMIAR_JACCARD) {
                    resultado.push_back({n, linha, score});
                }
            }
            n++;
        }
    }

    // Ordena por Jaccard decrescente e pega as top 10
    std::sort(resultado.begin(), resultado.end(),
              [](const Similar& a, const Similar& b) {
                  return a.jaccard > b.jaccard;
              });
               if (resultado.size() > MAX_RESULTADOS_SIMILARES)
        resultado.resize(MAX_RESULTADOS_SIMILARES);

    return resultado;

}