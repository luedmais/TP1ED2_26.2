#include <cstdio>
#include <cstdlib>
#include <chrono>
#include "arvoreBE.hpp"

static unsigned long long agoraNs() {
    return static_cast<unsigned long long>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
}

// ---------- INSERCAO ----------

TipoApontadorBE insere(TipoRegistro reg, TipoApontadorBE ap, Estatisticas *estatisticas) {
    unsigned long long t_inicio = agoraNs();
    TipoApontadorBE raizFinal;

    if (ap == NULL) { // primeira insercao: cria a raiz como folha
        TipoApontadorBE novaFolha = (TipoApontadorBE) malloc(sizeof(TipoPaginaBE));
        if (!novaFolha) exit(EXIT_FAILURE);
        novaFolha->pt = Externa;
        novaFolha->UU.U1.ne = 1;
        novaFolha->UU.U1.re[0] = reg;
        raizFinal = novaFolha;
    } else {
        bool cresceu;
        TipoRegistro regRetorno;
        TipoApontadorBE apRetorno, apTemp;

        ins(reg, ap, &cresceu, &regRetorno, &apRetorno, estatisticas);

        if (cresceu) { // divisao propagou ate a raiz: cria nova raiz
            apTemp = (TipoApontadorBE) malloc(sizeof(TipoPaginaBE));
            if (!apTemp) exit(EXIT_FAILURE);
            apTemp->pt = Interna;
            apTemp->UU.U0.ni = 1;
            apTemp->UU.U0.chave[0] = regRetorno.chave;
            apTemp->UU.U0.pi[0] = ap;        // filho esquerdo = raiz antiga
            apTemp->UU.U0.pi[1] = apRetorno; // filho direito = nova pagina
            raizFinal = apTemp;
        } else {
            raizFinal = ap;
        }
    }

    unsigned long long t_fim = agoraNs();
    estatisticas->time.inicio = t_inicio;
    estatisticas->time.fim = t_fim;
    estatisticas->tempo += t_fim - t_inicio;
    return raizFinal;
}

// Insere um registro recursivamente em uma arvore B*.
void ins(TipoRegistro registro, TipoApontadorBE paginaAtual, bool *cresceu,
         TipoRegistro *registroRetorno, TipoApontadorBE *paginaRetorno,
         Estatisticas *estatisticas) {
    int i;

    if (paginaAtual->pt == Interna) {
        i = 0;
        while (i < paginaAtual->UU.U0.ni && paginaAtual->UU.U0.chave[i] < registro.chave) i++;
        ins(registro, paginaAtual->UU.U0.pi[i], cresceu, registroRetorno, paginaRetorno, estatisticas);

        if (!*cresceu) return;

        if (paginaAtual->UU.U0.ni < 2 * M_BE) {
            inserePaginaInterna(paginaAtual, registroRetorno->chave, *paginaRetorno, estatisticas);
            *cresceu = false;
            return;
        }

        // Pagina de indice cheia: divide em duas.
        TipoApontadorBE apTemp = (TipoApontadorBE) malloc(sizeof(TipoPaginaBE));
        if (!apTemp) exit(EXIT_FAILURE);
        apTemp->pt = Interna;
        apTemp->UU.U0.ni = 0;

        if (i < M_BE + 1) {
            inserePaginaInterna(apTemp, paginaAtual->UU.U0.chave[2 * M_BE - 1], paginaAtual->UU.U0.pi[2 * M_BE], estatisticas);
            paginaAtual->UU.U0.ni--;
            inserePaginaInterna(paginaAtual, registroRetorno->chave, *paginaRetorno, estatisticas);
        } else {
            inserePaginaInterna(apTemp, registroRetorno->chave, *paginaRetorno, estatisticas);
        }

        for (int j = M_BE + 2; j <= 2 * M_BE; j++) {
            inserePaginaInterna(apTemp, paginaAtual->UU.U0.chave[j - 1], paginaAtual->UU.U0.pi[j], estatisticas);
        }

        paginaAtual->UU.U0.ni = M_BE;
        apTemp->UU.U0.pi[0] = paginaAtual->UU.U0.pi[M_BE + 1];
        TipoRegistro regPromovido;
        regPromovido.chave = paginaAtual->UU.U0.chave[M_BE];
        *registroRetorno = regPromovido;
        *paginaRetorno = apTemp;
        *cresceu = true;
    } else { // pagina externa (folha)
        i = 0;
        while (i < paginaAtual->UU.U1.ne && paginaAtual->UU.U1.re[i].chave < registro.chave) i++;

        if (i < paginaAtual->UU.U1.ne && registro.chave == paginaAtual->UU.U1.re[i].chave) {
            *cresceu = false; // chave duplicada: ignora
            return;
        }

        if (paginaAtual->UU.U1.ne < 2 * M_BE) {
            inserePaginaExterna(paginaAtual, registro, estatisticas);
            *cresceu = false;
            return;
        }

        // Folha cheia: insere (fica temporariamente com 2M+1 itens) e divide.
        inserePaginaExterna(paginaAtual, registro, estatisticas);

        TipoApontadorBE apTemp = (TipoApontadorBE) malloc(sizeof(TipoPaginaBE));
        if (!apTemp) exit(EXIT_FAILURE);
        apTemp->pt = Externa;
        apTemp->UU.U1.ne = 0;
        // Itens de M_BE..2*M_BE (M_BE+1 itens) vao para a folha da direita.
        for (int j = M_BE; j <= 2 * M_BE; j++) {
            inserePaginaExterna(apTemp, paginaAtual->UU.U1.re[j], estatisticas);
        }
        paginaAtual->UU.U1.ne = M_BE; // folha esquerda fica so com 0..M_BE-1
        *registroRetorno = apTemp->UU.U1.re[0]; // chave copiada para o indice
        *cresceu = true;
        *paginaRetorno = apTemp;
    }
}

