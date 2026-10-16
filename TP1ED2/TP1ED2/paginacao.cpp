#define _POSIX_C_SOURCE 199309L
#include <cstdio>
#include <cstdlib>
#include <chrono>
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

// Le ate TAM_PAG registros em uma unica operacao de leitura (1 pagina = 1
// transferencia/acesso), mesmo que a ultima pagina do arquivo esteja incompleta.
bool paginacao(FILE *arq, PagReg *pagina, Estatisticas *estatisticas) {
    const auto t_inicio = std::chrono::steady_clock::now();

    size_t lidos = fread(pagina->registros, sizeof(TipoRegistro), TAM_PAG, arq);
    pagina->quantidade = (int) lidos;

    if (lidos > 0) {
        estatisticas->acessos++;
    }

    const auto t_fim = std::chrono::steady_clock::now();
    estatisticas->time.inicio = std::chrono::duration_cast<std::chrono::nanoseconds>(
        t_inicio.time_since_epoch()).count();
    estatisticas->time.fim = std::chrono::duration_cast<std::chrono::nanoseconds>(
        t_fim.time_since_epoch()).count();
    estatisticas->tempo += estatisticas->time.fim - estatisticas->time.inicio;

    return lidos > 0;
}
