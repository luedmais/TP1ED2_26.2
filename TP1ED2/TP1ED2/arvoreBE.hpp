#ifndef ARVOREBE_HPP
#define ARVOREBE_HPP

#include "registro.hpp"
#include "paginacao.hpp"

#define M_BE  10     // ordem da pagina interna (índice)
#define MM_BE M_BE       // ordem da pagina externa (folha)

enum TipoIntExt { Interna, Externa };

struct TipoPaginaBE;
typedef TipoPaginaBE *TipoApontadorBE;

struct TipoPaginaBE {
    TipoIntExt pt;
    union {
        struct { // pagina interna (indice): so guarda chaves + ponteiros
            int ni;
            int chave[2 * M_BE];
            TipoApontadorBE pi[2 * M_BE + 1];
        } U0;
        struct { // pagina externa (folha): guarda os registros completos
            int ne;
            TipoRegistro re[2 * MM_BE + 1];
        } U1;
    } UU;
};

// Pesquisa: sempre desce ate uma pagina folha. Se achar a chave, devolve
// (por meio de paginaAchada) a folha onde ela esta.
bool pesquisaArvoreBE(TipoRegistro reg, TipoApontadorBE ap, Estatisticas *estatisticas,
                      TipoApontadorBE *paginaAchada);

// Insere um registro, cuidando de divisao de pagina e crescimento da raiz.
// Uso: ap = insere(reg, ap, &estatisticas);
TipoApontadorBE insere(TipoRegistro reg, TipoApontadorBE ap, Estatisticas *estatisticas);
void ins(TipoRegistro registro, TipoApontadorBE paginaAtual, bool *cresceu,
         TipoRegistro *registroRetorno, TipoApontadorBE *paginaRetorno,
         Estatisticas *estatisticas);
void inserePaginaInterna(TipoApontadorBE ap, int chavePromovida, TipoApontadorBE apDir,
                          Estatisticas *estatisticas);
void inserePaginaExterna(TipoApontadorBE ap, TipoRegistro reg, Estatisticas *estatisticas);

void destruirArvoreBE(TipoApontadorBE ap);

#endif // ARVOREBE_HPP
