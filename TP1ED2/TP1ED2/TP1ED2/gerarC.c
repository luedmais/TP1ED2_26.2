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

    tipoRegistro registros[100000];

    // Preenche em ordem crescente: 0, 1, 2, ..., 9999
    for (int i = 0; i < 100000; i++) {
        registros[i].chave = i;
    }

    // Escreve os 10000 registros no arquivo binário
    size_t escritos = fwrite(registros, sizeof(tipoRegistro), 100000, arquivo);
    if (escritos != 100000) {
        perror("Erro na escrita do arquivo");
        fclose(arquivo);
        return 1;
    }

    fclose(arquivo);
    printf("Arquivo criado com 10000 registros em ordem crescente.\n");
    return 0;
}
