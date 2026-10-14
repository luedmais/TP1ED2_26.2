#ifndef SEQUENCIAL_INDEXADO_HPP
#define SEQUENCIAL_INDEXADO_HPP

#include <string>
#include <vector>
#include "registro.hpp"

struct IndicePagina {
    int chave;
    long posicao;
};

class SequencialIndexado {
private:
    // Métricas de Pré-processamento
    long nTransfPre = 0;
    long nCompPre = 0;
    double tempoPre = 0.0;

    // Métricas de Pesquisa
    long nTransfPesq = 0;
    long nCompPesq = 0;
    double tempoPesq = 0.0;

public:
    static constexpr int ITENSPAGINA = 100;

    // Executa a busca recebendo apenas os parâmetros operacionais essenciais
    bool pesquisar(const std::string& nomeArquivo, 
                   int quantidade, 
                   int situacao, 
                   int chavePesquisada, 
                   TipoRegistro& registroRetorno);

    // Getters para consultar as métricas após a execução
    long getTransfPre() const { return nTransfPre; }
    long getCompPre() const { return nCompPre; }
    double getTempoPre() const { return tempoPre; }

    long getTransfPesq() const { return nTransfPesq; }
    long getCompPesq() const { return nCompPesq; }
    double getTempoPesq() const { return tempoPesq; }
};

#endif // SEQUENCIAL_INDEXADO_HPP