#include "registro.hpp"
#include "gerador_arquivo.hpp"

#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <numeric>
#include <cstring>
#include <iostream>
#include <sys/stat.h>

constexpr int TAM_LOTE = 5000;

static bool arquivoExiste(const std::string &nome) {
    struct stat buffer;
    return (stat(nome.c_str(), &buffer) == 0);
}

std::string gerarNomeArquivo(int quantidade, int situacao) {
    return "dados_" + std::to_string(quantidade) + "_" + std::to_string(situacao) + ".bin";
}

std::string garantirArquivoExiste(int quantidade, int situacao) {
    std::string nomeArquivo = gerarNomeArquivo(quantidade, situacao);
    if (arquivoExiste(nomeArquivo)) {
        return nomeArquivo; // já existe: não gera de novo
    }

    std::ofstream arqBin(nomeArquivo, std::ios::binary);
    if (!arqBin.is_open()) {
        std::cerr << "[ERRO] Nao foi possivel criar o arquivo: " << nomeArquivo << '\n';
        return nomeArquivo;
    }

    std::vector<char> bufBin(1024 * 1024);
    arqBin.rdbuf()->pubsetbuf(bufBin.data(), bufBin.size());

    std::vector<int> chaves(quantidade);
    std::iota(chaves.begin(), chaves.end(), 1); // 1, 2, ..., quantidade

    if (situacao == 2) { // decrescente
        std::reverse(chaves.begin(), chaves.end());
    } else if (situacao == 3) { // aleatório
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(chaves.begin(), chaves.end(), g);
    }
    // situacao == 1 (crescente): mantém 1..quantidade na ordem natural.

    std::vector<TipoRegistro> lote;
    lote.reserve(TAM_LOTE);

    for (int i = 0; i < quantidade; ++i) {
        TipoRegistro reg;
        reg.chave = chaves[i];
        reg.dado1 = static_cast<long>(reg.chave) * 10L;

        std::string texto = "registro_" + std::to_string(reg.chave);
        std::memset(reg.dado2, 0, sizeof(reg.dado2));
        std::memcpy(reg.dado2, texto.c_str(), std::min(texto.size(), sizeof(reg.dado2) - 1));

        lote.push_back(reg);
        if (lote.size() == TAM_LOTE) {
            arqBin.write(reinterpret_cast<const char *>(lote.data()), lote.size() * sizeof(TipoRegistro));
            lote.clear();
        }
    }

    if (!lote.empty()) {
        arqBin.write(reinterpret_cast<const char *>(lote.data()), lote.size() * sizeof(TipoRegistro));
    }

    arqBin.close();
    return nomeArquivo;
}
