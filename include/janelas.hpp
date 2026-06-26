#pragma once

#include "config.hpp"

#include <array>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <utility>

struct TermoEmergente{
    uint32_t id;
    float taxa;
};

class JanelasTemporais{
public:
    void reservar(uint32_t n);
    void registrar(uint32_t id_termo, uint32_t indice_manchete);
    std::vector<std::pair<uint32_t, uint32_t>> top_global(uint32_t n) const;
    std::vector<TermoEmergente> top_emergentes(uint32_t n) const;
private:
    std::array<std::unordered_map<uint32_t, uint32_t>, TOTAL_JANELAS> janelas;
    std::unordered_map<uint32_t, uint32_t> global;
};