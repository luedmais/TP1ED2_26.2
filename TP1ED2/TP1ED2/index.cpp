#include "registro.hpp"
#include "index.hpp"
#include <cstdio>
#include <cstdlib>
#include <cmath>

#define ITENSPAGINA 100

// Buffer de UMA página inteira do arquivo de dados. Só existe UMA instância
// disso no programa inteiro (usada mais abaixo, na hora de ler a página onde
// o item deve estar) -- não é o índice.
struct PaginaIdx {
    TipoRegistro itens[ITENSPAGINA];
};

// Entrada do índice de páginas em memória principal: um par <chave, página>,
// exatamente como descrito no slide "Acesso Sequencial Indexado" (índice
// contém pares <x, p>, x = chave do 1º item da página, p = endereço da
// página). Guardar só isso (e não a página inteira) é o que torna o método
// viável: o índice fica muito menor que o arquivo de dados.
// BUG CORRIGIDO: a versão anterior usava "PaginaIdx paginas[qntdPaginas]"
// como índice, ou seja, alocava uma página inteira (100 itens x ~5KB cada)
// para CADA entrada do índice, quando bastava guardar a chave e o número da
// página. Para 2.000.000 de registros (maior caso de teste do TP) isso
// chegava a reservar ~10 GB só para o índice, em vez de ~156 KB.
struct EntradaIndice {
    int chave;  // chave do 1º item da página
    int pagina; // número da página (0-based) no arquivo de dados
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

    // Monta o índice de páginas: guarda só o 1º item de cada página.
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

    // Lê a página completa onde o item deveria estar.
    // (único buffer de página alocado no programa: reaproveitado aqui, e só
    // aqui -- o índice acima não precisa dele.)
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