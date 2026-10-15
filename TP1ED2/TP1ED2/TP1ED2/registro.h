#ifndef REGISTRO_H
#define REGISTRO_H

// === Parâmetros da Árvore B ===
// M: grau mínimo → mínimo de chaves em cada página interna (exceto raiz)
// MM: ordem da árvore B (máximo de chaves = MM)

//#define M 2
//#define MM (2 * M)

// === Tipo de chave ===
typedef long TipoChave;

// === Registro ===
typedef struct TipoRegistro {
    TipoChave Chave;
    long dado1;
    char dado2[1000];
    char dado3[5000];
} TipoRegistro;

#endif
