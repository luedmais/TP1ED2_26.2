#ifndef ARVOREBE_H
#define ARVOREBE_H

#define M 10 //Ordem da pagina interna
#define MM M //Ordem pagina externa
#define INTERNO 0 //Variavel para definir pagina interna (U0)
#define EXTERNO 1 //Variavel para definir pagina externa (U1)

#include <stdbool.h>
#include "registro.h"

typedef struct TipoPagina TipoPagina;    
typedef TipoPagina* TipoApontador;
typedef enum {Interna, Externa} TipoIntExt;

typedef struct TipoPagina {
    TipoIntExt pt;
    union {
        struct { // Pagina interna
            int ni;
            int chave[2 * M];
            TipoApontador pi[2 * M + 1];
        }U0;
        struct { // Pagina externa
            int ne;
            TipoRegistro re[2 * MM];
        }U1;
    }UU;
} TipoPagina;

//Tempo de execucao
typedef struct {
    unsigned long long inicio;
    unsigned long long fim;
}Time;

//Estatisticas de execucao
typedef struct {
    unsigned long long tempo;
    int comparacoes;
    int acessos;
    Time time;
} Estatisticas;

// Pesquisa
bool pesquisaArvoreBE(TipoRegistro, TipoApontador, Estatisticas *, TipoApontador *);

// Insercao
TipoApontador insere(TipoRegistro, TipoApontador , Estatisticas *);
void ins(TipoRegistro, TipoApontador , bool *, TipoRegistro *, TipoApontador *, Estatisticas *);
void inserePaginaInterna(TipoApontador, int, TipoApontador, Estatisticas *);
void inserePaginaExterna(TipoApontador, TipoRegistro, Estatisticas *);

//Impressao
void imprimeArvoreBEstrela(TipoApontador);
void imprimirNo(TipoApontador );

// Destruir
void destruirArvoreBE(TipoApontador);

//Starta estatisticas
void iniciaEstatisticas(Estatisticas *);

#endif // ARVOREBE_H