#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <limits>

struct HashString{
    using is_transparent = void;
    std::size_t operator()(std::string_view sv) const noexcept{
        return std::hash<std::string_view>{}(sv);
    }
    std::size_t operator()(const std::string& s) const noexcept{
        return std::hash<std::string_view>{}(s);
    }
};

struct IgualString{
    using is_transparent = void;
    bool operator()(std::string_view a, std::string_view b) const noexcept{
        return a == b;
    }
};

class Dicionario{
public:
    void reservar(uint32_t n);
    uint32_t obter_ou_inserir(std::string_view termo);
    uint32_t buscar(std::string_view termo) const;
    const std::string& obter_termo(uint32_t id) const;
    uint32_t tamanho() const;
private:
    std::unordered_map<std::string, uint32_t, HashString, IgualString> mapa;
    std::vector<std::string> vocabulario;
};