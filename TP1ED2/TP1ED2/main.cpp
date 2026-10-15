#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>

#include "registro.hpp"
#include "paginacao.hpp"
#include "gerador_arquivo.hpp"
#include "index.hpp"
#include "arvore_binaria.hpp"
#include "arvoreB.hpp"
#include "arvoreBE.hpp"

static void imprimirChaves(const char *nomeArquivo, int quantidade) {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo) return;
    TipoRegistro temp;
    int count = 0;
    printf("Chaves:");
    while (fread(&temp, sizeof(TipoRegistro), 1, arquivo) == 1 && count < quantidade) {
        printf(" %d", temp.chave);
        count++;
    }
    printf("\n");
    fclose(arquivo);
}

static void imprimirEstatisticas(const char *rotulo, Estatisticas *e) {
    printf("%s -> transferencias: %d | comparacoes: %d | tempo: %f s\n",
           rotulo, e->acessos, e->comparacoes, e->tempo / 1e9);
}

int main(int argc, char *argv[]) {
    if (argc < 5 || argc > 6) {
        printf("Uso: ./pesquisa <metodo> <quantidade> <situacao> <chave> [-P]\n");
        return 1;
    }

    int metodo = atoi(argv[1]);
    int quantidade = atoi(argv[2]);
    int situacao = atoi(argv[3]);
    long chave = atol(argv[4]);
    bool mostrarChaves = (argc == 6 && strcmp(argv[5], "-P") == 0);

    if (metodo < 1 || metodo > 4) {
        printf("Metodo invalido. Use 1, 2, 3 ou 4.\n");
        return 1;
    }

    // Garante que o arquivo de entrada exista (gera automaticamente se necessário).
    std::string nomeArquivo = garantirArquivoExiste(quantidade, situacao);

    if (mostrarChaves) {
        imprimirChaves(nomeArquivo.c_str(), quantidade);
    }

    Estatisticas estatIndice, estatBusca;
    iniciaEstatisticas(&estatIndice);
    iniciaEstatisticas(&estatBusca);

    switch (metodo) {
    case 1: { // Acesso sequencial indexado
        if (situacao != 1 && situacao != 2) {
            printf("O acesso sequencial indexado so se aplica a arquivos ordenados (situacao 1 ou 2).\n");
            return 1;
        }
        sequencial(nomeArquivo.c_str(), quantidade, situacao, chave, &estatIndice, &estatBusca);
        break;
    }

    case 2: { // Arvore binaria externa
        FILE *arquivo = fopen(nomeArquivo.c_str(), "rb");
        if (!arquivo) { printf("Erro ao abrir %s\n", nomeArquivo.c_str()); return 1; }

        FILE *arv = fopen("arvore_binaria.bin", "wb+");
        if (!arv) { printf("Erro ao criar arvore_binaria.bin\n"); fclose(arquivo); return 1; }

        TipoRegistro temp;
        int pos = 0;
        while (fread(&temp, sizeof(TipoRegistro), 1, arquivo) == 1 && pos < quantidade) {
            NoArvore no;
            no.chave = temp.chave;
            no.dado1 = temp.dado1;
            strncpy(no.dado2, temp.dado2, sizeof(no.dado2));
            no.esquerda = no.direita = -1;
            no.indice = pos;
            fseek(arv, pos * sizeof(NoArvore), SEEK_SET);
            fwrite(&no, sizeof(NoArvore), 1, arv);
            pos++;
            estatIndice.acessos++;
        }
        fclose(arquivo);

        rewind(arv);
        for (int i = 1; i < pos; i++) {
            inserirNaArvore(arv, 0, i, &estatIndice.acessos, &estatIndice.comparacoes);
        }

        rewind(arv);
        int resultado = buscarNaArvore((int) chave, arv, &estatBusca.acessos, &estatBusca.comparacoes);
        printf(resultado != -1 ? "Registro com chave %ld encontrado.\n"
                                : "Registro com chave %ld nao encontrado.\n", chave);

        fclose(arv);
        break;
    }

    case 3: { // Arvore B
        FILE *arquivo = fopen(nomeArquivo.c_str(), "rb");
        if (!arquivo) { printf("Erro ao abrir %s\n", nomeArquivo.c_str()); return 1; }

        TipoApontadorB Arvore;
        Inicializa(&Arvore);

        // Le o arquivo pagina a pagina (TAM_PAG registros por vez), e cada
        // pagina lida conta como 1 transferencia (ver paginacao.hpp).
        PagReg pag;
        int inseridos = 0;
        while (inseridos < quantidade && paginacao(arquivo, &pag, &estatIndice)) {
            for (int i = 0; i < pag.quantidade && inseridos < quantidade; i++) {
                Insere(pag.registros[i], &Arvore, &estatIndice);
                inseridos++;
            }
        }
        fclose(arquivo);

        TipoRegistro buscado;
        buscado.chave = (int) chave;
        if (Pesquisa(&buscado, Arvore, &estatBusca)) {
            printf("Registro encontrado: [chave=%d, dado1=%ld, dado2=%s]\n",
                   buscado.chave, buscado.dado1, buscado.dado2);
        } else {
            printf("Registro com chave %ld nao encontrado.\n", chave);
        }

        Limpar(&Arvore);
        break;
    }

    case 4: { // Arvore B*
        FILE *arquivo = fopen(nomeArquivo.c_str(), "rb");
        if (!arquivo) { printf("Erro ao abrir %s\n", nomeArquivo.c_str()); return 1; }

        TipoApontadorBE Arvore = NULL;

        PagReg pag;
        int inseridos = 0;
        while (inseridos < quantidade && paginacao(arquivo, &pag, &estatIndice)) {
            for (int i = 0; i < pag.quantidade && inseridos < quantidade; i++) {
                Arvore = insere(pag.registros[i], Arvore, &estatIndice);
                inseridos++;
            }
        }
        fclose(arquivo);

        TipoRegistro buscado;
        buscado.chave = (int) chave;
        TipoApontadorBE folha;
        if (pesquisaArvoreBE(buscado, Arvore, &estatBusca, &folha)) {
            printf("Registro com chave %ld encontrado (folha com %d itens).\n", chave, folha->UU.U1.ne);
        } else {
            printf("Registro com chave %ld nao encontrado.\n", chave);
        }

        destruirArvoreBE(Arvore);
        break;
    }
    }

    printf("\n--- Estatisticas ---\n");
    imprimirEstatisticas("Criacao do indice", &estatIndice);
    imprimirEstatisticas("Pesquisa         ", &estatBusca);

    return 0;
}
