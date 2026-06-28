# 📚 Trabalho Prático Final - Algoritmos e Estruturas de Dados I

![Status](https://img.shields.io/badge/Status-Concluído-success?style=for-the-badge)
![Linguagem](https://img.shields.io/badge/Linguagem-C%2B%2B-blue?style=for-the-badge)
![Instituição](https://img.shields.io/badge/Instituição-CEFET--MG-red?style=for-the-badge)

---

## 📖 Introdução

Este repositório contém a implementação do trabalho final da disciplina de **Algoritmos e Estruturas de Dados I (AEDS I)** do CEFET-MG. 

O objetivo principal deste projeto é aplicar os conceitos teóricos de indexação e organização de dados estruturando soluções em C++. O código implementa e analisa o comportamento de Tabelas de Dispersão (Hash), Heaps e métodos clássicos de ordenação, avaliando suas complexidades e eficiências computacionais na prática.

**Autores:**
* Davi Enzo Vieira
* Otávio Alves Lima
* Samuel Pianetti Cordeiro
---

## ⚙️ Compilação e Execução

O projeto foi desenvolvido para rodar em ambientes Linux (ou Windows utilizando o Ubuntu WSL) e utiliza um `Makefile` para automatizar e facilitar o processo de compilação. Para testar o código em sua máquina, siga os passos abaixo no seu terminal.

### 📋 Pré-requisitos
Certifique-se de ter os compiladores (`gcc` / `g++`) e a ferramenta `make` devidamente instalados no seu ambiente.

### 🚀 Passo a Passo

**1. Clone este repositório:**
```bash
git clone https://github.com/V1eiraz/Final-AEDS
```

**2. Acesse a pasta raiz do projeto:**
```bash
cd Final-AEDS
```

**3. Compile o código-fonte:**
Basta executar o comando abaixo. Ele lerá as instruções do `Makefile` e gerará o arquivo executável.
```bash
make
```

**4. Execute o programa:**
Após a compilação bem-sucedida, rode o executável com o comando:
```bash
make run
```
> **Nota:** *Caso o seu Makefile não possua a regra `run`, você pode executar o programa diretamente digitando o nome do executável gerado, por exemplo: `./executavel`.*

### 🧹 Limpeza (Opcional)
Se precisar recompilar o código do zero ou apenas limpar sua pasta, você pode apagar os arquivos binários temporários executando:
```bash
make clean
```
