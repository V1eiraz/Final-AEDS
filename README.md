# 📰 Monitoramento de Tendências em Manchetes de Notícias
### Trabalho Prático Final — Algoritmos e Estruturas de Dados I

![Status](https://img.shields.io/badge/Status-Concluído-success?style=for-the-badge)
![Linguagem](https://img.shields.io/badge/Linguagem-C%2B%2B20-blue?style=for-the-badge)
![Instituição](https://img.shields.io/badge/Instituição-CEFET--MG-red?style=for-the-badge)

---

## 📖 Introdução

Este repositório contém a implementação do trabalho final da disciplina de **Algoritmos e Estruturas de Dados I (AEDS I)** do CEFET-MG, ministrada pelo Prof. Michel Pires da Silva.

O projeto implementa um sistema de análise de grandes volumes de texto jornalístico sobre a base [A Million News Headlines](https://www.kaggle.com/datasets/therohk/million-headlines) (ABC Australia / Kaggle), composta por aproximadamente **1,2 milhão de manchetes** coletadas entre 2003 e 2021.

O sistema resolve três problemas computacionais centrais:

1. **Ranqueamento global:** identifica as 100 palavras mais frequentes em todo o corpus, após normalização e limpeza do texto.
2. **Termos emergentes:** divide o corpus em 5 janelas cronológicas e calcula a taxa de crescimento de relevância $C(p) = (F_{J5} - F_{J1}) / (F_{J1} + 1)$ para cada termo, produzindo o Top-100 de palavras em ascensão ao longo do tempo.
3. **Busca por similaridade:** dado um conjunto de manchetes-consulta, encontra as 10 mais similares no corpus usando o **Índice de Jaccard**, cruzando os resultados com os termos emergentes para extrair palavras-chave contextuais.

Para processar essa escala de dados com eficiência, o sistema emprega:

- **Índice Invertido** com acumulação de interseção, substituindo a comparação ingênua $\mathcal{O}(n^2)$ por custo proporcional ao conjunto candidato real.
- **Vocabulário numérico** com hash transparente (`string_view` sem cópia), representando todas as manchetes como um único vetor contíguo de inteiros.
- **Tabela de remapeamento compile-time** (`constexpr`) no pré-processamento, eliminando desvios condicionais no laço mais interno.
- **Paralelismo de consultas** via `std::thread`, distribuindo o trabalho entre os núcleos disponíveis.

**Autores:**
- Davi Enzo Vieira
- Otávio Alves Lima
- Samuel Pianetti Cordeiro

---

## 🗂️ Estrutura do Projeto

```
Final-AEDS/
├── data/
│   ├── input.csv       # Base A Million News Headlines (não incluída no repo)
│   ├── input.dat       # ⚠️ Arquivo de consultas (veja seção abaixo)
│   ├── output.dat      # Saída gerada: resultados de similaridade por consulta
│   └── outros.dat      # Saída gerada: rankings global, emergente e keywords
├── include/
│   ├── arquivo.hpp
│   ├── config.hpp
│   ├── dicionario.hpp
│   ├── indiceInvertido.hpp
│   ├── janelas.hpp
│   ├── processador.hpp
│   └── sistemaAnalise.hpp
├── src/
│   ├── arquivo.cpp
│   ├── dicionario.cpp
│   ├── indiceInvertido.cpp
│   ├── janelas.cpp
│   ├── main.cpp
│   ├── processador.cpp
│   └── sistemaAnalise.cpp
└── Makefile
```

---

## 📂 Arquivos de Entrada e Saída

### Entradas obrigatórias

| Arquivo | Local | Descrição |
|---|---|---|
| `input.csv` | `data/input.csv` | Base A Million News Headlines. Deve ser baixada do [Kaggle](https://www.kaggle.com/datasets/therohk/million-headlines) e colocada nessa pasta. |
| `input.dat` | `data/input.dat` | Arquivo de consultas com as manchetes a pesquisar (uma por linha, texto puro). **Deve ser criado manualmente antes de executar** — veja o formato abaixo. |

### Formato do `input.dat`

O arquivo deve conter uma manchete por linha, em texto puro, sem cabeçalho. O sistema aplica o mesmo pipeline de pré-processamento (normalização, remoção de pontuação, tokenização) às consultas antes de buscar similaridade. Exemplo:

```
thousands gather for urban country music festival
moreton bay coral under threat
labor says govt using cole inquiry to avoid
police search for missing child
australia signs new trade agreement
```

> **⚠️ Atenção:** o arquivo **deve se chamar exatamente `input.dat`** e estar na pasta `data/`. Qualquer outro nome ou extensão causará erro de leitura.

### Saídas geradas

Após a execução, dois arquivos são gerados automaticamente em `data/`:

**`output.dat`** — Para cada consulta, lista as 10 manchetes mais similares com seu valor de Jaccard:

```
Manchete Original: thousands gather for urban country music festival
Top 10 similares:
1 - bat bus to run during country music festival [Jaccard = 0.4286]
2 - tamworth country music festival [Jaccard = 0.4286]
3 - grant opens tamworth country music festival [Jaccard = 0.3750]
...
```

**`outros.dat`** — Contém os rankings globais, os termos emergentes e as palavras-chave por consulta:

```
Top 100 Global:
1 - over = 54514
2 - police = 39756
3 - with = 36251
...

Top 100 Emergentes:
1 - coronavirus [C(p) = 4821.00]
2 - covid [C(p) = 1203.00]
...

Keywords por consulta:
[thousands gather for urban country music festival]
  → festival, music, country
```

---

## ⚙️ Compilação e Execução

O projeto foi desenvolvido para rodar em ambientes **Linux** (ou **Windows via WSL**) e utiliza um `Makefile` para automatizar a compilação. Requer `g++` com suporte a **C++20**.

### 📋 Pré-requisitos

```bash
# Ubuntu / Debian / WSL
sudo apt update && sudo apt install g++ make
```

### 🚀 Passo a Passo

**1. Clone o repositório:**
```bash
git clone https://github.com/V1eiraz/Final-AEDS
cd Final-AEDS
```

**2. Adicione os arquivos de entrada em `data/`:**

- Crie o arquivo `data/input.dat` com as manchetes que deseja consultar (uma por linha)

**3. Compile:**
```bash
make
```

**4. Execute:**
```bash
make run
```

O tempo de execução será impresso ao final. Os resultados estarão em `data/output.dat` e `data/outros.dat`.

### 🧹 Limpeza

```bash
make clean
```
