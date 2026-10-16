#ifndef REGISTRO_HPP
#define REGISTRO_HPP

struct TipoRegistro {
    int  chave;       // chave de pesquisa
    long dado1;       // valor inteiro longo
    char dado2[5000]; // cadeia de 5000 caracteres
};

#endif

