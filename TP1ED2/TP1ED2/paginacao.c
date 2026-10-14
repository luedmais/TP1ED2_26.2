#define _POSIX_C_SOURCE 199309L //Funcao para calcular tempo
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "paginacao.h"

FILE *abrirArquivo(int argv, const char *nomeDoArquivo) {
    if(argv != 2) {
        perror("Quantidade de argumentos invalida!\n");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(nomeDoArquivo, "rb");
    if(file == NULL)
        exit(EXIT_FAILURE);
    return file;
}

bool fecharArquivo(FILE *arq) {
    if(arq != NULL) {
        fclose(arq);
        return true;
    }
    return false;
}

bool paginacao(FILE *arq, PagReg *reg, Estatisticas *estatisticas) {
    struct timespec t_inicio, t_fim;
    clock_gettime(CLOCK_MONOTONIC, &t_inicio);

    estatisticas->acessos ++;

    size_t lidos = fread(reg, sizeof(PagReg), 1, arq);

    clock_gettime(CLOCK_MONOTONIC, &t_fim);
    estatisticas->time.inicio = t_inicio.tv_sec * 1000000000ULL + t_inicio.tv_nsec;
    estatisticas->time.fim    = t_fim.tv_sec    * 1000000000ULL + t_fim.tv_nsec;
    estatisticas->tempo += estatisticas->time.fim - estatisticas->time.inicio;
    
    return (lidos == 1);
}

void gerarAleatorios(int *vetor) {
    srand((unsigned)time(NULL));
    for(int i = 0; i < 10; i++)
        vetor[i] = rand() % 1000000; // valores entre 0 e 999
}