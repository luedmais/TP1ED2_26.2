#ifndef GERADOR_ARQUIVO_HPP
#define GERADOR_ARQUIVO_HPP

#include <string>

// Monta o nome padrão do arquivo de teste para uma dada quantidade/situação,
// ex.: "dados_2000_3.bin" (2000 registros, situação 3 = aleatório).
std::string gerarNomeArquivo(int quantidade, int situacao);

// Gera o arquivo se ele ainda não existir no disco. Retorna o caminho do arquivo.
std::string garantirArquivoExiste(int quantidade, int situacao);

#endif
