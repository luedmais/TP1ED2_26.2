#ifndef PAGINACAO_HPP
#define PAGINACAO_HPP

#include <cstdio>
#include "registro.hpp"

// TAM_PAG = quantidade de registros que cabem em uma "página" / moldura de
// página da memória secundária (ver slide "Sistema de Paginação").
// Cada leitura de uma página inteira == 1 transferência entre memória
// secundária e principal.
#define TAM_PAG 100

typedef struct {
    TipoRegistro registros[TAM_PAG];
    int quantidade; // quantos registros desta página são válidos (a última pode ser parcial)
} PagReg;

// Marcação de tempo (início/fim), em nanossegundos.
typedef struct {
    unsigned long long inicio;
    unsigned long long fim;
} Time;

// Estatísticas de execução de um método: tempo, nº de comparações entre
// chaves e nº de transferências (acessos) entre memória secundária e principal.
typedef struct {
    unsigned long long tempo;
    int comparacoes;
    int acessos;
    Time time;
} Estatisticas;

void iniciaEstatisticas(Estatisticas *estatisticas);

// Abre o arquivo binário de registros.
FILE *abrirArquivo(const char *nomeDoArquivo);
bool  fecharArquivo(FILE *arquivo);

// Lê a próxima página (até TAM_PAG registros) do arquivo, contando 1 acesso.
// Retorna true se leu ao menos 1 registro.
bool paginacao(FILE *arquivo, PagReg *pagina, Estatisticas *estatisticas);

#endif // PAGINACAO_HPP
