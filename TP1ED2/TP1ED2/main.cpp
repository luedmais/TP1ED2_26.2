#include "gerador_arquivo.hpp"
#include "index.hpp"
extern "C" {
#include "arvore_binaria.h"
}

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

namespace {

void imprimirUso(const char* programa) {
    std::cerr << "Uso: " << programa
              << " <metodo> <quantidade> <situacao> <chave> [-p]\n";
}

bool quantidadeValida(int quantidade) {
    return quantidade == 100 || quantidade == 200 || quantidade == 2000 ||
           quantidade == 20000 || quantidade == 200000 || quantidade == 2000000;
}

bool converterInteiro(const char* texto, int& valor) {
    try {
        std::size_t caracteresLidas = 0;
        valor = std::stoi(texto, &caracteresLidas);
        return caracteresLidas == std::strlen(texto);
    } catch (...) {
        return false;
    }
}

std::string nomeArvore(int quantidade, int situacao) {
    const std::string sufixo = situacao == 1 ? "c" : situacao == 2 ? "d" : "a";
    return "arvore_binaria_" + std::to_string(quantidade) + "_" + sufixo + ".bin";
}

bool garantirArvoreBinaria(const std::string& nomeArquivo,
                           const std::string& nomeDados,
                           int quantidade) {
    if (fs::exists(nomeArquivo)) {
        return true;
    }

    FILE* dados = std::fopen(nomeDados.c_str(), "rb");
    FILE* arvore = std::fopen(nomeArquivo.c_str(), "wb+");
    if (dados == nullptr || arvore == nullptr) {
        if (dados != nullptr) {
            std::fclose(dados);
        }
        if (arvore != nullptr) {
            std::fclose(arvore);
        }
        return false;
    }

    int acessos = 0;
    int comparacoes = 0;
    for (int indice = 0; indice < quantidade; ++indice) {
        TipoRegistro registro{};
        if (std::fread(&registro, sizeof(registro), 1, dados) != 1) {
            std::fclose(dados);
            std::fclose(arvore);
            std::remove(nomeArquivo.c_str());
            return false;
        }

        NoArvore no{};
        no.chave = registro.chave;
        no.dado1 = registro.dado1;
        std::memcpy(no.dado2, registro.dado2, sizeof(no.dado2));
        no.esquerda = -1;
        no.direita = -1;
        no.indice = indice;

        std::fseek(arvore, 0, SEEK_END);
        std::fwrite(&no, sizeof(no), 1, arvore);
        std::fflush(arvore);
        if (indice > 0) {
            inserirNaArvore(arvore, 0, indice, &acessos, &comparacoes);
        }
    }

    std::fclose(dados);
    std::fclose(arvore);
    return true;
}

bool lerRegistroArvore(const std::string& nomeArquivo, int chave, NoArvore& resultado) {
    FILE* arvore = std::fopen(nomeArquivo.c_str(), "rb");
    if (arvore == nullptr) {
        return false;
    }

    NoArvore no{};
    while (std::fread(&no, sizeof(no), 1, arvore) == 1) {
        if (no.chave == chave) {
            resultado = no;
            std::fclose(arvore);
            return true;
        }
    }

    std::fclose(arvore);
    return false;
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc != 5 && argc != 6) {
        imprimirUso(argv[0]);
        return 1;
    }

    int metodo = 0;
    int quantidade = 0;
    int situacao = 0;
    int chave = 0;

    if (!converterInteiro(argv[1], metodo) ||
        !converterInteiro(argv[2], quantidade) ||
        !converterInteiro(argv[3], situacao) ||
        !converterInteiro(argv[4], chave)) {
        std::cerr << "[ERRO] Os quatro primeiros argumentos devem ser inteiros.\n";
        imprimirUso(argv[0]);
        return 1;
    }

    const bool imprimirRegistro = argc == 6 && std::string(argv[5]) == "-p";
    if ((argc == 6 && !imprimirRegistro) || metodo < 1 || metodo > 4 ||
        !quantidadeValida(quantidade) || situacao < 1 || situacao > 3) {
        std::cerr << "[ERRO] Argumentos fora do formato exigido.\n";
        imprimirUso(argv[0]);
        return 1;
    }

    if (metodo != 1 && metodo != 2) {
        std::cerr << "[ERRO] O metodo " << metodo
                  << " ainda nao foi implementado.\n";
        return 1;
    }

    const std::string nomeArquivo = GeradorArquivo::gerarNomeArquivo(quantidade, situacao);
    if (!GeradorArquivo::garantirArquivoExiste(quantidade, situacao)) {
        std::cerr << "[ERRO] Nao foi possivel criar ou abrir " << nomeArquivo << ".\n";
        return 1;
    }

    if (metodo == 1) {
        SequencialIndexado pesquisa;
        TipoRegistro registro{};
        const bool encontrado = pesquisa.pesquisar(
            nomeArquivo, quantidade, situacao, chave, registro);

        std::cout << "Transferencias de pre-processamento: " << pesquisa.getTransfPre() << '\n';
        std::cout << "Comparacoes de pre-processamento: " << pesquisa.getCompPre() << '\n';
        std::cout << "Tempo de pre-processamento: " << pesquisa.getTempoPre() << " s\n";
        std::cout << "Transferencias da pesquisa: " << pesquisa.getTransfPesq() << '\n';
        std::cout << "Comparacoes da pesquisa: " << pesquisa.getCompPesq() << '\n';
        std::cout << "Tempo da pesquisa: " << pesquisa.getTempoPesq() << " s\n";

        if (!encontrado) {
            std::cout << "Chave nao encontrada.\n";
            return 0;
        }

        if (imprimirRegistro) {
            std::cout << "Chave: " << registro.chave << '\n';
            std::cout << "Dado 1: " << registro.dado1 << '\n';
            std::cout << "Dado 2: " << registro.dado2 << '\n';
        }
    } else {
        const std::string nomeArquivoArvore = nomeArvore(quantidade, situacao);
        if (!garantirArvoreBinaria(nomeArquivoArvore, nomeArquivo, quantidade)) {
            std::cerr << "[ERRO] Nao foi possivel construir " << nomeArquivoArvore << ".\n";
            return 1;
        }

        FILE* arvore = std::fopen(nomeArquivoArvore.c_str(), "rb");
        if (arvore == nullptr) {
            std::cerr << "[ERRO] Nao foi possivel abrir " << nomeArquivoArvore << ".\n";
            return 1;
        }

        int acessos = 0;
        int comparacoes = 0;
        const bool encontrado = buscarNaArvore(chave, arvore, &acessos, &comparacoes) != -1;
        std::fclose(arvore);

        std::cout << "Acessos: " << acessos << '\n';
        std::cout << "Comparacoes: " << comparacoes << '\n';

        if (!encontrado) {
            std::cout << "Chave nao encontrada.\n";
            return 0;
        }

        if (imprimirRegistro) {
            NoArvore registro{};
            if (lerRegistroArvore(nomeArquivoArvore, chave, registro)) {
                std::cout << "Chave: " << registro.chave << '\n';
                std::cout << "Dado 1: " << registro.dado1 << '\n';
                std::cout << "Dado 2: " << registro.dado2 << '\n';
            }
        }
    }

    return 0;
}
