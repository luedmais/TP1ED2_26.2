#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "registro.h"
#include "arvore_binaria.h"
#include "arvoreB.h"

#define ITENSPAGINA 100

extern void sequencial(int qntdItens, int situacao, long int chave, int *nTransferenciaPre, int *nTransferencia, int *nComparacoes);

int main(int argc, char *argv[]) {
    if (argc < 5 || argc > 6) {
        printf("Uso: ./pesquisa <metodo> <quantidade> <situacao> <chave> [-P]\n");
        return 1;
    }

    int metodo = atoi(argv[1]);
    int quantidade = atoi(argv[2]);
    int situacao = atoi(argv[3]);
    long chave = atol(argv[4]);
    int mostrarChaves = (argc == 6 && strcmp(argv[5], "-P") == 0);

    FILE *arquivo = fopen("out.bin", "rb");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo out.bin\n");
        return 1;
    }

    if (mostrarChaves) {
        TipoRegistro temp;
        int count = 0;
        printf("Chaves:");
        while (fread(&temp, sizeof(TipoRegistro), 1, arquivo) == 1 && count < quantidade) {
            printf(" %ld", temp.Chave);
            count++;
        }
        printf("\n");
        rewind(arquivo);
    }

    int nTransferenciaPre = 0, nTransferencia = 0, nComparacoes = 0, nComparacoesPre = 0;
    clock_t inicio = clock();

    switch (metodo) {
        case 1:
            sequencial(quantidade, situacao, chave, &nTransferenciaPre, &nTransferencia, &nComparacoes);
            break;

        case 2: {
            FILE *arv = fopen("arvore_binaria.bin", "wb+");
            if (!arv) {
                printf("Erro ao criar arvore_binaria.bin\n");
                fclose(arquivo);
                return 1;
            }

            TipoRegistro temp;
            int pos = 0;
            while (fread(&temp, sizeof(TipoRegistro), 1, arquivo) == 1 && pos < quantidade) {
                NoArvore no;
                no.chave = temp.Chave;
                no.dado1 = temp.dado1;
                strcpy(no.dado2, temp.dado2);
                no.esquerda = no.direita = -1;
                no.indice = pos;
                fseek(arv, pos * sizeof(NoArvore), SEEK_SET);
                fwrite(&no, sizeof(NoArvore), 1, arv);
                pos++;
                nTransferenciaPre++;
            }

            rewind(arv);
            for (int i = 1; i < pos; i++) {
                inserirNaArvore(arv, 0, i, &nTransferencia, &nComparacoes);
            }

            rewind(arv);
            int resultado = buscarNaArvore(chave, arv, &nTransferencia, &nComparacoes);
            if (resultado != -1)
                printf("Registro com chave %ld encontrado.\n", chave);
            else
                printf("Registro com chave %ld nao encontrado.\n", chave);

            fclose(arv);
            break;
        }

        case 3: {
            TipoRegistro temp;
            TipoApontador Arvore = NULL;
            Inicializa(&Arvore);
            int count = 0;

            while (fread(&temp, sizeof(TipoRegistro), 1, arquivo) == 1 && count < quantidade) {
                Insere(temp, &Arvore, &nComparacoesPre);
                count++;
                nTransferenciaPre++;
            }

            TipoRegistro buscado;
            buscado.Chave = chave;
            int ok = Pesquisa(&buscado, Arvore, &nComparacoes);
            if (ok) {
                printf("Registro encontrado: [%ld, %ld, %s, %s]\n", buscado.Chave, buscado.dado1, buscado.dado2, buscado.dado3);
            } else {
                printf("Registro com chave %ld nao encontrado.\n", chave);
            }

            Limpar(&Arvore);
            break;
        }

        default:
            printf("Metodo invalido. Use 1, 2 ou 3.\n");
            fclose(arquivo);
            return 1;
    }

    clock_t fim = clock();
    double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("\nTempo de execucao: %.6f segundos\n", tempo);
    printf("Transferencias (pre-processamento): %d\n", nTransferenciaPre);
    printf("Transferencias (durante busca): %d\n", nTransferencia);
    printf("Comparacoes (pre-processamento): %d\n", nComparacoesPre);
    printf("Comparacoes: %d\n", nComparacoes);

    fclose(arquivo);
    return 0;
}
