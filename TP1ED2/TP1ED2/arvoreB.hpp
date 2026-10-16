#ifndef ARVOREB_HPP
#define ARVOREB_HPP

#include "registro.hpp"
#include "paginacao.hpp"

#define M_B  2       // grau minimo (nº minimo de chaves por pagina, exceto raiz)
#define MM_B (2 * M_B) // ordem da arvore B (maximo de chaves por pagina)

struct TipoPaginaB;
typedef TipoPaginaB *TipoApontadorB;

struct TipoPaginaB {
    short n;
    TipoRegistro r[MM_B];
    TipoApontadorB p[MM_B + 1];
};

void Inicializa(TipoApontadorB *Arvore);

// Busca o registro de chave x->chave na arvore. Se encontrado, preenche *x
// com o registro completo e retorna 1; caso contrario retorna 0.
int Pesquisa(TipoRegistro *x, TipoApontadorB Ap, Estatisticas *estatisticas);

// Insere um registro na arvore B (cuida de divisao de pagina e crescimento da raiz).
void Insere(TipoRegistro Reg, TipoApontadorB *Ap, Estatisticas *estatisticas);

// Libera toda a memaria alocada pela arvore.
void Limpar(TipoApontadorB *Ap);

#endif // ARVOREB_HPP
