#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TOTAL 1000000

typedef struct {
    int chave;
    long dado1;
    char dado2[1000];
    char dado3[5000];
} Registro;

void shuffle(int *array, int n) {
    if (n > 1) {
        for (int i = 0; i < n - 1; i++) {
            int j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

void gerar_out_bin(int tipo, int quantidade) {
    FILE *out = fopen("out.bin", "wb");
    if (!out) {
        printf("Erro ao criar out.bin.\n");
        return;
    }

    Registro *regs = malloc(quantidade * sizeof(Registro));
    if (!regs) {
        printf("Erro de memória.\n");
        fclose(out);
        return;
    }

    // Preenche os registros conforme o tipo
    for (int i = 0; i < quantidade; i++) {
        if (tipo == 1) // crescente
            regs[i].chave = i + 1;
        else if (tipo == 2) // decrescente
            regs[i].chave = quantidade - i;
        else // aleatório
            regs[i].chave = i + 1;

        regs[i].dado1 = rand();
        strcpy(regs[i].dado2, "Dado2");
        strcpy(regs[i].dado3, "Dado3");
    }

    // Embaralha se for aleatório
    if (tipo == 3) {
        for (int i = quantidade - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            Registro temp = regs[i];
            regs[i] = regs[j];
            regs[j] = temp;
        }
    }

    // Escreve no arquivo
    fwrite(regs, sizeof(Registro), quantidade, out);

    free(regs);
    fclose(out);
    printf("Arquivo out.bin criado com %d registros!\n", quantidade);
}

void criar_txt_se_bin_existir() {
    const char *origem = "out.bin";

    FILE *bin = fopen(origem, "rb");
    if (!bin) {
        printf("Arquivo %s não existe. Gere o arquivo binário primeiro.\n", origem);
        return;
    }

    FILE *txt = fopen("dados.txt", "w");
    if (!txt) {
        printf("Erro ao criar o arquivo txt.\n");
        fclose(bin);
        return;
    }

    Registro reg;
    int count = 0;
    while (fread(&reg, sizeof(Registro), 1, bin) == 1) {
        fprintf(txt, "%d,%ld,%s,%s\n", reg.chave, reg.dado1, reg.dado2, reg.dado3);
        count++;
    }

    fclose(bin);
    fclose(txt);

    printf("Arquivo dados.txt criado com %d registros a partir de %s!\n", count, origem);
}

int main() {
    int tipo, quantidade;
    printf("Escolha o tipo de ordenação para gerar o out.bin:\n");
    printf("1 - Ordem crescente\n");
    printf("2 - Ordem decrescente\n");
    printf("3 - Ordem aleatória\n");
    printf("Opção: ");
    scanf("%d", &tipo);

    printf("Digite a quantidade de registros para o out.bin: ");
    scanf("%d", &quantidade);

    // Só gera os arquivos .bin se não existirem
    FILE *test = fopen("dados_asc.bin", "rb");
    if (!test) {
        printf("Arquivos binários não encontrados. Gerando todos...\n");
    } else {
        fclose(test);
    }

    gerar_out_bin(tipo, quantidade);
    criar_txt_se_bin_existir();
    return 0;
}

/*void shuffle(int *array, int n) {
    if (n > 1) {
        for (int i = 0; i < n - 1; i++) {
            int j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

void gerar_arquivos() {
    FILE *asc = fopen("dados_asc.bin", "wb");
    FILE *desc = fopen("dados_desc.bin", "wb");
    FILE *randf = fopen("dados_rand.bin", "wb");

    if (!asc || !desc || !randf) {
        printf("Erro ao abrir arquivos.\n");
        exit(1);
    }

    int *array = malloc(TOTAL * sizeof(int));
    for (int i = 0; i < TOTAL; i++) {
        array[i] = i + 1;
    }

    // Embaralhar para aleatório
    srand(time(NULL));
    shuffle(array, TOTAL);

    Registro reg;
    strcpy(reg.dado2, "LoremIpsum");
    strcpy(reg.dado3, "LoremIpsumDolorSitAmet");

    for (int i = 0; i < TOTAL; i++) {
        // Ascendente
        reg.chave = i + 1;
        reg.dado1 = reg.chave * 10;
        fwrite(&reg, sizeof(Registro), 1, asc);

        // Descendente
        reg.chave = TOTAL - i;
        reg.dado1 = reg.chave * 10;
        fwrite(&reg, sizeof(Registro), 1, desc);

        // Aleatório
        reg.chave = array[i];
        reg.dado1 = reg.chave * 10;
        fwrite(&reg, sizeof(Registro), 1, randf);

        if (i % 100000 == 0)
            printf("Progresso: %d registros\n", i);
    }

    fclose(asc);
    fclose(desc);
    fclose(randf);
    free(array);

    printf("Arquivos gerados com sucesso!\n");
}

int main() {
    gerar_arquivos();
    return 0;
}*/
