#ifndef INDEX_HPP
#define INDEX_HPP

#include "registro.hpp"
#include "paginacao.hpp"

// Metodo 1: acesso sequencial indexado.
// Le o arquivo "nomeArquivo", monta o indice de páginas em memoria e
// pesquisa pela chave informada, atualizando as estatisticas de
// comparacoes/transferencias/tempo.
void sequencial(const char *nomeArquivo, int qntdItens, int situacao, long chave,
                Estatisticas *estatisticasIndice, Estatisticas *estatisticasBusca);

#endif // INDEX_HPP
