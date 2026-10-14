#define _POSIX_C_SOURCE 199309L //Funcao para calcular tempo
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "arvoreBE.h"
#include "paginacao.h"

void testeInsercaoArvoreBE(int, char *);

int main(int argc, char *argv[]) {
    testeInsercaoArvoreBE(argc, argv[1]);
    return 0;
}

void testeInsercaoArvoreBE(int argc, char *argv) {
    // Inicializando estatisticas arvore
    Estatisticas estatisticas;
    iniciaEstatisticas(&estatisticas);

    //Inicializando estatisticas arquivo
    Estatisticas estatisticasArq;
    iniciaEstatisticas(&estatisticasArq);

    // Abertura do arquivo
    FILE *arq = abrirArquivo(argc, argv);

    // Inicializacao arvore
    TipoApontador ap = NULL;

    // Iniciando paginacao e lendo todas as paginas
    PagReg regPag;
    TipoRegistro reg;
    //Looping para passar por todas as paginas
    while(paginacao(arq, &regPag, &estatisticasArq)) {
        for (int i = 0; i < TAM_PAG; i++) {
            reg = regPag.registros[i];
            ap = insere(reg, ap, &estatisticas);
        }
    }
    
    //Pesquisa de 10 numeros aleatorios
    //Geracao de numeros 0 - 999
    int pesquisa[10];
    gerarAleatorios(pesquisa);
    //Tipo Pagina auxiliar para impressao
    TipoApontador pagAux;
    printf("Chaves Procuradas: ");
    for(int i = 0; i < 1; i++)
        printf(" %d ", pesquisa[i]);

    //Calculando tempo pesquisa
    struct timespec t_inicio, t_fim;
    clock_gettime(CLOCK_MONOTONIC, &t_inicio);
    for(int i = 0; i < 1; i++){
        reg.chave = pesquisa[i];
        if(pesquisaArvoreBE(reg, ap, &estatisticas, &pagAux))
        //Impressao do intervalo
            imprimirNo(pagAux);
        else printf("\nChave nao encontrada: chave - %d\n", pesquisa[i]);
    }   
    clock_gettime(CLOCK_MONOTONIC, &t_fim);
    unsigned long long inicio = t_inicio.tv_sec * 1000000000ULL + t_inicio.tv_nsec;
    unsigned long long fim = t_fim.tv_sec    * 1000000000ULL + t_fim.tv_nsec;
    unsigned long long total = fim - inicio;
    // fecha arquivo
    fecharArquivo(arq);

    //convertendo nano em segundos (arvore)
    double segundos = estatisticas.tempo / 1e9;
    double segundosPesquisa = total / 1e9;
    //Print das estatisticas arvore
    printf("----------------------------------------------------\n");
    printf("Dados arvore\nTempo de execucao(insercao): %f(segundos)\nDados arvore\nTempo de Execucao(pesquisa): %f(segundos)\nComparacoes: %d\nAcessos: %d\n", segundos, segundosPesquisa, estatisticas.comparacoes, 0);

    //convertendo nano em segundos (arvore)
    double segundosArq = estatisticasArq.tempo / 1e9;
    //Print das estatisticas arquivo
    printf("----------------------------------------------------\n");
    printf("Dados arquivo\nTempo de execucao(paginacao): %f(segundos)\nComparacoes: %d\nAcessos: %d\n", segundosArq, 0, estatisticasArq.acessos);
    printf("----------------------------------------------------\n");
    // destrói arvore
    
    destruirArvoreBE(ap);
}