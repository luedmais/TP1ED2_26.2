#include "registro.hpp"
#include "index.hpp"
#include <algorithm>
#include <fstream>
#include <cmath>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#define ITENSPAGINA 100

bool SequencialIndexado::pesquisar(const std::string& nomeArquivo, 
                                   int quantidade, 
                                   int situacao, 
                                   int chavePesquisada, 
                                   TipoRegistro& registroRetorno) {
    
    // Zera contadores internos a cada nova execução
    nTransfPre = 0;
    nCompPre = 0;
    tempoPre = 0.0;
    nTransfPesq = 0;
    nCompPesq = 0;
    tempoPesq = 0.0;

    if (situacao == 3) {
        std::cerr << "[AVISO] Metodo 1 nao se aplica a arquivos aleatorios (Situacao 3).\n";
        return false;
    }

    std::ifstream arq(nomeArquivo, std::ios::binary);
    if (!arq.is_open()) {
        std::cerr << "[ERRO] Falha ao abrir: " << nomeArquivo << '\n';
        return false;
    }

    int totalPaginas = std::ceil(static_cast<double>(quantidade) / ITENSPAGINA);
    std::vector<IndicePagina> tabelaIndice;
    tabelaIndice.reserve(totalPaginas);

    // ==========================================================
    // ETAPA 1: Construção do Índice
    // ==========================================================
    auto inicioPre = std::chrono::high_resolution_clock::now();

    for (int p = 0; p < totalPaginas; ++p) {
        long offset = static_cast<long>(p) * ITENSPAGINA * sizeof(TipoRegistro);
        arq.seekg(offset, std::ios::beg);

        TipoRegistro primeiroItem;
        if (arq.read(reinterpret_cast<char*>(&primeiroItem), sizeof(TipoRegistro))) {
            nTransfPre++;
            tabelaIndice.push_back({primeiroItem.chave, p});
        }
    }

    auto fimPre = std::chrono::high_resolution_clock::now();
    tempoPre = std::chrono::duration<double>(fimPre - inicioPre).count();

    // ==========================================================
    // ETAPA 2: Pesquisa da Chave
    // ==========================================================
    auto inicioPesq = std::chrono::high_resolution_clock::now();

    if (tabelaIndice.empty()) {
        arq.close();
        return false;
    }

    int paginaAlvo = -1;

    if (situacao == 1) { // Crescente
        nCompPesq++;
        if (chavePesquisada < tabelaIndice[0].chave) {
            paginaAlvo = -1;
        } else {
            int i = 0;
            while (i < static_cast<int>(tabelaIndice.size())) {
                nCompPesq++;
                if (tabelaIndice[i].chave > chavePesquisada) {
                    break;
                }
                i++;
            }
            paginaAlvo = i - 1;
        }
    } else if (situacao == 2) { // Decrescente
        nCompPesq++;
        if (chavePesquisada > tabelaIndice[0].chave) {
            paginaAlvo = -1;
        } else {
            int i = 0;
            while (i < static_cast<int>(tabelaIndice.size())) {
                nCompPesq++;
                if (tabelaIndice[i].chave < chavePesquisada) {
                    break;
                }
                i++;
            }
            paginaAlvo = i - 1;
        }
    }

    if (paginaAlvo < 0 || paginaAlvo >= static_cast<int>(tabelaIndice.size())) {
        auto fimPesq = std::chrono::high_resolution_clock::now();
        tempoPesq = std::chrono::duration<double>(fimPesq - inicioPesq).count();
        arq.close();
        return false;
    }

    long offsetPagina = static_cast<long>(paginaAlvo) * ITENSPAGINA * sizeof(TipoRegistro);
    arq.seekg(offsetPagina, std::ios::beg);

    int registrosRestantes = quantidade - (paginaAlvo * ITENSPAGINA);
    int itensParaLer = std::min(ITENSPAGINA, registrosRestantes);

    std::vector<TipoRegistro> bufferPagina(itensParaLer);
    arq.read(reinterpret_cast<char*>(bufferPagina.data()), itensParaLer * sizeof(TipoRegistro));
    nTransfPesq++;

    bool encontrado = false;
    for (int k = 0; k < itensParaLer; ++k) {
        nCompPesq++;
        if (bufferPagina[k].chave == chavePesquisada) {
            registroRetorno = bufferPagina[k];
            encontrado = true;
            break;
        }
    }

    auto fimPesq = std::chrono::high_resolution_clock::now();
    tempoPesq = std::chrono::duration<double>(fimPesq - inicioPesq).count();

    arq.close();
    return encontrado;
}

