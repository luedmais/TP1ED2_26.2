#include "arvore_binaria.hpp"

static NoArvore lerNo(FILE *arquivo, int pos, int *acessos) {
    NoArvore no;
    fseek(arquivo, pos * sizeof(NoArvore), SEEK_SET);
    fread(&no, sizeof(NoArvore), 1, arquivo);
    (*acessos)++;
    return no;
}

static void escreverNo(FILE *arquivo, int pos, NoArvore *no, int *acessos) {
    fseek(arquivo, pos * sizeof(NoArvore), SEEK_SET);
    fwrite(no, sizeof(NoArvore), 1, arquivo);
    (*acessos)++;
}

void inserirNaArvore(FILE *arquivo, int posRaiz, int posNovo, int *acessos, int *comparacoes) {
    NoArvore novo = lerNo(arquivo, posNovo, acessos);
    int posAtual = posRaiz;

    while (1) {
        NoArvore atual = lerNo(arquivo, posAtual, acessos);
        (*comparacoes)++;

        if (novo.chave < atual.chave) {
            if (atual.esquerda == -1) {
                atual.esquerda = posNovo;
                escreverNo(arquivo, posAtual, &atual, acessos);
                break;
            } else {
                posAtual = atual.esquerda;
            }
        } else {
            if (atual.direita == -1) {
                atual.direita = posNovo;
                escreverNo(arquivo, posAtual, &atual, acessos);
                break;
            } else {
                posAtual = atual.direita;
            }
        }
    }
}

int buscarNaArvore(int chave, FILE *arquivo, int *acessos, int *comparacoes) {
    int pos = 0;
    NoArvore atual;

    while (1) {
        fseek(arquivo, pos * sizeof(NoArvore), SEEK_SET);
        fread(&atual, sizeof(NoArvore), 1, arquivo);
        (*acessos)++;
        (*comparacoes)++;

        if (chave == atual.chave)
            return chave;

        if (chave < atual.chave) {
            if (atual.esquerda == -1) return -1;
            pos = atual.esquerda;
        } else {
            if (atual.direita == -1) return -1;
            pos = atual.direita;
        }
    }
}
