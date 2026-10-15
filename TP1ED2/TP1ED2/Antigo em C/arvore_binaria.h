#ifndef ARVORE_BINARIA_H
#define ARVORE_BINARIA_H

#include <stdio.h>

/*
 - Nó da árvore binária armazenada em disco.
 - Os campos 'esquerda' e 'direita' simulam os ponteiros, usando posições do arquivo.
 */
typedef struct {
    int chave;
    long dado1;
    char dado2[5000];
    int esquerda, direita; // Posições dos filhos no arquivo
    int indice;            // Posição do nó no arquivo
} NoArvore;

/*
 - Atualiza a ligação entre um novo nó (filho) e o nó pai, conforme a chave.
 - O nó pai é encontrado dinamicamente a partir da raiz.
 */
void ajustarLigacaoPaiFilho(int posFilho, FILE *arquivo, int *acessos, int *comparacoes);

/*
 - Realiza a busca de uma chave na árvore binária armazenada no arquivo.
 - Retorna a chave se encontrada, ou -1 caso contrário.
 */
int buscarNaArvore(int chave, FILE *arquivo, int *acessos, int *comparacoes);

void inserirNaArvore(FILE *arquivo, int posAtual, int posNovo, int *acessos, int *comparacoes);

#endif
