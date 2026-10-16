#include "registro.hpp"
#include "index.hpp"
#include <cstdio>
#include <cstdlib>
#include <cmath>

#define ITENSPAGINA 100
struct PaginaIdx {
    TipoRegistro itens[ITENSPAGINA];
};

struct EntradaIndice {
    int chave;  // chave do 1º item da pagina
    int pagina; // numero da pagina (0-based) no arquivo de dados
};

void sequencial(const char *nomeArquivo, int qntdItens, int situacao, long chave,
                Estatisticas *estatisticasIndice, Estatisticas *estatisticasBusca) {

    FILE *arq = fopen(nomeArquivo, "rb");
    if (arq == NULL) {
        printf("Arquivo inexistente!\n");
        return;
    }

    int qntdPaginas = (int) ceil((double) qntdItens / ITENSPAGINA);
    int i = 0;
    TipoRegistro item;

    // Monta o indice de paginas: guarda so o 1º item de cada pagina.
    EntradaIndice *indice = (EntradaIndice *) calloc(qntdPaginas, sizeof(EntradaIndice));
    if (indice == NULL) {
        printf("Erro de memoria!\n");
        fclose(arq);
        return;
    }

    while (fread(&item, sizeof(item), 1, arq) == 1 && i < qntdPaginas) {
        indice[i].chave = item.chave;
        indice[i].pagina = i;
        i++;
        fseek(arq, sizeof(TipoRegistro) * (ITENSPAGINA - 1), SEEK_CUR);
        estatisticasIndice->acessos++;
    }

    switch (situacao) {
    case 1: // ordem crescente
        if (indice[0].chave > chave) {
            estatisticasBusca->comparacoes++;
            printf("Nao ha uma chave correspondente no arquivo!\n");
            free(indice);
            fclose(arq);
            return;
        }
        for (i = 0; i < qntdPaginas; i++) {
            estatisticasBusca->comparacoes++;
            if (indice[i].chave == chave) { i++; break; }
            if (indice[i].chave > chave) break;
        }
        i--;
        estatisticasBusca->comparacoes--;
        fseek(arq, sizeof(TipoRegistro) * i * ITENSPAGINA, SEEK_SET);
        break;

    case 2: // ordem decrescente
        if (indice[0].chave < chave) {
            estatisticasBusca->comparacoes++;
            printf("Nao ha uma chave correspondente no arquivo!\n");
            free(indice);
            fclose(arq);
            return;
        }
        for (i = 0; i < qntdPaginas; i++) {
            estatisticasBusca->comparacoes++;
            if (indice[i].chave == chave) { i++; break; }
            if (indice[i].chave < chave) break;
        }
        i--;
        estatisticasBusca->comparacoes--;
        fseek(arq, sizeof(TipoRegistro) * i * ITENSPAGINA, SEEK_SET);
        break;

    default:
        printf("Situacao %d nao suportada pelo acesso sequencial indexado (so 1 ou 2).\n", situacao);
        free(indice);
        fclose(arq);
        return;
    }

    // Le a pagina completa onde o item deveria estar.
    // (unico buffer de pagina alocado no programa: reaproveitado aqui, e so
    // aqui -- o indice acima nao precisa dele.)
    PaginaIdx *pagina = (PaginaIdx *) malloc(sizeof(PaginaIdx));
    if (pagina == NULL) {
        printf("Erro de memoria!\n");
        free(indice);
        fclose(arq);
        return;
    }

    estatisticasBusca->acessos++;
    unsigned int count = 0;
    while ((fread(&item, sizeof(item), 1, arq) == 1) && count < ITENSPAGINA) {
        pagina->itens[count] = item;
        count++;
    }

    for (unsigned int k = 0; k < count; k++) {
        estatisticasBusca->comparacoes++;
        if (pagina->itens[k].chave == chave) {
            printf("----------------------------------------------\n"
                   "Item encontrado\nDetalhes [chave=%d, dado1=%ld, dado2=%s]\n"
                   "----------------------------------------------\n",
                   pagina->itens[k].chave, pagina->itens[k].dado1, pagina->itens[k].dado2);
            free(pagina);
            free(indice);
            fclose(arq);
            return;
        }
    }

    printf("Nao ha uma chave correspondente no arquivo!\n");
    free(pagina);
    free(indice);
    fclose(arq);
}