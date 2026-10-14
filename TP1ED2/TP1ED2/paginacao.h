#ifndef PAGINACAO_H
#define PAGINACAO_H
#include "arvoreBE.h"

#define TAM_PAG 100

#include "registro.h"
#include <stdbool.h>

typedef struct {
    TipoRegistro registros[TAM_PAG];
}PagReg;

//Funcoes
bool paginacao(FILE *, PagReg *, Estatisticas *);
bool fecharArquivo(FILE *);
FILE *abrirArquivo(int, const char *);
void gerarAleatorios(int *);
#endif //PAGINACAO_H
