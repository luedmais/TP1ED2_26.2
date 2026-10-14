#define _POSIX_C_SOURCE 199309L //Funcao para calcular tempo
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "arvoreBE.h"

//---------- PROCESSO DE INCERCAO ----------//
TipoApontador insere(TipoRegistro reg, TipoApontador ap, Estatisticas *estatisticas) {
    //Startando temporizador
    struct timespec t_inicio, t_fim;
    clock_gettime(CLOCK_MONOTONIC, &t_inicio);

    //Primeira chamada, cria raiz
    if (ap == NULL) {
    TipoApontador novaFolha = malloc(sizeof(TipoPagina));
    if (!novaFolha) exit(EXIT_FAILURE);
        novaFolha->pt        = EXTERNO;
        novaFolha->UU.U1.ne  = 1;
        novaFolha->UU.U1.re[0] = reg;
        return novaFolha;
    }
    
    bool cresceu;
    TipoRegistro regRetorno;
    TipoPagina *apRetorno, *apTemp;

    ins(reg, ap, &cresceu, &regRetorno, &apRetorno, estatisticas);
    //crescimento na raiz
    if(cresceu) {
        apTemp = malloc(sizeof(TipoPagina));
        if(apTemp == NULL) {
            printf("Erro ao alocar Raiz");
            exit(EXIT_FAILURE);
        }
        apTemp->pt = INTERNO;
        apTemp->UU.U0.ni = 1;
        apTemp->UU.U0.chave[0] = regRetorno.chave; //Nova chave (Raiz)
        apTemp->UU.U0.pi[0] = ap;       // filho esquerdo = raiz antiga
        apTemp->UU.U0.pi[1] = apRetorno; // filho direito = nova página
        return apTemp; // RETORNA NOVA RAIZ
    }
    clock_gettime(CLOCK_MONOTONIC, &t_fim);
    estatisticas->time.inicio = t_inicio.tv_sec * 1000000000ULL + t_inicio.tv_nsec;
    estatisticas->time.fim    = t_fim.tv_sec    * 1000000000ULL + t_fim.tv_nsec;
    estatisticas->tempo += estatisticas->time.fim - estatisticas->time.inicio;
    return ap; // retorna raiz original
}

// Insere um registro recursivamente em uma árvore B*
void ins(TipoRegistro registro, TipoApontador paginaAtual, bool *cresceu, TipoRegistro *registroRetorno, TipoApontador *paginaRetorno, Estatisticas *estatisticas) {
    unsigned int i;

    // Tratamento de pagina interna
    if (paginaAtual->pt == INTERNO) { i = 0;
        // Caminhamento pela arvore
        while (i < paginaAtual->UU.U0.ni && paginaAtual->UU.U0.chave[i] < registro.chave) i++;
        ins(registro, paginaAtual->UU.U0.pi[i], cresceu, registroRetorno, paginaRetorno, estatisticas);

        if (!*cresceu) {
            return;
        }

        // Pagina comporta novo registro
        if (paginaAtual->UU.U0.ni < 2 * M) {
            inserePaginaInterna(paginaAtual, registroRetorno->chave, *paginaRetorno, estatisticas);
            *cresceu = false;
            return;
        }

       // Criação da nova página
        TipoApontador apTemp = malloc(sizeof(TipoPagina));
        if (apTemp == NULL) {
            printf("Nao foi possivel alocar memoria (pagina Interna)\n"); 
            exit(EXIT_FAILURE);
        }
        apTemp->pt = INTERNO;
        apTemp->UU.U0.ni = 0;

        // Inserção do novo elemento na página correta
        if (i < M + 1) {
            // Inserir última chave antiga na nova página
            inserePaginaInterna(apTemp, paginaAtual->UU.U0.chave[2 * M - 1], paginaAtual->UU.U0.pi[2 * M], estatisticas);
            paginaAtual->UU.U0.ni--; // Decrementa a quantidade de chaves
            // Inserir nova chave na página original
            inserePaginaInterna(paginaAtual, registroRetorno->chave, *paginaRetorno, estatisticas);
        } else {
            inserePaginaInterna(apTemp, registroRetorno->chave, *paginaRetorno, estatisticas);
        }

        // Transferência das chaves maiores (M+2 a 2M)
        for (int j = M + 2; j <= 2 * M; j++) {
            inserePaginaInterna(apTemp,  paginaAtual->UU.U0.chave[j - 1], paginaAtual->UU.U0.pi[j], estatisticas);
        }
        // Reorganiza ponteiros e promove chave
        paginaAtual->UU.U0.ni = M; //Tamnho pagina atual = M
        apTemp->UU.U0.pi[0] = paginaAtual->UU.U0.pi[M + 1]; //Pagina temp recebe ponteiro 
        TipoRegistro regPromovido; //Registro temporario para subir chave
        regPromovido.chave = paginaAtual->UU.U0.chave[M]; //Item do meio promovido
        *registroRetorno = regPromovido; //Promove para proxima chamada da funcao
        *paginaRetorno = apTemp; //Retorna nova pagina
        *cresceu = true; //Arvore cresceu
    }
    // Tratamento da pagina externa (folha)
    else { i = 0;
        // Posicao correta para o item ficar
        while (i < paginaAtual->UU.U1.ne && paginaAtual->UU.U1.re[i].chave < registro.chave) i++;

        // Itens repetidos
        if (i < paginaAtual->UU.U1.ne && registro.chave == paginaAtual->UU.U1.re[i].chave) {
            *cresceu = false;
            return;
        }

        // Pagina atual "Comporta" novo item
        if (paginaAtual->UU.U1.ne < 2 * M) {
            inserePaginaExterna(paginaAtual, registro, estatisticas);
            *cresceu = false;
            return;
        }

        // insere e ordena
        inserePaginaExterna(paginaAtual, registro, estatisticas);
        // promove termo do meio
        *registroRetorno = paginaAtual->UU.U1.re[M];
        // cria nova folha e copia
        TipoApontador apTemp = malloc(sizeof(TipoPagina));
        if (!apTemp) exit(EXIT_FAILURE);
        apTemp->pt = EXTERNO;
        apTemp->UU.U1.ne = 0;
        for (int j = M+1; j <= 2 * M; j++) {
            inserePaginaExterna(apTemp, paginaAtual->UU.U1.re[j], estatisticas);
        }
        // ajusta ne da folha original para 0..M
        paginaAtual->UU.U1.ne = M + 1;
        *cresceu = true;
        *paginaRetorno = apTemp;
    }
}


