#ifndef ARVORE_BINARIA_HPP
#define ARVORE_BINARIA_HPP

#include <cstdio>

// Nó da árvore binária armazenada em disco.
// Os campos 'esquerda' e 'direita' simulam os ponteiros, usando posições do arquivo.
//
// BUG CORRIGIDO: faltava o "typedef" aqui (a versão anterior declarava uma
// variável global chamada NoArvore em vez de um tipo, o que impedia o
// projeto inteiro de compilar).
typedef struct {
    int chave;
    long dado1;
    char dado2[5000];
    int esquerda, direita; // posições dos filhos no arquivo
    int indice;            // posição do próprio nó no arquivo
} NoArvore;

int buscarNaArvore(int chave, FILE *arquivo, int *acessos, int *comparacoes);
void inserirNaArvore(FILE *arquivo, int posAtual, int posNovo, int *acessos, int *comparacoes);

#endif
