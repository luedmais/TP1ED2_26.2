#ifndef GERADOR_ARQUIVO_HPP
#define GERADOR_ARQUIVO_HPP

#include <string>

class GeradorArquivo {
public:
    // Retorna o caminho do arquivo gerado
    static std::string gerarNomeArquivo(int quantidade, int situacao);

    // Cria o arquivo se ele ainda não existir
    static bool garantirArquivoExiste(int quantidade, int situacao);

    // Geração do arquivo propriamente dito
    static bool gerar(const std::string &nomeArquivo, int quantidade, int situacao);
};

#endif