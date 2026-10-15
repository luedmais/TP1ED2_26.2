#define _POSIX_C_SOURCE 199309L
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "paginacao.hpp"

void iniciaEstatisticas(Estatisticas *estatisticas) {
    estatisticas->acessos = 0;
    estatisticas->comparacoes = 0;
    estatisticas->tempo = 0;
    estatisticas->time.inicio = 0;
    estatisticas->time.fim = 0;
}

FILE *abrirArquivo(const char *nomeDoArquivo) {
    FILE *file = fopen(nomeDoArquivo, "rb");
    if (file == NULL) {
        fprintf(stderr, "Erro: nao foi possivel abrir o arquivo %s\n", nomeDoArquivo);
        exit(EXIT_FAILURE);
    }
    return file;
}

bool fecharArquivo(FILE *arq) {
    if (arq != NULL) {
        fclose(arq);
        return true;
    }
    return false;
}

// Lê até TAM_PAG registros em uma única operação de leitura (1 página = 1
// transferência/acesso), mesmo que a última página do arquivo esteja incompleta.
bool paginacao(FILE *arq, PagReg *pagina, Estatisticas *estatisticas) {
    struct timespec t_inicio, t_fim;
    clock_gettime(CLOCK_MONOTONIC, &t_inicio);

    size_t lidos = fread(pagina->registros, sizeof(TipoRegistro), TAM_PAG, arq);
    pagina->quantidade = (int) lidos;

    if (lidos > 0) {
        estatisticas->acessos++;
    }

    clock_gettime(CLOCK_MONOTONIC, &t_fim);
    estatisticas->time.inicio = t_inicio.tv_sec * 1000000000ULL + t_inicio.tv_nsec;
    estatisticas->time.fim    = t_fim.tv_sec    * 1000000000ULL + t_fim.tv_nsec;
    estatisticas->tempo += estatisticas->time.fim - estatisticas->time.inicio;

    return lidos > 0;
}
