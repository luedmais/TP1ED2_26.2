#include <cstdio>
#include <cstdlib>
#include "arvoreB.hpp"

void Inicializa(TipoApontadorB *Arvore) {
    *Arvore = NULL;
}

int Pesquisa(TipoRegistro *x, TipoApontadorB Ap, Estatisticas *estatisticas) {
    int i = 1;

    if (Ap == NULL) {
        return 0; // nao encontrado
    }

    // Avança sequencialmente ate achar a posicao onde a chave poderia estar.
    while (i < Ap->n && x->chave > Ap->r[i - 1].chave) {
        estatisticas->comparacoes++;
        i++;
    }
    estatisticas->comparacoes++;

    if (x->chave == Ap->r[i - 1].chave) {
        *x = Ap->r[i - 1];
        return 1;
    }

    if (x->chave < Ap->r[i - 1].chave) {
        return Pesquisa(x, Ap->p[i - 1], estatisticas);
    } else {
        return Pesquisa(x, Ap->p[i], estatisticas);
    }
}

void Imprime(TipoApontadorB Arvore) {
    if (Arvore != NULL) {
        int i;
        for (i = 0; i < Arvore->n; i++) {
            Imprime(Arvore->p[i]);
            printf("%d ", Arvore->r[i].chave);
        }
        Imprime(Arvore->p[i]);
    }
}

// Insere um registro em uma pagina ja existente (sem causar overflow), mantendo a ordenacao.
static void InsereNaPagina(TipoApontadorB Ap, TipoRegistro Reg, TipoApontadorB ApDir) {
    int NaoAchouPosicao = (Ap->n > 0);
    int k = Ap->n;

    while (NaoAchouPosicao) {
        if (Reg.chave >= Ap->r[k - 1].chave) {
            NaoAchouPosicao = 0;
        } else {
            Ap->r[k] = Ap->r[k - 1];
            Ap->p[k + 1] = Ap->p[k];
            k--;
            if (k < 1) NaoAchouPosicao = 0;
        }
    }

    Ap->r[k] = Reg;
    Ap->p[k + 1] = ApDir;
    Ap->n++;
}

// Funcao recursiva que trata a insercao e as divisoes de pagina.
static void Ins(TipoRegistro Reg, TipoApontadorB Ap, bool *Cresceu,
                TipoRegistro *RegRetorno, TipoApontadorB *ApRetorno,
                Estatisticas *estatisticas) {
    int i = 1, j;
    TipoApontadorB ApTemp;

    if (Ap == NULL) { // caso base: posicao de folha vazia encontrada
        *Cresceu = true;
        *RegRetorno = Reg;
        *ApRetorno = NULL;
        return;
    }

    while (i < Ap->n && Reg.chave > Ap->r[i - 1].chave) {
        estatisticas->comparacoes++;
        i++;
    }
    estatisticas->comparacoes++;

    if (Reg.chave == Ap->r[i - 1].chave) { // chave duplicada: ignora
        *Cresceu = false;
        return;
    }

    if (Reg.chave < Ap->r[i - 1].chave) i--;

    Ins(Reg, Ap->p[i], Cresceu, RegRetorno, ApRetorno, estatisticas);

    if (!*Cresceu) return;

    if (Ap->n < MM_B) { // pagina comporta o novo item
        InsereNaPagina(Ap, *RegRetorno, *ApRetorno);
        *Cresceu = false;
        return;
    }

    // Pagina cheia: divide em duas.
    ApTemp = (TipoApontadorB) malloc(sizeof(TipoPaginaB));
    ApTemp->n = 0;
    ApTemp->p[0] = NULL;

    if (i < M_B + 1) {
        InsereNaPagina(ApTemp, Ap->r[MM_B - 1], Ap->p[MM_B]);
        Ap->n--;
        InsereNaPagina(Ap, *RegRetorno, *ApRetorno);
    } else {
        InsereNaPagina(ApTemp, *RegRetorno, *ApRetorno);
    }

    for (j = M_B + 2; j <= MM_B; j++) {
        InsereNaPagina(ApTemp, Ap->r[j - 1], Ap->p[j]);
    }

    Ap->n = M_B;
    ApTemp->p[0] = Ap->p[M_B + 1];
    *RegRetorno = Ap->r[M_B];
    *ApRetorno = ApTemp;
}

void Insere(TipoRegistro Reg, TipoApontadorB *Ap, Estatisticas *estatisticas) {
    bool Cresceu;
    TipoRegistro RegRetorno;
    TipoApontadorB ApRetorno, ApTemp;

    Ins(Reg, *Ap, &Cresceu, &RegRetorno, &ApRetorno, estatisticas);

    if (Cresceu) { // a divisao propagou ate a raiz: cria nova raiz
        ApTemp = (TipoApontadorB) malloc(sizeof(TipoPaginaB));
        ApTemp->n = 1;
        ApTemp->r[0] = RegRetorno;
        ApTemp->p[1] = ApRetorno;
        ApTemp->p[0] = *Ap;
        *Ap = ApTemp;
    }
}

void Limpar(TipoApontadorB *Ap) {
    if (*Ap != NULL) {
        for (int i = 0; i <= (*Ap)->n; i++) {
            Limpar(&(*Ap)->p[i]);
        }
        free(*Ap);
        *Ap = NULL;
    }
}
