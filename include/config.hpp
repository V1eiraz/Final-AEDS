#pragma once

#include <cstdint>

inline constexpr float LIMIAR_JACCARD = 0.5f;
inline constexpr uint32_t MAX_RESULTADOS_SIMILARES = 10;
inline constexpr int TOTAL_JANELAS = 5;
inline constexpr uint32_t TAMANHO_RANKING = 100;
inline constexpr uint32_t CAPACIDADE_DICIONARIO = 1 << 17;
inline constexpr uint32_t TAMANHO_BUFFER_IO = 1 << 20;
