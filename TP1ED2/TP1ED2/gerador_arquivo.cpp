#include "Registro.hpp"
#include "gerador_arquivo.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <numeric>
#include <cstring>
#include "registro.hpp"

// Configuração de tamanho do lote para gravação rápida sem estourar a RAM
constexpr int TAM_LOTE = 5000;

// Lista de nomes para gerar dados textuais com tamanho variável (até 5000 caracteres)
const std::vector<std::string> NOMES = {
    "luiza", "alice", "bruno", "carlos", "daniel", 
    "eduardo", "fernanda", "gabriel", "helena", "igor"
};

void gerarArquivos(const std::string& prefixo, int tipo, int quantidade) {
    std::string nomeBin = prefixo + ".bin";
    std::string nomeTxt = prefixo + ".txt";

    std::ofstream arqBin(nomeBin, std::ios::binary);
    std::ofstream arqTxt(nomeTxt);

    if (!arqBin.is_open() || !arqTxt.is_open()) {
        std::cerr << "[ERRO] Nao foi possivel criar os arquivos: " << prefixo << '\n';
        return;
    }

    // Buffers de 1 MB cada para otimizar escrita em disco
    std::vector<char> bufBin(1024 * 1024);
    std::vector<char> bufTxt(1024 * 1024);
    arqBin.rdbuf()->pubsetbuf(bufBin.data(), bufBin.size());
    arqTxt.rdbuf()->pubsetbuf(bufTxt.data(), bufTxt.size());

    // 1. Gera as chaves
    std::vector<int> chaves(quantidade);
    std::iota(chaves.begin(), chaves.end(), 1); // 1, 2, ..., quantidade

    if (tipo == 2) { // Decrescente
        std::reverse(chaves.begin(), chaves.end());
    } else if (tipo == 3) { // Aleatório
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(chaves.begin(), chaves.end(), g);
    }

    std::vector<TipoRegistro> lote;
    lote.reserve(TAM_LOTE);

    // 2. Monta os registros e grava
    for (int i = 0; i < quantidade; ++i) {
        TipoRegistro reg;
        reg.chave = chaves[i];
        reg.dado1 = static_cast<long>(reg.chave) * 10L;

        // Texto com tamanho dinâmico (não precisa estourar os 5000 caracteres)
        std::string texto = NOMES[i % NOMES.size()] + " " + std::to_string(reg.dado1);

        // Limpa o início e copia a string com '\0' garantido ao final
        std::memset(reg.dado2, 0, sizeof(reg.dado2));
        std::memcpy(reg.dado2, texto.c_str(), std::min(texto.size(), sizeof(reg.dado2) - 1));

        // Escrita contínua no .txt sem linhas em branco entre os registros
        arqTxt << reg.chave << "," << reg.dado1 << "," << reg.dado2 << '\n';

        lote.push_back(reg);

        // Despeja o lote no arquivo binário
        if (lote.size() == TAM_LOTE) {
            arqBin.write(reinterpret_cast<const char*>(lote.data()), lote.size() * sizeof(TipoRegistro));
            lote.clear();
        }
    }

    // Despeja os itens restantes do último lote
    if (!lote.empty()) {
        arqBin.write(reinterpret_cast<const char*>(lote.data()), lote.size() * sizeof(TipoRegistro));
        lote.clear();
    }

    arqBin.close();
    arqTxt.close();

    std::cout << "[SUCESSO] " << nomeBin << " e " << nomeTxt << " gerados com " 
              << quantidade << " registros!\n";
}

int main() {
    int tipo = 0;
    int quantidade = 0;

    std::cout << "Escolha o tipo de ordenacao:\n";
    std::cout << "1 - Ordem crescente\n";
    std::cout << "2 - Ordem decrescente\n";
    std::cout << "3 - Ordem aleatoria\n";
    std::cout << "Opcao: ";
    std::cin >> tipo;

    if (tipo < 1 || tipo > 3) {
        std::cerr << "Opcao invalida!\n";
        return 1;
    }

    std::cout << "Digite a quantidade de registros: ";
    std::cin >> quantidade;

    if (quantidade <= 0) {
        std::cerr << "Quantidade invalida!\n";
        return 1;
    }

    // Prefixo sugerido de acordo com a quantidade e a situação
    std::string sufixo = (tipo == 1) ? "c" : (tipo == 2 ? "d" : "a");
    std::string prefixo = "dados_" + std::to_string(quantidade) + "_" + sufixo;

    gerarArquivos(prefixo, tipo, quantidade);

    return 0;
}