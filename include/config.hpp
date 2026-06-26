#pragma once

#include <cstdint>

inline constexpr float LIMIAR_JACCARD = 0.0f;
inline constexpr uint32_t MAX_RESULTADOS_SIMILARES = 10;
inline constexpr uint32_t LIMITE_POSTAGENS_TOKEN = 300000;
inline constexpr int TOTAL_JANELAS = 5;
inline constexpr uint32_t TAMANHO_JANELA = 200000;
inline constexpr uint32_t TAMANHO_RANKING = 100;
inline constexpr uint32_t CAPACIDADE_DICIONARIO = 1 << 17;
inline constexpr uint32_t CAPACIDADE_MANCHETES = 1 << 20;
inline constexpr uint32_t CAPACIDADE_TOKENS_PLANOS = 7 << 20;
inline constexpr uint32_t TAMANHO_BUFFER_IO = 1 << 20;