#ifndef GERADOR_ARQUIVO_HPP
#define GERADOR_ARQUIVO_HPP

#include <string>

// Monta o nome padrao do arquivo de teste para uma dada quantidade/situacao,
// ex.: "dados_2000_3.bin" (2000 registros, situacao 3 = aleatorio).
std::string gerarNomeArquivo(int quantidade, int situacao);

// Gera o arquivo se ele ainda nao existir no disco. Retorna o caminho do arquivo.
std::string garantirArquivoExiste(int quantidade, int situacao);

#endif
