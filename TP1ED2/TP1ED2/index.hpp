#ifndef INDEX_HPP
#define INDEX_HPP

#include "registro.hpp"
#include "paginacao.hpp"

// Método 1: acesso sequencial indexado.
// Lê o arquivo "nomeArquivo", monta o índice de páginas em memória e
// pesquisa pela chave informada, atualizando as estatísticas de
// comparações/transferências/tempo.
void sequencial(const char *nomeArquivo, int qntdItens, int situacao, long chave,
                Estatisticas *estatisticasIndice, Estatisticas *estatisticasBusca);

#endif // INDEX_HPP
