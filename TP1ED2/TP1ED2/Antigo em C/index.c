#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include "registro.h"


#define ITENSPAGINA 100

typedef struct pagina {
    TipoRegistro itens[100];
} pagina;

typedef struct livro {
    pagina* paginas;
}Livro;

/*void pesquisa(TipoRegistro tab[], int tam, TipoRegistro* item, FILE *arq) {

    TipoRegistro pagina[ITENSPAGINA];
    int i, qtditens;
    long desloc;

    i = 0;
    while (i < tam && tab[i].chave <= item->dado1) {
        i++;
    }

    if (i == 0) {
        printf("Chave nao encontrada\n");
        return;
    } else {
        if (i < tam) qtditens = ITENSPAGINA;
        else {
            fseek(arq, 0, SEEK_END);
            qtditens = (ftell(arq) / (3 * sizeof(TipoRegistro))) % ITENSPAGINA;
        }
        desloc = (tab[i - 1].chave - 1)*ITENSPAGINA*sizeof(TipoRegistro);
        fseek(arq, desloc, SEEK_SET);

        fread(&pagina[0], sizeof(TipoRegistro), 1, arq);

        fseek(arq, 99 * sizeof(TipoRegistro), SEEK_CUR);

        if (pagina[0].chave == item->chave) {
            *item = pagina[0];
            printf("Item encontrado: %ld, %s, %s\n", item->dado1, item->dado2, item->dado3);
            return;
        } else {
            printf("Chave nao encontrada\n");
            return;
        }

        for (i = 0; i < qtditens; i++) {
            if (pagina[i].chave == item->chave) {
                *item = pagina[i];
                printf("Item encontrado: %ld, %s, %s\n", item->dado1, item->dado2, item->dado3);
                return;
            }
        }
        return;
    }
}*/

void sequencial(int qntdItens, int situacao, long int chave, long *nTransferenciaPre, long *nTransferencia, long *nComparacoes);


//PESQUISA SEQUENCIAL
void sequencial(int qntdItens, int situacao, long int chave, long *nTransferenciaPre, long *nTransferencia, long *nComparacoes) {
    
    //ABERTURA DOS ARQUIVOS
    FILE* arq = fopen("out.bin", "rb");
    if(arq == NULL) {
        printf("Arquivo inexistente!\n");
        return;
    }

    //DECLARAÇÕES
    int qntdPaginas = ceil((double)qntdItens / ITENSPAGINA), i = 0; // calcula a quantidade de páginas
    Livro l = { 0 }; 
    TipoRegistro item; 

    //ALOCAÇÃO DE MEMÓRIA PARA AS PÁGINAS (apenas uma vez!)
    l.paginas = calloc(qntdPaginas, sizeof(pagina));
    if (l.paginas == NULL) {
        printf("Erro de memória!\n");
        fclose(arq);
        return;
    }
    i = 0;
    
    //PAGINAÇÃO DO ARQUIVO (tabela de paginas)
    while (fread(&item, sizeof(item), 1, arq) == 1 && i < qntdPaginas) {
        l.paginas[i++].itens[0] = item;
        fseek(arq, sizeof(TipoRegistro) * (ITENSPAGINA - 1), SEEK_CUR);
        (*nTransferenciaPre)++;
    } 
    
    switch (situacao) {

    //ORDEM CRESCENTE
    case 1:

        //VERIFICANDO SE EXISTE A CHAVE NO ARQUIVO
        if(l.paginas[0].itens[0].Chave > chave) {
            (*nComparacoes)++; 
            printf("Não há uma chave correspondente no arquivo!\n");

            //LIBERAÇÃO DE MEMÓRIA ALOCADA
            free(l.paginas);

            //FECHAMENTO DOS ARQUIVOS
            fclose(arq);
            return;
        }
        for (i = 0; i < qntdPaginas; i++)
        {
            (*nComparacoes)++; //ACRESCENTA UM POR CADA COMPARAÇÃO DO PRIMEIRO ITEM DA PÁGINA
            if(l.paginas[i].itens[0].Chave == chave) {
                i++;
                break;
            }
            else if(l.paginas[i].itens[0].Chave > chave) {
                break;
            }
        }
        i--;
        (*nComparacoes)--;  //É DECRESCIDO PARA NÃO HAVER LEITURA DUPLA DO PRIMEIRO ITEM DA PÁGINA

        //PULANDO PRA PÁGINA QUE SERÁ EFETUADA A LEITURA COMPLETA
        fseek(arq, sizeof(TipoRegistro) * i * ITENSPAGINA, SEEK_SET);
        break;

    //ORDEM DECRESCENTE
    case 2:
        
        //VERIFICANDO SE EXISTE A CHAVE NO ARQUIVO
        if(l.paginas[0].itens[0].Chave < chave) {
            (*nComparacoes)++;
            printf("Não há uma chave correspondente no arquivo!\n");
            //LIBERAÇÃO DE MEMÓRIA ALOCADA
            free(l.paginas);

            //FECHAMENTO DOS ARQUIVOS
            fclose(arq);
            return;
        }
        for (i = 0; i < qntdPaginas; i++)
        {
            (*nComparacoes)++; //ACRESCENTA UM POR CADA COMPARAÇÃO DO PRIMEIRO ITEM DA PÁGINA
            if(l.paginas[i].itens[0].Chave == chave) {
                i++;
                break;
            }
            else if(l.paginas[i].itens[0].Chave < chave) {
                break;
            }
        }
        i--;
        (*nComparacoes)--;  //É DECRESCIDO PARA NÃO HAVER LEITURA DUPLA DO PRIMEIRO ITEM DA PÁGINA

        //PULANDO PRA PÁGINA QUE SERÁ EFETUADA A LEITURA COMPLETA
        fseek(arq, sizeof(TipoRegistro) * i * ITENSPAGINA, SEEK_SET);
        break;
    
    default:
        break;
    }


    //LEITURA DA PÁGINA
    (*nTransferencia)++;  //TRANSFERENCIA DA PÁGINA QUE ESTÁ LOCALIZADA O ITEM
    unsigned int count = 0;
    while((fread(&item, sizeof(item), 1, arq) == 1) && (count < ITENSPAGINA)) {
        (*nTransferenciaPre)++;
        l.paginas[i].itens[count] = item;
        count++;
    }

    //PROCURA DO ITEM
    for (unsigned int k = 0; k < count; k++) {
        (*nComparacoes)++;
        if(l.paginas[i].itens[k].Chave == chave){
            printf("----------------------------------------------\nItem encontrado\n\nDetalhes ['%d', '%ld', '%s', '%s']\n----------------------------------------------", l.paginas[i].itens[k].Chave, l.paginas[i].itens[k].dado1, l.paginas[i].itens[k].dado2, l.paginas[i].itens[k].dado3);

            //LIBERAÇÃO DE MEMÓRIA ALOCADA
            free(l.paginas);

            //FECHAMENTO DOS ARQUIVOS
            fclose(arq);
            return;
        }
    }
    printf("Não há uma chave correspondente no arquivo!\n");

    //LIBERAÇÃO DE MEMÓRIA ALOCADA
    free(l.paginas);

    //FECHAMENTO DOS ARQUIVOS
    fclose(arq);
}

