#ifndef ARVOREB_H
#define ARVOREB_H

#include "registro.h"

#define M 2  // Grau mínimo
#define MM (2*M) // Ordem da árvore (máximo de chaves)

typedef struct TipoPagina* TipoApontador;

// === Estrutura de uma página (nó) da árvore B ===
typedef struct TipoPagina {
    short n;
    TipoRegistro r[MM];
    TipoApontador p[MM + 1];
} TipoPagina;
// Inicializa a árvore como vazia
void Inicializa(TipoApontador *Arvore);
// Realiza a busca de um registro pela chave
int Pesquisa(TipoRegistro *x, TipoApontador Ap, int *comparacoes);

void Imprime(TipoApontador Arvore);
// Insere um registro em uma página (mantém ordenação)
void InsereNaPagina(TipoApontador Ap, TipoRegistro Reg, TipoApontador ApDir, int *comparacoes);
// Função recursiva principal de inserção
void Ins(TipoRegistro Reg, TipoApontador Ap, short *Cresceu,
         TipoRegistro *RegRetorno, TipoApontador *ApRetorno, int *comparacoes);
// Wrapper da inserção (chamada externa)
void Insere(TipoRegistro Reg, TipoApontador *Ap, int *comparacoes);
// Libera memória da árvore
void Limpar(TipoApontador *Ap);

#endif
