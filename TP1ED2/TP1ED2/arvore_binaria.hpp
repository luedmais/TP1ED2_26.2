#ifndef ARVORE_BINARIA_HPP
#define ARVORE_BINARIA_HPP

#include <cstdio>

// No da arvore binaria armazenada em disco.
// Os campos 'esquerda' e 'direita' simulam os ponteiros, usando posicoes do arquivo.

typedef struct {
    int chave;
    long dado1;
    char dado2[5000];
    int esquerda, direita; // posicoes dos filhos no arquivo
    int indice;            // posicao do proprio no no arquivo
} NoArvore;

int buscarNaArvore(int chave, FILE *arquivo, int *acessos, int *comparacoes);
void inserirNaArvore(FILE *arquivo, int posAtual, int posNovo, int *acessos, int *comparacoes);

#endif
