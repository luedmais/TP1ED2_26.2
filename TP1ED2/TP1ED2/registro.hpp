#ifndef REGISTRO_HPP
#define REGISTRO_HPP

// Registro de dados do arquivo binário (conforme especificação do TP01).
// IMPORTANTE: esta é a ÚNICA definição de TipoRegistro do projeto.
// Antes havia 3 versões diferentes (registro.h, registro.hpp, registrobe.h)
// com campos e nomes diferentes; todos os módulos agora usam esta aqui.
struct TipoRegistro {
    int  chave;       // chave de pesquisa
    long dado1;       // valor inteiro longo
    char dado2[5000]; // cadeia de 5000 caracteres
};

#endif

