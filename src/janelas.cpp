#include "janelas.hpp"

void Janelas::addManchete(int lineNumber, int totalLines,
                          const std::vector<uint32_t>& tokens) {

    // Calcula a quantidade de linhas por janela
    int linesPerWindow = totalLines / TOTAL_JANELAS;
    // Calcula em qual janela a linha vai ficar ex: lineNumber = 0 => j = 0 / 208771 = 0 => (Janela 1)
    int j = lineNumber / linesPerWindow;
    // última janela fica com as linhas excedentes 
    if (j >= TOTAL_JANELAS) j = TOTAL_JANELAS - 1;
    

    // Adiciona cada token à janela
    for (uint32_t id : tokens) {

        // Garante que o vetor tem espaço para esse ID
        if (id >= freq[j].size()) {
            freq[j].resize(id + 1, 0);
        }

        freq[j][id]++;
    }
}

uint32_t Janelas::getFreq(int janela, uint32_t id) const {
    if (id >= freq[janela].size()) return 0;
    return freq[janela][id];
}