void inserePaginaExterna(TipoApontadorBE ap, TipoRegistro reg, Estatisticas *estatisticas) {
    (void) estatisticas; // reservado: nenhuma comparacao de pesquisa extra aqui
    int k = ap->UU.U1.ne;
    while (k > 0 && reg.chave < ap->UU.U1.re[k - 1].chave) {
        ap->UU.U1.re[k] = ap->UU.U1.re[k - 1];
        k--;
    }
    ap->UU.U1.re[k] = reg;
    ap->UU.U1.ne++;
}

void inserePaginaInterna(TipoApontadorBE ap, int chavePromovida, TipoApontadorBE apDir, Estatisticas *estatisticas) {
    (void) estatisticas;
    int k = ap->UU.U0.ni;
    while (k > 0 && chavePromovida < ap->UU.U0.chave[k - 1]) {
        ap->UU.U0.chave[k] = ap->UU.U0.chave[k - 1];
        ap->UU.U0.pi[k + 1] = ap->UU.U0.pi[k];
        k--;
    }
    ap->UU.U0.chave[k] = chavePromovida;
    ap->UU.U0.pi[k + 1] = apDir;
    ap->UU.U0.ni++;
}

// ---------- IMPRESSAO (debug) ----------

void imprimeArvoreBEstrela(TipoApontadorBE ap) {
    if (ap == NULL) return;

    if (ap->pt == Interna) {
        for (int i = 0; i < ap->UU.U0.ni; i++) {
            imprimeArvoreBEstrela(ap->UU.U0.pi[i]);
        }
        imprimeArvoreBEstrela(ap->UU.U0.pi[ap->UU.U0.ni]);
    } else {
        for (int i = 0; i < ap->UU.U1.ne; i++) {
            printf("%d ", ap->UU.U1.re[i].chave);
        }
    }
}

void imprimirNo(TipoApontadorBE ap) {
    printf("\nIntervalo: \n|");
    for (int i = 0; i < ap->UU.U1.ne; i++)
        printf(" %d ", ap->UU.U1.re[i].chave);
    printf("|\n");
}

// ---------- PESQUISA ----------

bool pesquisaArvoreBE(TipoRegistro reg, TipoApontadorBE ap, Estatisticas *estatisticas, TipoApontadorBE *paginaAchada) {
    if (ap == NULL) return false;

    unsigned int i = 0;
    if (ap->pt == Interna) {
        while (i < (unsigned int) ap->UU.U0.ni && reg.chave >= ap->UU.U0.chave[i]) {
            i++;
            estatisticas->comparacoes++;
        }
        return pesquisaArvoreBE(reg, ap->UU.U0.pi[i], estatisticas, paginaAchada);
    } else {
        while (i < (unsigned int) ap->UU.U1.ne && ap->UU.U1.re[i].chave < reg.chave) {
            i++;
            estatisticas->comparacoes++;
        }
        if (i < (unsigned int) ap->UU.U1.ne && reg.chave == ap->UU.U1.re[i].chave) {
            *paginaAchada = ap;
            return true;
        }
        return false;
    }
}

// ---------- DESTRUICAO ----------

void destruirArvoreBE(TipoApontadorBE ap) {
    if (ap == NULL) return;
    if (ap->pt == Interna) {
        for (int i = 0; i <= ap->UU.U0.ni; i++) {
            destruirArvoreBE(ap->UU.U0.pi[i]);
        }
    }
    free(ap);
}