//insere na pagina externa 
void inserePaginaExterna(TipoApontador ap, TipoRegistro reg, Estatisticas *estatisticas) {
    int k = ap->UU.U1.ne;

    while(k > 0) {
        if(reg.chave >= ap->UU.U1.re[k - 1].chave)
            break;
        ap->UU.U1.re[k] = ap->UU.U1.re[k - 1];
        k--;
    }

    ap->UU.U1.re[k] = reg;
    ap->UU.U1.ne++;
}

//insere pagina interna
void inserePaginaInterna(TipoApontador ap, int chavePromovida, TipoApontador apDir, Estatisticas *estatisticas) {
    int k = ap->UU.U0.ni;

    // Loop para encontrar a posição correta
    while (k > 0 && chavePromovida < ap->UU.U0.chave[k - 1]) {
        ap->UU.U0.chave[k] = ap->UU.U0.chave[k - 1];
        ap->UU.U0.pi[k + 1] = ap->UU.U0.pi[k];
        k--;
    }

    ap->UU.U0.chave[k] = chavePromovida;
    ap->UU.U0.pi[k + 1] = apDir;
    ap->UU.U0.ni++;
}

//---------- IMPRESSAO ----------//
void imprimeArvoreBEstrela(TipoApontador ap) {
    if (ap == NULL || ap == NULL)
        return;

    if (ap->pt == INTERNO) {
        for (int i = 0; i < (ap)->UU.U0.ni; i++) {
            imprimeArvoreBEstrela(ap->UU.U0.pi[i]);
        }
        imprimeArvoreBEstrela(ap->UU.U0.pi[ap->UU.U0.ni]);
    } else {
        for (int i = 0; i < ap->UU.U1.ne; i++) {
            printf("%d ", ap->UU.U1.re[i].chave);
        }
    }
}

void imprimirNo(TipoApontador ap) {
    printf("\nIntervalo: \n|");
    for(int i = 0; i < ap->UU.U1.ne; i++)
        printf(" %d ", ap->UU.U1.re[i].chave);
    printf("|\n");
}

//---------- PESQUISA ----------//
bool pesquisaArvoreBE(TipoRegistro reg, TipoApontador ap, Estatisticas *estatisticas, TipoApontador *paginaAtual) {
    if(ap == NULL) return false; //Se raiz nula, retorna
    
    unsigned int i = 0;
    //Caminhamento paginas internas
    if(ap->pt == INTERNO) {
        while(i < ap->UU.U0.ni && reg.chave >= ap->UU.U0.chave[i]){ 
            i++;
            estatisticas->comparacoes ++;
        }
        return pesquisaArvoreBE(reg, ap->UU.U0.pi[i], estatisticas, paginaAtual);
    }
    //Verificacao chave
    else while(i < ap->UU.U1.ne && ap->UU.U1.re[i].chave < reg.chave){ 
        i++;
        estatisticas->comparacoes ++;
    }
    if(i < ap->UU.U1.ne && reg.chave == ap->UU.U1.re[i].chave) {
        (*paginaAtual) = ap; //folha no qual a chave esta presente
        return true;
    }
    else return false;
}

//---------- DESTRUIR ----------//
void destruirArvoreBE(TipoApontador ap) {
    if (ap == NULL)
        return;

    if (ap->pt == INTERNO) {
        for (int i = 0; i <= ap->UU.U0.ni; i++) {
            destruirArvoreBE(ap->UU.U0.pi[i]);
        }
    }

    free(ap);
}

void iniciaEstatisticas(Estatisticas *estatisticas) {
    estatisticas->acessos = 0;
    estatisticas->comparacoes = 0;
    estatisticas->tempo = 0;
    estatisticas->time.inicio = 0;
    estatisticas->time.fim = 0;
}