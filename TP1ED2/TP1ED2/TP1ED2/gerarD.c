#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int chave;
} tipoRegistro;

int main() {
    FILE *arquivo = fopen("registros.bin", "wb");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    // 1 milhão de registros
    tipoRegistro registros[1000000];

    // Preenche em ordem decrescente: 999999, 999998, ..., 0
    for (int i = 0; i < 1000000; i++) {
        registros[i].chave = 1000000 - 1 - i;
    }

    // Grava exatamente 1.000.000 registros
    size_t escritos = fwrite(registros, sizeof(tipoRegistro), 1000000, arquivo);
    if (escritos != 1000000) {
        perror("Erro na escrita do arquivo");
        fclose(arquivo);
        return 1;
    }

    fclose(arquivo);
    printf("Arquivo criado com 1000000 registros em ordem decrescente.\n");
    return 0;
}
