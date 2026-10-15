#ifndef ARVOREB_HPP
#define ARVOREB_HPP

#include "registro.hpp"
#include "paginacao.hpp"

// A árvore B funciona como ÍNDICE em memória interna (conforme permitido
// pelo enunciado: "memória interna disponível para armazenar os índices
// necessários"). Os registros são lidos do arquivo externo em páginas
// (ver paginacao.hpp) durante a criação do índice -- isso é o que conta
// como "transferências" nessa fase. A busca em si ocorre toda em memória,
// por isso não há transferências adicionais durante a pesquisa, só
// comparações.

#define M_B  2       // grau mínimo (nº mínimo de chaves por página, exceto raiz)
#define MM_B (2 * M_B) // ordem da árvore B (máximo de chaves por página)

struct TipoPaginaB;
typedef TipoPaginaB *TipoApontadorB;

struct TipoPaginaB {
    short n;
    TipoRegistro r[MM_B];
    TipoApontadorB p[MM_B + 1];
};

void Inicializa(TipoApontadorB *Arvore);

// Busca o registro de chave x->chave na árvore. Se encontrado, preenche *x
// com o registro completo e retorna 1; caso contrário retorna 0.
int Pesquisa(TipoRegistro *x, TipoApontadorB Ap, Estatisticas *estatisticas);

// Insere um registro na árvore B (cuida de divisão de página e crescimento da raiz).
void Insere(TipoRegistro Reg, TipoApontadorB *Ap, Estatisticas *estatisticas);

// Libera toda a memória alocada pela árvore.
void Limpar(TipoApontadorB *Ap);

// Impressão em ordem (debug).
void Imprime(TipoApontadorB Arvore);

#endif // ARVOREB_HPP
