#ifndef PAGINACAO_HPP
#define PAGINACAO_HPP

#include <cstdio>
#include "registro.hpp"

// TAM_PAG = quantidade de registros que cabem em uma "pagina" / moldura de pagina da memoria secundaria
// Cada leitura de uma pagina inteira == 1 transferencia entre memoria secundaria e principal.
#define TAM_PAG 100

typedef struct {
    TipoRegistro registros[TAM_PAG];
    int quantidade; // quantos registros desta pagina sao validos (a ultima pode ser parcial)
} PagReg;

// Marcacao de tempo (inicio/fim), em nanossegundos.
typedef struct {
    unsigned long long inicio;
    unsigned long long fim;
} Time;

// Estatisticas de execucao de um metodo: tempo, nº de comparacoes entre
// chaves e nº de transferencias (acessos) entre memoria secundaria e principal.
typedef struct {
    unsigned long long tempo;
    int comparacoes;
    int acessos;
    Time time;
} Estatisticas;

void iniciaEstatisticas(Estatisticas *estatisticas);

// Abre o arquivo binario de registros.
FILE *abrirArquivo(const char *nomeDoArquivo);
bool  fecharArquivo(FILE *arquivo);

// Le a proxima pagina (ate TAM_PAG registros) do arquivo, contando 1 acesso.
// Retorna true se leu ao menos 1 registro.
bool paginacao(FILE *arquivo, PagReg *pagina, Estatisticas *estatisticas);

#endif // PAGINACAO_HPP
